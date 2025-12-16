#define _USE_MATH_DEFINES // ★修正: M_PI などの数学定数を有効にする
#include "Turret.h"
#include "../Core/Game.h"
#include "../Core/Time.h"
#include "../Core/Physics.h" // 距離計算などに利用
#include "../Objects/Enemy.h" // ターゲットとして利用
#include "../Objects/Bullet.h" // 弾丸生成に利用
#include <cmath> // ★修正: 数学関数と定数のためにインクルード
#include <random>
#include <algorithm> 
#include <limits> 
#include <iostream> // Debug用

// Turretの幅と高さのデフォルト値
const int TURRET_DEFAULT_SIZE = 32;

// -----------------------------------------------------
// コンストラクタ
// -----------------------------------------------------

Turret::Turret(float x, float y, const WeaponConfig& config, SDL_Texture* tex)
    : GameObject(x, y, TURRET_DEFAULT_SIZE, TURRET_DEFAULT_SIZE, tex),
    weaponConfig(config),
    fireCooldown(0.0f),
    currentTarget(nullptr),
    rotationAngle(0.0f)
{
    // GameObjectの camera ポインタは Turret クラスで管理されていることを想定

    name = "Turret (" + config.name + ")";
    useGravity = false;
    isTrigger = false;
    if (config.fireRate > 0) {
        fireCooldown = 1.0f / config.fireRate;
    }
}

// -----------------------------------------------------
// Update
// -----------------------------------------------------

void Turret::Update(Game* game) {
    if (isDead) return;

    // 1. クールダウン時間経過
    if (fireCooldown > 0) {
        fireCooldown -= Time::deltaTime;
    }

    // 2. ターゲット探索 
    // Game::GetCurrentSceneObjects() が Game.cpp に実装されている必要あり
    if (!currentTarget || currentTarget->isDead) {
        FindTarget(game->GetCurrentSceneObjects());
    }

    // 3. 射撃ロジック
    if (currentTarget) {
        RotateTowardTarget(Time::deltaTime);

        if (fireCooldown <= 0.0f) {
            Fire(game);
        }
    }
}

// -----------------------------------------------------
// ターゲット選定と回転
// -----------------------------------------------------

void Turret::FindTarget(const std::vector<std::unique_ptr<GameObject>>& gameObjects) {
    currentTarget = nullptr;
    float rangeSq = weaponConfig.range * weaponConfig.range;
    float shortestDistanceSq = rangeSq + 1.0f;

    float turretX = x + (float)width / 2.0f; // ★修正: widthをfloatにキャスト
    float turretY = y + (float)height / 2.0f; // ★修正: heightをfloatにキャスト

    for (const auto& obj : gameObjects) {
        Enemy* enemy = dynamic_cast<Enemy*>(obj.get());

        if (enemy && !enemy->isDead) {
            float enemyX = enemy->x + (float)enemy->width / 2.0f;
            float enemyY = enemy->y + (float)enemy->height / 2.0f;

            // Physics::DistanceSquared が Physics.h に実装されている必要あり
            float distSq = Physics::DistanceSquared(turretX, turretY, enemyX, enemyY);

            if (distSq < rangeSq && distSq < shortestDistanceSq) {
                shortestDistanceSq = distSq;
                currentTarget = enemy;
            }
        }
    }
}

// ターゲットに向かって回転角度を更新する
void Turret::RotateTowardTarget(float deltaTime) {
    float targetAngle = GetAngleToTarget();
    rotationAngle = targetAngle;
}

// ターゲットの中心に向かう角度を計算 (度数法)
float Turret::GetAngleToTarget() const {
    if (!currentTarget) return rotationAngle;

    // ★修正: width, height を float にキャストして計算の精度を確保
    float dx = (currentTarget->x + (float)currentTarget->width / 2.0f) - (x + (float)width / 2.0f);
    float dy = (currentTarget->y + (float)currentTarget->height / 2.0f) - (y + (float)height / 2.0f);

    // ★修正: M_PIを使用
    return (float)std::atan2(dy, dx) * 180.0f / (float)M_PI;
}

// -----------------------------------------------------
// 射撃
// -----------------------------------------------------

void Turret::Fire(Game* game) {
    if (!currentTarget) return;

    // 1. クールダウンをリセット
    if (weaponConfig.fireRate > 0) {
        fireCooldown = 1.0f / weaponConfig.fireRate;
    }
    else {
        fireCooldown = std::numeric_limits<float>::max();
    }

    // 2. 目標角度を計算
    float targetAngle = GetAngleToTarget();

    // 3. ★★★ 修正: 乱数生成器を関数内で初期化し、集弾性のブレを適用 ★★★
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distrib(-weaponConfig.spreadAngle / 2.0f, weaponConfig.spreadAngle / 2.0f);
    float finalAngle = targetAngle + (float)distrib(gen);

    // 4. 弾丸生成
    SpawnBullet(game, finalAngle);
}

void Turret::SpawnBullet(Game* game, float initialAngle) {
    // ★修正: width, height を float にキャスト
    float startX = x + (float)width / 2.0f;
    float startY = y + (float)height / 2.0f;

    // ★修正: M_PI を使用
    float angleRad = initialAngle * ((float)M_PI / 180.0f);

    float velX = weaponConfig.bulletSpeed * std::cos(angleRad);
    float velY = weaponConfig.bulletSpeed * std::sin(angleRad);

    // Bullet コンストラクタのシグネチャが合っていることを前提とする
    auto bullet = std::make_unique<Bullet>(
        startX, startY,
        weaponConfig.bulletWidth, weaponConfig.bulletHeight,
        velX, velY,
        weaponConfig.damage,
        game->GetBulletTexture()
    );

    game->Instantiate(std::move(bullet));
}

// -----------------------------------------------------
// Render (描画) 
// -----------------------------------------------------

void Turret::OnRender(SDL_Renderer* renderer, int drawX, int drawY) {
    SDL_Rect destRect = { drawX, drawY, width, height };

    // 1. タレット本体の描画 (青い四角)
    SDL_SetRenderDrawColor(renderer, 50, 50, 150, 255);
    SDL_RenderFillRect(renderer, &destRect);

    // 2. ターゲットがいる場合、射程と回転を描画
    if (currentTarget) {
        // Turretの中心座標 (画面上の描画座標 drawX, drawY を使う)
        int turretCenterX = drawX + width / 2;
        int turretCenterY = drawY + height / 2;

        // 3. 砲身の描画
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        float angleRad = rotationAngle * ((float)M_PI / 180.0f); // M_PI を使用
        int lineEndX = turretCenterX + (int)(width * 1.5 * std::cos(angleRad));
        int lineEndY = turretCenterY + (int)(width * 1.5 * std::sin(angleRad));

        SDL_RenderDrawLine(renderer, turretCenterX, turretCenterY, lineEndX, lineEndY);
    }

    // Debug: 射程サークルの描画 (もしあれば、ここで行う)
}