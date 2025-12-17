#include "EditorGUI.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <string>
#include <iostream>
#include <map>
#include <algorithm> 
#include "../Core/GameParams.h" 
#include "../Scenes/Scene.h"
#include "../Objects/GameObject.h"
#include "../Core/ConfigManager.h" 

// 静的メンバ変数の実体
GameObject* EditorGUI::selectedObject = nullptr;
EditorGUI::Mode EditorGUI::currentMode = EditorGUI::Mode::GAME;
EditorGUI::ConfigViewMode EditorGUI::currentConfigView = EditorGUI::ConfigViewMode::NONE; // 初期はNONE

// --- 内部描画ヘルパー関数の宣言 (クラス外のstatic関数として定義) ---
static void DrawPlayerConfigPanel(GameParams& params);
static void DrawEnemyConfigPanel(GameParams& params);
static void DrawPhysicsConfigPanel(GameParams& params);

// --------------------------------------------------------------------------------------
// --- 必須関数の実装 ---

void EditorGUI::Init(SDL_Window* window, SDL_Renderer* renderer) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    GameParams& params = GameParams::GetInstance();
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
        DrawParameters(); // Config Launcher (ボタン配置)

        // Config Editor Windowの描画制御
        if (currentConfigView != ConfigViewMode::NONE) {
            DrawConfigEditorWindow();
        }
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

    if (selectedObject) {
        if (selectedObject->isDead) {
            selectedObject = nullptr;
            ImGui::End();
            return;
        }

        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Target: %s", selectedObject->name.c_str());
        ImGui::Separator();

        ImGui::Text("Transform");
        ImGui::DragFloat("X", &selectedObject->x, 1.0f);
        ImGui::DragFloat("Y", &selectedObject->y, 1.0f);
        ImGui::DragInt("W", &selectedObject->width, 1);
        ImGui::DragInt("H", &selectedObject->height, 1);

        ImGui::Separator();
        ImGui::Text("Physics");
        ImGui::Checkbox("Gravity", &selectedObject->useGravity);
        ImGui::DragFloat("Vel X", &selectedObject->velX, 0.1f);
        ImGui::DragFloat("Vel Y", &selectedObject->velY, 0.1f);

        ImGui::Separator();
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

// DrawParameters の実体 (Config Launcher - ボタン配置のみ)
void EditorGUI::DrawParameters() {
    ImGui::SetNextWindowPos(ImVec2(260, 0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(280, 0), ImGuiCond_Once);

    ImGui::Begin("🛠️ Config Launcher");

    // --- Config View Mode 切り替えボタン ---
    ImGui::Text("--- Config Editors ---");

    if (ImGui::Button("Player Config", ImVec2(-1, 25))) {
        currentConfigView = ConfigViewMode::PLAYER;
    }
    if (ImGui::Button("Enemy Config", ImVec2(-1, 25))) {
        currentConfigView = ConfigViewMode::ENEMY;
    }
    if (ImGui::Button("Physics Config", ImVec2(-1, 25))) {
        currentConfigView = ConfigViewMode::PHYSICS;
    }

    ImGui::Separator();

    // --- 全体保存ボタン ---
    if (ImGui::Button("Save ALL Config", ImVec2(-1, 30))) {
        if (ConfigManager::Save(GameParams::GetInstance())) {
            std::cout << "Configuration saved successfully!" << std::endl;
        }
        else {
            std::cerr << "Configuration save FAILED!" << std::endl;
        }
    }

    ImGui::End();
}

// ★★★ DrawConfigEditorWindow の実体 (メインの編集画面 - 安定版) ★★★
void EditorGUI::DrawConfigEditorWindow() {
    GameParams& params = GameParams::GetInstance();

    // ウィンドウタイトルを動的に設定
    std::string title = "⚙️ Global Configuration Editor";
    switch (currentConfigView) {
    case ConfigViewMode::PLAYER: title += " (Player)"; break;
    case ConfigViewMode::ENEMY: title += " (Enemy)"; break;
    case ConfigViewMode::PHYSICS: title += " (Physics)"; break;
    case ConfigViewMode::NONE: return;
    }

    ImGui::SetNextWindowPos(ImVec2(810, 0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(300, 600), ImGuiCond_Once);

    // ★ 修正箇所: ImGuiWindowFlags_NoClose, ImGuiWindowFlags_NoCollapse がエラーになるため、
    // 代わりに ImGuiWindowFlags_NoTitleBar を使用してトグル/閉じる機能を無効化する
    if (ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_NoTitleBar)) {

        switch (currentConfigView) {
        case ConfigViewMode::PLAYER:
            DrawPlayerConfigPanel(params);
            break;
        case ConfigViewMode::ENEMY:
            DrawEnemyConfigPanel(params);
            break;
        case ConfigViewMode::PHYSICS:
            DrawPhysicsConfigPanel(params);
            break;
        case ConfigViewMode::NONE:
            break;
        }

        ImGui::End();
    }
    // ウィンドウが閉じられることはないため、リセットロジックは不要
}
// ★★★ DrawConfigEditorWindow 終了 ★★★


// ★★★ 各設定パネルの描画関数 (ロジックを完全に記述) ★★★

static void DrawPlayerConfigPanel(GameParams& params) {
    // 1. 現在のグローバルパラメータの調整
    if (ImGui::CollapsingHeader("🏃 Active Player Stats", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderFloat("Move Speed (px/s)", &params.player.moveSpeed, 50.0f, 600.0f, "%.1f");
        ImGui::SliderFloat("Jump Velocity", &params.player.jumpVelocity, 100.0f, 1000.0f, "%.1f");
        ImGui::InputFloat("Max Health", &params.player.maxHealth, 10.0f, 50.0f, "%.0f");
    }

    // 2. プリセット管理UI
    if (ImGui::CollapsingHeader("📋 Player Presets Management")) {

        ImGui::Text("Current Active: %s", params.activePlayerPresetName.c_str());
        ImGui::Separator();

        if (ImGui::BeginCombo("Select Preset to Load", params.activePlayerPresetName.c_str())) {
            for (auto const& item : params.playerPresets) {
                const std::string& name = item.first;
                const PlayerParams& preset = item.second;

                bool isSelected = (params.activePlayerPresetName == name);
                if (ImGui::Selectable(name.c_str(), isSelected)) {
                    params.player = preset;
                    params.activePlayerPresetName = name;
                }
                if (isSelected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        static char newPresetName[128] = "NewPlayerConfig";
        ImGui::InputText("##PlayerPresetName", newPresetName, IM_ARRAYSIZE(newPresetName));
        ImGui::SameLine();

        if (ImGui::Button("Save Current as New/Overwrite")) {
            std::string name(newPresetName);
            if (!name.empty()) {
                params.playerPresets[name] = params.player;
                params.activePlayerPresetName = name;
                std::cout << "Player Preset saved: " << name << std::endl;
            }
        }
    }
}

static void DrawEnemyConfigPanel(GameParams& params) {
    // 1. 現在のグローバルパラメータの調整 (EnemyStats)
    if (ImGui::CollapsingHeader("👹 Active Enemy Stats", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderFloat("Base Speed (px/s)", &params.enemy.baseSpeed, 10.0f, 150.0f, "%.1f");
        ImGui::InputInt("Base Health", &params.enemy.baseHealth, 10, 50);
    }

    // 2. プリセット管理UI (EnemyPresets)
    if (ImGui::CollapsingHeader("📋 Enemy Presets Management")) {

        ImGui::Text("Current Active: %s", params.activeEnemyPresetName.c_str());
        ImGui::Separator();

        if (ImGui::BeginCombo("Select Preset to Load", params.activeEnemyPresetName.c_str())) {
            for (auto const& item : params.enemyPresets) {
                const std::string& name = item.first;
                const EnemyParams& preset = item.second;

                bool isSelected = (params.activeEnemyPresetName == name);
                if (ImGui::Selectable(name.c_str(), isSelected)) {
                    params.enemy = preset;
                    params.activeEnemyPresetName = name;
                }
                if (isSelected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        static char newPresetName[128] = "NewEnemyConfig";
        ImGui::InputText("##EnemyPresetName", newPresetName, IM_ARRAYSIZE(newPresetName));
        ImGui::SameLine();

        if (ImGui::Button("Save Current as New/Overwrite")) {
            std::string name(newPresetName);
            if (!name.empty()) {
                params.enemyPresets[name] = params.enemy;
                params.activeEnemyPresetName = name;
                std::cout << "Enemy Preset saved: " << name << std::endl;
            }
        }
    }
}

static void DrawPhysicsConfigPanel(GameParams& params) {
    if (ImGui::CollapsingHeader("🌍 Physics Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderFloat("Gravity (x100 px/s^2)", &params.physics.gravity, 0.0f, 50.0f, "%.2f");
        ImGui::SliderFloat("Terminal Velocity", &params.physics.terminalVelocity, 100.0f, 3000.0f, "%.0f");
    }
}
// ★★★ 修正終わり ★★★