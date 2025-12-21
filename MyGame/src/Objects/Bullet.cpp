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

// 角度指定 (Playerなど)
Bullet::Bullet(float startX, float startY, double angleDegrees, SDL_Texture* tex, BulletSide side)
    : GameObject(startX, startY, 10, 10, tex),
    damageValue(10),
    side(side)
{
    double radian = angleDegrees * M_PI / 180.0;
    float speed = 15.0f;

    this->velX = std::cos(radian) * speed;
    this->velY = std::sin(radian) * speed;
    this->angle = angleDegrees;

    isTrigger = true;
    useGravity = false;

    // デバッグ用に名前は残しておいても良いが、ロジックには使用しない
    name = (side == BulletSide::Player) ? "PlayerBullet" : "EnemyBullet";
}

// 詳細指定 (Turret/Enemyなど)
Bullet::Bullet(float x, float y, int w, int h,
    float velX, float velY,
    int damage,
    SDL_Texture* tex,
    BulletSide side)
    : GameObject(x, y, w, h, tex),
    damageValue(damage),
    side(side)
{
    this->velX = velX;
    this->velY = velY;
    this->angle = std::atan2(velY, velX) * 180.0 / M_PI;

    isTrigger = true;
    useGravity = false;
    name = (side == BulletSide::Player) ? "PlayerBullet" : "EnemyBullet";
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

    // --- 陣営(BulletSide)による条件分岐 ---

    if (this->side == BulletSide::Player) {
        // 【プレイヤー側の弾】

        // 敵への判定
        Enemy* enemy = dynamic_cast<Enemy*>(other);
        if (enemy) {
            enemy->TakeDamage(damageValue);
            isDead = true;
            return;
        }

        // 地形に当たった
        if (other->name == "Editor Ground" || other->name == "Block") {
            isDead = true;
            return;
        }
    }
    else if (this->side == BulletSide::Enemy) {
        // 【エネミー側の弾】

        // プレイヤーへの判定
        Player* player = dynamic_cast<Player*>(other);
        if (player) {
            player->TakeDamage(damageValue);
            isDead = true;
            return;
        }

        // 拠点への判定
        Base* baseObj = dynamic_cast<Base*>(other);
        if (baseObj) {
            // 拠点へのダメージは GameSession 経由で行う
            GameSession::GetInstance().DamageBase(damageValue);
            isDead = true;
            return;
        }

        // 地形に当たった
        if (other->name == "Editor Ground" || other->name == "Block") {
            isDead = true;
            return;
        }
    }
}

void Bullet::OnRender(SDL_Renderer* renderer, int drawX, int drawY) {
    SDL_Rect destRect = { drawX, drawY, width, height };

    if (texture) {
        SDL_RenderCopyEx(renderer, texture, NULL, &destRect, angle, NULL, SDL_FLIP_NONE);
    }
    else {
        // 画像がない場合の色分け（陣営で分ける）
        if (side == BulletSide::Enemy)
            SDL_SetRenderDrawColor(renderer, 255, 100, 0, 255); // エネミー：オレンジ
        else
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // プレイヤー：黄色

        SDL_RenderFillRect(renderer, &destRect);
    }
}