#pragma once
#include "GameObject.h"
#include <SDL.h>
#include <vector>   // std::vector を使うために追加
#include <memory>   // std::unique_ptr を使うために追加

// ★★★ 修正箇所: この行をキーボードで正確に入力し直してください ★★★
#include "../Weapons/WeaponConfig.h" 

class Enemy; // 敵クラスの前方宣言
class Game;

class Turret : public GameObject {
public:
    // WeaponConfig を受け取って初期化する
    Turret(float x, float y, const WeaponConfig& config, SDL_Texture* tex = nullptr);
    virtual ~Turret() {}

    void Update(Game* game) override;
    void OnRender(SDL_Renderer* renderer, int drawX, int drawY) override;
    void OnTriggerEnter(GameObject* other) override {}

private:
    // --- 武器設定と状態 ---
    WeaponConfig weaponConfig;
    float fireCooldown;

    // Turret::OnRender でカメラ情報が必要なため、Turret.h に追加していることを前提
    // Camera* camera; // (GameObjectに無ければTurretに追加が必要)

    // --- ターゲット情報 ---
    Enemy* currentTarget;
    float rotationAngle;

    // --- ロジック ---
    void FindTarget(const std::vector<std::unique_ptr<GameObject>>& gameObjects);
    void RotateTowardTarget(float deltaTime);
    void Fire(Game* game);

    // ヘルパーメソッド
    float GetAngleToTarget() const;
    void SpawnBullet(Game* game, float initialAngle);
};