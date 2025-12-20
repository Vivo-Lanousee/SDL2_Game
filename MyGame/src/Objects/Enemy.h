#pragma once
#include "GameObject.h"
#include "../Core/Animator.h"
#include "../TextureManager.h"
#include <vector>
#include <memory> 
#include <SDL.h>

class Game;
struct SDL_FPoint;

/**
 * @brief エネミーの挙動を管理するクラス
 */
class Enemy : public GameObject {
public:
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

    float GetCurrentHP() const { return (float)hp; }

private:
    // --- ステータス ---
    int hp;
    int maxHp;
    int attackPower;
    float moveSpeed;
    float attackRange;
    float attackInterval;
    float attackTimer;

    // --- パス追従ロジック ---
    const std::vector<SDL_FPoint> targetPath;
    int currentWaypointIndex;
    float distanceTolerance;

    // --- AI ステート ---
    bool isAttacking;

    // --- 跳躍移動用 ---
    float jumpTimer;
    float jumpInterval;

    // --- リソース ---
    SharedTexturePtr bulletTexture;

    void MoveLogic();
    void AttackLogic(Game* game);

    // --- その他 ---
    std::unique_ptr<Animator> animator;
};