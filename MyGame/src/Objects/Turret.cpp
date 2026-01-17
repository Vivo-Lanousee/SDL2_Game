#define _USE_MATH_DEFINES
#include "Turret.h"
#include "../Core/Game.h"
#include "../Core/Time.h"
#include "../Core/Physics.h"
#include "../Objects/Enemy.h"
#include "../Objects/Bullet.h"
#include <cmath>
#include <random>
#include <algorithm> 
#include <limits> 
#include <iostream>

const int TURRET_DEFAULT_SIZE = 32;

Turret::Turret(float x, float y, const WeaponConfig& config, SDL_Texture* tex)
    : GameObject(x, y, TURRET_DEFAULT_SIZE, TURRET_DEFAULT_SIZE, tex),
    weaponConfig(config),
    fireCooldown(0.0f),
    currentTarget(nullptr),
    rotationAngle(0.0f),
    reloadTimer(0.0f),
    isReloading(false)
{
    name = "Turret (" + config.name + ")";
    useGravity = false;
    isTrigger = false;

    // 残弾初期化
    currentAmmo = config.magazineSize;

    if (config.fireRate > 0) {
        fireCooldown = 1.0f / config.fireRate;
    }
}

void Turret::Update(Game* game) {
    if (isDead) return;

    // リロードタイマーの更新
    if (isReloading) {
        reloadTimer -= Time::deltaTime;
        if (reloadTimer <= 0) {
            currentAmmo = weaponConfig.magazineSize;
            isReloading = false;
            std::cout << name << " Reload Complete!" << std::endl;
        }
        return; // リロード中は索敵・射撃を行わない
    }

    if (fireCooldown > 0) {
        fireCooldown -= Time::deltaTime;
    }

    if (!currentTarget || currentTarget->isDead) {
        FindTarget(game->GetCurrentSceneObjects());
    }

    if (currentTarget) {
        RotateTowardTarget(Time::deltaTime);

        // クールダウン終了判定
        if (fireCooldown <= 0.0f) {
            // 弾がある場合は射撃、弾がない場合はリロード開始
            if (currentAmmo > 0) {
                Fire(game);
            }
            else {
                isReloading = true;
                reloadTimer = weaponConfig.reloadTime;
                std::cout << name << " Out of ammo! Reloading..." << std::endl;
            }
        }
    }
}

void Turret::FindTarget(const std::vector<std::unique_ptr<GameObject>>& gameObjects) {
    currentTarget = nullptr;
    float rangeSq = weaponConfig.range * weaponConfig.range;
    float shortestDistanceSq = rangeSq + 1.0f;

    float turretX = x + (float)width / 2.0f;
    float turretY = y + (float)height / 2.0f;

    for (const auto& obj : gameObjects) {
        Enemy* enemy = dynamic_cast<Enemy*>(obj.get());
        if (enemy && !enemy->isDead) {
            float enemyX = enemy->x + (float)enemy->width / 2.0f;
            float enemyY = enemy->y + (float)enemy->height / 2.0f;
            float distSq = Physics::DistanceSquared(turretX, turretY, enemyX, enemyY);

            if (distSq < rangeSq && distSq < shortestDistanceSq) {
                shortestDistanceSq = distSq;
                currentTarget = enemy;
            }
        }
    }
}

void Turret::RotateTowardTarget(float deltaTime) {
    rotationAngle = GetAngleToTarget();
}

float Turret::GetAngleToTarget() const {
    if (!currentTarget) return rotationAngle;
    float dx = (currentTarget->x + (float)currentTarget->width / 2.0f) - (x + (float)width / 2.0f);
    float dy = (currentTarget->y + (float)currentTarget->height / 2.0f) - (y + (float)height / 2.0f);
    return (float)(atan2(dy, dx) * 180.0 / M_PI);
}

void Turret::Fire(Game* game) {
    if (!currentTarget) return;

    if (weaponConfig.fireRate > 0) {
        fireCooldown = 1.0f / weaponConfig.fireRate;
    }
    else {
        fireCooldown = std::numeric_limits<float>::max();
    }

    // 弾数を減らす
    currentAmmo--;

    float targetAngle = GetAngleToTarget();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distrib(-weaponConfig.spreadAngle / 2.0f, weaponConfig.spreadAngle / 2.0f);
    float finalAngle = targetAngle + (float)distrib(gen);

    SpawnBullet(game, finalAngle);
}

void Turret::SpawnBullet(Game* game, float initialAngle) {
    float startX = x + (float)width / 2.0f;
    float startY = y + (float)height / 2.0f;
    float angleRad = initialAngle * ((float)M_PI / 180.0f);

    float velX = weaponConfig.bulletSpeed * cos(angleRad);
    float velY = weaponConfig.bulletSpeed * sin(angleRad);

    // BulletSide::Player を追加
    auto bullet = std::make_unique<Bullet>(
        startX, startY,
        weaponConfig.bulletWidth, weaponConfig.bulletHeight,
        velX, velY,
        weaponConfig.damage,
        game->GetBulletTexture(),
        BulletSide::Player
    );

    game->Instantiate(std::move(bullet));
}

void Turret::OnRender(SDL_Renderer* renderer, int drawX, int drawY) {
    SDL_Rect destRect = { drawX, drawY, width, height };

    // リロード中は色を変えるなどの視覚効果
    if (isReloading) {
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 50, 50, 150, 255);
    }

    SDL_RenderFillRect(renderer, &destRect);

    int turretCenterX = drawX + width / 2;
    int turretCenterY = drawY + height / 2;
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    float angleRad = rotationAngle * ((float)M_PI / 180.0f);
    int lineEndX = turretCenterX + (int)(width * 1.5 * cos(angleRad));
    int lineEndY = turretCenterY + (int)(width * 1.5 * sin(angleRad));
    SDL_RenderDrawLine(renderer, turretCenterX, turretCenterY, lineEndX, lineEndY);
}