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

class Game;

class EditorScene : public Scene {
public:
    EditorScene();
    ~EditorScene() override = default;

    void OnEnter(Game* game) override;
    void OnExit(Game* game) override;

    void HandleEvents(Game* game, SDL_Event* event) override;
    void Render(Game* game) override;

    // 基底クラス Scene::Update から呼ばれる、このシーン独自のロジック
    void OnUpdate(Game* game) override;

    bool ShowImGui() const override { return true; };

    std::vector<std::unique_ptr<GameObject>>& GetObjects() override { return gameObjects; }

    std::vector<SDL_FPoint>& GetEnemyPath() { return enemyPath; }

    // GUIから呼び出すための敵生成関数
    void SpawnTestEnemy(SDL_Renderer* renderer);

private:
    std::vector<std::unique_ptr<GameObject>> gameObjects;
    std::vector<SDL_FPoint> enemyPath;
    std::unique_ptr<Camera> camera;

    // テストプレイ用のポインタ
    Player* testPlayer = nullptr;

    // エディタでもプレイヤーを表示するためのテクスチャ
    SharedTexturePtr playerTexture;
    SharedTexturePtr bulletTexture;

    GameObject* selectedObject = nullptr;
};