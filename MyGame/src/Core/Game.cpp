#include "Game.h"
#include <iostream>
#include <SDL_image.h>
#include "../Scenes/Scene.h"
#include "../Scenes/TitleScene.h" 


Game::Game() : isRunning(false), window(nullptr), renderer(nullptr), font(nullptr), currentScene(nullptr) {
}

Game::~Game() {}

bool Game::Init(const char* title, int x, int y, int width, int height, bool fullscreen) {
    int flags = 0;
    if (fullscreen) {
        flags = SDL_WINDOW_FULLSCREEN;
    }

    // SDLの初期化
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        window = SDL_CreateWindow(title, x, y, width, height, flags);
        if (window) {
            renderer = SDL_CreateRenderer(window, -1, 0);
            if (renderer) {
                // 背景を黒で初期化
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                isRunning = true;
            }
        }
    }
    else {
        return false;
    }

    // 画像システムの初期化
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cout << "IMG_Init Error: " << IMG_GetError() << std::endl;
        return false;
    }

    // 文字システムの初期化
    if (TTF_Init() == -1) {
        std::cout << "TTF_Init Error: " << TTF_GetError() << std::endl;
        return false;
    }

    // フォント読み込み（ゲーム全体で共通使用）
    font = TTF_OpenFont("assets/fonts/PixelMplus10.ttf", 24);
    if (!font) {
        std::cout << "Failed to load font! Check assets/fonts folder." << std::endl;
    }

    // ★重要：最初は「タイトルシーン」からスタート
    ChangeScene(new TitleScene());

    return true;
}

// シーン切り替え処理
void Game::ChangeScene(Scene* newScene) {
    // 1. 古いシーンがあれば片付ける（OnExit呼び出し）
    if (currentScene) {
        currentScene->OnExit(this);
        delete currentScene; // メモリ解放
    }

    // 2. 新しいシーンに入れ替える
    currentScene = newScene;

    // 3. 新しいシーンの初期化（OnEnter呼び出し）
    if (currentScene) {
        currentScene->OnEnter(this);
    }
}

// 入力処理（シーンへ委譲）
void Game::HandleEvents() {
    if (currentScene) {
        currentScene->HandleEvents(this);
    }
}

// 更新処理（シーンへ委譲）
void Game::Update() {
    if (currentScene) {
        currentScene->Update(this);
    }
}

// 描画処理（シーンへ委譲）
void Game::Render() {
    // 画面をクリア
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // 現在のシーンを描画
    if (currentScene) {
        currentScene->Render(this);
    }

    // 画面を更新（フリップ）
    SDL_RenderPresent(renderer);
}

// 終了処理
void Game::Clean() {
    // 最後のシーンを片付ける
    if (currentScene) {
        currentScene->OnExit(this);
        delete currentScene;
    }

    // フォント解放
    if (font) {
        TTF_CloseFont(font);
    }

    // SDL関連の終了
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

// 共通機能：文字描画
void Game::DrawText(const char* text, int x, int y, SDL_Color color) {
    if (!font) return;

    // 文字からサーフェスを作成
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    if (!surface) return;

    // サーフェスからテクスチャを作成
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    // 描画範囲の設定
    SDL_Rect destRect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &destRect);

    // メモリ解放（重要）
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}