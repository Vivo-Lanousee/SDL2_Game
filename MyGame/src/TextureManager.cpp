#include "TextureManager.h"
#include <iostream>

// ★重要：staticメンバ変数の実体をここで定義する
// ヘッダーには「あるよ」と書いただけなので、ここに「箱」を用意する必要があります
std::map<std::string, SharedTexturePtr> TextureManager::textureCache;

SharedTexturePtr TextureManager::LoadTexture(const std::string& fileName, SDL_Renderer* renderer) {
    // 1. まずキャッシュ（辞書）の中を探す
    auto it = textureCache.find(fileName);

    if (it != textureCache.end()) {
        // ★見つかった！
        // すでに読み込まれている shared_ptr を返します（これでメモリ節約！）
        std::cout << "[Cache Hit] Use existing texture: " << fileName << std::endl;
        return it->second;
    }

    // 2. キャッシュになかったので、新しくロードする
    std::cout << "[Load New] Loading texture from disk: " << fileName << std::endl;

    SDL_Surface* tempSurface = IMG_Load(fileName.c_str());
    if (!tempSurface) {
        std::cout << "Failed to load image: " << fileName << std::endl;
        return nullptr; // 失敗したら空のポインタを返す
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    if (tex) {
        // 3. ロード成功。shared_ptr を作成する。
        // ここで「掃除係 (TextureDestroyer)」をセットするのがポイント！
        SharedTexturePtr newPtr(tex, TextureDestroyer());

        // 4. キャッシュに登録する（次からはここから取り出される）
        textureCache[fileName] = newPtr;

        return newPtr;
    }

    return nullptr;
}

void TextureManager::Clean() {
    std::cout << "Clearing texture cache..." << std::endl;
    // キャッシュを空にする
    // 参照カウントが0になったテクスチャは自動的に SDL_DestroyTexture されます
    textureCache.clear();
}