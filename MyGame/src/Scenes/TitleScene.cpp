#include "TitleScene.h"
#include "../Core/Game.h"
#include "PlayScene.h"
#include "EditorScene.h" 
#include "../UI/TextRenderer.h"
#include "../UI/Button.h" 
#include "../Editor/EditorGUI.h"

TitleScene::TitleScene() {
}

TitleScene::~TitleScene() {
    // unique_ptr のため自動的に解放されます
}

void TitleScene::OnEnter(Game* game) {
    // --- ボタンの生成 ---
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
        // EditorScene への遷移
        // モードの変更は EditorScene::OnEnter で行う設計にするとより安全です
        game->ChangeScene(new EditorScene());
        };
}

void TitleScene::OnExit(Game* game) {
    // ボタンの unique_ptr は自動解放されます
}

// ★修正ポイント: 引数を (Game* game, SDL_Event* event) に変更し、ループを削除
void TitleScene::HandleEvents(Game* game, SDL_Event* event) {
    // Game::HandleEvents 側で SDL_PollEvent が回っており、
    // 発生したイベントが一つずつこの関数に渡されてきます。

    if (event->type == SDL_QUIT) {
        game->Quit();
        return;
    }

    // 各ボタンにイベントを渡して判定（Button::HandleEvents が SDL_Event* を受ける前提）
    if (startButton && startButton->HandleEvents(event)) return;
    if (exitButton && exitButton->HandleEvents(event)) return;
    if (debugButton && debugButton->HandleEvents(event)) return;
}

void TitleScene::Update(Game* game) {
    // 必要なら更新ロジックを記述
}

void TitleScene::Render(Game* game) {
    SDL_Color white = { 255, 255, 255, 255 };
    TextRenderer::Draw(game->GetRenderer(), "SHOOTING GAME", 300, 150, white);

    if (startButton) startButton->Render(game->GetRenderer());
    if (exitButton) exitButton->Render(game->GetRenderer());
    if (debugButton) debugButton->Render(game->GetRenderer());
}