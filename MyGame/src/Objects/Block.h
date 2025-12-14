#pragma once
#include "GameObject.h"

class Block : public GameObject {
public:
    Block(float x, float y, int w, int h) : GameObject(x, y, w, h) {
        useGravity = false; // 地面は落ちない
    }

    void Update(Game* game) override {
        // 動かないので何もしない
    }

    void Render(SDL_Renderer* renderer) override {
        // グレーの四角形を描画
        SDL_Rect rect = { (int)x, (int)y, width, height };
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
};