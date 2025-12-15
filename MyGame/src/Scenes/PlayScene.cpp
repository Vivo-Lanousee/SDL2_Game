#include "PlayScene.h"
#include "../Core/Game.h"
#include "../TextureManager.h"
#include <iostream>
#include <cmath>
#include <algorithm>

#include "../Objects/Block.h"
#include "../Core/Physics.h"
#include "../Core/Camera.h" // Cameraクラスを使うために必要

void PlayScene::OnEnter(Game* game) {
    std::cout << "Entering PlayScene..." << std::endl;

    playerTexture = TextureManager::LoadTexture("assets/images/player.png", game->GetRenderer());
    bulletTexture = TextureManager::LoadTexture("assets/images/bullet.png", game->GetRenderer());

    if (!playerTexture || !bulletTexture) {
        std::cout << "Failed to load textures!" << std::endl;
    }

    // 1. カメラ生成 (画面サイズ 800x600 とする)
    camera = new Camera(800, 600);
    camera->limitX = 2000; // ステージの広さ（横）
    camera->limitY = 1000; // ステージの広さ（縦）

    // 2. プレイヤー生成
    // ★修正：第5引数に作成した camera を渡す！
    // これでエラー「引数リストが一致しません」が解消されます
    player = new Player(100, 100, playerTexture, bulletTexture, camera);

    gameObjects.push_back(player);

    // 3. 地面と足場生成
    gameObjects.push_back(new Block(0, 500, 800, 50));
    gameObjects.push_back(new Block(200, 350, 200, 30));
    gameObjects.push_back(new Block(500, 250, 150, 30));

    // カメラのテスト用に遠くのブロックも置いてみる
    gameObjects.push_back(new Block(1000, 400, 200, 30));
}

void PlayScene::OnExit(Game* game) {
    std::cout << "Exiting PlayScene..." << std::endl;
    for (auto obj : gameObjects) {
        delete obj;
    }
    gameObjects.clear();

    // カメラの削除
    if (camera) {
        delete camera;
        camera = nullptr;
    }

    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(bulletTexture);
}

void PlayScene::Update(Game* game) {
    // 1. プレイヤーの更新
    player->Update(game);
    player->ApplyPhysics();
    player->isGrounded = false;

    // カメラをプレイヤーに追従させる
    if (camera) {
        camera->Follow(player);
    }

    // 2. トリガー（弾など）の当たり判定
    for (auto obj : gameObjects) {
        if (obj->isTrigger) {
            for (auto target : gameObjects) {
                if (obj == target) continue;
                if (Physics::CheckAABB(obj, target)) {
                    obj->OnTriggerEnter(target);
                }
            }
        }
    }

    // 3. プレイヤーの物理衝突解決
    for (auto obj : gameObjects) {
        if (obj == player) continue;
        if (obj->isTrigger) continue;

        if (Physics::ResolveCollision(player, obj)) {
            player->isGrounded = true;
        }
    }

    // 4. その他のオブジェクト更新
    for (auto obj : gameObjects) {
        if (obj != player) {
            obj->Update(game);
        }
    }

    // 5. お掃除タイム
    auto it = std::remove_if(gameObjects.begin(), gameObjects.end(), [](GameObject* obj) {
        if (obj->isDead) {
            delete obj;
            return true;
        }
        return false;
        });
    gameObjects.erase(it, gameObjects.end());

    // 6. 新しく生まれたオブジェクトを回収
    std::vector<GameObject*>& newObjs = game->GetPendingObjects();
    for (auto obj : newObjs) {
        gameObjects.push_back(obj);
    }
    game->ClearPendingObjects();
}

void PlayScene::Render(Game* game) {
    // 描画時にカメラを渡す
    // GameObject::Render が (renderer, camera) を受け取るように修正されている前提
    for (auto obj : gameObjects) {
        obj->Render(game->GetRenderer(), camera);
    }

    SDL_Color white = { 255, 255, 255, 255 };
    game->DrawText("WASD/Arrows: Move | R-Click: Raycast", 10, 10, white);
}

void PlayScene::HandleEvents(Game* game) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            game->Quit();
        }

        if (event.type == SDL_MOUSEBUTTONDOWN) {
            int mx, my;
            SDL_GetMouseState(&mx, &my);

            // 右クリック：レイキャスト（視線判定）テスト
            if (event.button.button == SDL_BUTTON_RIGHT) {
                std::cout << "--- Raycast Test ---" << std::endl;

                // ★修正：デバッグ機能もカメラ座標に対応させる
                // マウス位置(画面)をワールド座標に変換
                SDL_FPoint worldPoint = camera->ScreenToWorld(mx, my);

                float startX = player->x + player->width / 2;
                float startY = player->y + player->height / 2;
                bool hitWall = false;

                for (auto obj : gameObjects) {
                    if (obj == player) continue;
                    if (obj->isTrigger) continue;

                    // 変換後の座標を使って判定
                    if (Physics::LineVsAABB(startX, startY, worldPoint.x, worldPoint.y, obj)) {
                        std::cout << "[Block] detected!" << std::endl;
                        hitWall = true;
                    }
                }
                if (!hitWall) std::cout << "Clear!" << std::endl;
            }
        }
    }
}