#include "PlayScene.h"
#include "../Core/Game.h"
#include "../TextureManager.h"
#include <iostream>
#include <cmath>
#include <algorithm> // ★追加：remove_if を使うために必要

#include "../Objects/Block.h"
#include "../Core/Physics.h"

void PlayScene::OnEnter(Game* game) {
    std::cout << "Entering PlayScene..." << std::endl;

    playerTexture = TextureManager::LoadTexture("assets/images/player.png", game->GetRenderer());
    bulletTexture = TextureManager::LoadTexture("assets/images/bullet.png", game->GetRenderer());

    if (!playerTexture || !bulletTexture) {
        std::cout << "Failed to load textures!" << std::endl;
    }

    // 1. プレイヤー生成
    player = new Player(100, 100, playerTexture);
    // Player.h で name="Player" と設定されている前提
    gameObjects.push_back(player);

    // 2. 地面と足場生成
    gameObjects.push_back(new Block(0, 500, 800, 50));
    gameObjects.push_back(new Block(200, 350, 200, 30));
    gameObjects.push_back(new Block(500, 250, 150, 30));
}

void PlayScene::OnExit(Game* game) {
    std::cout << "Exiting PlayScene..." << std::endl;
    for (auto obj : gameObjects) {
        delete obj;
    }
    gameObjects.clear();
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(bulletTexture);
}

void PlayScene::Update(Game* game) {
    // 1. プレイヤーの更新
    player->Update(game);
    player->ApplyPhysics();
    player->isGrounded = false;

    // 2. トリガー（弾など）の当たり判定
    // シーンは「当たったこと」を伝えるだけで、どうなるか（消えるか等）はオブジェクトに任せる
    for (auto obj : gameObjects) {
        // 自分がトリガーなら、誰かに当たっているか調べる
        if (obj->isTrigger) {
            for (auto target : gameObjects) {
                if (obj == target) continue; // 自分自身は無視

                // 物理判定のみ実施（CheckAABB）
                if (Physics::CheckAABB(obj, target)) {
                    // ★重要：ここで「当たったよ！」と本人に伝える
                    // Bulletクラスなら、この中で「壁なら isDead = true」にする処理が走る
                    obj->OnTriggerEnter(target);
                }
            }
        }
    }

    // 3. プレイヤーの物理衝突解決（壁・床）
    for (auto obj : gameObjects) {
        if (obj == player) continue;
        if (obj->isTrigger) continue; // 弾やコインには乗れない

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

    // ★5. お掃除タイム（isDeadフラグが立ったオブジェクトをまとめて消去）
    // erase-remove イディオムと呼ばれるC++の定石です
    auto it = std::remove_if(gameObjects.begin(), gameObjects.end(), [](GameObject* obj) {
        if (obj->isDead) {
            delete obj;  // メモリ解放
            return true; // リストから外す対象にする
        }
        return false;
        });
    // 実際にリストから削除
    gameObjects.erase(it, gameObjects.end());
}

void PlayScene::Render(Game* game) {
    for (auto obj : gameObjects) {
        obj->Render(game->GetRenderer());
    }

    SDL_Color white = { 255, 255, 255, 255 };
    game->DrawText("L-Click: Shoot / R-Click: Raycast Test", 10, 10, white);
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

            // 左クリック：射撃
            if (event.button.button == SDL_BUTTON_LEFT) {
                if (!player) return;
                Bullet* newBullet = player->Shoot(mx, my, bulletTexture);
                if (newBullet) {
                    gameObjects.push_back(newBullet);
                }
            }

            // 右クリック：レイキャスト（視線判定）テスト
            if (event.button.button == SDL_BUTTON_RIGHT) {
                std::cout << "--- Raycast Test ---" << std::endl;

                float startX = player->x + player->width / 2;
                float startY = player->y + player->height / 2;
                bool hitWall = false;

                for (auto obj : gameObjects) {
                    if (obj == player) continue;
                    if (obj->isTrigger) continue;

                    if (Physics::LineVsAABB(startX, startY, mx, my, obj)) {
                        std::cout << "[Block] detected!" << std::endl;
                        hitWall = true;
                    }
                }
                if (!hitWall) std::cout << "Clear!" << std::endl;
            }
        }
    }
}