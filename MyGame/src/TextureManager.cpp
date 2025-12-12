#include "TextureManager.h"
#include <SDL_image.h>
#include <iostream>

SDL_Texture* TextureManager::LoadTexture(const char* fileName, SDL_Renderer* ren) {
    // 画像をサーフェスとして読み込む
    SDL_Surface* tempSurface = IMG_Load(fileName);

    if (tempSurface == nullptr) {
        std::cout << "Failed to load image: " << fileName << std::endl;
        return nullptr;
    }

    // テクスチャに変換する
    SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, tempSurface);
    SDL_FreeSurface(tempSurface);

    return tex;
}