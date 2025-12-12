#pragma once
#include "GameObject.h"
#include <cmath>

class Bullet : public GameObject {
public:
    Bullet(float startX, float startY, double angleDegrees, SDL_Texture* tex)
        : GameObject(startX, startY, 10, 10, tex) {

        //ラジアンに
        double radian = angleDegrees * 3.14159265 / 180.0;
        float speed = 10.0f;
        velX = cos(radian) * speed;
        velY = sin(radian) * speed;

        this->angle = angleDegrees;
    }

    void Update() override {
        x += velX;
        y += velY;
    }

    // ★変更点4：Render関数を画像対応にする
    void Render(SDL_Renderer* renderer) override {
        SDL_Rect destRect = { (int)x, (int)y, width, height };

        if (texture) {
            SDL_RenderCopyEx(renderer, texture, NULL, &destRect, angle, NULL, SDL_FLIP_NONE);
        }
        else {
            // 画像がない場合黄色い四角に
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_RenderFillRect(renderer, &destRect);
        }
    }

private:
    float velX, velY;
};