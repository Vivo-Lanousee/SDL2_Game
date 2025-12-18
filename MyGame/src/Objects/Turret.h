#pragma once
#include "GameObject.h"
#include <SDL.h>
#include <vector>
#include <memory>
#include "../Weapons/WeaponConfig.h" 

class Enemy;
class Game;

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

    void FindTarget(const std::vector<std::unique_ptr<GameObject>>& gameObjects);
    void RotateTowardTarget(float deltaTime);
    void Fire(Game* game);

    float GetAngleToTarget() const;
    void SpawnBullet(Game* game, float initialAngle);
};