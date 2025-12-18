#pragma once
#include "Scene.h"
#include <memory> 
#include <vector>
#include "../UI/Button.h"

class Game;
class GameObject;

class TitleScene : public Scene {
public:
    TitleScene();
    ~TitleScene();

    void OnEnter(Game* game) override;
    void OnExit(Game* game) override;
    void Update(Game* game) override;
    void Render(Game* game) override;

    // 引数を (Game* game, SDL_Event* event) に修正
    void HandleEvents(Game* game, SDL_Event* event) override;

    // タイトルでも ImGui は使わないので false (必要なら true)
    bool ShowImGui() const override { return false; };

    // GetObjects の実装（タイトルにはオブジェクトがないので空のリストを返す）
    std::vector<std::unique_ptr<GameObject>>& GetObjects() override {
        static std::vector<std::unique_ptr<GameObject>> empty;
        return empty;
    }

private:
    std::unique_ptr<Button> startButton;
    std::unique_ptr<Button> exitButton;
    std::unique_ptr<Button> debugButton;
};