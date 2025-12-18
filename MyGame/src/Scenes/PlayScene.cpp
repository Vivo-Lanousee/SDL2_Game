#include "PlayScene.h"
#include "../Core/Game.h"
#include "../Core/Time.h" 
#include "../TextureManager.h" 
#include <iostream>
#include <algorithm> 

#include "../Objects/Block.h"
#include "../Core/Physics.h"
#include "../Core/Camera.h"
#include "../UI/TextRenderer.h"

void PlayScene::OnEnter(Game* game) {
    std::cout << "Entering PlayScene..." << std::endl;

    // ★変更点1：テクスチャ読み込み
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

    playerPtr->name = "Player";
    player = playerPtr.get();
    gameObjects.push_back(std::move(playerPtr));

    // ブロック生成
    auto ground = std::make_unique<Block>(0, 500, 800, 50);
    ground->name = "Ground";
    gameObjects.push_back(std::move(ground));

    auto platform1 = std::make_unique<Block>(200, 350, 200, 30);
    platform1->name = "Platform 1";
    gameObjects.push_back(std::move(platform1));

    auto platform2 = std::make_unique<Block>(500, 250, 150, 30);
    platform2->name = "Platform 2";
    gameObjects.push_back(std::move(platform2));

    auto platform3 = std::make_unique<Block>(1000, 400, 200, 30);
    platform3->name = "Platform 3";
    gameObjects.push_back(std::move(platform3));
}

void PlayScene::OnExit(Game* game) {
    std::cout << "Exiting PlayScene..." << std::endl;
    gameObjects.clear();
}

void PlayScene::Update(Game* game) {
    float deltaTime = Time::deltaTime; // Physics::ApplyPhysics に必要

    // 1. プレイヤーの更新
    if (player) {
        player->Update(game);

        // ★★★ 修正箇所 1: Physics::ApplyPhysics を呼び出す ★★★
        Physics::ApplyPhysics(player, deltaTime);
        player->isGrounded = false;

        if (camera) {
            camera->Follow(player);
        }
    }

    // 2. その他のオブジェクト更新と物理演算
    for (auto& obj : gameObjects) {
        if (obj.get() != player) {
            obj->Update(game);
            // player以外の重力を持つオブジェクトにも Physics を適用
            Physics::ApplyPhysics(obj.get(), deltaTime);
        }
    }

    // 3. トリガーの当たり判定
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

    // 4. プレイヤーの物理衝突解決
    if (player) {
        for (auto& obj : gameObjects) {
            if (obj.get() == player) continue;
            if (obj->isTrigger) continue;

            if (Physics::ResolveCollision(player, obj.get())) {
                player->isGrounded = true;
            }
        }
    }

    // 5. お掃除タイム
    auto it = std::remove_if(gameObjects.begin(), gameObjects.end(),
        [](const std::unique_ptr<GameObject>& obj) {
            return obj->isDead;
        });

    for (auto it2 = it; it2 != gameObjects.end(); ++it2) {
        if (it2->get() == player) {
            player = nullptr;
        }
    }

    gameObjects.erase(it, gameObjects.end());

    // 6. 新しく生まれたオブジェクトを回収
    std::vector<std::unique_ptr<GameObject>>& newObjs = game->GetPendingObjects();

    for (auto& obj : newObjs) {
        gameObjects.push_back(std::move(obj));
    }

    game->ClearPendingObjects();
}

void PlayScene::Render(Game* game) {
    for (auto& obj : gameObjects) {
        // ★★★ 修正箇所 2: RenderWithCamera を呼び出す ★★★
        obj->RenderWithCamera(game->GetRenderer(), camera.get());
    }

    SDL_Color white = { 255, 255, 255, 255 };
    TextRenderer::Draw(game->GetRenderer(), "WASD: Move | Mouse: Adjust Params", 10, 10, white);
}

// ★重要修正箇所: 引数を (Game* game, SDL_Event* event) に変更
void PlayScene::HandleEvents(Game* game, SDL_Event* event) {
    // Game::HandleEvents の PollEvent ループから 1 つずつイベントが渡されます。

    if (event->type == SDL_QUIT) {
        game->Quit();
        return;
    }

    // プレイヤーのジャンプボタン（単発押し下げ）などのイベント処理が必要な場合
    if (player) {
        // player->HandleInput(event); などのメソッドがある場合はここで呼ぶ
    }
}