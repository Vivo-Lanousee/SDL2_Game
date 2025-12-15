#include "PlayScene.h"
#include "../Core/Game.h"
#include "../TextureManager.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <memory>

#include "../Objects/Block.h"
#include "../Core/Physics.h"
#include "../Core/Camera.h"
#include "../UI/TextRenderer.h"

void PlayScene::OnEnter(Game* game) {
    std::cout << "Entering PlayScene..." << std::endl;

    playerTexture.reset(TextureManager::LoadTexture("assets/images/player.png", game->GetRenderer()));
    bulletTexture.reset(TextureManager::LoadTexture("assets/images/bullet.png", game->GetRenderer()));

    if (!playerTexture || !bulletTexture) {
        std::cout << "Failed to load textures!" << std::endl;
    }


    camera = std::make_unique<Camera>(800, 600);
    camera->limitX = 2000;
    camera->limitY = 1000;

    auto playerPtr = std::make_unique<Player>(
        100,
        100,
        playerTexture.get(),
        bulletTexture.get(), 
        camera.get()
    );

    player = playerPtr.get();
    gameObjects.push_back(std::move(playerPtr));

    // 3. 地面と足場生成
    gameObjects.push_back(std::make_unique<Block>(0, 500, 800, 50));
    gameObjects.push_back(std::make_unique<Block>(200, 350, 200, 30));
    gameObjects.push_back(std::make_unique<Block>(500, 250, 150, 30));
    gameObjects.push_back(std::make_unique<Block>(1000, 400, 200, 30));
}

void PlayScene::OnExit(Game* game) {
    std::cout << "Exiting PlayScene..." << std::endl;

    gameObjects.clear();
}

void PlayScene::Update(Game* game) {
    // 1. プレイヤーの更新
    player->Update(game);
    player->ApplyPhysics();
    player->isGrounded = false;

    if (camera) {
        camera->Follow(player);
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
    for (auto& obj : gameObjects) {
        if (obj.get() == player) continue;
        if (obj->isTrigger) continue;

        if (Physics::ResolveCollision(player, obj.get())) {
            player->isGrounded = true;
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
    gameObjects.erase(it, gameObjects.end());

    // 6. 新しく生まれたオブジェクトを回収
    std::vector<GameObject*>& newObjs = game->GetPendingObjects();
    for (auto obj : newObjs) {
        gameObjects.push_back(std::unique_ptr<GameObject>(obj));
    }
    game->ClearPendingObjects();
}

void PlayScene::Render(Game* game) {
    for (auto& obj : gameObjects) {
        obj->Render(game->GetRenderer(), camera.get());
    }

    SDL_Color white = { 255, 255, 255, 255 };
    TextRenderer::Draw(game->GetRenderer(), "WASD/Arrows: Move | R-Click: Raycast", 10, 10, white);
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

            if (event.button.button == SDL_BUTTON_RIGHT) {
                std::cout << "--- Raycast Test ---" << std::endl;

                SDL_FPoint worldPoint = camera->ScreenToWorld(mx, my);

                float startX = player->x + player->width / 2;
                float startY = player->y + player->height / 2;
                bool hitWall = false;

                for (auto& obj : gameObjects) {
                    if (obj.get() == player) continue;
                    if (obj->isTrigger) continue;

                    if (Physics::LineVsAABB(startX, startY, worldPoint.x, worldPoint.y, obj.get())) {
                        std::cout << "[Block] detected!" << std::endl;
                        hitWall = true;
                    }
                }
                if (!hitWall) std::cout << "Clear!" << std::endl;
            }
        }
    }
}