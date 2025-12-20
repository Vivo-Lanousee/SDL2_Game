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

    // 1. シーン独自の更新ロジック実行（スポーン処理など）
    OnUpdate(game);

    // 2. 全オブジェクトの個別Updateを実行（★ここで入力を受け取ります）
    for (auto& obj : objects) {
        if (obj->isDead) continue;
        obj->Update(game);
    }

    // 3. 物理演算の適用（Updateで設定された速度を座標に反映）
    for (auto& obj : objects) {
        if (obj->isDead) continue;
        if (obj->useGravity || std::abs(obj->velX) > 0 || std::abs(obj->velY) > 0) {
            Physics::ApplyPhysics(obj.get(), dt);
        }
    }

    // 4. 衝突解決とトリガー判定
    for (size_t i = 0; i < objects.size(); ++i) {
        auto& a = objects[i];
        if (a->isDead) continue;

        // 接地判定などの物理衝突（Solid同士）
        if (a->name == "Player" || a->name == "TestPlayer" || a->name == "Enemy" || a->name == "Test Enemy") {
            a->isGrounded = false;
            for (auto& b : objects) {
                if (a == b || b->isTrigger) continue;
                if (Physics::ResolveCollision(a.get(), b.get())) {
                    a->isGrounded = true;
                }
            }
        }

        // トリガー判定（重なりチェック）
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

    // 5. 新規生成されたオブジェクトの回収
    std::vector<std::unique_ptr<GameObject>>& newObjs = game->GetPendingObjects();
    for (auto& obj : newObjs) {
        objects.push_back(std::move(obj));
    }
    game->ClearPendingObjects();

    // 6. 死亡フラグが立ったオブジェクトの削除
    auto it = std::remove_if(objects.begin(), objects.end(),
        [](const std::unique_ptr<GameObject>& obj) { return obj->isDead; });
    objects.erase(it, objects.end());
}

bool Scene::CheckOverlap(GameObject* a, GameObject* b) {
    return (a->x < b->x + b->width && a->x + a->width > b->x &&
        a->y < b->y + b->height && a->y + a->height > b->y);
}