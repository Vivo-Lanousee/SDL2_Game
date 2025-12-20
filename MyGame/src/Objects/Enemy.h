#pragma once
#include "GameObject.h"
#include "../Core/Animator.h"
#include <vector>
#include <memory> 
#include <SDL.h>

class Game;
struct SDL_FPoint;

/**
 * @brief エネミーの挙動を管理するクラス
 * 設定された8つのパラメータに基づき、進軍と攻撃を行う
 */
class Enemy : public GameObject {
public:
    Enemy(float x, float y, int w, int h, SDL_Texture* tex,
        const std::vector<SDL_FPoint>& path);

    virtual ~Enemy() {}

    void Update(Game* game) override;
    void OnRender(SDL_Renderer* renderer, int drawX, int drawY) override;

    // 設定を最新の GameParams から反映させる（画像パスの更新含む）
    void RefreshConfig(SDL_Renderer* renderer);

    // --- ゲームロジック用メソッド ---
    void TakeDamage(int damage);
    void OnTriggerEnter(GameObject* other) override;

    float GetCurrentHP() const { return (float)hp; }

private:
    // --- 1-5. ステータス (数値系) ---
    int hp;
    int maxHp;
    int attackPower;
    float moveSpeed;
    float attackRange;
    float attackInterval; // 攻撃の速さ
    float attackTimer;

    // --- 経路情報 ---
    const std::vector<SDL_FPoint> targetPath;
    int currentWaypointIndex;
    float distanceTolerance;

    // --- 6-7. AI ステート (移動・攻撃方法) ---
    bool isAttacking;

    void MoveLogic();
    void AttackLogic(Game* game);

    // --- 8. その他 (外見・アニメーション) ---
    std::unique_ptr<Animator> animator;
};