#pragma once
#include "Scene.h"
#include <memory> 

class Game;
class Button;

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
};