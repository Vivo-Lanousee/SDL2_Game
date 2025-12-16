#include "PlayScene.h"
#include "../Core/Game.h"
#include "../TextureManager.h" // 修正済みのヘッダー
#include <iostream>
#include <algorithm> // std::remove_if

#include "../Objects/Block.h"
#include "../Core/Physics.h"
#include "../Core/Camera.h"
#include "../UI/TextRenderer.h"

void PlayScene::OnEnter(Game* game) {
    std::cout << "Entering PlayScene..." << std::endl;

    // ★変更点1：テクスチャ読み込み
    // TextureManager は SharedTexturePtr (shared_ptr) を返すようになったので、
    // reset() ではなく '=' で代入します。
    playerTexture = TextureManager::LoadTexture("assets/images/player.png", game->GetRenderer());
    bulletTexture = TextureManager::LoadTexture("assets/images/bullet.png", game->GetRenderer());

    if (!playerTexture || !bulletTexture) {
        std::cout << "Failed to load textures!" << std::endl;
    }

    // カメラ生成
    camera = std::make_unique<Camera>(800, 600);
    camera->limitX = 2000;
    camera->limitY = 1000;

    // プレイヤー生成
    auto playerPtr = std::make_unique<Player>(
        100,
        100,
        playerTexture.get(),
        bulletTexture.get(),
        camera.get()
    );

    // ★変更点2：GUI用に名前をつける
    playerPtr->name = "Player";

    player = playerPtr.get();
    gameObjects.push_back(std::move(playerPtr));

    // ★変更点3：ブロックにも名前をつけて生成

    // 床
    auto ground = std::make_unique<Block>(0, 500, 800, 50);
    ground->name = "Ground";
    gameObjects.push_back(std::move(ground));

    // 足場1
    auto platform1 = std::make_unique<Block>(200, 350, 200, 30);
    platform1->name = "Platform 1";
    gameObjects.push_back(std::move(platform1));

    // 足場2
    auto platform2 = std::make_unique<Block>(500, 250, 150, 30);
    platform2->name = "Platform 2";
    gameObjects.push_back(std::move(platform2));

    // 足場3
    auto platform3 = std::make_unique<Block>(1000, 400, 200, 30);
    platform3->name = "Platform 3";
    gameObjects.push_back(std::move(platform3));
}

void PlayScene::OnExit(Game* game) {
    std::cout << "Exiting PlayScene..." << std::endl;
    gameObjects.clear();
}

void PlayScene::Update(Game* game) {
    // 1. プレイヤーの更新
    if (player) { // playerが死んで消えている可能性を考慮してチェック
        player->Update(game);
        player->ApplyPhysics();
        player->isGrounded = false;

        if (camera) {
            camera->Follow(player);
        }
    }

    // 2. トリガーの当たり判定
    for (auto& obj : gameObjects) {
        if (obj->isTrigger) {
            for (auto& target : gameObjects) {
                if (obj.get() == target.get()) continue;

                if (Physics::CheckAABB(obj.get(), target.get())) {
                    obj->OnTriggerEnter(target.get());
                }
            }
        }
    }

    // 3. プレイヤーの物理衝突解決
    if (player) {
        for (auto& obj : gameObjects) {
            if (obj.get() == player) continue;
            if (obj->isTrigger) continue;

            if (Physics::ResolveCollision(player, obj.get())) {
                player->isGrounded = true;
            }
        }
    }

    // 4. その他のオブジェクト更新
    for (auto& obj : gameObjects) {
        if (obj.get() != player) {
            obj->Update(game);
        }
    }

    // 5. お掃除タイム
    auto it = std::remove_if(gameObjects.begin(), gameObjects.end(),
        [](const std::unique_ptr<GameObject>& obj) {
            return obj->isDead;
        });

    // もしプレイヤーが削除対象ならポインタをnullにする（ダングリングポインタ防止）
    for (auto it2 = it; it2 != gameObjects.end(); ++it2) {
        if (it2->get() == player) {
            player = nullptr;
        }
    }

    gameObjects.erase(it, gameObjects.end());

    // 6. 新しく生まれたオブジェクトを回収
    // ★★★ 修正箇所: Game::GetPendingObjects の戻り値の型が変わったため修正 ★★★
    std::vector<std::unique_ptr<GameObject>>& newObjs = game->GetPendingObjects();

    // unique_ptr ごと PlayScene の gameObjects に移動 (move) する
    for (auto& obj : newObjs) {
        gameObjects.push_back(std::move(obj));
    }

    // pendingObjects の中身を移動したので、Game側のリストはクリアする
    game->ClearPendingObjects();
    // ★★★ 修正終わり ★★★
}

void PlayScene::Render(Game* game) {
    for (auto& obj : gameObjects) {
        obj->Render(game->GetRenderer(), camera.get());
    }

    SDL_Color white = { 255, 255, 255, 255 };
    // UIテキストも少し更新
    TextRenderer::Draw(game->GetRenderer(), "WASD: Move | Mouse: Debug GUI", 10, 10, white);
}

void PlayScene::HandleEvents(Game* game) {
    SDL_Event event;
    // ImGui のイベント処理は Game.cpp 側で一括で行われているため、ここではゲーム固有のイベントを処理
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            game->Quit();
        }

        if (event.type == SDL_MOUSEBUTTONDOWN) {
            // 右クリックでのレイキャストテスト
            if (event.button.button == SDL_BUTTON_RIGHT) {
                std::cout << "--- Raycast Test ---" << std::endl;
                if (!camera || !player) return;

                int mx, my;
                SDL_GetMouseState(&mx, &my);
                SDL_FPoint worldPoint = camera->ScreenToWorld(mx, my);

                float startX = player->x + player->width / 2;
                float startY = player->y + player->height / 2;
                bool hitWall = false;

                for (auto& obj : gameObjects) {
                    if (obj.get() == player) continue;
                    if (obj->isTrigger) continue;

                    if (Physics::LineVsAABB(startX, startY, worldPoint.x, worldPoint.y, obj.get())) {
                        std::cout << "[Block] detected: " << obj->name << std::endl; // 名前を表示
                        hitWall = true;
                    }
                }
                if (!hitWall) std::cout << "Clear!" << std::endl;
            }
        }
    }
}