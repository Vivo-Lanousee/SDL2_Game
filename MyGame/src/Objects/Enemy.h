#pragma once
#include "GameObject.h"
#include "../Core/Animator.h"
#include "../TextureManager.h"
#include <vector>
#include <memory> 
#include <SDL.h>

class Game;
struct SDL_FPoint;

class Enemy : public GameObject {
public:
    // パスの引数は削除せず、互換性のために残していますが内部では無視します
    Enemy(float x, float y, int w, int h, SDL_Texture* tex,
        const std::vector<SDL_FPoint>& path);

    virtual ~Enemy() {}
    void Update(Game* game) override;
    void OnRender(SDL_Renderer* renderer, int drawX, int drawY) override;
    void RefreshConfig(SDL_Renderer* renderer);
    void TakeDamage(int damage);
    void OnTriggerEnter(GameObject* other) override;

    float GetCurrentHP() const { return (float)hp; }

private:
    int hp;
    int maxHp;
    int attackPower;
    float moveSpeed;
    float attackRange;
    float attackInterval;
    float attackTimer;

    // AI状態
    bool isAttacking;

    // 跳躍移動用タイマー
    float jumpTimer;
    float jumpInterval;

    // リソース
    SharedTexturePtr bulletTexture;

    void MoveLogic();
    void AttackLogic(Game* game);

    std::unique_ptr<Animator> animator;
};