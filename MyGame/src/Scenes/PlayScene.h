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
    void OnEnter(Game* game) override;
    void OnExit(Game* game) override;
    void Update(Game* game) override;
    void Render(Game* game) override;

    // 引数を (Game* game, SDL_Event* event) に修正
    void HandleEvents(Game* game, SDL_Event* event) override;

    // プレイシーンでは ImGui を使わない
    bool ShowImGui() const override { return false; };

    std::vector<std::unique_ptr<GameObject>>& GetObjects() override {
        return gameObjects;
    }

    SDL_Texture* GetBulletTexturePtr() const { return bulletTexture.get(); }

private:
    Player* player;
    std::vector<std::unique_ptr<GameObject>> gameObjects;
    std::unique_ptr<Camera> camera;
    SharedTexturePtr playerTexture;
    SharedTexturePtr bulletTexture;
};