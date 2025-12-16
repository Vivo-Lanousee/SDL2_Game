#include "Game.h"
#include "Time.h"
#include "../Scenes/Scene.h" 
#include "../Objects/GameObject.h" 
#include "Game.h"


Game* game = nullptr;

int main(int argc, char* argv[]) {
    // FPS制御用の定数 (60FPSを目指す)
    const int FPS = 60;
    const int frameDelay = 1000 / FPS; // 1フレームあたりの目標時間 (約16ms)

    Uint32 frameStart;
    int frameTime;

    game = new Game();

    // 初期化
    game->Init("My SDL2 Game Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, false);

    // ゲームループ
    while (game->Running()) {
        // フレーム開始時刻を記録
        frameStart = SDL_GetTicks();
        Time::Update();

        //  入力
        game->HandleEvents();
        //  更新
        game->Update();
        // 描画
        game->Render();

        // --- FPS制御ロジック ---
        // 「処理にかかった時間」を計算
        frameTime = SDL_GetTicks() - frameStart;

        // もし目標時間(16ms)より早く処理が終わったら、残りの時間を待機する
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    // 終了処理
    game->Clean();
    delete game; // newしたのでdeleteも忘れずに

    return 0;
}