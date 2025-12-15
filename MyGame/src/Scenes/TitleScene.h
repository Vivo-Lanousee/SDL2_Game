#pragma once
#include "Scene.h"
#include "../Core/Game.h"
#include "PlayScene.h" // PlaySceneに移動するために必要

class TitleScene : public Scene {
public:
    void OnEnter(Game* game) override {
        // 必要ならBGM再生など
    }

    void HandleEvents(Game* game) override {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            // ×ボタン処理
            if (event.type == SDL_QUIT) {
                game->Quit();
            }

            // キー入力処理
            if (event.type == SDL_KEYDOWN) {
                // エンターキーでゲーム開始
                if (event.key.keysym.sym == SDLK_RETURN) {
                    game->ChangeScene(new PlayScene());
                }
            }
        }
    }

    void Update(Game* game) override {
        // 入力処理をHandleEventsに移したので、ここは空でもOK
    }

    void Render(Game* game) override {
        // タイトル画面の文字表示
        SDL_Color white = { 255, 255, 255, 255 };
        game->DrawText("SHOOTING GAME", 300, 200, white);

        SDL_Color yellow = { 255, 255, 0, 255 };
        game->DrawText("Press Enter to Start", 280, 300, yellow);
    }

    void OnExit(Game* game) override {
        // 画像の破棄などがあればここで
    }
};