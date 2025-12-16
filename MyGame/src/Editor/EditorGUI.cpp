#include "EditorGUI.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <string>
#include "../Core/GameParams.h" // GameParams をインクルード
#include "../Scenes/Scene.h"
#include "../Objects/GameObject.h"

// 静的メンバ変数の実体
GameObject* EditorGUI::selectedObject = nullptr;
EditorGUI::Mode EditorGUI::currentMode = EditorGUI::Mode::GAME;

void EditorGUI::Init(SDL_Window* window, SDL_Renderer* renderer) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
}

void EditorGUI::HandleEvents(SDL_Event* event) {
    ImGui_ImplSDL2_ProcessEvent(event);
}

void EditorGUI::Clean() {
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void EditorGUI::SetMode(Mode newMode) {
    currentMode = newMode;
}

// ★★★ 修正箇所 2: Render 関数でモードに応じて描画を制御 ★★★
void EditorGUI::Render(SDL_Renderer* renderer, Scene* currentScene) {
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // EditorScene の場合のみ、HierarchyとInspectorを表示
    if (currentMode == Mode::EDITOR) {
        DrawHierarchy(currentScene);
        DrawInspector();
        DrawParameters();
    }
    else {
        // Gameモードの場合、全てのデバッグUIを非表示
    }

    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
}

void EditorGUI::DrawHierarchy(Scene* currentScene) {
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(250, 600), ImGuiCond_Once);

    ImGui::Begin("Hierarchy");
    if (currentScene) {
        auto& objects = currentScene->GetObjects();
        int index = 0;
        for (auto& obj : objects) {
            if (!obj) continue;
            ImGui::PushID(index);
            std::string label = obj->name.empty() ? "Object " + std::to_string(index) : obj->name;

            if (ImGui::Selectable(label.c_str(), selectedObject == obj.get())) {
                selectedObject = obj.get();
            }
            ImGui::PopID();
            index++;
        }
    }
    ImGui::End();
}

void EditorGUI::DrawInspector() {
    ImGui::SetNextWindowPos(ImVec2(550, 0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(250, 600), ImGuiCond_Once);

    ImGui::Begin("Inspector");

    if (selectedObject && selectedObject->isDead) {
        selectedObject = nullptr;
    }

    if (selectedObject) {
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Target: %s", selectedObject->name.c_str());
        ImGui::Separator();

        // Transform (既存のコードを維持)
        ImGui::Text("Transform");
        ImGui::DragFloat("X", &selectedObject->x, 1.0f);
        ImGui::DragFloat("Y", &selectedObject->y, 1.0f);
        ImGui::DragInt("W", &selectedObject->width, 1);
        ImGui::DragInt("H", &selectedObject->height, 1);

        // Physics (既存のコードを維持)
        ImGui::Separator();
        ImGui::Text("Physics");
        ImGui::Checkbox("Gravity", &selectedObject->useGravity);
        ImGui::DragFloat("Vel X", &selectedObject->velX, 0.1f);
        ImGui::DragFloat("Vel Y", &selectedObject->velY, 0.1f);

        // Delete (既存のコードを維持)
        ImGui::Separator();
        ImGui::Spacing();
        if (ImGui::Button("Delete Object", ImVec2(-1, 0))) {
            selectedObject->isDead = true;
            selectedObject = nullptr;
        }
    }
    else {
        ImGui::Text("Select object from Hierarchy.");
    }
    ImGui::End();
}

// ★★★ 修正箇所 3: DrawParameters のスライダー範囲とラベルを修正 ★★★
void EditorGUI::DrawParameters() {
    ImGui::SetNextWindowPos(ImVec2(260, 0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(280, 0), ImGuiCond_Once);

    ImGui::Begin("⚙️ Global Parameters");

    GameParams& params = GameParams::GetInstance();

    // --- 1. Player Parameters ---
    if (ImGui::CollapsingHeader("Player Stats")) {
        ImGui::SliderFloat("Move Speed (px/s)",
            &params.player.moveSpeed,
            50.0f, 600.0f, "%.1f");

        ImGui::SliderFloat("Jump Velocity",
            &params.player.jumpVelocity,
            100.0f, 1000.0f, "%.1f"); // 上限を拡大

        ImGui::InputFloat("Max Health",
            &params.player.maxHealth,
            10.0f, 50.0f, "%.0f");
    }

    // --- 2. Physics Parameters ---
    if (ImGui::CollapsingHeader("Physics")) {
        // GUIでは 9.8 などの値を扱い、内部で 100 倍されることをラベルで示す
        ImGui::SliderFloat("Gravity (x100 px/s^2)",
            &params.physics.gravity,
            0.0f, 50.0f, "%.2f"); // 調整上限を 50.0f (内部で 5000.0f) に拡大

        // 終端速度の上限を拡大
        ImGui::SliderFloat("Terminal Velocity",
            &params.physics.terminalVelocity,
            100.0f, 3000.0f, "%.0f");
    }

    // --- 3. Enemy Parameters ---
    if (ImGui::CollapsingHeader("Enemy Stats")) {
        ImGui::SliderFloat("Base Speed (px/s)",
            &params.enemy.baseSpeed,
            10.0f, 150.0f, "%.1f");

        ImGui::InputInt("Base Health",
            &params.enemy.baseHealth,
            10, 50);
    }

    ImGui::End();
}