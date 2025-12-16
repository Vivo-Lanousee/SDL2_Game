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
    // ★★★ 修正箇所 1: コンストラクタから speed, reward の引数を削除 ★★★
    // HP, 速度, 報酬は GameParams::EnemyParams から取得します。
    Enemy(float x, float y, int w, int h, SDL_Texture* tex,
        const std::vector<SDL_FPoint>& path);

    virtual ~Enemy() {}

    void Update(Game* game) override;
    void OnRender(SDL_Renderer* renderer, int drawX, int drawY) override;

    // --- ゲームロジック用メソッド ---

    void TakeDamage(int damage);
    void OnTriggerEnter(GameObject* other) override;

    // 敵固有の情報取得
    bool IsGoalReached() const { return goalReached; }
    // ★★★ 修正箇所 2: rewardValue をメンバ変数として維持 (GameParams にないので) ★★★
    int GetRewardValue() const { return rewardValue; }
    float GetCurrentHP() const { return (float)hp; } // HPはintですが、念の為floatで返す

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