#include "EditorGUI.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <string>
#include <iostream>
#include <map>
#include <algorithm> 
#include <cstring> 
#include "../Core/GameParams.h" 
#include "../Scenes/Scene.h"
#include "../Objects/GameObject.h"
#include "../Core/ConfigManager.h" 

// 静的メンバ変数の実体
GameObject* EditorGUI::selectedObject = nullptr;
EditorGUI::Mode EditorGUI::currentMode = EditorGUI::Mode::GAME;
EditorGUI::ConfigViewMode EditorGUI::currentConfigView = EditorGUI::ConfigViewMode::NONE;

// --- 内部描画ヘルパー関数の宣言 ---
static void DrawPlayerConfigPanel(GameParams& params);
static void DrawEnemyConfigPanel(GameParams& params);
static void DrawPhysicsConfigPanel(GameParams& params);

// --------------------------------------------------------------------------------------

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
        DrawParameters(); // Launcher

        if (currentConfigView != ConfigViewMode::NONE) {
            DrawConfigEditorWindow();
        }
    }

    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
}

void EditorGUI::DrawHierarchy(Scene* currentScene) {
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(220, 780), ImGuiCond_Once);

    ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoCollapse);
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
    ImGui::SetNextWindowPos(ImVec2(890, 10), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(300, 350), ImGuiCond_Once);

    ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoCollapse);
    if (selectedObject) {
        if (selectedObject->isDead) { selectedObject = nullptr; ImGui::End(); return; }
        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Target: %s", selectedObject->name.c_str());
        ImGui::Separator();
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::DragFloat("X", &selectedObject->x, 1.0f);
            ImGui::DragFloat("Y", &selectedObject->y, 1.0f);
            ImGui::DragInt("W", &selectedObject->width, 1, 1, 1200);
            ImGui::DragInt("H", &selectedObject->height, 1, 1, 800);
        }
        if (ImGui::CollapsingHeader("Physics", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Checkbox("Gravity", &selectedObject->useGravity);
            ImGui::DragFloat("Vel X", &selectedObject->velX, 0.1f);
            ImGui::DragFloat("Vel Y", &selectedObject->velY, 0.1f);
        }
        ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 35);
        if (ImGui::Button("Delete Object", ImVec2(-1, 0))) { selectedObject->isDead = true; selectedObject = nullptr; }
    }
    else { ImGui::TextDisabled("(No object selected)"); }
    ImGui::End();
}

void EditorGUI::DrawParameters() {
    ImGui::SetNextWindowPos(ImVec2(240, 10), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(200, 220), ImGuiCond_Once);

    ImGui::Begin("🛠️ Launcher", nullptr, ImGuiWindowFlags_NoCollapse);
    ImGui::TextDisabled("Category Select:");
    if (ImGui::Button("🏃 Player", ImVec2(-1, 35))) currentConfigView = ConfigViewMode::PLAYER;
    if (ImGui::Button("👹 Enemy", ImVec2(-1, 35)))  currentConfigView = ConfigViewMode::ENEMY;
    if (ImGui::Button("🌍 Physics", ImVec2(-1, 35))) currentConfigView = ConfigViewMode::PHYSICS;

    ImGui::Separator();
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.4f, 0.6f, 0.6f));
    if (ImGui::Button("💾 SAVE ALL TO FILE", ImVec2(-1, 40))) {
        ConfigManager::Save(GameParams::GetInstance());
    }
    ImGui::PopStyleColor();
    ImGui::End();
}

void EditorGUI::DrawConfigEditorWindow() {
    GameParams& params = GameParams::GetInstance();
    ImGui::SetNextWindowPos(ImVec2(890, 370), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(300, 420), ImGuiCond_Once);

    std::string title = "⚙️ Editor";
    switch (currentConfigView) {
    case ConfigViewMode::PLAYER: title += " [Player]"; break;
    case ConfigViewMode::ENEMY:  title += " [Enemy]"; break;
    case ConfigViewMode::PHYSICS: title += " [Physics]"; break;
    default: return;
    }

    if (ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize)) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), title.c_str());
        ImGui::Separator();
        switch (currentConfigView) {
        case ConfigViewMode::PLAYER: DrawPlayerConfigPanel(params); break;
        case ConfigViewMode::ENEMY:  DrawEnemyConfigPanel(params); break;
        case ConfigViewMode::PHYSICS: DrawPhysicsConfigPanel(params); break;
        default: break;
        }
        ImGui::End();
    }
}

// --------------------------------------------------------------------------------------

static void DrawPlayerConfigPanel(GameParams& params) {
    static char nameBuf[64] = "";
    if (nameBuf[0] == '\0' && !params.activePlayerPresetName.empty()) {
        strncpy_s(nameBuf, params.activePlayerPresetName.c_str(), _TRUNCATE);
    }

    if (ImGui::CollapsingHeader("Edit Active Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderFloat("Speed", &params.player.moveSpeed, 50.0f, 600.0f, "%.1f");
        ImGui::SliderFloat("Jump", &params.player.jumpVelocity, 100.0f, 1000.0f, "%.1f");
        ImGui::InputFloat("Health", &params.player.maxHealth, 10.0f, 100.0f, "%.0f");
    }

    if (ImGui::CollapsingHeader("Presets", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::TextDisabled("Presets List (Click to Load):");
        std::string toDelete = "";

        ImGui::BeginChild("PlayerPresetList", ImVec2(0, 120), true);
        for (auto it = params.playerPresets.begin(); it != params.playerPresets.end(); ++it) {
            const std::string& name = it->first;
            const PlayerParams& data = it->second;

            ImGui::PushID(name.c_str());
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.2f, 0.2f, 1.0f));
            if (ImGui::Button("X", ImVec2(25, 0))) { toDelete = name; }
            ImGui::PopStyleColor();
            ImGui::SameLine();

            bool isCurrent = (params.activePlayerPresetName == name);
            if (isCurrent) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
            if (ImGui::Button(name.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
                params.player = data;
                params.activePlayerPresetName = name;
                strncpy_s(nameBuf, name.c_str(), _TRUNCATE);
            }
            if (isCurrent) ImGui::PopStyleColor();
            ImGui::PopID();
        }
        ImGui::EndChild();

        if (!toDelete.empty()) {
            params.playerPresets.erase(toDelete);
            if (params.activePlayerPresetName == toDelete) params.activePlayerPresetName = "";
        }

        ImGui::Separator();
        // ★ 修正箇所：newNameBuf から nameBuf に修正
        ImGui::InputText("Name", nameBuf, IM_ARRAYSIZE(nameBuf));
        float btnW = ImGui::GetContentRegionAvail().x * 0.48f;
        if (ImGui::Button("OVERWRITE", ImVec2(btnW, 30))) {
            params.playerPresets[nameBuf] = params.player;
            params.activePlayerPresetName = nameBuf;
        }
        ImGui::SameLine();
        if (ImGui::Button("SAVE AS NEW", ImVec2(btnW, 30))) {
            params.playerPresets[nameBuf] = params.player;
            params.activePlayerPresetName = nameBuf;
        }
    }
}

static void DrawEnemyConfigPanel(GameParams& params) {
    static char nameBuf[64] = "";
    if (nameBuf[0] == '\0' && !params.activeEnemyPresetName.empty()) {
        strncpy_s(nameBuf, params.activeEnemyPresetName.c_str(), _TRUNCATE);
    }

    if (ImGui::CollapsingHeader("Edit Active Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderFloat("Speed", &params.enemy.baseSpeed, 10.0f, 300.0f, "%.1f");
        ImGui::InputInt("Health", &params.enemy.baseHealth, 10, 500);
    }

    if (ImGui::CollapsingHeader("Presets", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::TextDisabled("Presets List (Click to Load):");
        std::string toDelete = "";

        ImGui::BeginChild("EnemyPresetList", ImVec2(0, 120), true);
        for (auto it = params.enemyPresets.begin(); it != params.enemyPresets.end(); ++it) {
            const std::string& name = it->first;
            const EnemyParams& data = it->second;

            ImGui::PushID(name.c_str());
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.2f, 0.2f, 1.0f));
            if (ImGui::Button("X", ImVec2(25, 0))) { toDelete = name; }
            ImGui::PopStyleColor();
            ImGui::SameLine();

            bool isCurrent = (params.activeEnemyPresetName == name);
            if (isCurrent) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
            if (ImGui::Button(name.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
                params.enemy = data;
                params.activeEnemyPresetName = name;
                strncpy_s(nameBuf, name.c_str(), _TRUNCATE);
            }
            if (isCurrent) ImGui::PopStyleColor();
            ImGui::PopID();
        }
        ImGui::EndChild();

        if (!toDelete.empty()) {
            params.enemyPresets.erase(toDelete);
            if (params.activeEnemyPresetName == toDelete) params.activeEnemyPresetName = "";
        }

        ImGui::Separator();
        ImGui::InputText("Name", nameBuf, IM_ARRAYSIZE(nameBuf));
        float btnW = ImGui::GetContentRegionAvail().x * 0.48f;
        if (ImGui::Button("OVERWRITE", ImVec2(btnW, 30))) {
            params.enemyPresets[nameBuf] = params.enemy;
            params.activeEnemyPresetName = nameBuf;
        }
        ImGui::SameLine();
        if (ImGui::Button("SAVE AS NEW", ImVec2(btnW, 30))) {
            params.enemyPresets[nameBuf] = params.enemy;
            params.activeEnemyPresetName = nameBuf;
        }
    }
}

static void DrawPhysicsConfigPanel(GameParams& params) {
    if (ImGui::CollapsingHeader("Global Physics", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderFloat("Gravity", &params.physics.gravity, 0.0f, 100.0f, "%.2f");
        ImGui::SliderFloat("Terminal Vel", &params.physics.terminalVelocity, 100.0f, 5000.0f, "%.0f");
    }
}