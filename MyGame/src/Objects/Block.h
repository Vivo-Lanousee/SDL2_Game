#pragma once
#include "GameObject.h"

class Block : public GameObject {
public:
    Block(float x, float y, int w, int h) : GameObject(x, y, w, h) {
        useGravity = false; // 地面は落ちない
        name = "Block";     
    }

    void Update(Game* game) override {
    }

    void OnRender(SDL_Renderer* renderer, int drawX, int drawY) override {

        SDL_Rect rect = { drawX, drawY, width, height };

        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
};