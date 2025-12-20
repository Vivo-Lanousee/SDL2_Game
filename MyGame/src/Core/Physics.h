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


    // --- 衝突判定ロジック (既存) ---

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

        // X軸方向の判定（スラブ法）
        if (x2 != x1) {
            float tx1 = (minX - x1) / (x2 - x1);
            float tx2 = (maxX - x1) / (x2 - x1);

            tMin = std::max(tMin, std::min(tx1, tx2));
            tMax = std::min(tMax, std::max(tx1, tx2));
        }
        else {
            if (x1 < minX || x1 > maxX) return false;
        }

        // Y軸方向の判定
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

    // 衝突解決（当たっていたら押し戻す）
    static bool ResolveCollision(GameObject* player, GameObject* wall) {
        if (!CheckAABB(player, wall)) return false;

        float playerCenterX = player->x + player->width / 2.0f;
        float playerCenterY = player->y + player->height / 2.0f;
        float wallCenterX = wall->x + wall->width / 2.0f;
        float wallCenterY = wall->y + wall->height / 2.0f;

        float dx = playerCenterX - wallCenterX;
        float dy = playerCenterY - wallCenterY;

        float combinedHalfWidth = (player->width / 2.0f) + (wall->width / 2.0f);
        float combinedHalfHeight = (player->height / 2.0f) + (wall->height / 2.0f);

        float overlapX = combinedHalfWidth - std::abs(dx);
        float overlapY = combinedHalfHeight - std::abs(dy);

        if (overlapX < overlapY) {
            if (dx > 0) player->x += overlapX;
            else        player->x -= overlapX;
            player->velX = 0;
            return false;
        }
        else {
            if (dy > 0) {
                player->y += overlapY;
                player->velY = 0;
                return false;
            }
            else {
                player->y -= overlapY;
                player->velY = 0;
                return true; // 着地
            }
        }
    }
};