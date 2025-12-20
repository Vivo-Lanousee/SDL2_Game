#include "EditorScene.h"
#include "../Core/Game.h"
#include "../Core/Time.h"
#include "../Core/InputHandler.h"
#include "../Core/Physics.h"
#include "../Editor/EditorGUI.h"
#include "../Objects/Block.h"
#include "../Objects/Enemy.h"
#include "../Objects/Base.h" 
#include "../TextureManager.h"
#include "../Core/GameParams.h"
#include "../Core/GameSession.h" 
#include "../UI/TextRenderer.h"
#include "imgui.h" 
#include <iostream>
#include <algorithm>
#include <string>

EditorScene::EditorScene()
    : selectedObject(nullptr), testPlayer(nullptr)
{
    camera = std::make_unique<Camera>(800, 600);

    // 経路の初期値
    enemyPath.push_back({ 100.0f, 100.0f });
    enemyPath.push_back({ 700.0f, 100.0f });
    enemyPath.push_back({ 700.0f, 500.0f });
    enemyPath.push_back({ 100.0f, 500.0f });

    // 拠点の生成
    auto baseObj = std::make_unique<Base>(80, 300, 80, 250);
    baseObj->name = "Base Gate";
    gameObjects.push_back(std::move(baseObj));

    // 地面の生成
    auto ground = std::make_unique<Block>(0, 550, 5000, 50);
    ground->name = "Editor Ground";
    gameObjects.push_back(std::move(ground));
}

void EditorScene::OnEnter(Game* game) {
    std::cout << "Entering Editor Scene." << std::endl;
    EditorGUI::SetMode(EditorGUI::Mode::EDITOR);
    EditorGUI::isTestMode = false;

    playerTexture = TextureManager::LoadTexture("assets/images/player.png", game->GetRenderer());
    bulletTexture = TextureManager::LoadTexture("assets/images/bullet.png", game->GetRenderer());

    for (auto& obj : gameObjects) {
        Base* b = dynamic_cast<Base*>(obj.get());
        if (b) b->RefreshConfig(game->GetRenderer());
    }

    GameSession::GetInstance().ResetSession();
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

void EditorScene::SpawnTestEnemy(SDL_Renderer* renderer) {
    float spawnX = camera->x + 850.0f;
    float spawnY = 100.0f;

    auto enemy = std::make_unique<Enemy>(spawnX, spawnY, 64, 64, nullptr, enemyPath);
    enemy->name = "Test Enemy";
    enemy->RefreshConfig(renderer);

    gameObjects.push_back(std::move(enemy));
}

void EditorScene::OnUpdate(Game* game) {

    if (EditorGUI::isTestMode) {
        if (!testPlayer) {
            auto pPtr = std::make_unique<Player>(400, 100, playerTexture.get(), bulletTexture.get(), camera.get());
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
}

void EditorScene::Render(Game* game) {
    SDL_Renderer* renderer = game->GetRenderer();
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderClear(renderer);

    for (const auto& obj : gameObjects) {
        obj->RenderWithCamera(renderer, camera.get());
    }

    // 拠点HPバーの描画
    GameSession& session = GameSession::GetInstance();
    float hpRatio = (session.maxBaseHP > 0) ? (float)session.currentBaseHP / session.maxBaseHP : 0;

    SDL_Rect barBG = { 200, 20, 400, 20 };
    SDL_Rect barFG = { 200, 20, (int)(400 * hpRatio), 20 };

    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderFillRect(renderer, &barBG);

    if (hpRatio > 0.5f) SDL_SetRenderDrawColor(renderer, 0, 200, 50, 255);
    else if (hpRatio > 0.2f) SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);
    else SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    SDL_RenderFillRect(renderer, &barFG);

    TextRenderer::Draw(renderer, "GATE STATUS", barBG.x, barBG.y - 15, { 255, 255, 255, 255 });

    // UI (弾数)
    std::string ammoText = "Ammo: 0 / 0";
    SDL_Color textColor = { 200, 200, 200, 255 };

    if (testPlayer && !testPlayer->isDead) {
        int current = testPlayer->GetCurrentAmmo();
        int max = GameParams::GetInstance().gun.magazineSize;
        ammoText = "Ammo: " + std::to_string(current) + " / " + std::to_string(max);
        textColor = { 255, 255, 255, 255 };
        if (testPlayer->GetIsReloading()) {
            ammoText += " (RELOADING...)";
            textColor = { 255, 255, 0, 255 };
        }
        else if (current == 0) {
            textColor = { 255, 0, 0, 255 };
        }
    }

    TextRenderer::Draw(renderer, ammoText, 20, 550, textColor);
    EditorGUI::Render(renderer, this);
}