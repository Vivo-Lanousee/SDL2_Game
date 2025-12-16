#include "Bullet.h"
#include "../Core/Game.h"
#include "../Core/Physics.h" // M_PI, std::atan2 のために必要
#include "Enemy.h" // Enemy クラスにアクセスするため
#include <cmath>
#include <iostream>

// -----------------------------------------------------
// コンストラクタ 1: 4引数版 (Playerなどレガシーコード用)
// -----------------------------------------------------
Bullet::Bullet(float startX, float startY, double angleDegrees, SDL_Texture* tex)
// 既存の Player の弾丸サイズ (10x10) とデフォルトダメージ (10) を設定
    : GameObject(startX, startY, 10, 10, tex),
    damageValue(10) // ★修正: デフォルトのダメージ値を設定
{
    // ラジアンに変換
    double radian = angleDegrees * M_PI / 180.0;
    float speed = 10.0f;

    // 親クラス(GameObject)の変数 velX, velY を使う
    this->velX = std::cos(radian) * speed;
    this->velY = std::sin(radian) * speed;

    this->angle = angleDegrees;

    // 設定
    isTrigger = true;
    name = "Bullet";
}

// -----------------------------------------------------
// コンストラクタ 2: 8引数版 (Turretシステム用)
// -----------------------------------------------------
Bullet::Bullet(float x, float y, int w, int h,
    float velX, float velY,
    int damage,
    SDL_Texture* tex)
    : GameObject(x, y, w, h, tex),
    damageValue(damage) // ★修正: ダメージ値をメンバに格納
{
    this->velX = velX;
    this->velY = velY;

    this->angle = std::atan2(velY, velX) * 180.0 / M_PI;

    isTrigger = true;
    useGravity = false;
    name = "Bullet";
}

// -----------------------------------------------------
// Update
// -----------------------------------------------------

void Bullet::Update(Game* game) {
    // 速度を適用して移動
    x += velX;
    y += velY;
}

// -----------------------------------------------------
// 衝突処理
// -----------------------------------------------------

void Bullet::OnTriggerEnter(GameObject* other) {
    // プレイヤー自身や他のトリガーには当たらない
    if (other->name == "Player" || other->isTrigger) return;

    // 弾が消滅
    isDead = true;

    // Enemyに当たった場合
    Enemy* enemy = dynamic_cast<Enemy*>(other);
    if (enemy) {
        // Enemyの TakeDamage メソッドを呼び出す
        enemy->TakeDamage(damageValue);
    }
    // Blockなどのソリッドオブジェクトに当たった場合も isDead = true で消滅する
}

// -----------------------------------------------------
// Render
// -----------------------------------------------------

void Bullet::OnRender(SDL_Renderer* renderer, int drawX, int drawY) {
    SDL_Rect destRect = { drawX, drawY, width, height };

    if (texture) {
        SDL_RenderCopyEx(renderer, texture, NULL, &destRect, angle, NULL, SDL_FLIP_NONE);
    }
    else {
        // 画像がない場合黄色い四角に
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderFillRect(renderer, &destRect);
    }
}