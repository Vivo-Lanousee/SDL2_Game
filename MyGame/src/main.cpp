#include <SDL.h>
#include <iostream>

// ★重要：SDL2では、mainの引数は必ずこの形でなければなりません！
// int main() や void main() だとエラーになります。
int main(int argc, char* argv[]) {

    // 1. SDLの初期化
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // 2. ウィンドウ作成
    SDL_Window* window = SDL_CreateWindow(
        "SDL2 Game",             // タイトル
        SDL_WINDOWPOS_CENTERED,  // X座標
        SDL_WINDOWPOS_CENTERED,  // Y座標
        800,                     // 幅
        600,                     // 高さ
        SDL_WINDOW_SHOWN         // フラグ
    );

    if (window == nullptr) {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // 3. レンダラー（描画機能）作成
    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (renderer == nullptr) {
        SDL_DestroyWindow(window);
        std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // 4. ゲームループ
    bool isRunning = true;
    SDL_Event event;

    while (isRunning) {
        // --- 入力処理 ---
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false; // ×ボタンが押されたら終了
            }
        }

        // --- 描画処理 ---

        // 画面を黒でクリア
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // 赤い四角を描画（テスト用）
        SDL_Rect rect;
        rect.x = 350;
        rect.y = 250;
        rect.w = 100;
        rect.h = 100;
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // 赤
        SDL_RenderFillRect(renderer, &rect);

        // 画面を更新（これをしないと真っ暗なまま）
        SDL_RenderPresent(renderer);
    }

    // 5. 終了処理（重要：メモリ解放）
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}