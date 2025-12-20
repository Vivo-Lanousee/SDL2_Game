#include "Enemy.h"
#include "../Core/Game.h"
#include "../Core/Time.h"
#include "../Core/Physics.h"
#include "../Core/GameParams.h" 
#include "../Core/GameSession.h" 
#include "../TextureManager.h"
#include "Bullet.h"
#include <cmath>
#include <iostream>
#include <algorithm> 

Enemy::Enemy(float x, float y, int w, int h, SDL_Texture* tex,
    const std::vector<SDL_FPoint>& path)
    : GameObject(x, y, w, h, tex),
    targetPath(path), currentWaypointIndex(0),
    distanceTolerance(5.0f), isAttacking(false), attackTimer(0.0f),
    jumpTimer(0.0f), jumpInterval(1.5f)
{
    RefreshConfig(nullptr);
    name = "Enemy";
    isTrigger = false;
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
        if (!params.enemy.texturePath.empty()) {
            SharedTexturePtr newTex = TextureManager::LoadTexture(params.enemy.texturePath, renderer);
            if (newTex) texture = newTex.get();
        }
        if (!params.enemy.bulletTexturePath.empty()) {
            bulletTexture = TextureManager::LoadTexture(params.enemy.bulletTexturePath, renderer);
        }
    }
}

void Enemy::Update(Game* game) {
    if (isDead) return;

    float targetX = 150.0f;
    float distToTarget = std::abs(x - targetX);

    if (distToTarget <= attackRange) {
        if (!isAttacking) {
            isAttacking = true;
            velX = 0;
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

    if (params.enemy.locomotionStyle == LocomotionType::Jumping) {
        if (isGrounded) {
            jumpTimer += dt;
            if (jumpTimer >= jumpInterval) {
                jumpTimer = 0;
                velY = -350.0f;
                velX = -moveSpeed;
            }
            else {
                velX = 0;
            }
        }
        return;
    }

    if (params.enemy.moveMethod == MovementType::Linear) {
        float targetX = 150.0f;
        if (x > targetX) {
            x -= moveSpeed * dt;
            if (x < targetX) x = targetX;
        }
    }
    else if (params.enemy.moveMethod == MovementType::PathFollow) {
        if (currentWaypointIndex >= targetPath.size()) return;
        SDL_FPoint target = targetPath[currentWaypointIndex];
        float dx = target.x - (x + width / 2.0f);
        if (params.enemy.locomotionStyle == LocomotionType::Flying) {
            float dy = target.y - (y + height / 2.0f);
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist < distanceTolerance) currentWaypointIndex++;
            else {
                x += (dx / dist) * moveSpeed * dt;
                y += (dy / dist) * moveSpeed * dt;
            }
        }
        else {
            if (std::abs(dx) < distanceTolerance) currentWaypointIndex++;
            else x += (dx > 0 ? 1.0f : -1.0f) * moveSpeed * dt;
        }
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
            auto bullet = std::make_unique<Bullet>(spawnX, spawnY, 180.0, bulletTexture ? bulletTexture.get() : nullptr);
            bullet->name = "EnemyBullet";
            game->GetPendingObjects().push_back(std::move(bullet));
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
    int barH = 4;
    float hpRatio = (maxHp > 0) ? (float)hp / maxHp : 0;
    SDL_Rect bg = { drawX, drawY - 10, width, barH };
    SDL_Rect fg = { drawX, drawY - 10, (int)(width * hpRatio), barH };
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderFillRect(renderer, &bg);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &fg);
}

// 衝突時：何かが自分（Enemy）に触れた
void Enemy::OnTriggerEnter(GameObject* other) {
    if (isDead || other->isDead) return;

    // プレイヤーの弾丸に当たった場合（二重判定防止のためBullet側と同期）
    if (other->name == "Bullet") {
        // Bullet側がTakeDamageを呼ぶため、ここでは何もしない。
        // もし、エネミー側で「弾を弾く」などの特殊な処理をしたい場合はここに書く。
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