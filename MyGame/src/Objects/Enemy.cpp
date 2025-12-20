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
    jumpTimer(0.0f), jumpInterval(1.5f) // 跳躍間隔の初期値
{
    // 初期設定のロード
    RefreshConfig(nullptr);

    name = "Enemy";
    isTrigger = false;
}

void Enemy::RefreshConfig(SDL_Renderer* renderer) {
    GameParams& params = GameParams::GetInstance();

    // 1. パラメータの同期
    hp = params.enemy.baseHealth;
    maxHp = params.enemy.baseHealth;
    moveSpeed = params.enemy.baseSpeed;
    attackPower = params.enemy.attackPower;
    attackRange = params.enemy.attackRange;
    attackInterval = params.enemy.attackInterval;

    // 2. 移動スタイルに応じた物理設定の切り替え
    // 飛行タイプ(Flying)のみ重力を無効化し、それ以外は有効にする
    if (params.enemy.locomotionStyle == LocomotionType::Flying) {
        useGravity = false;
        velY = 0; // 空中で静止させる
    }
    else {
        useGravity = true;
    }

    // 3. テクスチャ読み込み
    if (renderer) {
        if (!params.enemy.texturePath.empty()) {
            SharedTexturePtr newTex = TextureManager::LoadTexture(params.enemy.texturePath, renderer);
            if (newTex) texture = newTex.get();
        }
        // 遠距離攻撃用の弾丸テクスチャをロード
        if (!params.enemy.bulletTexturePath.empty()) {
            bulletTexture = TextureManager::LoadTexture(params.enemy.bulletTexturePath, renderer);
        }
    }
}

void Enemy::Update(Game* game) {
    if (isDead) return;

    // 拠点（Gate）のライン (x=150) を目標とする
    float targetX = 150.0f;
    float distToTarget = std::abs(x - targetX);

    // --- 状態遷移ロジック ---
    if (distToTarget <= attackRange) {
        // 射程内に入った瞬間（移動から攻撃への切り替え）
        if (!isAttacking) {
            isAttacking = true;
            velX = 0; // 移動停止
            // 初撃を即座に出すために、タイマーをインターバル満了状態にする
            attackTimer = attackInterval;
        }
        AttackLogic(game);
    }
    else {
        // 射程外なら進軍
        isAttacking = false;
        MoveLogic();
    }
}

void Enemy::MoveLogic() {
    GameParams& params = GameParams::GetInstance();
    float dt = Time::deltaTime;

    // --- スタイル別移動挙動 ---

    // A. 跳躍移動 (Jumping)
    if (params.enemy.locomotionStyle == LocomotionType::Jumping) {
        if (isGrounded) {
            // 地面についている間、タイマーを回す
            jumpTimer += dt;
            if (jumpTimer >= jumpInterval) {
                jumpTimer = 0;
                velY = -350.0f;    // ジャンプ（上方向）
                velX = -moveSpeed; // 前進（左方向）
            }
            else {
                // 着地待機中は水平移動を止める
                velX = 0;
            }
        }
        // 空中にいる間（isGrounded=false）は Physics::ApplyPhysics が放物線を描かせる
        return;
    }

    // B. 通常歩行(Ground) または 飛行(Flying)
    if (params.enemy.moveMethod == MovementType::Linear) {
        // 直進移動
        float targetX = 150.0f;
        if (x > targetX) {
            x -= moveSpeed * dt;
            if (x < targetX) x = targetX;
        }
    }
    else if (params.enemy.moveMethod == MovementType::PathFollow) {
        // パス追従移動
        if (currentWaypointIndex >= targetPath.size()) return;

        SDL_FPoint target = targetPath[currentWaypointIndex];
        float dx = target.x - (x + width / 2.0f);

        if (params.enemy.locomotionStyle == LocomotionType::Flying) {
            // 飛行：Y座標もパスに合わせて動く
            float dy = target.y - (y + height / 2.0f);
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist < distanceTolerance) {
                currentWaypointIndex++;
            }
            else {
                x += (dx / dist) * moveSpeed * dt;
                y += (dy / dist) * moveSpeed * dt;
            }
        }
        else {
            // 地上：X座標のみパスを追い、Yは重力に任せる
            if (std::abs(dx) < distanceTolerance) {
                currentWaypointIndex++;
            }
            else {
                x += (dx > 0 ? 1.0f : -1.0f) * moveSpeed * dt;
            }
        }
    }
}

void Enemy::AttackLogic(Game* game) {
    attackTimer += Time::deltaTime;

    if (attackTimer >= attackInterval) {
        attackTimer = 0.0f;

        GameParams& params = GameParams::GetInstance();
        GameSession& session = GameSession::GetInstance();

        // 攻撃方法の分岐
        switch (params.enemy.attackMethod) {
        case AttackType::Melee:
            // 直接拠点のHPを減らす
            session.DamageBase(attackPower);
            std::cout << "[Melee] Attack successful!" << std::endl;
            break;

        case AttackType::Ranged:
            // 遠距離弾の生成
        {
            // エネミーの目の前から左(180度)へ向けて発射
            float spawnX = x - 10.0f;
            float spawnY = y + height / 2.0f;
            auto bullet = std::make_unique<Bullet>(
                spawnX, spawnY, 180.0,
                bulletTexture ? bulletTexture.get() : nullptr
            );
            bullet->name = "EnemyBullet"; // 敵の弾として識別
            // 物理エンジンの更新対象に加えるため Pending リストへ
            game->GetPendingObjects().push_back(std::move(bullet));
        }
        std::cout << "[Ranged] Bullet fired!" << std::endl;
        break;

        case AttackType::Kamikaze:
            // 自爆攻撃：拠点に特大ダメージを与えて死亡
            session.DamageBase(attackPower * 5);
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
        // 画像未設定時のデバッグ表示
        SDL_SetRenderDrawColor(renderer, 220, 50, 50, 255);
        SDL_RenderFillRect(renderer, &destRect);
    }

    // HPバーの描画
    int barH = 4;
    float hpRatio = (maxHp > 0) ? (float)hp / maxHp : 0;
    SDL_Rect bg = { drawX, drawY - 10, width, barH };
    SDL_Rect fg = { drawX, drawY - 10, (int)(width * hpRatio), barH };

    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255); // 背景
    SDL_RenderFillRect(renderer, &bg);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // 前景
    SDL_RenderFillRect(renderer, &fg);
}

void Enemy::OnTriggerEnter(GameObject* other) {
    // プレイヤーの弾丸(`Bullet`)との衝突処理は Bullet 側で行う
}

void Enemy::TakeDamage(int damage) {
    if (isDead) return;
    hp -= damage;
    if (hp <= 0) {
        hp = 0;
        isDead = true;
        std::cout << "Enemy neutralized." << std::endl;
    }
}