#pragma once
#include "Scene.h"
#include <vector>
#include "../Objects/GameObject.h"
#include "../Objects/Player.h"
#include "../Objects/Bullet.h"
#include "../Core/Camera.h"
#include <memory>
#include "../TextureManager.h"

class Game;

class PlayScene : public Scene {
public:
    PlayScene() = default;
    ~PlayScene() override = default;

    void OnEnter(Game* game) override;
    void OnExit(Game* game) override;

    // 基底クラス Scene::Update から呼び出される固有ロジック
    void OnUpdate(Game* game) override;

    void Render(Game* game) override;
    void HandleEvents(Game* game, SDL_Event* event) override;

    // プレイシーンでは ImGui を使わない
    bool ShowImGui() const override { return false; };

    std::vector<std::unique_ptr<GameObject>>& GetObjects() override {
        return gameObjects;
    }

    SDL_Texture* GetBulletTexturePtr() const { return bulletTexture.get(); }

private:
    Player* player = nullptr;
    std::vector<std::unique_ptr<GameObject>> gameObjects;
    std::unique_ptr<Camera> camera;
    SharedTexturePtr playerTexture;
    SharedTexturePtr bulletTexture;
};