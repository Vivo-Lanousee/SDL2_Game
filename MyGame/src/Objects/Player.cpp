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

    // 初回の銃テクスチャ読み込み
    // 実際の運用では Game::GetRenderer() などから renderer を渡す必要があります
}

void Player::Update(Game* game) {
    InputHandler* input = game->GetInput();
    GameParams& params = GameParams::GetInstance();

    animator->Update();

    // 1. 移動処理
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

    // 2. ジャンプ処理
    if (input->IsJustPressed(GameAction::MoveUp)) {
        if (isGrounded) {
            velY = -params.player.jumpVelocity;
            isGrounded = false;
        }
    }

    // 3. 射撃タイマー更新
    if (fireCooldown > 0) {
        fireCooldown -= Time::deltaTime;
    }

    // 4. マウス位置取得
    int screenMouseX, screenMouseY;
    SDL_GetMouseState(&screenMouseX, &screenMouseY);
    SDL_FPoint worldMouse = camera->ScreenToWorld(screenMouseX, screenMouseY);

    // 5. 射撃実行
    // params.gun.fireRate (秒) を使用
    if (input->IsPressed(GameAction::Shoot) && fireCooldown <= 0.0f) {
        auto newBullet = Shoot(worldMouse.x, worldMouse.y, bulletTexture);

        if (newBullet) {
            game->Instantiate(std::move(newBullet));
            // 次の射撃までの待ち時間を設定
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
    else {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &destRect);
    }

    // --- 銃の描画ロジック (追加) ---
    // マウスの方向を向く「銃」のビジュアルを作成
    int mx, my;
    SDL_GetMouseState(&mx, &my);

    float centerX = (float)drawX + (float)width / 2.0f;
    float centerY = (float)drawY + (float)height / 2.0f;
    double gunAngle = atan2(my - (centerY - camera->y + camera->y), mx - (centerX - camera->x + camera->x)) * 180.0 / M_PI;

    // 簡易的な銃のライン描画 (テクスチャがない場合の代わり)
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    int lineEndX = (int)(centerX + cos(gunAngle * M_PI / 180.0) * 40);
    int lineEndY = (int)(centerY + sin(gunAngle * M_PI / 180.0) * 40);
    SDL_RenderDrawLine(renderer, (int)centerX, (int)centerY, lineEndX, lineEndY);
}

std::unique_ptr<Bullet> Player::Shoot(float targetX, float targetY, SDL_Texture* bulletTex) {
    GameParams& params = GameParams::GetInstance();

    float spawnX = x + (width / 2.0f);
    float spawnY = y + (height / 2.0f);

    float dx = targetX - spawnX;
    float dy = targetY - spawnY;
    float distance = sqrt(dx * dx + dy * dy);

    // 弾の速度ベクトルを計算 (GameParams の bulletSpeed を使用)
    float vx = (dx / distance) * params.gun.bulletSpeed;
    float vy = (dy / distance) * params.gun.bulletSpeed;

    // Bullet のコンストラクタに合わせて生成
    // (x, y, width, height, velX, velY, damage, texture)
    return std::make_unique<Bullet>(
        spawnX - 5, spawnY - 5,
        10, 10,
        vx, vy,
        params.gun.damage,
        bulletTex
    );
}

void Player::TakeDamage(int damage) {
    currentHealth -= damage;
    float maxHp = GameParams::GetInstance().player.maxHealth;
    if (currentHealth < 0) currentHealth = 0;
    if (currentHealth > maxHp) currentHealth = maxHp;
}

int Player::GetMaxHP() const {
    return (int)GameParams::GetInstance().player.maxHealth;
}

void Player::RefreshGunTexture(SDL_Renderer* renderer) {
    std::string path = GameParams::GetInstance().gun.texturePath;
    gunTexture = TextureManager::LoadTexture(path.c_str(), renderer);
}