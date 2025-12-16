#pragma once
#include "Scene.h"
#include <memory> 
#include "../UI/Button.h" // Button クラスが必要

class Game;
// class Button; // Button.h をインクルードしたので不要

class TitleScene : public Scene {
public:
    TitleScene();
    ~TitleScene();

    void OnEnter(Game* game) override;
    void OnExit(Game* game) override;
    void Update(Game* game) override;
    void Render(Game* game) override;
    void HandleEvents(Game* game) override;

private:
    std::unique_ptr<Button> startButton;
    std::unique_ptr<Button> exitButton;
    // ★★★ 修正箇所 1: デバッグエディタボタンを追加 ★★★
    std::unique_ptr<Button> debugButton;
};