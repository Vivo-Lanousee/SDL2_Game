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
#include "../Core/GameSession.h"

void PlayScene::OnEnter(Game* game) {
    std::cout << "Entering PlayScene..." << std::endl;

    playerTexture = TextureManager::LoadTexture("assets/images/player.png", game->GetRenderer());
    bulletTexture = TextureManager::LoadTexture("assets/images/bullet.png", game->GetRenderer());

    // カメラ生成
    camera = std::make_unique<Camera>(800, 600);
    camera->limitX = 5000;
    camera->limitY = 2000;

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

    // 地面生成
    auto ground = std::make_unique<Block>(0, 500, 5000, 50);
    ground->name = "Ground";
    gameObjects.push_back(std::move(ground));

    // セッション初期化
    GameSession::GetInstance().ResetSession();
}

void PlayScene::OnExit(Game* game) {
    player = nullptr;
    gameObjects.clear();
}

void PlayScene::OnUpdate(Game* game) {
    // GameObject::Update は Scene::Update 内で自動実行されるため、
    // ここではカメラの追従のみを行います。
    if (player && !player->isDead) {
        if (camera) {
            camera->Follow(player);
        }
    }
    else {
        player = nullptr;
    }
}

void PlayScene::HandleEvents(Game* game, SDL_Event* event) {
    if (event->type == SDL_QUIT) {
        game->Quit();
    }
}

void PlayScene::Render(Game* game) {
    SDL_Renderer* renderer = game->GetRenderer();
    SDL_SetRenderDrawColor(renderer, 40, 40, 50, 255);
    SDL_RenderClear(renderer);

    for (auto& obj : gameObjects) {
        obj->RenderWithCamera(renderer, camera.get());
    }

    SDL_Color white = { 255, 255, 255, 255 };
    TextRenderer::Draw(renderer, "WASD: Move | Mouse: Shoot", 10, 10, white);
}