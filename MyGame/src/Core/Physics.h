#pragma once
#include <SDL.h>
#include <cmath>
#include <algorithm>
#include <limits>
#include "../Objects/GameObject.h"

// 前方宣言
class Game;

class Physics {
public:
    // 物理演算の適用（重力と終端速度の計算を含む）
    static void ApplyPhysics(GameObject* obj, float deltaTime);

    // --- 衝突判定ロジック ---

    static bool CheckAABB(GameObject* a, GameObject* b) {
        return (a->x < b->x + b->width &&
            a->x + a->width > b->x &&
            a->y < b->y + b->height &&
            a->y + a->height > b->y);
    }

    // 点とAABBの判定
    static bool PointInAABB(float px, float py, GameObject* obj) {
        return (px >= obj->x && px <= obj->x + obj->width &&
            py >= obj->y && py <= obj->y + obj->height);
    }

    // レイキャスト
    static bool LineVsAABB(float x1, float y1, float x2, float y2, GameObject* obj) {
        float minX = obj->x;
        float maxX = obj->x + obj->width;
        float minY = obj->y;
        float maxY = obj->y + obj->height;

        float tMin = 0.0f;
        float tMax = 1.0f;

        if (x2 != x1) {
            float tx1 = (minX - x1) / (x2 - x1);
            float tx2 = (maxX - x1) / (x2 - x1);
            tMin = std::max(tMin, std::min(tx1, tx2));
            tMax = std::min(tMax, std::max(tx1, tx2));
        }
        else {
            if (x1 < minX || x1 > maxX) return false;
        }

        if (y2 != y1) {
            float ty1 = (minY - y1) / (y2 - y1);
            float ty2 = (maxY - y1) / (y2 - y1);
            tMin = std::max(tMin, std::min(ty1, ty2));
            tMax = std::min(tMax, std::max(ty1, ty2));
        }
        else {
            if (y1 < minY || y1 > maxY) return false;
        }

        return tMax >= tMin;
    }

    static float DistanceSquared(float x1, float y1, float x2, float y2) {
        float dx = x2 - x1;
        float dy = y2 - y1;
        return dx * dx + dy * dy;
    }

    /**
     * @brief 衝突解決（押し戻し処理）
     * @return true: 下方向に衝突（着地）した / false: それ以外
     */
    static bool ResolveCollision(GameObject* a, GameObject* b) {
        if (!CheckAABB(a, b)) return false;

        // --- 修正点: Trigger（通り抜け）の判定ロジック ---
        // どちらかがTrigger設定されている場合
        if (a->isTrigger || b->isTrigger) {
            // 基本的には押し戻さないが、「地面（Block）」との判定時のみ物理的にぶつかる
            // 名前で地面かどうかを判定する（dynamic_castによる循環参照を避けるため）
            bool aIsGround = (a->name == "Block" || a->name == "Editor Ground");
            bool bIsGround = (b->name == "Block" || b->name == "Editor Ground");

            // aがTriggerの場合、bが地面でなければ無視
            if (a->isTrigger && !bIsGround) return false;
            // bがTriggerの場合、aが地面でなければ無視
            if (b->isTrigger && !aIsGround) return false;

            // 両方Triggerなら当然無視
            if (a->isTrigger && b->isTrigger) return false;
        }

        // --- 以下の押し戻し処理は、上記のフィルタを通過した（＝物理的にぶつかるべき）場合のみ実行される ---

        float aCenterX = a->x + a->width / 2.0f;
        float aCenterY = a->y + a->height / 2.0f;
        float bCenterX = b->x + b->width / 2.0f;
        float bCenterY = b->y + b->height / 2.0f;

        float dx = aCenterX - bCenterX;
        float dy = aCenterY - bCenterY;

        float combinedHalfWidth = (a->width / 2.0f) + (b->width / 2.0f);
        float combinedHalfHeight = (a->height / 2.0f) + (b->height / 2.0f);

        float overlapX = combinedHalfWidth - std::abs(dx);
        float overlapY = combinedHalfHeight - std::abs(dy);

        if (overlapX < overlapY) {
            // 横方向の押し戻し
            if (dx > 0) a->x += overlapX;
            else        a->x -= overlapX;
            a->velX = 0;
            return false;
        }
        else {
            // 縦方向の押し戻し
            if (dy > 0) {
                // 上からぶつかった（頭をぶつけた）
                a->y += overlapY;
                a->velY = 0;
                return false;
            }
            else {
                // 下にぶつかった（着地した）
                a->y -= overlapY;
                a->velY = 0;
                return true; // 着地フラグを返す
            }
        }
    }
};