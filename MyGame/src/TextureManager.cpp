#include "TextureManager.h"
#include <iostream>

std::map<std::string, SharedTexturePtr> TextureManager::textureCache;

SharedTexturePtr TextureManager::LoadTexture(const std::string& fileName, SDL_Renderer* renderer) {
    auto it = textureCache.find(fileName);

    if (it != textureCache.end()) {
        std::cout << "[Cache Hit] Use existing texture: " << fileName << std::endl;
        return it->second;
    }

    //キャッシュになかったので、新しくロードする
    std::cout << "[Load New] Loading texture from disk: " << fileName << std::endl;

    SDL_Surface* tempSurface = IMG_Load(fileName.c_str());
    if (!tempSurface) {
        std::cout << "Failed to load image: " << fileName << std::endl;
        return nullptr; 
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    if (tex) {
        SharedTexturePtr newPtr(tex, TextureDestroyer());
        textureCache[fileName] = newPtr;

        return newPtr;
    }

    return nullptr;
}

void TextureManager::Clean() {
    std::cout << "Clearing texture cache..." << std::endl;
    // キャッシュを空にする
    textureCache.clear();
}