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
    distanceTolerance(5.0f), isAttacking(false), attackTimer(0.0f)
{
    // 設定をロード
    RefreshConfig(nullptr);

    name = "Enemy";
    isTrigger = false;
    useGravity = true; // 重力を有効化
}

void Enemy::RefreshConfig(SDL_Renderer* renderer) {
    GameParams& params = GameParams::GetInstance();

    // パラメータの同期
    hp = params.enemy.baseHealth;
    maxHp = params.enemy.baseHealth;
    moveSpeed = params.enemy.baseSpeed;
    attackPower = params.enemy.attackPower;
    attackRange = params.enemy.attackRange;
    attackInterval = params.enemy.attackInterval;

    // テクスチャ読み込み
    if (renderer && !params.enemy.texturePath.empty()) {
        SharedTexturePtr newTex = TextureManager::LoadTexture(params.enemy.texturePath, renderer);
        if (newTex) {
            texture = newTex.get();
        }
    }
}

void Enemy::Update(Game* game) {
    if (isDead) return;

    // 拠点ライン（x=150）を目指す設計
    float targetX = 150.0f;
    float distToTarget = std::abs(x - targetX);

    // 射程内であれば移動を止めて攻撃、それ以外は進軍
    if (distToTarget <= attackRange) {
        isAttacking = true;
        velX = 0; // 攻撃中は止まる
    }
    else {
        isAttacking = false;
        MoveLogic();
    }

    if (isAttacking) {
        AttackLogic(game);
    }
}

void Enemy::MoveLogic() {
    GameParams& params = GameParams::GetInstance();
    float dt = Time::deltaTime;

    if (params.enemy.moveMethod == MovementType::Linear) {
        // 直進：拠点方向（左）へ進む。x座標のみ操作し、yは物理に任せる
        float targetX = 150.0f;
        if (x > targetX) {
            x -= moveSpeed * dt;
            if (x < targetX) x = targetX;
        }
    }
    else if (params.enemy.moveMethod == MovementType::PathFollow) {
        // パス追従
        if (currentWaypointIndex >= targetPath.size()) {
            isAttacking = true;
            return;
        }

        SDL_FPoint target = targetPath[currentWaypointIndex];
        float dx = target.x - (x + width / 2.0f);
        // PathFollow時はyも制御（飛行ユニットなどの想定）
        float dy = target.y - (y + height / 2.0f);
        float distance = std::sqrt(dx * dx + dy * dy);

        if (distance < distanceTolerance) {
            currentWaypointIndex++;
        }
        else {
            x += (dx / distance) * moveSpeed * dt;
            // ウェイポイント追従中のみ、重力の影響を無視してyを動かす場合はここを有効化
            // y += (dy / distance) * moveSpeed * dt; 
        }
    }
}

void Enemy::AttackLogic(Game* game) {
    attackTimer += Time::deltaTime;

    if (attackTimer >= attackInterval) {
        attackTimer = 0.0f;

        GameParams& params = GameParams::GetInstance();
        GameSession& session = GameSession::GetInstance();

        // 拠点にダメージを与える
        session.DamageBase(attackPower);

        switch (params.enemy.attackMethod) {
        case AttackType::Melee:
            std::cout << "[Melee Attack] Base Damaged!" << std::endl;
            break;
        case AttackType::Ranged:
            std::cout << "[Ranged Attack] Base Shot!" << std::endl;
            break;
        case AttackType::Kamikaze:
            std::cout << "[Kamikaze] Exploded!" << std::endl;
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
        SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);
        SDL_RenderFillRect(renderer, &destRect);
    }

    // HPバー描画
    int barH = 4;
    float hpRatio = (maxHp > 0) ? (float)hp / maxHp : 0;
    SDL_Rect bg = { drawX, drawY - 10, width, barH };
    SDL_Rect fg = { drawX, drawY - 10, (int)(width * hpRatio), barH };

    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderFillRect(renderer, &bg);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &fg);
}

void Enemy::OnTriggerEnter(GameObject* other) {
}

void Enemy::TakeDamage(int damage) {
    if (isDead) return;

    hp -= damage;
    if (hp <= 0) {
        hp = 0;
        isDead = true;
        std::cout << "Enemy Defeated!" << std::endl;
    }
}