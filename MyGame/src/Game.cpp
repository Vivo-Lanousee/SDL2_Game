#include "Game.h"
#include <iostream>
#include <cmath>

#include "Player.h"

#include "Bullet.h"

Game::Game() : isRunning(false), window(nullptr), renderer(nullptr) {}

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
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // 背景黒
                isRunning = true;
            }
        }

        player = new Player(350, 250); // 変数に入れておく
        gameObjects.push_back(player); // リストにも入れる
    }
    else {
        return false;
    }
    return true;
}

/// <summary>
/// キー入力処理
/// </summary>
void Game::HandleEvents() {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
    case SDL_QUIT:
        isRunning = false;
        break;
        // キー入力処理
    case SDL_MOUSEBUTTONDOWN:
        if (event.button.button == SDL_BUTTON_LEFT) {
            // プレイヤーの中心位置を計算
            float spawnX = player->x + (player->width / 2);
            float spawnY = player->y + (player->height / 2);

            // プレイヤーが今向いている角度を取得（Player.hにGetAngleが必要）
            // ※もしPlayer.hにGetAngleがない場合は、次の補足を読んでください

            // 角度計算をここでもう一度やってもOK（とりあえず動かすために）
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            double radian = atan2(mouseY - spawnY, mouseX - spawnX);
            double angle = radian * 180.0 / 3.14159265;

            // 弾を生成してリストに追加！
            gameObjects.push_back(new Bullet(spawnX, spawnY, angle));
        }
        break;
    }
}

/// <summary>
/// 毎秒計算
/// </summary>
void Game::Update() {
    // UnityのUpdate()にあたる部分
    // ここに「自動で動く敵」や「当たり判定」などを書く

    
    for (auto obj : gameObjects) {
        obj->Update();
    }
}

/// <summary>
/// 描画
/// </summary>
void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    //Renderを一斉に
    for (auto obj : gameObjects) {
        obj->Render(renderer);
    }

    SDL_RenderPresent(renderer);
}

void Game::Clean() {

    // C++ではnewしたものは必ずdelete！
    for (auto obj : gameObjects) {
        delete obj;
    }
    gameObjects.clear();

    SDL_DestroyRenderer(renderer); // 筆を片付ける
    SDL_DestroyWindow(window);     // 窓を閉じる
    SDL_Quit();
}