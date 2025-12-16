#include "Enemy.h"
#include "../Core/Game.h"
#include "../Core/Time.h"
#include "../Core/Physics.h"
#include "../Core/GameParams.h" 
#include "Bullet.h"
#include <cmath>
#include <iostream>
#include <algorithm> 

const float DEFAULT_DISTANCE_TOLERANCE = 5.0f;

// ★★★ コンストラクタ: Enemy.hでhp, maxHp, moveSpeedが定義されている前提 ★★★
Enemy::Enemy(float x, float y, int w, int h, SDL_Texture* tex,
    const std::vector<SDL_FPoint>& path)
    : GameObject(x, y, w, h, tex),
    targetPath(path), currentWaypointIndex(0),
    distanceTolerance(DEFAULT_DISTANCE_TOLERANCE), goalReached(false)
{
    GameParams& params = GameParams::GetInstance();

    hp = params.enemy.baseHealth;
    maxHp = params.enemy.baseHealth;
    moveSpeed = params.enemy.baseSpeed;

    rewardValue = 10;

    name = "Enemy";
    isTrigger = false;
    useGravity = false;

    // ... (アニメーターの初期化) ...
}

void Enemy::Update(Game* game) {
    if (isDead) return;

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
        SDL_SetRenderDrawColor(renderer, 0, 150, 0, 255);
        SDL_RenderFillRect(renderer, &destRect);
    }

    // デバッグ情報: HPバーを描画
    int barHeight = 5;
    // ★★★ 修正箇所: hpとmaxHpが int 型であることを前提にキャストを削除 ★★★
    float hpRatio = (float)hp / maxHp;
    SDL_Rect bgRect = { drawX, drawY - barHeight - 2, width, barHeight };
    SDL_Rect hpRect = { drawX, drawY - barHeight - 2, (int)(width * hpRatio), barHeight };

    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderFillRect(renderer, &bgRect);

    SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
    SDL_RenderFillRect(renderer, &hpRect);
}

void Enemy::OnTriggerEnter(GameObject* other) {
    if (other->name == "Bullet") {
        int damage = 10;
        TakeDamage(damage);
        other->isDead = true;
    }
}

// ★★★ 修正箇所: TakeDamage関数は問題ない構文だが、念の為確認 ★★★
void Enemy::TakeDamage(int damage) {
    hp -= damage;
    if (hp <= 0) {
        hp = 0;
        isDead = true;
    }
}
// ★★★ 97行目付近に';'が欠けている場合、ここか直前の行で何かが閉じられていない可能性が高いです。 ★★★