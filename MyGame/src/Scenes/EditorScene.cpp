#include "EditorScene.h"
#include "../Core/Game.h"
#include "../Core/Time.h"
#include "../Core/InputHandler.h"
#include "../Core/Physics.h"
#include "../Editor/EditorGUI.h"
#include "../Objects/Block.h"
#include "../TextureManager.h"
#include "../Core/GameParams.h"
#include "../UI/TextRenderer.h"
#include "imgui.h" 
#include <iostream>
#include <algorithm>
#include <string>

EditorScene::EditorScene()
    : selectedObject(nullptr), testPlayer(nullptr)
{
    camera = std::make_unique<Camera>(800, 600);
    enemyPath.push_back({ 100.0f, 100.0f });
    enemyPath.push_back({ 700.0f, 100.0f });
    enemyPath.push_back({ 700.0f, 500.0f });

    auto ground = std::make_unique<Block>(0, 550, 1200, 50);
    ground->name = "Editor Ground";
    gameObjects.push_back(std::move(ground));
}

void EditorScene::OnEnter(Game* game) {
    std::cout << "Entering Editor Scene." << std::endl;
    EditorGUI::SetMode(EditorGUI::Mode::EDITOR);
    EditorGUI::isTestMode = false;

    playerTexture = TextureManager::LoadTexture("assets/images/player.png", game->GetRenderer());
    bulletTexture = TextureManager::LoadTexture("assets/images/bullet.png", game->GetRenderer());
}

void EditorScene::OnExit(Game* game) {
    EditorGUI::SetMode(EditorGUI::Mode::GAME);
    EditorGUI::selectedObject = nullptr;
    testPlayer = nullptr;
    gameObjects.clear();
}

void EditorScene::HandleEvents(Game* game, SDL_Event* event) {
    EditorGUI::HandleEvents(event);
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse || io.WantCaptureKeyboard) return;

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

    // --- 1. テストモードの管理 ---
    if (EditorGUI::isTestMode) {
        if (!testPlayer) {
            auto pPtr = std::make_unique<Player>(100, 100, playerTexture.get(), bulletTexture.get(), camera.get());
            pPtr->name = "TestPlayer";
            testPlayer = pPtr.get();
            gameObjects.push_back(std::move(pPtr));
        }
        camera->Follow(testPlayer);
    }
    else {
        if (testPlayer) {
            testPlayer->isDead = true;
            testPlayer = nullptr;
        }
        camera->Follow(nullptr);
    }

    // --- 2. 全オブジェクトの更新 ---
    for (auto& obj : gameObjects) {
        obj->Update(game);

        // 物理適用 (プレイヤーや弾など重力・速度が必要なもの)
        if (obj->useGravity || std::abs(obj->velX) > 0 || std::abs(obj->velY) > 0) {
            Physics::ApplyPhysics(obj.get(), deltaTime);
        }
    }

    // --- 3. 衝突解決 (テストプレイヤーのみ) ---
    if (testPlayer) {
        testPlayer->isGrounded = false;
        for (auto& obj : gameObjects) {
            if (obj.get() == testPlayer || obj->isTrigger) continue;
            if (Physics::ResolveCollision(testPlayer, obj.get())) {
                testPlayer->isGrounded = true;
            }
        }
    }

    // --- 4. 新しいオブジェクト（弾など）の回収 ---
    std::vector<std::unique_ptr<GameObject>>& newObjs = game->GetPendingObjects();
    for (auto& obj : newObjs) {
        gameObjects.push_back(std::move(obj));
    }
    game->ClearPendingObjects();

    // --- 5. お掃除 ---
    auto it = std::remove_if(gameObjects.begin(), gameObjects.end(),
        [](const std::unique_ptr<GameObject>& obj) { return obj->isDead; });
    gameObjects.erase(it, gameObjects.end());
}

void EditorScene::Render(Game* game) {
    SDL_Renderer* renderer = game->GetRenderer();
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderClear(renderer);

    // 経路描画
    SDL_SetRenderDrawColor(renderer, 255, 100, 0, 255);
    if (enemyPath.size() > 1) {
        for (size_t i = 0; i < enemyPath.size() - 1; ++i) {
            SDL_RenderDrawLineF(renderer, enemyPath[i].x - camera->x, enemyPath[i].y - camera->y,
                enemyPath[i + 1].x - camera->x, enemyPath[i + 1].y - camera->y);
        }
    }

    // オブジェクト描画
    for (const auto& obj : gameObjects) {
        obj->RenderWithCamera(renderer, camera.get());
    }

    // --- UI 描画 (弾数表示) ---
    std::string ammoText = "Ammo: 0 / 0";
    SDL_Color textColor = { 200, 200, 200, 255 }; // デフォルトはグレー

    if (testPlayer && !testPlayer->isDead) {
        int current = testPlayer->GetCurrentAmmo();
        int max = GameParams::GetInstance().gun.magazineSize;
        ammoText = "Ammo: " + std::to_string(current) + " / " + std::to_string(max);
        textColor = { 255, 255, 255, 255 }; // プレイヤーがいる時は白

        // リロード中の演出
        if (testPlayer->GetIsReloading()) {
            ammoText += " (RELOADING...)";
            textColor = { 255, 255, 0, 255 }; // リロード中は黄色
        }
        else if (current == 0) {
            textColor = { 255, 0, 0, 255 };   // 弾切れは赤
        }
    }

    // 左下に描画 (x=20, y=画面下部から少し上)
    TextRenderer::Draw(renderer, ammoText, 20, 550, textColor);

    EditorGUI::Render(renderer, this);
}