#include "EditorScene.h"
#include "../Core/Game.h"
#include "../Core/Time.h"
#include "../Core/InputHandler.h"
#include "../Editor/EditorGUI.h"
#include "imgui.h" // WantCaptureMouseの判定に必要
#include <iostream>

EditorScene::EditorScene()
    : selectedObject(nullptr)
{
    // エディタ専用のカメラを初期化
    camera = std::make_unique<Camera>(800, 600);

    // 初期経路データ (テスト用)
    enemyPath.push_back({ 100.0f, 100.0f });
    enemyPath.push_back({ 700.0f, 100.0f });
    enemyPath.push_back({ 700.0f, 500.0f });
}

void EditorScene::OnEnter(Game* game) {
    std::cout << "Entering Editor Scene." << std::endl;
    // エディタモードに設定（GUIの配置などがEDITOR用になる）
    EditorGUI::SetMode(EditorGUI::Mode::EDITOR);
}

void EditorScene::OnExit(Game* game) {
    std::cout << "Exiting Editor Scene." << std::endl;
    // 次のシーンに影響を与えないよう、モードをリセット
    EditorGUI::SetMode(EditorGUI::Mode::GAME);
    // 選択状態もクリアしておく
    EditorGUI::selectedObject = nullptr;
}

void EditorScene::HandleEvents(Game* game, SDL_Event* event) {
    // 1. まず ImGui にイベントを渡す
    // これにより、GUIウィンドウ上のクリックなどが処理される
    EditorGUI::HandleEvents(event);

    // 2. ImGuiがマウスやキーボードを「使いたい（Capture中）」と言っているか確認
    ImGuiIO& io = ImGui::GetIO();

    // GUIを操作している場合は、背後のエディタシーン（カメラ操作やオブジェクト選択）の処理をスキップ
    if (io.WantCaptureMouse || io.WantCaptureKeyboard) {
        return;
    }

    // 3. エディタ独自の操作ロジック
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        if (event->button.button == SDL_BUTTON_LEFT) {
            // 例: クリックした位置のオブジェクトを選択するロジック
            float mouseX = (float)event->button.x + camera->x;
            float mouseY = (float)event->button.y + camera->y;

            bool found = false;
            for (auto& obj : gameObjects) {
                // 簡易的な当たり判定
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

    // キー入力によるカメラ操作などは InputHandler を経由するか、ここで行う
}

void EditorScene::Update(Game* game) {
    // エディタでは自動追従させず、自由移動させる場合は nullptr を渡す
    camera->Follow(nullptr);
}

void EditorScene::Render(Game* game) {
    SDL_Renderer* renderer = game->GetRenderer();

    // 1. エディタの背景色 (ゲームと区別するため暗いグレー)
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderClear(renderer);

    // 2. 経路の描画 (デバッグ用/エディタ機能)
    SDL_SetRenderDrawColor(renderer, 255, 100, 0, 255); // オレンジ色
    if (enemyPath.size() > 1) {
        for (size_t i = 0; i < enemyPath.size() - 1; ++i) {
            // カメラ座標系に変換して描画
            float p1x = enemyPath[i].x - camera->x;
            float p1y = enemyPath[i].y - camera->y;
            float p2x = enemyPath[i + 1].x - camera->x;
            float p2y = enemyPath[i + 1].y - camera->y;

            SDL_RenderDrawLineF(renderer, p1x, p1y, p2x, p2y);

            // ウェイポイントの矩形描画
            SDL_Rect pointRect = { (int)p1x - 5, (int)p1y - 5, 10, 10 };
            SDL_RenderFillRect(renderer, &pointRect);
        }
        // 最後のポイント
        SDL_FPoint lastPoint = enemyPath.back();
        SDL_Rect lastRect = { (int)(lastPoint.x - camera->x) - 5, (int)(lastPoint.y - camera->y) - 5, 10, 10 };
        SDL_RenderFillRect(renderer, &lastRect);
    }

    // 3. ゲームオブジェクトの描画
    for (const auto& obj : gameObjects) {
        obj->RenderWithCamera(renderer, camera.get());
    }

    // 4. EditorGUI の描画
    // この関数の中で ImGui::NewFrame() から RenderDrawData() までが完結する。
    // シーンの最後に呼ぶことで、UIが常に最前面に表示される。
    EditorGUI::Render(renderer, this);
}