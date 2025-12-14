#pragma once
#include "GameObject.h"
#include <cmath>

class Bullet : public GameObject {
public:
    Bullet(float startX, float startY, double angleDegrees, SDL_Texture* tex)
        : GameObject(startX, startY, 10, 10, tex) {

        // ラジアンに変換
        double radian = angleDegrees * 3.14159265 / 180.0;
        float speed = 10.0f;

        // 親クラス(GameObject)の変数 velX, velY を使う
        // （private変数は削除しました）
        this->velX = cos(radian) * speed;
        this->velY = sin(radian) * speed;

        this->angle = angleDegrees;

        // ★設定
        isTrigger = true; // 弾はトリガー（物理演算ですり抜ける）
        name = "Bullet";  // 名前を設定（Playerが識別できるようにする）
    }

    void Update(Game* game) override {
        // 親クラスの変数を使っているのでそのまま移動
        x += velX;
        y += velY;
    }

    // ★追加：当たった時の処理（PlaySceneから呼ばれる）
    void OnTriggerEnter(GameObject* other) override {
        // 1. プレイヤー自身には当たらない（発射した瞬間の自爆防止）
        if (other->name == "Player") return;

        // 2. 他の弾やトリガー（アイテム等）には当たらない
        if (other->isTrigger) return;

        // 3. それ以外（壁や地面）に当たったら消滅フラグを立てる
        isDead = true;
    }

    // Render関数
    void Render(SDL_Renderer* renderer) override {
        SDL_Rect destRect = { (int)x, (int)y, width, height };

        // テクスチャ描画
        if (texture) {
            SDL_RenderCopyEx(renderer, texture, NULL, &destRect, angle, NULL, SDL_FLIP_NONE);
        }
        else {
            // 画像がない場合黄色い四角に
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_RenderFillRect(renderer, &destRect);
        }
    }
};