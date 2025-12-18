#pragma once
#include <SDL.h>
#include "../Scenes/Scene.h"
#include "../Objects/GameObject.h"

class EditorGUI {
public:
    enum class Mode {
        GAME,    // 通常のゲーム実行モード
        EDITOR  // オブジェクト配置や経路編集が可能なエディタモード
    };

    enum class ConfigViewMode {
        NONE,    // 初期状態/非表示
        PLAYER,
        ENEMY,
        PHYSICS
    };

    // 初期化・イベント・描画・終了
    static void Init(SDL_Window* window, SDL_Renderer* renderer);
    static void HandleEvents(SDL_Event* event);
    static void Render(SDL_Renderer* renderer, Scene* currentScene);
    static void Clean();

    static void SetMode(Mode newMode);

    // 選択中のオブジェクト (Inspector連動用)
    static GameObject* selectedObject;

    // --- 追加: テストモードの管理 ---
    static bool isTestMode;

private:
    // 内部描画関数
    static void DrawHierarchy(Scene* currentScene);
    static void DrawInspector();
    static void DrawParameters(); // Config Launcher (ボタン配置)

    static void DrawConfigEditorWindow(); // メインの編集画面を表示する関数

    // 現在の設定ビューを保持するメンバ
    static ConfigViewMode currentConfigView;

    static Mode currentMode;
};