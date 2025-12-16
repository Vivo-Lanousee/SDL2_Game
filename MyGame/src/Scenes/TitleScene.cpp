#include "TitleScene.h"
#include "../Core/Game.h"
#include "PlayScene.h"
#include "EditorScene.h" // ★★★ 修正箇所 2: EditorScene をインクルード ★★★
#include "../UI/TextRenderer.h"
#include "../UI/Button.h" 
#include "../Editor/EditorGUI.h" // EditorGUI::SetMode に必要

TitleScene::TitleScene() {
}
TitleScene::~TitleScene() {
    // unique_ptr のため自動的に解放されます
}

void TitleScene::OnEnter(Game* game) {
    // --- 既存のボタンの生成 ---
    startButton = std::make_unique<Button>(300, 300, 200, 50, "GAME START");
    exitButton = std::make_unique<Button>(300, 400, 200, 50, "EXIT GAME");

    // ボタンに機能を登録
    startButton->OnClick = [game]() {
        game->ChangeScene(new PlayScene());
        };

    exitButton->OnClick = [game]() {
        game->Quit();
        };

    debugButton = std::make_unique<Button>(300, 460, 200, 50, "DEBUG EDITOR");
    debugButton->OnClick = [game]() {
        // EditorScene に遷移し、エディタモードに切り替える
        EditorGUI::SetMode(EditorGUI::Mode::EDITOR);
        game->ChangeScene(new EditorScene());
        };
}

void TitleScene::OnExit(Game* game) {
    // ボタンの unique_ptr は自動解放されます
}

void TitleScene::HandleEvents(Game* game) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            game->Quit();
            return;
        }

        // ImGuiのイベント処理は Game.cpp で制御されていますが、
        // ここでは純粋なSDLボタンのイベント処理を行います。

        // シーン遷移ガード
        if (startButton && startButton->HandleEvents(&event)) return;
        if (exitButton && exitButton->HandleEvents(&event)) return;
        // ★★★ 修正箇所 4: デバッグボタンのイベント処理 ★★★
        if (debugButton && debugButton->HandleEvents(&event)) return;
    }
}

void TitleScene::Update(Game* game) {
    // 必要なら
}

void TitleScene::Render(Game* game) {
    SDL_Color white = { 255, 255, 255, 255 };
    TextRenderer::Draw(game->GetRenderer(), "SHOOTING GAME", 300, 150, white);

    if (startButton) startButton->Render(game->GetRenderer());
    if (exitButton) exitButton->Render(game->GetRenderer());
    // ★★★ 修正箇所 5: デバッグボタンの描画 ★★★
    if (debugButton) debugButton->Render(game->GetRenderer());
}