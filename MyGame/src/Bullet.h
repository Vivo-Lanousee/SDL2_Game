#pragma once
#include "GameObject.h"
#include <cmath> // 計算用

class Bullet : public GameObject {
public:
    // 作られる時に「角度(degree)」を受け取る
    Bullet(float startX, float startY, double angleDegrees)
        : GameObject(startX, startY, 10, 10) { // サイズは10x10

        // 角度をラジアン（数学用の単位）に変換
        double radian = angleDegrees * 3.14159265 / 180.0;

        // 三角関数(cos, sin)で「進むべき速度」を計算
        float speed = 10.0f; // 弾の速さ
        velX = cos(radian) * speed;
        velY = sin(radian) * speed;
    }

    void Update() override {
        // 毎フレーム移動
        x += velX;
        y += velY;
    }

    void Render(SDL_Renderer* renderer) override {
        // 黄色で描画
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_Rect rect = { (int)x, (int)y, width, height };
        SDL_RenderFillRect(renderer, &rect);
    }

private:
    float velX, velY;
};