#include "PlayScene.h"
#include "../Core/Game.h"
#include "../TextureManager.h"
#include <iostream>
#include <cmath>

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
    player->useGravity = true;
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
    // 1. プレイヤーの入力処理
    player->Update(game);

    // 2. 物理移動（重力適用）
    player->ApplyPhysics();
    player->isGrounded = false;

    // ★追加：弾（Trigger）の当たり判定と削除処理
    // リストからオブジェクトを削除する場合、普通のfor文だとバグるのでイテレータを使います
    auto it = gameObjects.begin();
    while (it != gameObjects.end()) {
        GameObject* obj = *it;

        // 「これは弾（Trigger）か？」
        if (obj->isTrigger) {
            bool hit = false;

            // 他のすべてのオブジェクトと衝突チェック
            for (auto target : gameObjects) {
                if (obj == target) continue;       // 自分自身は無視
                if (target->isTrigger) continue;   // 弾同士は無視

                // 弾が出た瞬間にプレイヤーに当たらないように除外する
                if (target == player) continue;

                // 物理演算なしの単純な重なり判定 (CheckAABB)
                if (Physics::CheckAABB(obj, target)) {
                    hit = true; // 当たった！
                    break;
                }
            }

            // 当たっていたら削除する
            if (hit) {
                delete obj;                 // メモリ解放
                it = gameObjects.erase(it); // リストから削除し、イテレータを進める
                continue;                   // 次のループへ（下の++itをスキップ）
            }
        }
        ++it;
    }


    // 3. プレイヤーの衝突解決（壁・床）
    for (auto obj : gameObjects) {
        if (obj == player) continue;
        if (obj->isTrigger) continue; // 弾などのTriggerには乗れないようにする

        // 物理演算（押し戻し＆着地判定）
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

                // プレイヤーの中心座標
                float startX = player->x + player->width / 2;
                float startY = player->y + player->height / 2;

                bool hitWall = false;
                for (auto obj : gameObjects) {
                    if (obj == player) continue;
                    if (obj->isTrigger) continue; // 弾は無視して壁だけ調べる

                    // 線分判定 (Start -> MousePosition)
                    if (Physics::LineVsAABB(startX, startY, mx, my, obj)) {
                        std::cout << "[Block] detected between player and mouse!" << std::endl;
                        hitWall = true;
                        // ここで break すれば「一番手前の壁」だけ検知できる
                    }
                }

                if (!hitWall) {
                    std::cout << "Clear line of sight!" << std::endl;
                }
            }
        }
    }
}