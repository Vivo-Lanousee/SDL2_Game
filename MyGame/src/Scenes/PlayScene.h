#pragma once
#include "Scene.h"
#include <vector>
#include "../Objects/GameObject.h"
#include "../Objects/Player.h"
#include "../Objects/Bullet.h"
#include "../Core/Camera.h"
#include <memory>
#include "../TextureManager.h"

class PlayScene : public Scene {
public:
    // シーンに入った時
    void OnEnter(Game* game) override;

    // シーンを出る時
    void OnExit(Game* game) override;

    // 更新（移動など）
    void Update(Game* game) override;

    // 描画
    void Render(Game* game) override;

    // 入力
    void HandleEvents(Game* game) override;

    std::vector<std::unique_ptr<GameObject>>& GetObjects() override {
        return gameObjects;
    }

private:
    Player* player;

    // GameObjectのリスト
    std::vector<std::unique_ptr<GameObject>> gameObjects;

    std::unique_ptr<Camera> camera;

    SharedTexturePtr playerTexture;
    SharedTexturePtr bulletTexture;
};