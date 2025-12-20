#pragma once
#include "GameObject.h"
#include "../Core/Animator.h"
#include <vector>
#include <memory> 
#include <SDL.h>

class Game;
struct SDL_FPoint; // SDLの浮動小数点座標構造体を使用

class Enemy : public GameObject {
public:
    // HP, 速度, 報酬は GameParams::EnemyParams から取得します。
    Enemy(float x, float y, int w, int h, SDL_Texture* tex,
        const std::vector<SDL_FPoint>& path);

    virtual ~Enemy() {}

    void Update(Game* game) override;
    void OnRender(SDL_Renderer* renderer, int drawX, int drawY) override;

    // 設定を最新の GameParams から反映させる
    void RefreshConfig(SDL_Renderer* renderer);

    // --- ゲームロジック用メソッド ---

    void TakeDamage(int damage);
    void OnTriggerEnter(GameObject* other) override;

    // 敵固有の情報取得
    bool IsGoalReached() const { return goalReached; }
    int GetRewardValue() const { return rewardValue; }
    float GetCurrentHP() const { return (float)hp; }

private:
    // --- ステータス ---
    int hp;
    int maxHp;
    float moveSpeed;
    int rewardValue;

    // --- パス追従ロジック ---
    const std::vector<SDL_FPoint> targetPath;
    int currentWaypointIndex;
    float distanceTolerance;
    bool goalReached;

    void MoveAlongPath();

    // --- その他 ---
    std::unique_ptr<Animator> animator;
};