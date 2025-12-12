#include "Game.h"

Game* game = nullptr;

int main(int argc, char* argv[]) {

    game = new Game();

    // 初期化
    game->Init("My SDL2 Game Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, false);

    // ゲームループ
    while (game->Running()) {

        //  入力
        game->HandleEvents();
        //  更新
        game->Update();
        // 描画
        game->Render();

        // Wait（フレームレート的）
        SDL_Delay(16);
    }

    // 終了処理
    game->Clean();

    return 0;
}