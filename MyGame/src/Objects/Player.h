#pragma once
#include "GameObject.h"
#include "../Core/Timer.h"
#include "../Core/Animator.h"
#include "../TextureManager.h"
#include <memory>

// 前方宣言
class Game;
class Camera;
class Bullet;
struct SDL_Texture;

struct UnitStatus {
    int hp;
    int maxHp;
    float speed;
    int attackPower;
    float fireInterval;
};

class Player : public GameObject {
public:
    Player(float x, float y, SDL_Texture* tex, SDL_Texture* bulletTex, Camera* cam);

    void Update(Game* game) override;
    void OnRender(SDL_Renderer* renderer, int drawX, int drawY) override;

    void TakeDamage(int damage);
    int GetHP() const { return (int)currentHealth; }
    int GetMaxHP() const;

    // 現在のGameParamsに合わせて銃の見た目を更新する
    void RefreshGunTexture(SDL_Renderer* renderer);

public:
    UnitStatus status;

private:
    // unique_ptr<Bullet> を返すように変更
    std::unique_ptr<Bullet> Shoot(float targetX, float targetY, SDL_Texture* bulletTex);

    double angle;
    SDL_Texture* bulletTexture; // 弾のテクスチャ
    SharedTexturePtr gunTexture; // 銃本体のテクスチャ
    Camera* camera;

    float fireCooldown; // 連射管理用タイマー
    float currentHealth;

    std::unique_ptr<Animator> animator;

    bool isFlipLeft = false;
};