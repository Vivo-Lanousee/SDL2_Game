#pragma once
#include "GameObject.h"
#include <cmath>
#include <SDL.h>

class Game;
class Enemy;

/**
 * @brief 弾丸クラス
 */
class Bullet : public GameObject {
public:
    // コンストラクタ 1: 角度指定版
    Bullet(float startX, float startY, double angleDegrees, SDL_Texture* tex);

    // コンストラクタ 2: 詳細指定版
    Bullet(float x, float y, int w, int h,
        float velX, float velY,
        int damage,
        SDL_Texture* tex);

    virtual ~Bullet() {}

    void Update(Game* game) override;
    void OnTriggerEnter(GameObject* other) override;
    void OnRender(SDL_Renderer* renderer, int drawX, int drawY) override;

    int GetDamage() const { return damageValue; }

private:
    int damageValue;
};