#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <memory> // ★必須

// ★追加：SDL_Texture を捨てるための「掃除係」構造体
struct TextureDestroyer {
    void operator()(SDL_Texture* t) const {
        if (t) SDL_DestroyTexture(t);
    }
};

// ★追加：掃除係付きのスマートポインタ型を定義
// これで "TexturePtr" と書くだけで、自動で DestroyTexture してくれるポインタになります
using TexturePtr = std::unique_ptr<SDL_Texture, TextureDestroyer>;

class TextureManager {
public:
    static SDL_Texture* LoadTexture(const char* fileName, SDL_Renderer* renderer);
};