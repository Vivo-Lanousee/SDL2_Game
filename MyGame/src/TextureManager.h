#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <memory>
#include <string>
#include <map>

// テクスチャ削除用の関数オブジェクト（これはそのまま）
struct TextureDestroyer {
    void operator()(SDL_Texture* t) const {
        if (t) SDL_DestroyTexture(t);
    }
};


using SharedTexturePtr = std::shared_ptr<SDL_Texture>;

class TextureManager {
public:
    static SharedTexturePtr LoadTexture(const std::string& fileName, SDL_Renderer* renderer);

    // ゲーム終了時にキャッシュを空にする関数
    static void Clean();

private:
    static std::map<std::string, SharedTexturePtr> textureCache;
};