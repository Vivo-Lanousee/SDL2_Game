#include "Game.h"
#include <iostream>
#include <cmath>
#include <SDL_image.h>

#include "../TextureManager.h"
#include "../Constants.h"
#include "../Objects/Player.h"
#include "../Objects/Bullet.h"

// コンストラクタでテクスチャ変数を初期化（重要）
Game::Game() : isRunning(false), window(nullptr), renderer(nullptr),
playerTexture(nullptr), bulletTexture(nullptr) {
}

Game::~Game() {}

bool Game::Init(const char* title, int x, int y, int width, int height, bool fullscreen) {
    int flags = 0;
    if (fullscreen) {
        flags = SDL_WINDOW_FULLSCREEN;
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        window = SDL_CreateWindow(title, x, y, width, height, flags);
        if (window) {
            renderer = SDL_CreateRenderer(window, -1, 0);
            if (renderer) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                isRunning = true;
            }
        }
    }
    else {
        return false;
    }

    // SDL_imageの初期化（PNGを使えるようにする）
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cout << "IMG_Init Error: " << IMG_GetError() << std::endl;
        return false;
    }

    if (TTF_Init() == -1) {
        std::cout << "TTF_Init Error: " << TTF_GetError() << std::endl;
        return false;
    }

    // フォントの読み込み
    font = TTF_OpenFont("assets/fonts/PixelMplus10.ttf", 24);
    if (!font) {
        std::cout << "Failed to load font!" << std::endl;
    }

    if (renderer) {
        // 画像の読み込み
        playerTexture = IMG_LoadTexture(renderer, "assets/images/player.png");
        bulletTexture = IMG_LoadTexture(renderer, "bullet.png");

        // 画像の読み込みに失敗したらエラーを出す
        if (!playerTexture || !bulletTexture) {
            std::cout << "Texture Load Failed! Make sure png files are in the project folder." << std::endl;
        }

        // プレイヤー生成時にテクスチャを渡す
        player = new Player(350, 250, playerTexture);
        gameObjects.push_back(player);
    }

    return true;
}
void Game::DrawText(const char* text, int x, int y, SDL_Color color) {
    if (!font) return;
    // 文字からサーフェス（生画像）を作る
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    //描画する場所とサイズを決める
    SDL_Rect destRect = { x, y, surface->w, surface->h }; // 文字のサイズに合わせる
    SDL_RenderCopy(renderer, texture, NULL, &destRect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void Game::HandleEvents() {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
    case SDL_QUIT:
        isRunning = false;
        break;

    case SDL_MOUSEBUTTONDOWN:
        if (event.button.button == SDL_BUTTON_LEFT) {
            // プレイヤーの中心位置を計算
            float spawnX = player->x + (player->width / 2) - 5; // -5は弾の半分のサイズ(微調整)
            float spawnY = player->y + (player->height / 2) - 5;

            // 角度計算
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            // プレイヤー中心からの角度
            float centerX = player->x + (player->width / 2);
            float centerY = player->y + (player->height / 2);
            double radian = atan2(mouseY - centerY, mouseX - centerX);
            double angle = radian * 180.0 / 3.14159265;

            //　弾を生成してリストに追加（テクスチャも渡す！）
            gameObjects.push_back(new Bullet(spawnX, spawnY, angle, bulletTexture));
        }
        break;
    }
}

void Game::Update() {
    for (auto obj : gameObjects) {
        obj->Update();
    }
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (auto obj : gameObjects) {
        obj->Render(renderer);
    }

    SDL_RenderPresent(renderer);


    SDL_Color white = { 255, 255, 255, 255 }; // 白色
    DrawText("SCORE: 100", 10, 10, white);

    SDL_RenderPresent(renderer);
}

void Game::Clean() {
    // オブジェクトのメモリ解放
    for (auto obj : gameObjects) {
        delete obj;
    }
    gameObjects.clear();

    // ★追加：読み込んだ画像のメモリ解放
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(bulletTexture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    IMG_Quit(); // ★追加：SDL_imageの終了処理
}