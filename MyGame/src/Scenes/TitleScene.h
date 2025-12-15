#pragma once
#include "Scene.h"
#include "../UI/Button.h"
#include "../Core/Game.h" // TitleSceneはGameの機能を使うのでインクルードOK

#include "PlayScene.h"

class TitleScene : public Scene {
public:
    void OnEnter(Game* game) override {
        startButton = new Button(300, 300, 200, 50, "GAME START");
        exitButton = new Button(300, 400, 200, 50, "EXIT GAME");
    }

    void HandleEvents(Game* game) override {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) game->Quit();

            if (startButton && startButton->HandleEvents(&event)) {
                game->ChangeScene(new PlayScene());
                return; // ★★★重要！シーンが変わったら、この関数の処理は即終了する！★★★
            }

            // ↑ここで return しないと、削除されたメモリにある exitButton を触りに行ってクラッシュします
            if (exitButton && exitButton->HandleEvents(&event)) {
                game->Quit();
            }
        }
    }

    void Update(Game* game) override {}

    void Render(Game* game) override {
        SDL_Color white = { 255, 255, 255, 255 };
        game->DrawText("SHOOTING GAME", 300, 150, white);

        if (startButton) startButton->Render(game);
        if (exitButton) exitButton->Render(game);
    }

    void OnExit(Game* game) override {
        if (startButton) { delete startButton; startButton = nullptr; }
        if (exitButton) { delete exitButton; exitButton = nullptr; }
    }

private:
    Button* startButton = nullptr;
    Button* exitButton = nullptr;
};