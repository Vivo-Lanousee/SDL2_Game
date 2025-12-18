#pragma once
#include "Scene.h"
#include <vector>
#include <memory>
#include <SDL.h>
#include "../Objects/GameObject.h"
#include "../Core/Camera.h"

class Game;

class EditorScene : public Scene {
public:
    EditorScene();
    ~EditorScene() override = default;

    void OnEnter(Game* game) override;
    void OnExit(Game* game) override;

    // 引数を (Game* game, SDL_Event* event) に修正
    void HandleEvents(Game* game, SDL_Event* event) override;
    void Update(Game* game) override;
    void Render(Game* game) override;

    // エディタシーンなので true を返す
    bool ShowImGui() const override { return true; };

    // オブジェクトリストの実装
    std::vector<std::unique_ptr<GameObject>>& GetObjects() override { return gameObjects; }

    std::vector<SDL_FPoint>& GetEnemyPath() { return enemyPath; }

private:
    std::vector<std::unique_ptr<GameObject>> gameObjects;
    std::vector<SDL_FPoint> enemyPath;
    std::unique_ptr<Camera> camera;
    GameObject* selectedObject;
};