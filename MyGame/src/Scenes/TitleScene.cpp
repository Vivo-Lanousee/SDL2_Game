#include "TitleScene.h"
#include "../Core/Game.h"
#include "PlayScene.h"
#include "EditorScene.h" 
#include "../UI/TextRenderer.h"
#include "../UI/Button.h" 
#include "../Editor/EditorGUI.h"
#include <iostream>

TitleScene::TitleScene() {
}

TitleScene::~TitleScene() {
    // unique_ptr のため自動的に解放されます
}

void TitleScene::OnEnter(Game* game) {
    std::cout << "Entering TitleScene..." << std::endl;

    // --- ボタンの生成と配置 ---
    startButton = std::make_unique<Button>(300, 300, 200, 50, "GAME START");
    exitButton = std::make_unique<Button>(300, 400, 200, 50, "EXIT GAME");
    debugButton = std::make_unique<Button>(300, 470, 200, 50, "DEBUG EDITOR");

    // --- ボタンクリック時の挙動登録 ---

    // ゲーム本編へ遷移
    startButton->OnClick = [game]() {
        game->ChangeScene(new PlayScene());
        };

    // ゲーム終了
    exitButton->OnClick = [game]() {
        game->Quit();
        };

    // エディターシーンへ遷移
    debugButton->OnClick = [game]() {
        game->ChangeScene(new EditorScene());
        };
}

void TitleScene::OnExit(Game* game) {
    std::cout << "Exiting TitleScene..." << std::endl;
    gameObjects.clear();
}

void TitleScene::HandleEvents(Game* game, SDL_Event* event) {

    if (event->type == SDL_QUIT) {
        game->Quit();
        return;
    }

    // ボタンのイベント判定
    if (startButton && startButton->HandleEvents(event)) return;
    if (exitButton && exitButton->HandleEvents(event)) return;
    if (debugButton && debugButton->HandleEvents(event)) return;
}

void TitleScene::OnUpdate(Game* game) {
}

void TitleScene::Render(Game* game) {
    SDL_Renderer* renderer = game->GetRenderer();

    // 背景の塗りつぶし（暗い紺色）
    SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);
    SDL_RenderClear(renderer);

    // タイトルロゴの描画
    SDL_Color white = { 255, 255, 255, 255 };
    TextRenderer::Draw(renderer, "MELTED DEFENSE", 280, 150, white);

    // ボタンの描画
    if (startButton) startButton->Render(renderer);
    if (exitButton) exitButton->Render(renderer);
    if (debugButton) debugButton->Render(renderer);
}