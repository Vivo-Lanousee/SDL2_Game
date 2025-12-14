#include "PlayScene.h"
#include "../Core/Game.h"
#include "../TextureManager.h" // TextureManagerを使う場合
#include <iostream>
#include <cmath>

void PlayScene::OnEnter(Game* game) {
    std::cout << "Entering PlayScene..." << std::endl;

    // 1. 画像の読み込み（TextureManagerを使うと楽！）
    // TextureManagerがない場合は IMG_LoadTexture(game->GetRenderer(), "path") を使ってください
    playerTexture = TextureManager::LoadTexture("assets/images/player.png", game->GetRenderer());
    bulletTexture = TextureManager::LoadTexture("assets/images/bullet.png", game->GetRenderer());

    if (!playerTexture || !bulletTexture) {
        std::cout << "Failed to load textures in PlayScene!" << std::endl;
    }

    // 2. プレイヤー生成
    player = new Player(350, 250, playerTexture);
    gameObjects.push_back(player);
}

void PlayScene::OnExit(Game* game) {
    std::cout << "Exiting PlayScene..." << std::endl;

    // メモリのお片付け
    for (auto obj : gameObjects) {
        delete obj;
    }
    gameObjects.clear();

    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(bulletTexture);
}

void PlayScene::Update(Game* game) {
    // 全オブジェクトの更新
    for (auto obj : gameObjects) {
        obj->Update(game);
    }
}

void PlayScene::Render(Game* game) {
    // 全オブジェクトの描画
    for (auto obj : gameObjects) {
        obj->Render(game->GetRenderer());
    }

    // UI（スコアなど）の描画
    SDL_Color white = { 255, 255, 255, 255 };
    game->DrawText("SCORE: 100", 10, 10, white);
}

void PlayScene::HandleEvents(Game* game) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {

        // ×ボタンで終了
        if (event.type == SDL_QUIT) {
            game->Quit(); 
        }

        // クリックで弾発射
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                // プレイヤーがいない時は発射しない（安全装置）
                if (!player) return;

                // --- ここからは以前のGame.cppと同じ計算 ---
                float spawnX = player->x + (player->width / 2) - 5;
                float spawnY = player->y + (player->height / 2) - 5;

                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                // 「撃ってくれ！」と頼む
                Bullet* newBullet = player->Shoot(mouseX, mouseY, bulletTexture);

                // 弾が返ってきたら、シーンのリストに追加してあげる
                if (newBullet) {
                    gameObjects.push_back(newBullet);
                }
            }
        }
    }
}