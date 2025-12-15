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
        this->velX = cos(radian) * speed;
        this->velY = sin(radian) * speed;

        this->angle = angleDegrees;

        // 設定
        isTrigger = true; // 弾はトリガー
        name = "Bullet";  // 名前を設定
    }

    void Update(Game* game) override {
        // 親クラスの変数を使っているのでそのまま移動
        x += velX;
        y += velY;
    }

    // 当たった時の処理
    void OnTriggerEnter(GameObject* other) override {
        // プレイヤー自身には当たらない
        if (other->name == "Player") return;

        // 他の弾やトリガーには当たらない
        if (other->isTrigger) return;

        // それ以外（壁や地面）に当たったら消滅
        isDead = true;
    }

    void OnRender(SDL_Renderer* renderer, int drawX, int drawY) override {

        // (int)x, (int)y の代わりに、渡された drawX, drawY を使います
        SDL_Rect destRect = { drawX, drawY, width, height };

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