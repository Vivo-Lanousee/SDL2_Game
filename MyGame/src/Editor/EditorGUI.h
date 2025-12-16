#pragma once
#include <SDL.h>
#include "../Scenes/Scene.h"
#include "../Objects/GameObject.h"

class EditorGUI {
public:
    // 初期化・イベント・描画・終了
    static void Init(SDL_Window* window, SDL_Renderer* renderer);
    static void HandleEvents(SDL_Event* event);
    static void Render(SDL_Renderer* renderer, Scene* currentScene);
    static void Clean();

private:
    // 内部描画関数
    static void DrawHierarchy(Scene* currentScene);
    static void DrawInspector();

    // 選択中のオブジェクト
    static GameObject* selectedObject;
};