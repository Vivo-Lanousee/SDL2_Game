#pragma once
#include <SDL.h>
#include "../Scenes/Scene.h"
#include "../Objects/GameObject.h"

class EditorGUI {
public:
    // ★★★ 修正箇所 1: エディタモードの定義 ★★★
    enum class Mode {
        GAME,   // 通常のゲーム実行モード
        EDITOR  // オブジェクト配置や経路編集が可能なエディタモード
    };

    // 初期化・イベント・描画・終了
    static void Init(SDL_Window* window, SDL_Renderer* renderer);
    static void HandleEvents(SDL_Event* event);
    static void Render(SDL_Renderer* renderer, Scene* currentScene);
    static void Clean();

    // ★★★ 修正箇所 2: モード設定機能の追加 ★★★
    static void SetMode(Mode newMode);

private:
    // 内部描画関数
    static void DrawHierarchy(Scene* currentScene);
    static void DrawInspector();
    // ★★★ 修正箇所 3: パラメータ調整ウィンドウの追加 ★★★
    static void DrawParameters();

    // 選択中のオブジェクト
    static GameObject* selectedObject;

    // ★★★ 修正箇所 4: 現在のモードを保持する静的メンバの追加 ★★★
    static Mode currentMode;
};