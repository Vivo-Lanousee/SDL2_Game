#include "Bullet.h"
#include "../Core/Game.h"
#include "../Core/Physics.h"
#include "../Core/GameSession.h"
#include "Enemy.h"
#include "Player.h"
#include "Base.h"
#include <cmath>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// -----------------------------------------------------
// コンストラクタ 1: 角度指定版 (Playerなど)
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
    name = "Bullet"; // デフォルトはプレイヤーの弾
}

// -----------------------------------------------------
// コンストラクタ 2: 詳細指定版 (Turret/Enemyなど)
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

    // 画面外判定（広めに設定）
    if (x < -1000 || x > 6000 || y < -1000 || y > 2000) {
        isDead = true;
    }
}

void Bullet::OnTriggerEnter(GameObject* other) {
    if (isDead || other->isDead) return;

    // 1. 自身が「プレイヤーの弾(Bullet)」の場合
    if (this->name == "Bullet") {
        // エネミーに当たった
        if (other->name == "Enemy" || other->name == "Test Enemy") {
            Enemy* enemy = dynamic_cast<Enemy*>(other);
            if (enemy) {
                enemy->TakeDamage(damageValue);
                isDead = true;
            }
        }
        // 地形に当たった
        else if (other->name == "Editor Ground" || other->name == "Block") {
            isDead = true;
        }
    }
    // 2. 自身が「エネミーの弾(EnemyBullet)」の場合
    else if (this->name == "EnemyBullet") {
        // プレイヤーに当たった
        if (other->name == "Player" || other->name == "TestPlayer") {
            Player* player = dynamic_cast<Player*>(other);
            if (player) {
                player->TakeDamage(damageValue);
                isDead = true;
            }
        }
        // 拠点に当たった
        else if (other->name == "Base Gate" || other->name == "Base") {
            // 拠点へのダメージは GameSession 経由で行う
            GameSession::GetInstance().DamageBase(damageValue);
            isDead = true;
        }
        // 地形に当たった
        else if (other->name == "Editor Ground" || other->name == "Block") {
            isDead = true;
        }
    }
}

void Bullet::OnRender(SDL_Renderer* renderer, int drawX, int drawY) {
    SDL_Rect destRect = { drawX, drawY, width, height };

    if (texture) {
        SDL_RenderCopyEx(renderer, texture, NULL, &destRect, angle, NULL, SDL_FLIP_NONE);
    }
    else {
        // 画像がない場合の色分け
        if (name == "EnemyBullet") SDL_SetRenderDrawColor(renderer, 255, 100, 0, 255); // オレンジ
        else SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // 黄色
        SDL_RenderFillRect(renderer, &destRect);
    }
}