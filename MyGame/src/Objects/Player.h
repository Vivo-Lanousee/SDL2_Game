#pragma once
#include "GameObject.h"
#include "../Core/Timer.h"
#include "../Core/Animator.h"
#include "../TextureManager.h"
#include <memory>
#include <string>

// 前方宣言
class Game;
class Camera;
class Bullet;
struct SDL_Texture;

// 共通のスマートポインタ定義
using SharedTexturePtr = std::shared_ptr<SDL_Texture>;

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

    // エディタで画像が変更された際などに呼び出してテクスチャを再ロードする
    void RefreshGunConfig(SDL_Renderer* renderer);

    // ★ EditorSceneから情報を取得するための関数を追加
    int GetCurrentAmmo() const { return currentAmmo; }
    bool GetIsReloading() const { return isReloading; }

public:
    UnitStatus status;

private:
    // 弾を生成して返す内部関数
    std::unique_ptr<Bullet> Shoot(float targetX, float targetY, SDL_Texture* bulletTex);

    double angle;
    SDL_Texture* bulletTexture;  // 弾のテクスチャ
    SharedTexturePtr gunTexture; // 銃本体のテクスチャ
    Camera* camera;

    float fireCooldown;          // 連射間隔の管理用
    float currentHealth;

    // リロード管理
    int currentAmmo;
    float reloadTimer;
    bool isReloading;

    std::unique_ptr<Animator> animator;

    bool isFlipLeft = false;
};