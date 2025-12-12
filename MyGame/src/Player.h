#pragma once
#include "GameObject.h"
#include <cmath> 

class Player : public GameObject {
public:
    // 親クラスのコンストラクタを呼ぶ
    Player(float x, float y) : GameObject(x, y, 50, 50) {}

    // 親との約束「Update」の中身を書く
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
        // 本体（赤）
        SDL_Rect rect = { (int)x, (int)y, width, height };
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);

        // レーザー（緑）
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        SDL_RenderDrawLine(renderer, (int)(x + width / 2), (int)(y + height / 2), mouseX, mouseY);
    }

private:
    double angle = 0;
};