#include "Bullet.h"
#include "../Core/Game.h"
#include "../Core/Physics.h"
#include "Enemy.h"
#include <cmath>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// -----------------------------------------------------
// コンストラクタ 1: 角度指定版
// -----------------------------------------------------
Bullet::Bullet(float startX, float startY, double angleDegrees, SDL_Texture* tex)
    : GameObject(startX, startY, 10, 10, tex),
    damageValue(10)
{
    double radian = angleDegrees * M_PI / 180.0;
    float speed = 15.0f;

    this->velX = std::cos(radian) * speed;
    this->velY = std::sin(radian) * speed;
    this->angle = angleDegrees;

    isTrigger = true;
    useGravity = false;
    name = "Bullet";
}

// -----------------------------------------------------
// コンストラクタ 2: 詳細指定版
// -----------------------------------------------------
Bullet::Bullet(float x, float y, int w, int h,
    float velX, float velY,
    int damage,
    SDL_Texture* tex)
    : GameObject(x, y, w, h, tex),
    damageValue(damage)
{
    this->velX = velX;
    this->velY = velY;
    this->angle = std::atan2(velY, velX) * 180.0 / M_PI;

    isTrigger = true;
    useGravity = false;
    name = "Bullet";
}

void Bullet::Update(Game* game) {
    if (isDead) return;

    x += velX;
    y += velY;

    // 画面外判定
    if (x < -500 || x > 5000 || y < -500 || y > 5000) {
        isDead = true;
    }
}

void Bullet::OnTriggerEnter(GameObject* other) {
    if (isDead) return;

    // トリガーや死んでいるオブジェクトには当たらない
    if (other->isTrigger || other->isDead) {
        return;
    }

    // プレイヤーが発射した弾がプレイヤー自身に当たるのを防ぐ
    if (other->name == "Player" || other->name == "TestPlayer") {
        return;
    }

    // 敵に当たった場合
    if (other->name == "Enemy" || other->name == "Test Enemy") {
        Enemy* enemy = dynamic_cast<Enemy*>(other);
        if (enemy) {
            enemy->TakeDamage(damageValue);
            isDead = true;
        }
    }
    // 壁などに当たった場合
    else if (other->name == "Block" || other->name == "Editor Ground") {
        isDead = true;
    }
}

void Bullet::OnRender(SDL_Renderer* renderer, int drawX, int drawY) {
    SDL_Rect destRect = { drawX, drawY, width, height };

    if (texture) {
        SDL_RenderCopyEx(renderer, texture, NULL, &destRect, angle, NULL, SDL_FLIP_NONE);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderFillRect(renderer, &destRect);
    }
}