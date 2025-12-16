#include "EditorGUI.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <string>
#include "../Core/GameParams.h" 
#include "../Scenes/Scene.h"
#include "../Objects/GameObject.h"
// ConfigManager をインクルード
#include "../Core/ConfigManager.h" 
#include <iostream>

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

    // ゲーム起動時に設定をロード
    GameParams& params = GameParams::GetInstance();
    // ★★★ 修正箇所 1: パス引数を削除 ★★★
    ConfigManager::Load(params);
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

void EditorGUI::Render(SDL_Renderer* renderer, Scene* currentScene) {
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    if (currentMode == Mode::EDITOR) {
        DrawHierarchy(currentScene);
        DrawInspector();
        DrawParameters();
    }

    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
}

void EditorGUI::DrawHierarchy(Scene* currentScene) {
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(250, 600), ImGuiCond_Once);

    ImGui::Begin("Hierarchy");
    // ... (中略) ...
    ImGui::End();
}

void EditorGUI::DrawInspector() {
    ImGui::SetNextWindowPos(ImVec2(550, 0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(250, 600), ImGuiCond_Once);

    ImGui::Begin("Inspector");
    // ... (中略) ...
    ImGui::End();
}

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
            100.0f, 1000.0f, "%.1f");

        ImGui::InputFloat("Max Health",
            &params.player.maxHealth,
            10.0f, 50.0f, "%.0f");
    }

    // --- 2. Physics Parameters ---
    if (ImGui::CollapsingHeader("Physics")) {
        ImGui::SliderFloat("Gravity (x100 px/s^2)",
            &params.physics.gravity,
            0.0f, 50.0f, "%.2f");

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

    ImGui::Separator();

    // 設定保存ボタン
    if (ImGui::Button("Save Config to File", ImVec2(-1, 0))) {
        // ★★★ 修正箇所 2: パス引数を削除 ★★★
        if (ConfigManager::Save(params)) {
            // 保存成功のメッセージ
            std::cout << "Configuration saved successfully!" << std::endl;
        }
        else {
            // 保存失敗のメッセージ
            std::cerr << "Configuration save FAILED!" << std::endl;
        }
    }

    ImGui::End();
}