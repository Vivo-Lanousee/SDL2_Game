#include "EditorScene.h"
#include "../Core/Game.h"
#include "../Core/Time.h"
#include "../Core/InputHandler.h"
#include "../Core/Physics.h"
#include "../Editor/EditorGUI.h"
#include "../Objects/Block.h"
#include "imgui.h" 
#include <iostream>

EditorScene::EditorScene()
    : selectedObject(nullptr), testPlayer(nullptr)
{
    // エディタ専用のカメラを初期化
    camera = std::make_unique<Camera>(800, 600);

    // 初期経路データ
    enemyPath.push_back({ 100.0f, 100.0f });
    enemyPath.push_back({ 700.0f, 100.0f });
    enemyPath.push_back({ 700.0f, 500.0f });

    // テスト用に足場（地面）を1つ配置しておく
    auto ground = std::make_unique<Block>(0, 550, 1200, 50);
    ground->name = "Editor Ground";
    gameObjects.push_back(std::move(ground));
}

void EditorScene::OnEnter(Game* game) {
    std::cout << "Entering Editor Scene." << std::endl;
    EditorGUI::SetMode(EditorGUI::Mode::EDITOR);
    EditorGUI::isTestMode = false; // 最初はオフ

    // プレイヤーのテクスチャをエディタでもロードする
    playerTexture = TextureManager::LoadTexture("assets/images/player.png", game->GetRenderer());
    bulletTexture = TextureManager::LoadTexture("assets/images/bullet.png", game->GetRenderer());
}

void EditorScene::OnExit(Game* game) {
    std::cout << "Exiting Editor Scene." << std::endl;
    EditorGUI::SetMode(EditorGUI::Mode::GAME);
    EditorGUI::selectedObject = nullptr;
    testPlayer = nullptr;
    gameObjects.clear();
}

void EditorScene::HandleEvents(Game* game, SDL_Event* event) {
    EditorGUI::HandleEvents(event);

    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse || io.WantCaptureKeyboard) {
        return;
    }

    if (event->type == SDL_MOUSEBUTTONDOWN) {
        if (event->button.button == SDL_BUTTON_LEFT) {
            float mouseX = (float)event->button.x + camera->x;
            float mouseY = (float)event->button.y + camera->y;

            bool found = false;
            for (auto& obj : gameObjects) {
                if (mouseX >= obj->x && mouseX <= obj->x + obj->width &&
                    mouseY >= obj->y && mouseY <= obj->y + obj->height) {
                    EditorGUI::selectedObject = obj.get();
                    found = true;
                    break;
                }
            }
            if (!found) EditorGUI::selectedObject = nullptr;
        }
    }
}

void EditorScene::Update(Game* game) {
    float deltaTime = Time::deltaTime;

    // --- テストモードの処理 ---
    if (EditorGUI::isTestMode) {
        if (!testPlayer) {
            // プレイヤーがいない場合は生成
            auto pPtr = std::make_unique<Player>(100, 100, playerTexture.get(), bulletTexture.get(), camera.get());
            pPtr->name = "TestPlayer";
            testPlayer = pPtr.get();
            gameObjects.push_back(std::move(pPtr));
        }

        // プレイヤーの更新と物理演算 (PlaySceneと同じロジック)
        testPlayer->Update(game);
        Physics::ApplyPhysics(testPlayer, deltaTime);
        testPlayer->isGrounded = false;

        // エディタ内の配置物との衝突判定
        for (auto& obj : gameObjects) {
            if (obj.get() == testPlayer) continue;
            if (obj->isTrigger) continue;

            if (Physics::ResolveCollision(testPlayer, obj.get())) {
                testPlayer->isGrounded = true;
            }
        }

        // テストプレイ中はカメラを追従させる
        camera->Follow(testPlayer);

    }
    else {
        // テストモードがオフになったらプレイヤーを消す
        if (testPlayer) {
            testPlayer->isDead = true;
            testPlayer = nullptr;
        }
        camera->Follow(nullptr);
    }

    // オブジェクトのお掃除
    auto it = std::remove_if(gameObjects.begin(), gameObjects.end(),
        [](const std::unique_ptr<GameObject>& obj) { return obj->isDead; });
    gameObjects.erase(it, gameObjects.end());

    // pendingObjects からの回収（弾など）
    std::vector<std::unique_ptr<GameObject>>& newObjs = game->GetPendingObjects();
    for (auto& obj : newObjs) {
        gameObjects.push_back(std::move(obj));
    }
    game->ClearPendingObjects();
}

void EditorScene::Render(Game* game) {
    SDL_Renderer* renderer = game->GetRenderer();

    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderClear(renderer);

    // 経路の描画
    SDL_SetRenderDrawColor(renderer, 255, 100, 0, 255);
    if (enemyPath.size() > 1) {
        for (size_t i = 0; i < enemyPath.size() - 1; ++i) {
            float p1x = enemyPath[i].x - camera->x;
            float p1y = enemyPath[i].y - camera->y;
            float p2x = enemyPath[i + 1].x - camera->x;
            float p2y = enemyPath[i + 1].y - camera->y;
            SDL_RenderDrawLineF(renderer, p1x, p1y, p2x, p2y);
        }
    }

    // オブジェクトの描画
    for (const auto& obj : gameObjects) {
        obj->RenderWithCamera(renderer, camera.get());
    }

    EditorGUI::Render(renderer, this);
}