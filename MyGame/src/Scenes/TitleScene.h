#pragma once
#include "Scene.h"
#include "../Core/Game.h" // Gameクラスの機能を使うため

class TitleScene : public Scene {
public:
    void OnEnter(Game* game) override {
        // 背景音楽を鳴らすならここ
    }

    void Update(Game* game) override {
        // エンターキーでプレイ画面へ遷移
        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_RETURN]) {
            // ★ここで「シーン切り替え」を命令する！
            game->ChangeScene(new PlayScene());
        }
    }

    void Render(Game* game) override {
        // タイトルの文字を描画
        SDL_Color white = { 255, 255, 255, 255 };
        game->DrawText("TITLE SCREEN", 300, 200, white);
    }

    void OnExit(Game* game) override {
        // メモリ解放など
    }
};