#pragma once
#include "GameObject.h"
#include <cmath>
#include <SDL.h> // SDL_Texture のために必要

class Game; // Game クラスの前方宣言
class Enemy;

class Bullet : public GameObject {
public:
    Bullet(float startX, float startY, double angleDegrees, SDL_Texture* tex);

    Bullet(float x, float y, int w, int h,
        float velX, float velY,
        int damage,
        SDL_Texture* tex);

    virtual ~Bullet() {}

    void Update(Game* game) override;

    void OnTriggerEnter(GameObject* other) override;

    void OnRender(SDL_Renderer* renderer, int drawX, int drawY) override;

    // 弾のダメージを取得できるようにする
    int GetDamage() const { return damageValue; }

private:
    int damageValue; // 弾丸の攻撃力
};