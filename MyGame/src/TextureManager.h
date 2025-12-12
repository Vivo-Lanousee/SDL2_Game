#pragma once
#include "Game.h" // SDLやSDL_imageのためにインクルード

class TextureManager {
public:
    // ファイルパスとレンダラーを渡すと、テクスチャを返してくれる便利関数
    static SDL_Texture* LoadTexture(const char* fileName, SDL_Renderer* ren);
};