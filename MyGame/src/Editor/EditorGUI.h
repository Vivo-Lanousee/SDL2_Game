#pragma once
#include <SDL.h>
#include "../Scenes/Scene.h"
#include "../Objects/GameObject.h"
#include <string>

// エディタ用のGUIを管理する静的クラス

class EditorGUI {
public:
    enum class Mode {
        GAME,
        EDITOR
    };

    enum class ConfigViewMode {
        NONE,
        PLAYER,
        GUN,
        ENEMY,
        PHYSICS,
        CAMERA,
        BASE 
    };

    static void Init(SDL_Window* window, SDL_Renderer* renderer);
    static void HandleEvents(SDL_Event* event);
    static void Render(SDL_Renderer* renderer, Scene* currentScene);
    static void Clean();

    static void SetMode(Mode newMode);

    // 画像ファイルをプロジェクト内に取り込むユーティリティ
    static std::string ImportTexture();

    static GameObject* selectedObject;
    static bool isTestMode;

private:
    static void DrawHierarchy(Scene* currentScene);
    static void DrawInspector();
    static void DrawParameters();
    static void DrawConfigEditorWindow();

    static ConfigViewMode currentConfigView;
    static Mode currentMode;
};