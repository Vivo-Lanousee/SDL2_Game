#include "Physics.h"
#include "../Objects/GameObject.h"
#include "../Core/Time.h"
#include "../Core/GameParams.h" 
void Physics::ApplyPhysics(GameObject* obj, float deltaTime) {
    if (!obj) return;

    GameParams& params = GameParams::GetInstance();

    if (obj->useGravity) {
        // gravity (9.8) * GravityScale (100.0) = 980.0 px/s^2 相当となる
        float effectiveGravity = params.physics.gravity * PhysicsSettings::GravityScale;

        // 重力加速度 (px/s^2) * deltaTime (s) = 速度の変化量 (px/s)
        obj->velY += effectiveGravity * deltaTime;

        // 終端速度を制限
        if (obj->velY > params.physics.terminalVelocity) {
            obj->velY = params.physics.terminalVelocity;
        }
        else if (obj->velY < -params.physics.terminalVelocity) {
            obj->velY = -params.physics.terminalVelocity;
        }
    }

    // 2. 加速度の適用とリセット
    obj->velX += obj->accX * deltaTime;
    obj->velY += obj->accY * deltaTime;

    // 3. 速度を位置に適用
    obj->x += obj->velX * deltaTime;
    obj->y += obj->velY * deltaTime;

    // 4. 加速度のリセット
    obj->accX = 0;
    obj->accY = 0;
}

// 衝突判定ロジック (既存のものを維持)
/*
// ... (CheckAABB, PointInAABB, LineVsAABB, DistanceSquared, ResolveCollision の実装は省略) ...
*/