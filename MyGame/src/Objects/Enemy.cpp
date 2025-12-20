#include "Enemy.h"
#include "../Core/Game.h"
#include "../Core/Time.h"
#include "../Core/Physics.h"
#include "../Core/GameParams.h" 
#include "../TextureManager.h"
#include "Bullet.h"
#include <cmath>
#include <iostream>
#include <algorithm> 

const float DEFAULT_DISTANCE_TOLERANCE = 5.0f;

Enemy::Enemy(float x, float y, int w, int h, SDL_Texture* tex,
    const std::vector<SDL_FPoint>& path)
    : GameObject(x, y, w, h, tex),
    targetPath(path), currentWaypointIndex(0),
    distanceTolerance(DEFAULT_DISTANCE_TOLERANCE), goalReached(false)
{
    // 初期設定の反映
    RefreshConfig(nullptr);

    rewardValue = 10;
    name = "Enemy";
    isTrigger = false;
    useGravity = false;
}

void Enemy::RefreshConfig(SDL_Renderer* renderer) {
    GameParams& params = GameParams::GetInstance();

    // ステータス更新
    hp = params.enemy.baseHealth;
    maxHp = params.enemy.baseHealth;
    moveSpeed = params.enemy.baseSpeed;

    // テクスチャ更新 (rendererが渡された場合のみ読み込み)
    if (renderer && !params.enemy.texturePath.empty()) {
        SharedTexturePtr newTex = TextureManager::LoadTexture(params.enemy.texturePath, renderer);
        if (newTex) {
            texture = newTex.get();
        }
    }
}

void Enemy::Update(Game* game) {
    if (isDead) return;

    // 実行中に動的に速度が変わるようにする場合
    GameParams& params = GameParams::GetInstance();
    moveSpeed = params.enemy.baseSpeed;

    MoveAlongPath();
}

void Enemy::MoveAlongPath() {
    if (goalReached) return;

    if (currentWaypointIndex >= targetPath.size()) {
        goalReached = true;
        isDead = true;
        return;
    }

    SDL_FPoint target = targetPath[currentWaypointIndex];

    float centerX = x + width / 2.0f;
    float centerY = y + height / 2.0f;

    float dirX = target.x - centerX;
    float dirY = target.y - centerY;

    float distance = std::sqrt(dirX * dirX + dirY * dirY);

    if (distance < distanceTolerance) {
        currentWaypointIndex++;
        return;
    }

    float speedPerFrame = moveSpeed * Time::deltaTime;

    velX = (dirX / distance) * speedPerFrame;
    velY = (dirY / distance) * speedPerFrame;

    x += velX;
    y += velY;
}

void Enemy::OnRender(SDL_Renderer* renderer, int drawX, int drawY) {
    SDL_Rect destRect = { drawX, drawY, width, height };

    if (texture) {
        SDL_RenderCopyEx(renderer, texture, NULL, &destRect, angle, NULL, SDL_FLIP_NONE);
    }
    else {
        // テクスチャがない場合のフォールバック（緑の矩形）
        SDL_SetRenderDrawColor(renderer, 0, 150, 0, 255);
        SDL_RenderFillRect(renderer, &destRect);
    }

    // デバッグ情報: HPバーを描画
    int barHeight = 5;
    float hpRatio = (maxHp > 0) ? (float)hp / maxHp : 0;
    SDL_Rect bgRect = { drawX, drawY - barHeight - 2, width, barHeight };
    SDL_Rect hpRect = { drawX, drawY - barHeight - 2, (int)(width * hpRatio), barHeight };

    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderFillRect(renderer, &bgRect);

    SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
    SDL_RenderFillRect(renderer, &hpRect);
}

void Enemy::OnTriggerEnter(GameObject* other) {
    if (other->name == "Bullet") {
        // 本来はBullet側の攻撃力を参照する
        int damage = GameParams::GetInstance().gun.damage;
        TakeDamage(damage);
        other->isDead = true;
    }
}

void Enemy::TakeDamage(int damage) {
    hp -= damage;
    if (hp <= 0) {
        hp = 0;
        isDead = true;
    }
}