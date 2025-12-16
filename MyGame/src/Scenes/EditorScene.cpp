#include "EditorScene.h"
#include "../Core/Game.h"
#include "../Core/Time.h"
#include "../Core/InputHandler.h"
#include "../Editor/EditorGUI.h" // エディタUIの描画呼び出し
#include <iostream>

EditorScene::EditorScene()
    : selectedObject(nullptr)
{
    // エディタ専用のカメラを初期化 (仮の画面サイズ)
    camera = std::make_unique<Camera>(800, 600);

    // 初期経路データ (テスト用)
    enemyPath.push_back({ 100.0f, 100.0f });
    enemyPath.push_back({ 700.0f, 100.0f });
    enemyPath.push_back({ 700.0f, 500.0f });
}

void EditorScene::OnEnter(Game* game) {
    std::cout << "Entering Editor Scene." << std::endl;
    // エディタUIをこのシーンでのみ有効にする設定などを行う
    EditorGUI::SetMode(EditorGUI::Mode::EDITOR); // EditorGUIにモード設定機能を追加する前提

    // 初期オブジェクトの配置 (例: ベースとなるタワープレースメントなど)
}

void EditorScene::OnExit(Game* game) {
    std::cout << "Exiting Editor Scene." << std::endl;
    // 変更が保存されていない場合は警告を出すなどの処理
    EditorGUI::SetMode(EditorGUI::Mode::GAME); // EditorGUIにモード設定機能を追加する前提
}

void EditorScene::HandleEvents(Game* game) {
    // 独自のカメラ操作 (WASDやマウスドラッグでの移動)
    // InputHandler* input = game->GetInput();

    // マウス左クリックでオブジェクトを選択/配置/経路編集などのロジック
}

void EditorScene::Update(Game* game) {
    // エディタシーンでは、オブジェクトの動きは基本的に停止している（ただし、アニメーションなどは動かせる）
    // カメラの更新
    camera->Follow(nullptr); // 追従はせず、手動操作に任せる

    // 経路編集ロジックの更新 (pathEditor->Update() など)
}

void EditorScene::Render(Game* game) {
    SDL_Renderer* renderer = game->GetRenderer();

    // 1. エディタの背景色 (ゲームと区別するため明るい色に)
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderClear(renderer);

    // 2. 経路の描画 (デバッグ用/エディタ機能)
    SDL_SetRenderDrawColor(renderer, 255, 100, 0, 255); // 経路はオレンジ色
    if (enemyPath.size() > 1) {
        for (size_t i = 0; i < enemyPath.size() - 1; ++i) {
            // ワールド座標をカメラで変換して描画
            float p1x = enemyPath[i].x - camera->x;
            float p1y = enemyPath[i].y - camera->y;
            float p2x = enemyPath[i + 1].x - camera->x;
            float p2y = enemyPath[i + 1].y - camera->y;

            SDL_RenderDrawLineF(renderer, p1x, p1y, p2x, p2y);

            // ウェイポイントの描画
            SDL_Rect pointRect = { (int)p1x - 5, (int)p1y - 5, 10, 10 };
            SDL_RenderFillRect(renderer, &pointRect);
        }
        // 最後のポイントの描画
        SDL_FPoint lastPoint = enemyPath.back();
        SDL_Rect lastRect = { (int)(lastPoint.x - camera->x) - 5, (int)(lastPoint.y - camera->y) - 5, 10, 10 };
        SDL_RenderFillRect(renderer, &lastRect);
    }

    // 3. ゲームオブジェクトの描画 (あれば)
    for (const auto& obj : gameObjects) {
        obj->RenderWithCamera(renderer, camera.get());
    }

    // 4. EditorGUIは Game::Render() の中で描画されるため、ここでは省略
}