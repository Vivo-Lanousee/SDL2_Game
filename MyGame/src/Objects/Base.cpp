#include "Base.h"
#include "../Core/Game.h"
#include "../Core/GameParams.h"
#include "../Core/GameSession.h"
#include "../TextureManager.h"
#include <iostream>

Base::Base(float x, float y, int w, int h, SDL_Texture* tex)
    : GameObject(x, y, w, h, tex), damageFlashTimer(0.0f)
{
    name = "Base";
    isTrigger = false;
    useGravity = false;
}

void Base::RefreshConfig(SDL_Renderer* renderer) {
    GameParams& params = GameParams::GetInstance();

    // 8. 画像パスに基づいたテクスチャ読み込み
    if (renderer && !params.base.texturePath.empty()) {
        SharedTexturePtr newTex = TextureManager::LoadTexture(params.base.texturePath, renderer);
        if (newTex) {
            texture = newTex.get();
        }
    }
}

void Base::Update(Game* game) {
    if (damageFlashTimer > 0) {
        damageFlashTimer -= 0.1f; // 簡易的な被弾演出タイマー
    }
}

void Base::OnRender(SDL_Renderer* renderer, int drawX, int drawY) {
    SDL_Rect destRect = { drawX, drawY, width, height };

    // ダメージを受けた時に少し赤くする演出（簡易版）
    if (damageFlashTimer > 0) {
        SDL_SetTextureColorMod(texture, 255, 100, 100);
    }
    else {
        SDL_SetTextureColorMod(texture, 255, 255, 255);
    }

    if (texture) {
        SDL_RenderCopyEx(renderer, texture, NULL, &destRect, angle, NULL, SDL_FLIP_NONE);
    }
    else {
        // テクスチャがない場合は頑丈そうな鉄扉色
        SDL_SetRenderDrawColor(renderer, 80, 80, 90, 255);
        SDL_RenderFillRect(renderer, &destRect);
    }

    // --- 拠点HPバーの描画 (マルフーシャ風に拠点直上に表示する場合) ---
    GameSession& session = GameSession::GetInstance();
    float hpRatio = (session.maxBaseHP > 0) ? (float)session.currentBaseHP / session.maxBaseHP : 0;

    int barW = width;
    int barH = 8;
    SDL_Rect bg = { drawX, drawY - 20, barW, barH };
    SDL_Rect fg = { drawX, drawY - 20, (int)(barW * hpRatio), barH };

    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderFillRect(renderer, &bg);

    // HP量に応じて色を変える (緑 -> 黄 -> 赤)
    if (hpRatio > 0.5f) SDL_SetRenderDrawColor(renderer, 0, 255, 120, 255);
    else if (hpRatio > 0.2f) SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);
    else SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255);

    SDL_RenderFillRect(renderer, &fg);
}