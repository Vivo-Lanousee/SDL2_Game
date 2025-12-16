#pragma once
#include "Scene.h"
#include <vector>
#include <memory>
#include <SDL.h>
#include "../Objects/GameObject.h"
#include "../Core/Camera.h" // カメラ操作のために必要

// Forward Declarations
class Game;
class EnemyPathEditor; // 経路編集ツール (仮定)

class EditorScene : public Scene {
public:
    EditorScene();
    ~EditorScene() override = default;

    // --- Scene Interfaceの実装 ---
    void OnEnter(Game* game) override;
    void OnExit(Game* game) override;

    void HandleEvents(Game* game) override;
    void Update(Game* game) override;
    void Render(Game* game) override;

    // ★ Scene::GetObjects() をオーバーライド (GameObjectリストへのアクセスを提供)
    std::vector<std::unique_ptr<GameObject>>& GetObjects() override { return gameObjects; }

    // --- エディタ専用機能 ---
    // 経路へのアクセス (デバッグ描画、保存用)
    std::vector<SDL_FPoint>& GetEnemyPath() { return enemyPath; }

private:
    // シーンが所有するすべてのゲームオブジェクト
    std::vector<std::unique_ptr<GameObject>> gameObjects;

    // エディタで編集する主要なデータ（タワーディフェンスの経路）
    std::vector<SDL_FPoint> enemyPath;

    // エディタ操作用のカメラ (EditorScene独自のカメラを持つ)
    std::unique_ptr<Camera> camera;

    // 経路編集ツールへのポインタ/ユニークポインタ (未実装)
    // std::unique_ptr<EnemyPathEditor> pathEditor;

    // 現在選択中のオブジェクトのポインタなど
    GameObject* selectedObject;
};