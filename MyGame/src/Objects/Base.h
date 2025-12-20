#pragma once
#include "GameObject.h"
#include <SDL.h>

// 防衛対象となる拠点のクラス
class Base : public GameObject {
public:
    Base(float x, float y, int w, int h, SDL_Texture* tex = nullptr);
    virtual ~Base() {}

    void Update(Game* game) override;

    // 設定の反映（画像の再読み込みなど）
    void RefreshConfig(SDL_Renderer* renderer);

protected:
    void OnRender(SDL_Renderer* renderer, int drawX, int drawY) override;

private:
    // 内部的な演出用（ダメージを受けた時のフラッシュなど）
    float damageFlashTimer;
};