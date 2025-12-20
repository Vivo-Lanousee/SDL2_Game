#pragma once
#include "Scene.h"
#include <memory> 
#include <vector>
#include "../UI/Button.h"

class Game;
class GameObject;

/**
 * @brief ゲームのタイトル画面シーン
 */
class TitleScene : public Scene {
public:
    TitleScene();
    ~TitleScene() override;

    void OnEnter(Game* game) override;
    void OnExit(Game* game) override;

    // 基底クラス Scene::Update から呼び出される固有ロジック
    void OnUpdate(Game* game) override;

    void Render(Game* game) override;
    void HandleEvents(Game* game, SDL_Event* event) override;

    // タイトルシーンでは ImGui は使用しない
    bool ShowImGui() const override { return false; };

    // 物理演算対象のオブジェクトリスト（タイトルでは空のリストを返す）
    std::vector<std::unique_ptr<GameObject>>& GetObjects() override {
        return gameObjects;
    }

private:
    // UIボタン
    std::unique_ptr<Button> startButton;
    std::unique_ptr<Button> exitButton;
    std::unique_ptr<Button> debugButton;

    // GameObject管理用（タイトルで物理オブジェクトを使いたい場合に使用）
    std::vector<std::unique_ptr<GameObject>> gameObjects;
};