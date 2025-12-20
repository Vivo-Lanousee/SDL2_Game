#include "EditorGUI.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <string>
#include <iostream>
#include <map>
#include <algorithm> 
#include <cstring> 
#include <filesystem> 
#include <windows.h>
#include <commdlg.h> 

#include "../Core/GameParams.h" 
#include "../Core/GameSession.h"
#include "../Scenes/Scene.h"
#include "../Scenes/EditorScene.h"
#include "../Objects/GameObject.h"
#include "../Objects/Player.h"
#include "../Objects/Enemy.h"
#include "../Objects/Base.h"
#include "../Core/ConfigManager.h" 

// Shorten filesystem namespace
namespace fs = std::filesystem;

// Static member definitions
GameObject* EditorGUI::selectedObject = nullptr;
EditorGUI::Mode EditorGUI::currentMode = EditorGUI::Mode::GAME;
EditorGUI::ConfigViewMode EditorGUI::currentConfigView = EditorGUI::ConfigViewMode::NONE;
bool EditorGUI::isTestMode = false;

// --- Forward declarations of helper functions ---
static void DrawPlayerConfigPanel(GameParams& params);
static void DrawGunConfigPanel(GameParams& params, SDL_Renderer* renderer, Scene* currentScene);
static void DrawEnemyConfigPanel(GameParams& params, SDL_Renderer* renderer, Scene* currentScene);
static void DrawPhysicsConfigPanel(GameParams& params);
static void DrawCameraConfigPanel(GameParams& params);
static void DrawBaseConfigPanel(GameParams& params, SDL_Renderer* renderer, Scene* currentScene);

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

std::string EditorGUI::ImportTexture() {
    char szFile[260] = { 0 };
    OPENFILENAMEA ofn;
    SecureZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "PNG Files\0*.png\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn)) {
        fs::path srcPath(szFile);
        std::string destDir = "assets/images/editor_imports/";

        try {
            if (!fs::exists(destDir)) fs::create_directories(destDir);
            std::string fileName = srcPath.filename().string();
            std::string destPath = destDir + fileName;
            fs::copy_file(srcPath, destPath, fs::copy_options::overwrite_existing);
            return destPath;
        }
        catch (const fs::filesystem_error& e) {
            std::cerr << "File system error: " << e.what() << std::endl;
        }
    }
    return "";
}

static void NotifyPlayerGunChanged(SDL_Renderer* renderer, Scene* currentScene) {
    if (!currentScene) return;
    auto& objects = currentScene->GetObjects();
    for (auto& obj : objects) {
        Player* player = dynamic_cast<Player*>(obj.get());
        if (player) {
            player->RefreshGunConfig(renderer);
        }
    }
}

static void NotifyEnemyConfigChanged(SDL_Renderer* renderer, Scene* currentScene) {
    if (!currentScene) return;
    auto& objects = currentScene->GetObjects();
    for (auto& obj : objects) {
        Enemy* enemy = dynamic_cast<Enemy*>(obj.get());
        if (enemy) {
            enemy->RefreshConfig(renderer);
        }
    }
}

static void NotifyBaseConfigChanged(SDL_Renderer* renderer, Scene* currentScene) {
    if (!currentScene) return;
    auto& objects = currentScene->GetObjects();
    for (auto& obj : objects) {
        Base* b = dynamic_cast<Base*>(obj.get());
        if (b) {
            b->RefreshConfig(renderer);
        }
    }
    GameSession::GetInstance().maxBaseHP = GameParams::GetInstance().base.maxHealth;
}

void EditorGUI::Render(SDL_Renderer* renderer, Scene* currentScene) {
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    if (currentMode == Mode::EDITOR) {
        DrawHierarchy(currentScene);
        DrawInspector();
        DrawParameters();

        if (currentConfigView != ConfigViewMode::NONE) {
            GameParams& params = GameParams::GetInstance();
            ImGui::SetNextWindowPos(ImVec2(890, 370), ImGuiCond_Once);
            ImGui::SetNextWindowSize(ImVec2(300, 420), ImGuiCond_Once);

            std::string title = "Editor Panel";
            switch (currentConfigView) {
            case ConfigViewMode::PLAYER:  title += " [Player]"; break;
            case ConfigViewMode::GUN:      title += " [Gun]";    break;
            case ConfigViewMode::ENEMY:   title += " [Enemy]";  break;
            case ConfigViewMode::PHYSICS: title += " [Physics]"; break;
            case ConfigViewMode::CAMERA:  title += " [Camera]";  break;
            case ConfigViewMode::BASE:    title += " [Base]";    break;
            default: return;
            }

            if (ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize)) {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), title.c_str());
                ImGui::Separator();
                switch (currentConfigView) {
                case ConfigViewMode::PLAYER:  DrawPlayerConfigPanel(params);  break;
                case ConfigViewMode::GUN:      DrawGunConfigPanel(params, renderer, currentScene); break;
                case ConfigViewMode::ENEMY:   DrawEnemyConfigPanel(params, renderer, currentScene); break;
                case ConfigViewMode::PHYSICS: DrawPhysicsConfigPanel(params); break;
                case ConfigViewMode::CAMERA:  DrawCameraConfigPanel(params);  break;
                case ConfigViewMode::BASE:    DrawBaseConfigPanel(params, renderer, currentScene); break;
                default: break;
                }
                ImGui::End();
            }
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
            ImGui::Checkbox("Use Gravity", &selectedObject->useGravity);
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
    ImGui::SetNextWindowSize(ImVec2(200, 350), ImGuiCond_Once);

    ImGui::Begin(" Launcher", nullptr, ImGuiWindowFlags_NoCollapse);
    ImGui::TextDisabled("Category Select:");
    if (ImGui::Button("Player", ImVec2(-1, 35))) currentConfigView = ConfigViewMode::PLAYER;
    if (ImGui::Button("Gun", ImVec2(-1, 35)))    currentConfigView = ConfigViewMode::GUN;
    if (ImGui::Button("Enemy", ImVec2(-1, 35)))  currentConfigView = ConfigViewMode::ENEMY;
    if (ImGui::Button("Base", ImVec2(-1, 35)))   currentConfigView = ConfigViewMode::BASE;
    if (ImGui::Button("Physics", ImVec2(-1, 35))) currentConfigView = ConfigViewMode::PHYSICS;
    if (ImGui::Button("Camera", ImVec2(-1, 35)))  currentConfigView = ConfigViewMode::CAMERA;

    ImGui::Separator();
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.4f, 0.6f, 0.6f));
    if (ImGui::Button("SAVE ALL TO FILE", ImVec2(-1, 40))) {
        ConfigManager::Save(GameParams::GetInstance());
    }
    ImGui::PopStyleColor();
    ImGui::End();
}

static void DrawPlayerConfigPanel(GameParams& params) {
    static char nameBuf[64] = "";
    if (nameBuf[0] == '\0' && !params.activePlayerPresetName.empty()) {
        strncpy_s(nameBuf, params.activePlayerPresetName.c_str(), _TRUNCATE);
    }

    ImGui::PushStyleColor(ImGuiCol_Button, EditorGUI::isTestMode ? ImVec4(0.8f, 0.2f, 0.2f, 1.0f) : ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
    std::string btnLabel = EditorGUI::isTestMode ? " STOP TEST" : "SPAWN TEST PLAYER";
    if (ImGui::Button(btnLabel.c_str(), ImVec2(-1, 40))) {
        EditorGUI::isTestMode = !EditorGUI::isTestMode;
    }
    ImGui::PopStyleColor();
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Edit Active Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderFloat("Speed", &params.player.moveSpeed, 50.0f, 600.0f, "%.1f");
        ImGui::SliderFloat("Jump", &params.player.jumpVelocity, 100.0f, 1000.0f, "%.1f");
        ImGui::InputFloat("Health", &params.player.maxHealth, 10.0f, 100.0f, "%.0f");
    }

    if (ImGui::CollapsingHeader("Presets", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::BeginChild("PlayerPresetList", ImVec2(0, 120), true);
        for (auto it = params.playerPresets.begin(); it != params.playerPresets.end(); ++it) {
            const std::string& name = it->first;
            if (ImGui::Selectable(name.c_str(), params.activePlayerPresetName == name)) {
                params.player = it->second;
                params.activePlayerPresetName = name;
                strncpy_s(nameBuf, name.c_str(), _TRUNCATE);
            }
        }
        ImGui::EndChild();

        ImGui::InputText("Name", nameBuf, IM_ARRAYSIZE(nameBuf));
        if (ImGui::Button("SAVE PRESET", ImVec2(-1, 0))) {
            params.playerPresets[nameBuf] = params.player;
            params.activePlayerPresetName = nameBuf;
        }
    }
}

static void DrawGunConfigPanel(GameParams& params, SDL_Renderer* renderer, Scene* currentScene) {
    static char nameBuf[64] = "";
    if (nameBuf[0] == '\0' && !params.activeGunPresetName.empty()) {
        strncpy_s(nameBuf, params.activeGunPresetName.c_str(), _TRUNCATE);
    }

    if (ImGui::CollapsingHeader("Edit Active Gun Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::SliderFloat("Fire Rate", &params.gun.fireRate, 0.05f, 1.0f, "%.2f sec")) NotifyPlayerGunChanged(renderer, currentScene);
        if (ImGui::SliderFloat("Bullet Speed", &params.gun.bulletSpeed, 10.0f, 2000.0f, "%.0f")) NotifyPlayerGunChanged(renderer, currentScene);
        if (ImGui::InputInt("Damage", &params.gun.damage)) NotifyPlayerGunChanged(renderer, currentScene);
        ImGui::SliderFloat("Spread", &params.gun.spreadAngle, 0.0f, 90.0f, "%.1f deg");
        ImGui::SliderInt("Shot Count", &params.gun.shotCount, 1, 20);

        ImGui::Separator();
        ImGui::Text("Appearance");
        if (ImGui::Button("Import Gun Image", ImVec2(-1, 30))) {
            std::string newPath = EditorGUI::ImportTexture();
            if (!newPath.empty()) {
                params.gun.texturePath = newPath;
                NotifyPlayerGunChanged(renderer, currentScene);
            }
        }
    }

    if (ImGui::CollapsingHeader("Gun Presets", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::BeginChild("GunPresetList", ImVec2(0, 100), true);
        for (auto it = params.gunPresets.begin(); it != params.gunPresets.end(); ++it) {
            const std::string& name = it->first;
            if (ImGui::Selectable(name.c_str(), params.activeGunPresetName == name)) {
                params.gun = it->second;
                params.activeGunPresetName = name;
                strncpy_s(nameBuf, name.c_str(), _TRUNCATE);
                NotifyPlayerGunChanged(renderer, currentScene);
            }
        }
        ImGui::EndChild();

        ImGui::InputText("Preset Name", nameBuf, IM_ARRAYSIZE(nameBuf));
        if (ImGui::Button("SAVE PRESET", ImVec2(-1, 0))) {
            params.gunPresets[nameBuf] = params.gun;
            params.activeGunPresetName = nameBuf;
        }
    }
}

static void DrawEnemyConfigPanel(GameParams& params, SDL_Renderer* renderer, Scene* currentScene) {
    static char nameBuf[64] = "";
    if (nameBuf[0] == '\0' && !params.activeEnemyPresetName.empty()) {
        strncpy_s(nameBuf, params.activeEnemyPresetName.c_str(), _TRUNCATE);
    }

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.8f, 1.0f));
    if (ImGui::Button("SPAWN TEST ENEMY", ImVec2(-1, 40))) {
        EditorScene* edScene = dynamic_cast<EditorScene*>(currentScene);
        if (edScene) edScene->SpawnTestEnemy(renderer);
    }
    ImGui::PopStyleColor();
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Status Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::InputInt("Health", &params.enemy.baseHealth)) NotifyEnemyConfigChanged(renderer, currentScene);
        if (ImGui::InputInt("Attack", &params.enemy.attackPower)) NotifyEnemyConfigChanged(renderer, currentScene);
        if (ImGui::SliderFloat("Speed", &params.enemy.baseSpeed, 10.0f, 500.0f, "%.1f px/s")) NotifyEnemyConfigChanged(renderer, currentScene);
        if (ImGui::SliderFloat("Range", &params.enemy.attackRange, 0.0f, 1000.0f, "%.0f px")) NotifyEnemyConfigChanged(renderer, currentScene);
        if (ImGui::SliderFloat("Interval", &params.enemy.attackInterval, 0.1f, 10.0f, "%.2f sec")) NotifyEnemyConfigChanged(renderer, currentScene);
    }

    if (ImGui::CollapsingHeader("Behavior Style", ImGuiTreeNodeFlags_DefaultOpen)) {
        // A. Move Mode
        const char* moveMethods[] = { "Linear", "PathFollow" };
        int currentMove = static_cast<int>(params.enemy.moveMethod);
        if (ImGui::Combo("Move Mode", &currentMove, moveMethods, IM_ARRAYSIZE(moveMethods))) {
            params.enemy.moveMethod = static_cast<MovementType>(currentMove);
            NotifyEnemyConfigChanged(renderer, currentScene);
        }

        // B. Locomotion Style
        const char* locomotionStyles[] = { "Ground", "Flying", "Jumping" };
        int currentLoco = static_cast<int>(params.enemy.locomotionStyle);
        if (ImGui::Combo("Locomotion", &currentLoco, locomotionStyles, IM_ARRAYSIZE(locomotionStyles))) {
            params.enemy.locomotionStyle = static_cast<LocomotionType>(currentLoco);
            NotifyEnemyConfigChanged(renderer, currentScene);
        }

        // C. Attack Type
        const char* attackMethods[] = { "Melee", "Ranged", "Kamikaze" };
        int currentAtk = static_cast<int>(params.enemy.attackMethod);
        if (ImGui::Combo("Attack Type", &currentAtk, attackMethods, IM_ARRAYSIZE(attackMethods))) {
            params.enemy.attackMethod = static_cast<AttackType>(currentAtk);
            NotifyEnemyConfigChanged(renderer, currentScene);
        }
    }

    if (ImGui::CollapsingHeader("Appearance Images", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::TextWrapped("Body Path: %s", params.enemy.texturePath.c_str());
        if (ImGui::Button("Import Body Image", ImVec2(-1, 30))) {
            std::string newPath = EditorGUI::ImportTexture();
            if (!newPath.empty()) {
                params.enemy.texturePath = newPath;
                NotifyEnemyConfigChanged(renderer, currentScene);
            }
        }

        ImGui::Separator();
        ImGui::TextWrapped("Bullet Path: %s", params.enemy.bulletTexturePath.c_str());
        if (ImGui::Button("Import Bullet Image", ImVec2(-1, 30))) {
            std::string newPath = EditorGUI::ImportTexture();
            if (!newPath.empty()) {
                params.enemy.bulletTexturePath = newPath;
                NotifyEnemyConfigChanged(renderer, currentScene);
            }
        }
    }

    if (ImGui::CollapsingHeader("Presets", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::BeginChild("EnemyPresetList", ImVec2(0, 100), true);
        for (auto it = params.enemyPresets.begin(); it != params.enemyPresets.end(); ++it) {
            if (ImGui::Selectable(it->first.c_str(), params.activeEnemyPresetName == it->first)) {
                params.enemy = it->second;
                params.activeEnemyPresetName = it->first;
                strncpy_s(nameBuf, it->first.c_str(), _TRUNCATE);
                NotifyEnemyConfigChanged(renderer, currentScene);
            }
        }
        ImGui::EndChild();

        ImGui::InputText("Preset Name", nameBuf, IM_ARRAYSIZE(nameBuf));
        if (ImGui::Button("SAVE PRESET", ImVec2(-1, 0))) {
            params.enemyPresets[nameBuf] = params.enemy;
            params.activeEnemyPresetName = nameBuf;
        }
    }
}

static void DrawBaseConfigPanel(GameParams& params, SDL_Renderer* renderer, Scene* currentScene) {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.6f, 0.4f, 1.0f));
    if (ImGui::Button("RESTORE BASE HP", ImVec2(-1, 45))) {
        GameSession::GetInstance().currentBaseHP = params.base.maxHealth;
    }
    ImGui::PopStyleColor();
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Base Stats", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::InputInt("Max Health", &params.base.maxHealth)) NotifyBaseConfigChanged(renderer, currentScene);
        if (ImGui::SliderFloat("Defense", &params.base.defense, 0.0f, 100.0f)) NotifyBaseConfigChanged(renderer, currentScene);
    }

    if (ImGui::CollapsingHeader("Appearance", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Button("Import Base Image", ImVec2(-1, 30))) {
            std::string newPath = EditorGUI::ImportTexture();
            if (!newPath.empty()) {
                params.base.texturePath = newPath;
                NotifyBaseConfigChanged(renderer, currentScene);
            }
        }
        ImGui::TextWrapped("Current Path: %s", params.base.texturePath.c_str());
    }
}

static void DrawPhysicsConfigPanel(GameParams& params) {
    if (ImGui::CollapsingHeader("Global Physics", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderFloat("Gravity", &params.physics.gravity, 0.0f, 100.0f, "%.2f");
        ImGui::SliderFloat("Terminal Vel", &params.physics.terminalVelocity, 100.0f, 5000.0f, "%.0f");
    }
}

static void DrawCameraConfigPanel(GameParams& params) {
    if (ImGui::CollapsingHeader("Camera Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderFloat("Offset X", &params.camera.offsetX, -400.0f, 400.0f, "%.1f px");
        ImGui::SliderFloat("Offset Y", &params.camera.offsetY, -500.0f, 300.0f, "%.1f px");
        ImGui::DragInt("Map Limit Width", &params.camera.limitX, 10, 800, 10000);
    }
}