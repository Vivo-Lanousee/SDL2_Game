#include "EditorScene.h"
#include "../Core/Game.h"
#include "../Core/Time.h"
#include "../Core/InputHandler.h"
#include "../Core/Physics.h"
#include "../Editor/EditorGUI.h"
#include "../Objects/Block.h"
#include "../Objects/Enemy.h"
#include "../Objects/Base.h" // 拠点クラス
#include "../TextureManager.h"
#include "../Core/GameParams.h"
#include "../Core/GameSession.h" // セッション管理
#include "../UI/TextRenderer.h"
#include "imgui.h" 
#include <iostream>
#include <algorithm>
#include <string>

EditorScene::EditorScene()
    : selectedObject(nullptr), testPlayer(nullptr)
{
    camera = std::make_unique<Camera>(800, 600);

    // 経路の初期値（Linear移動の目標地点などに使用可能）
    enemyPath.push_back({ 100.0f, 100.0f });
    enemyPath.push_back({ 700.0f, 100.0f });
    enemyPath.push_back({ 700.0f, 500.0f });
    enemyPath.push_back({ 100.0f, 500.0f });

    // --- 拠点の生成 (マルフーシャ風に左側に配置) ---
    // エネミーの目標地点 (x=150) の少し後ろに配置
    auto baseObj = std::make_unique<Base>(80, 300, 80, 250);
    baseObj->name = "Base Gate";
    gameObjects.push_back(std::move(baseObj));

    // 地面の生成 (幅を広めに設定)
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

    // 拠点のテクスチャなどをパラメータから反映
    for (auto& obj : gameObjects) {
        Base* b = dynamic_cast<Base*>(obj.get());
        if (b) b->RefreshConfig(game->GetRenderer());
    }

    // ゲームセッション（動的ステータス）の初期化
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
    // --- エネミーの生成位置を調整 ---
    // カメラの右端の外側 (x + 850) かつ 空中 (y=100) に生成
    float spawnX = camera->x + 850.0f;
    float spawnY = 100.0f;

    auto enemy = std::make_unique<Enemy>(spawnX, spawnY, 64, 64, nullptr, enemyPath);
    enemy->name = "Test Enemy";

    // エネミー内部で useGravity = true と RefreshConfig が呼ばれることを想定
    enemy->RefreshConfig(renderer);

    gameObjects.push_back(std::move(enemy));
}

void EditorScene::Update(Game* game) {
    float deltaTime = Time::deltaTime;

    // --- 1. テストモードの管理 ---
    if (EditorGUI::isTestMode) {
        if (!testPlayer) {
            // プレイヤーも空中から落とす位置にスポーン
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

    // --- 2. 全オブジェクトの更新と物理適用 ---
    for (auto& obj : gameObjects) {
        obj->Update(game);

        // 重力が有効、または速度がある場合に物理演算（移動と落下の計算）を適用
        if (obj->useGravity || std::abs(obj->velX) > 0 || std::abs(obj->velY) > 0) {
            Physics::ApplyPhysics(obj.get(), deltaTime);
        }
    }

    // --- 3. 衝突解決 (接地判定) ---
    // プレイヤーとエネミー、両方に対して地面との当たり判定を行う
    for (auto& obj : gameObjects) {
        // 対象はテストプレイヤー、またはエネミー
        if (obj->name == "TestPlayer" || obj->name == "Enemy" || obj->name == "Test Enemy") {
            obj->isGrounded = false; // 判定前に一度空中状態にする

            for (auto& target : gameObjects) {
                // 自分自身やトリガーオブジェクト（弾など）とは衝突しない
                if (obj.get() == target.get() || target->isTrigger) continue;

                // 物理エンジンで衝突を解決し、押し戻しが発生したら接地とする
                if (Physics::ResolveCollision(obj.get(), target.get())) {
                    obj->isGrounded = true;
                }
            }
        }
    }

    // 新しいオブジェクト（弾など）の回収
    std::vector<std::unique_ptr<GameObject>>& newObjs = game->GetPendingObjects();
    for (auto& obj : newObjs) {
        gameObjects.push_back(std::move(obj));
    }
    game->ClearPendingObjects();

    // 死亡フラグの掃除
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

    // --- 拠点の総合HPバー描画 (画面上部固定) ---
    GameSession& session = GameSession::GetInstance();
    float hpRatio = (session.maxBaseHP > 0) ? (float)session.currentBaseHP / session.maxBaseHP : 0;

    int barWidth = 400;
    int barHeight = 20;
    int screenW = 800;
    SDL_Rect barBG = { (screenW - barWidth) / 2, 20, barWidth, barHeight };
    SDL_Rect barFG = { (screenW - barWidth) / 2, 20, (int)(barWidth * hpRatio), barHeight };

    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255); // 背景
    SDL_RenderFillRect(renderer, &barBG);

    // HPに応じて色を変更
    if (hpRatio > 0.5f) SDL_SetRenderDrawColor(renderer, 0, 200, 50, 255);
    else if (hpRatio > 0.2f) SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);
    else SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    SDL_RenderFillRect(renderer, &barFG);

    TextRenderer::Draw(renderer, "GATE STATUS", barBG.x, barBG.y - 15, { 255, 255, 255, 255 });

    // --- UI 描画 (弾数) ---
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