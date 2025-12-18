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
#include <random> // ★追加: 集弾率の計算に必要

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

    // 弾の発射位置（プレイヤーの中心付近）
    float spawnX = x + (width / 2.0f);
    float spawnY = y + (height / 2.0f);

    // 1. ターゲット（マウス座標）への基本角度を計算
    float dx = targetX - spawnX;
    float dy = targetY - spawnY;
    float baseAngleRad = atan2(dy, dx);

    // 0除算防止
    if (dx == 0 && dy == 0) return nullptr;

    // 2. ★集弾率（スプレッド）の適用
    // 狙った角度に対して ±(spreadAngle / 2) の範囲でランダムな誤差を加える
    static std::random_device rd;
    static std::mt19937 gen(rd());

    // 度数法をラジアンに変換
    float spreadRad = params.gun.spreadAngle * (M_PI / 180.0f);
    std::uniform_real_distribution<float> dist(-spreadRad / 2.0f, spreadRad / 2.0f);

    // 最終的な発射角度
    double finalAngleRad = baseAngleRad + dist(gen);

    // 3. 最終的な速度ベクトルを算出
    // $vx = \cos(angle) \times bulletSpeed$
    // $vy = \sin(angle) \times bulletSpeed$
    float vx = (float)cos(finalAngleRad) * params.gun.bulletSpeed;
    float vy = (float)sin(finalAngleRad) * params.gun.bulletSpeed;

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