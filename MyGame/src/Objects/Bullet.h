#pragma once
#include "GameObject.h"
#include <cmath>
#include <SDL.h> // SDL_Texture のために必要

class Game; // Game クラスの前方宣言
class Enemy;

class Bullet : public GameObject {
public:
    // ★★★ 修正箇所1: 4引数コンストラクタ (Playerなどレガシーコード用) を復活 ★★★
    Bullet(float startX, float startY, double angleDegrees, SDL_Texture* tex);

    // ★★★ 修正箇所2: 8引数コンストラクタ (Turretシステム用) ★★★
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