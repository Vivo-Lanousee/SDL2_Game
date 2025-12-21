#include "Enemy.h"
#include "../Core/Game.h"
#include "../Core/Time.h"
#include "../Core/Physics.h"
#include "../Core/GameParams.h" 
#include "../Core/GameSession.h" 
#include "../TextureManager.h"
#include "Bullet.h"
#include "Block.h" 
#include <cmath>
#include <iostream>
#include <algorithm> 

Enemy::Enemy(float x, float y, int w, int h, SDL_Texture* tex,
    const std::vector<SDL_FPoint>& path)
    : GameObject(x, y, w, h, tex),
    isAttacking(false), attackTimer(0.0f),
    jumpTimer(0.0f), jumpInterval(1.5f)
{
    // 初期化時は仮のサイズ（w, h）が入るが、RefreshConfig で画像サイズに上書きされる
    RefreshConfig(nullptr);
    this->name = "Enemy";
    this->isTrigger = true;
}

void Enemy::RefreshConfig(SDL_Renderer* renderer) {
    GameParams& params = GameParams::GetInstance();
    hp = params.enemy.baseHealth;
    maxHp = params.enemy.baseHealth;
    moveSpeed = params.enemy.baseSpeed;
    attackPower = params.enemy.attackPower;
    attackRange = params.enemy.attackRange;
    attackInterval = params.enemy.attackInterval;

    if (params.enemy.locomotionStyle == LocomotionType::Flying) {
        useGravity = false;
        velY = 0;
    }
    else {
        useGravity = true;
    }

    if (renderer) {
        // 画像のロード
        if (!params.enemy.texturePath.empty()) {
            SharedTexturePtr newTex = TextureManager::LoadTexture(params.enemy.texturePath, renderer);
            if (newTex) {
                texture = newTex.get();

                // 【追加】テクスチャの実際のサイズを取得して反映
                int realW, realH;
                if (SDL_QueryTexture(texture, NULL, NULL, &realW, &realH) == 0) {
                    this->width = realW;
                    this->height = realH;
                }
            }
        }

        if (!params.enemy.bulletTexturePath.empty()) {
            bulletTexture = TextureManager::LoadTexture(params.enemy.bulletTexturePath, renderer);
        }
    }
}

void Enemy::Update(Game* game) {
    if (isDead) return;

    // 拠点（Base Gate）のX座標
    float targetX = 150.0f;

    // 拠点までの距離（X軸のみで判定）
    float distToTarget = std::abs((x + width / 2.0f) - targetX);

    // 射程内に入ったら攻撃、そうでなければ移動
    if (distToTarget <= attackRange) {
        if (!isAttacking) {
            isAttacking = true;
            velX = 0;
            velY = 0;
            attackTimer = attackInterval;
        }
        AttackLogic(game);
    }
    else {
        isAttacking = false;
        MoveLogic();
    }
}

void Enemy::MoveLogic() {
    GameParams& params = GameParams::GetInstance();
    float dt = Time::deltaTime;
    float targetX = 150.0f;
    float targetY = 450.0f; // 飛行型が目指す高さ

    if (params.enemy.locomotionStyle == LocomotionType::Jumping) {
        if (isGrounded) {
            jumpTimer += dt;
            if (jumpTimer >= jumpInterval) {
                jumpTimer = 0;
                velY = -350.0f; // 上に跳ねる
                velX = -moveSpeed; // 左に進む
                isGrounded = false;
            }
            else {
                velX = 0;
            }
        }
        return;
    }

    if (params.enemy.locomotionStyle == LocomotionType::Flying) {
        float dx = targetX - (x + width / 2.0f);
        float dy = targetY - (y + height / 2.0f);
        float dist = std::sqrt(dx * dx + dy * dy);

        if (dist > 5.0f) {
            x += (dx / dist) * moveSpeed * dt;
            y += (dy / dist) * moveSpeed * dt;
        }
        return;
    }

    if (x > targetX) {
        x -= moveSpeed * dt;
        if (x < targetX) x = targetX;
    }
}

void Enemy::AttackLogic(Game* game) {
    attackTimer += Time::deltaTime;
    if (attackTimer >= attackInterval) {
        attackTimer = 0.0f;
        GameParams& params = GameParams::GetInstance();
        GameSession& session = GameSession::GetInstance();

        switch (params.enemy.attackMethod) {
        case AttackType::Melee:
            session.DamageBase(attackPower);
            break;
        case AttackType::Ranged:
        {
            float spawnX = x - 10.0f;
            float spawnY = y + height / 2.0f;
            auto bullet = std::make_unique<Bullet>(spawnX, spawnY, 180.0, bulletTexture ? bulletTexture.get() : nullptr, BulletSide::Enemy);
            bullet->name = "EnemyBullet";
            game->Instantiate(std::move(bullet));
        }
        break;
        case AttackType::Kamikaze:
            session.DamageBase(attackPower * 5);
            isDead = true;
            break;
        }
    }
}

void Enemy::OnRender(SDL_Renderer* renderer, int drawX, int drawY) {
    SDL_Rect destRect = { drawX, drawY, width, height };
    if (texture) {
        SDL_RenderCopyEx(renderer, texture, NULL, &destRect, angle, NULL, SDL_FLIP_NONE);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 220, 50, 50, 255);
        SDL_RenderFillRect(renderer, &destRect);
    }

    // HPバーの表示
    int barH = 4;
    float hpRatio = (maxHp > 0) ? (float)hp / maxHp : 0;
    SDL_Rect bg = { drawX, drawY - 10, width, barH };
    SDL_Rect fg = { drawX, drawY - 10, (int)(width * hpRatio), barH };
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderFillRect(renderer, &bg);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &fg);
}

void Enemy::OnTriggerEnter(GameObject* other) {
    if (isDead || other->isDead) return;

    if (other->name == "Block" || other->name == "Editor Ground") {
        isGrounded = true;
        velY = 0;
    }
}

void Enemy::TakeDamage(int damage) {
    if (isDead) return;
    hp -= damage;
    if (hp <= 0) {
        hp = 0;
        isDead = true;
    }
}