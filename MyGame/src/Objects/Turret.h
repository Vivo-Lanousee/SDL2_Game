#pragma once
#include "GameObject.h"
#include <SDL.h>
#include <vector>
#include <memory>
#include <string>

class Enemy;
class Game;

// 武器の設定用構造体
struct WeaponConfig {
    std::string name;
    float fireRate;      // 発射速度（秒間）
    float bulletSpeed;
    int damage;
    float range;
    float spreadAngle;
    int bulletWidth;
    int bulletHeight;
    int magazineSize;
    float reloadTime;
};

class Turret : public GameObject {
public:
    Turret(float x, float y, const WeaponConfig& config, SDL_Texture* tex = nullptr);
    virtual ~Turret() {}

    void Update(Game* game) override;
    void OnRender(SDL_Renderer* renderer, int drawX, int drawY) override;
    void OnTriggerEnter(GameObject* other) override {}

private:
    WeaponConfig weaponConfig;
    float fireCooldown;

    Enemy* currentTarget;
    float rotationAngle;

    int currentAmmo;
    float reloadTimer;
    bool isReloading;

    void FindTarget(const std::vector<std::unique_ptr<GameObject>>& gameObjects);
    void RotateTowardTarget(float deltaTime);
    void Fire(Game* game);

    float GetAngleToTarget() const;
    void SpawnBullet(Game* game, float initialAngle);
};