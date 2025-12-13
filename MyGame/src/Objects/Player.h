#pragma once
#include "GameObject.h"
#include <cmath> 

/// <summary>
/// プレイヤーのオブジェクト情報
/// </summary>
class Player : public GameObject {
public:
    Player(float x, float y, SDL_Texture* tex) : GameObject(x, y, 46, 128, tex) {}


    void Update() override {
        // キー入力状態を取得
        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_UP])    y -= 5;
        if (state[SDL_SCANCODE_DOWN])  y += 5;
        if (state[SDL_SCANCODE_LEFT])  x -= 5;
        if (state[SDL_SCANCODE_RIGHT]) x += 5;

        // マウスの方向を向く計算
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        float centerX = x + width / 2;
        float centerY = y + height / 2;
        angle = atan2(mouseY - centerY, mouseX - centerX) * 180 / M_PI;
    }

    

    void Render(SDL_Renderer* renderer) override {
        SDL_Rect destRect = { (int)x, (int)y, width, height };

        if (texture) {
            SDL_RenderCopyEx(renderer, texture, NULL, &destRect, angle, NULL, SDL_FLIP_NONE);
        }
        else {
            // 画像がないなら赤い四角
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(renderer, &destRect);
        }

        // レーザー（緑）
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        SDL_RenderDrawLine(renderer, (int)(x + width / 2), (int)(y + height / 2), mouseX, mouseY);
    }

private:
    double angle = 0;
};