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
    rotationAngle(0.0f)
{
    name = "Turret (" + config.name + ")";
    useGravity = false;
    isTrigger = false;
    if (config.fireRate > 0) {
        fireCooldown = 1.0f / config.fireRate;
    }
}

void Turret::Update(Game* game) {
    if (isDead) return;

    if (fireCooldown > 0) {
        fireCooldown -= Time::deltaTime;
    }

    if (!currentTarget || currentTarget->isDead) {
        FindTarget(game->GetCurrentSceneObjects());
    }

    if (currentTarget) {
        RotateTowardTarget(Time::deltaTime);
        if (fireCooldown <= 0.0f) {
            Fire(game);
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

    auto bullet = std::make_unique<Bullet>(
        startX, startY,
        weaponConfig.bulletWidth, weaponConfig.bulletHeight,
        velX, velY,
        weaponConfig.damage,
        game->GetBulletTexture()
    );

    game->Instantiate(std::move(bullet));
}

void Turret::OnRender(SDL_Renderer* renderer, int drawX, int drawY) {
    SDL_Rect destRect = { drawX, drawY, width, height };
    SDL_SetRenderDrawColor(renderer, 50, 50, 150, 255);
    SDL_RenderFillRect(renderer, &destRect);

    int turretCenterX = drawX + width / 2;
    int turretCenterY = drawY + height / 2;
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    float angleRad = rotationAngle * ((float)M_PI / 180.0f);
    int lineEndX = turretCenterX + (int)(width * 1.5 * cos(angleRad));
    int lineEndY = turretCenterY + (int)(width * 1.5 * sin(angleRad));
    SDL_RenderDrawLine(renderer, turretCenterX, turretCenterY, lineEndX, lineEndY);
}