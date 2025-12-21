#include "Scene.h"
#include "../Objects/GameObject.h"
#include "../Core/Game.h"
#include "../Core/Physics.h"
#include "../Core/Time.h"
#include <algorithm>
#include <cmath>

void Scene::Update(Game* game) {
    float dt = Time::deltaTime;
    auto& objects = GetObjects();
    std::vector<std::unique_ptr<GameObject>>& newObjs = game->GetPendingObjects();
    if (!newObjs.empty()) {
        for (auto& obj : newObjs) {
            objects.push_back(std::move(obj));
        }
        game->ClearPendingObjects();
    }

    OnUpdate(game);

    for (auto& obj : objects) {
        if (obj->isDead) continue;
        obj->Update(game);
    }

    // 物理演算の適用
    for (auto& obj : objects) {
        if (obj->isDead) continue;
        if (obj->useGravity || std::abs(obj->velX) > 0 || std::abs(obj->velY) > 0) {
            Physics::ApplyPhysics(obj.get(), dt);
        }
    }

    // 衝突判定と解決
    for (size_t i = 0; i < objects.size(); ++i) {
        auto& a = objects[i];
        if (a->isDead) continue;

        // 接地判定などの物理衝突（対象を絞る）
        if (a->name == "Player" || a->name == "TestPlayer" || a->name == "Enemy" || a->name == "Test Enemy") {
            a->isGrounded = false;
            for (auto& b : objects) {
                if (a == b || b->isTrigger) continue;
                // 地面(Block/Editor Ground)との衝突を Physics::ResolveCollision で解決
                if (Physics::ResolveCollision(a.get(), b.get())) {
                    a->isGrounded = true;
                }
            }
        }
        // トリガー判定（重なりチェック：攻撃判定など）
        for (size_t j = i + 1; j < objects.size(); ++j) {
            auto& b = objects[j];
            if (b->isDead) continue;

            if (a->isTrigger || b->isTrigger) {
                if (CheckOverlap(a.get(), b.get())) {
                    a->OnTriggerEnter(b.get());
                    b->OnTriggerEnter(a.get());
                }
            }
        }
    }
    auto it = std::remove_if(objects.begin(), objects.end(),
        [](const std::unique_ptr<GameObject>& obj) { return obj->isDead; });
    objects.erase(it, objects.end());
}

bool Scene::CheckOverlap(GameObject* a, GameObject* b) {
    return (a->x < b->x + b->width && a->x + a->width > b->x &&
        a->y < b->y + b->height && a->y + a->height > b->y);
}