#include "TitleScene.h"
#include "../Core/Game.h"
#include "PlayScene.h"
#include "../UI/TextRenderer.h"
#include "../UI/Button.h" 

TitleScene::TitleScene() {
}
TitleScene::~TitleScene() {
}

void TitleScene::OnEnter(Game* game) {
    // コンストラクタ引数に合わせて生成
    startButton = std::make_unique<Button>(300, 300, 200, 50, "GAME START");
    exitButton = std::make_unique<Button>(300, 400, 200, 50, "EXIT GAME");

    // ボタンに機能を登録
    startButton->OnClick = [game]() {
        game->ChangeScene(new PlayScene());
        };

    exitButton->OnClick = [game]() {
        game->Quit();
        };
}

void TitleScene::OnExit(Game* game) {
    // unique_ptr なので記述不要（勝手に消えます）
}

void TitleScene::HandleEvents(Game* game) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            game->Quit();
            return;
        }

        // シーン遷移ガード
        if (startButton && startButton->HandleEvents(&event)) return;
        if (exitButton && exitButton->HandleEvents(&event)) return;
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
}