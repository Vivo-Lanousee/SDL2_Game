#pragma once
#include "Scene.h"
#include <vector>
#include <memory>
#include <SDL.h>
#include "../Objects/GameObject.h"
#include "../Objects/Player.h"
#include "../Objects/Base.h"
#include "../Core/Camera.h"
#include "../TextureManager.h"
#include "../GameLogic/WaveManager.h"

class Game;

class PlayScene : public Scene {
public:
    PlayScene() = default;
    ~PlayScene() override = default;

    void OnEnter(Game* game) override;
    void OnExit(Game* game) override;

    void HandleEvents(Game* game, SDL_Event* event) override;
    void OnUpdate(Game* game) override;
    void Render(Game* game) override;

    // ゲーム本編では ImGui は表示しない
    bool ShowImGui() const override { return false; };

    std::vector<std::unique_ptr<GameObject>>& GetObjects() override { return gameObjects; }

    SDL_Texture* GetBulletTexturePtr() const { return bulletTexture.get(); }

private:
    std::vector<std::unique_ptr<GameObject>> gameObjects;
    std::unique_ptr<Camera> camera;

    // プレイヤーへの生ポインタ（追従・参照用）
    Player* player = nullptr;

    // ウェーブ管理
    WaveManager waveManager;

    // リソース保持
    SharedTexturePtr playerTexture;
    SharedTexturePtr bulletTexture;
};