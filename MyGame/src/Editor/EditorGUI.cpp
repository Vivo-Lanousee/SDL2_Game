#include "EditorGUI.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <string>

// 静的メンバ変数の実体
GameObject* EditorGUI::selectedObject = nullptr;

void EditorGUI::Init(SDL_Window* window, SDL_Renderer* renderer) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark(); // ダークモード

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

void EditorGUI::Render(SDL_Renderer* renderer, Scene* currentScene) {
    // ImGuiフレーム開始
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // 2つのウィンドウを描画
    DrawHierarchy(currentScene);
    DrawInspector();

    // 描画実行
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
            ImGui::PushID(index);
            std::string label = obj->name.empty() ? "Object " + std::to_string(index) : obj->name;

            // リスト表示
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
    ImGui::SetNextWindowPos(ImVec2(550, 0), ImGuiCond_Once); // 右側
    ImGui::SetNextWindowSize(ImVec2(250, 600), ImGuiCond_Once);

    ImGui::Begin("Inspector");

    // ダングリングポインタ対策（死んだオブジェクトを選択していたら解除）
    if (selectedObject && selectedObject->isDead) {
        selectedObject = nullptr;
    }

    if (selectedObject) {
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Target: %s", selectedObject->name.c_str());
        ImGui::Separator();

        // Transform
        ImGui::Text("Transform");
        ImGui::DragFloat("X", &selectedObject->x, 1.0f);
        ImGui::DragFloat("Y", &selectedObject->y, 1.0f);
        ImGui::DragInt("W", &selectedObject->width, 1);
        ImGui::DragInt("H", &selectedObject->height, 1);

        // Physics
        ImGui::Separator();
        ImGui::Text("Physics");
        ImGui::Checkbox("Gravity", &selectedObject->useGravity);
        ImGui::DragFloat("Vel X", &selectedObject->velX, 0.1f);
        ImGui::DragFloat("Vel Y", &selectedObject->velY, 0.1f);

        // Delete
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