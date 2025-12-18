#include "Player.h"
#include "../Core/Game.h"
#include "../Core/InputHandler.h"
#include "../Core/Camera.h"
#include "../Core/Time.h"
#include "../Core/GameParams.h" 
#include "Bullet.h"
#include <cmath>
#include <memory>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Player::Player(float x, float y, SDL_Texture* tex, SDL_Texture* bulletTex, Camera* cam)
    : GameObject(x, y, 46, 128, tex),
    currentHealth(GameParams::GetInstance().player.maxHealth),
    fireCooldown(0.0f)
{
    angle = 0;
    useGravity = true;
    name = "Player";
    this->bulletTexture = bulletTex;
    this->camera = cam;
    isFlipLeft = false;

    animator = std::make_unique<Animator>();
    animator->LoadFromJson("assets/data/player.json");

    // 初期化時に現在の設定で銃をロード
    // ※この時点ではまだ renderer が確定していない場合があるため、
    // 最初の描画やUpdate内で必要に応じて RefreshGunConfig が呼ばれるようにします。
}

void Player::Update(Game* game) {
    InputHandler* input = game->GetInput();
    GameParams& params = GameParams::GetInstance();

    animator->Update();

    // 1. 移動ロジック (GameParamsの数値を即時反映)
    float moveSpeed = params.player.moveSpeed;
    velX = 0;
    if (input->IsPressed(GameAction::MoveLeft)) {
        velX = -moveSpeed;
        isFlipLeft = true;
    }
    if (input->IsPressed(GameAction::MoveRight)) {
        velX = moveSpeed;
        isFlipLeft = false;
    }

    if (velX != 0) {
        animator->Play("Run");
    }
    else {
        animator->Play("Idle");
    }

    // 2. ジャンプロジック
    if (input->IsJustPressed(GameAction::MoveUp)) {
        if (isGrounded) {
            velY = -params.player.jumpVelocity;
            isGrounded = false;
        }
    }

    // 3. 銃のテクスチャが未ロード、またはパスが変更された場合に更新
    if (!gunTexture) {
        RefreshGunConfig(game->GetRenderer());
    }

    // 4. 連射クールダウンの更新
    if (fireCooldown > 0) {
        fireCooldown -= Time::deltaTime;
    }

    // 5. 射撃処理
    int screenMouseX, screenMouseY;
    SDL_GetMouseState(&screenMouseX, &screenMouseY);
    SDL_FPoint worldMouse = camera->ScreenToWorld(screenMouseX, screenMouseY);

    if (input->IsPressed(GameAction::Shoot) && fireCooldown <= 0.0f) {
        auto newBullet = Shoot(worldMouse.x, worldMouse.y, bulletTexture);
        if (newBullet) {
            game->Instantiate(std::move(newBullet));
            // GameParamsに設定された連射間隔を適用
            fireCooldown = params.gun.fireRate;
        }
    }
}

void Player::OnRender(SDL_Renderer* renderer, int drawX, int drawY) {
    SDL_Rect destRect = { drawX, drawY, width, height };
    SDL_Rect srcRect = animator->GetSrcRect(width, height);
    SDL_RendererFlip flip = isFlipLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

    // プレイヤー本体の描画
    if (texture) {
        SDL_RenderCopyEx(renderer, texture, &srcRect, &destRect, angle, NULL, flip);
    }

    // --- 銃の描画ロジック ---
    if (gunTexture) {
        int mx, my;
        SDL_GetMouseState(&mx, &my);

        // プレイヤーの中心（肩のあたり）を回転軸にする
        float centerX = (float)drawX + (float)width / 2.0f;
        float centerY = (float)drawY + (float)height / 2.0f;

        // マウスへの角度計算
        double gunAngle = atan2(my - (centerY - camera->y + camera->y), mx - (centerX - camera->x + camera->x)) * 180.0 / M_PI;

        // 銃のサイズ（仮に 64x32 とする。必要ならGameParamsに持たせても良い）
        int gunW = 64;
        int gunH = 32;
        SDL_Rect gunDest = { (int)centerX - gunW / 4, (int)centerY - gunH / 2, gunW, gunH };

        // 銃が逆さまにならないようにフリップ処理
        SDL_RendererFlip gunFlip = (gunAngle > 90 || gunAngle < -90) ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;

        SDL_RenderCopyEx(renderer, gunTexture.get(), NULL, &gunDest, gunAngle, NULL, gunFlip);
    }
}

std::unique_ptr<Bullet> Player::Shoot(float targetX, float targetY, SDL_Texture* bulletTex) {
    GameParams& params = GameParams::GetInstance();

    float spawnX = x + (width / 2.0f);
    float spawnY = y + (height / 2.0f);

    float dx = targetX - spawnX;
    float dy = targetY - spawnY;
    float distance = sqrt(dx * dx + dy * dy);

    // 0除算防止
    if (distance == 0) return nullptr;

    // GameParams の bulletSpeed を使って弾の速度を決定
    float vx = (dx / distance) * params.gun.bulletSpeed;
    float vy = (dy / distance) * params.gun.bulletSpeed;

    // Bulletの生成 (x, y, w, h, vx, vy, damage, texture)
    return std::make_unique<Bullet>(
        spawnX - 5, spawnY - 5,
        10, 10,
        vx, vy,
        params.gun.damage,
        bulletTex
    );
}

void Player::TakeDamage(int damage) {
    currentHealth -= (float)damage;
    float maxHp = GameParams::GetInstance().player.maxHealth;
    if (currentHealth < 0) currentHealth = 0;
    if (currentHealth > maxHp) currentHealth = maxHp;
}

int Player::GetMaxHP() const {
    return (int)GameParams::GetInstance().player.maxHealth;
}

void Player::RefreshGunConfig(SDL_Renderer* renderer) {
    // GameParams に保存されているパスからテクスチャをロード
    std::string path = GameParams::GetInstance().gun.texturePath;
    if (!path.empty()) {
        gunTexture = TextureManager::LoadTexture(path.c_str(), renderer);
    }
}