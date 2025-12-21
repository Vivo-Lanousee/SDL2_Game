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
        BASE,
        WAVE
    };

    static void Init(SDL_Window* window, SDL_Renderer* renderer);
    static void HandleEvents(SDL_Event* event);
    // 第3引数に Game* を追加
    static void Render(SDL_Renderer* renderer, Scene* currentScene, class Game* game);
    static void Clean();

    static void SetMode(Mode newMode);

    // 画像ファイルをプロジェクト内に取り込むユーティリティ
    static std::string ImportTexture();

    static GameObject* selectedObject;
    static bool isTestMode;       // プレイヤーのテスト操作
    static bool isWaveSimMode;    // ウェーブのシミュレーション実行中フラグ
    static int simLevelID;        // シミュレーション対象のレベルID

private:
    static void DrawHierarchy(Scene* currentScene);
    static void DrawInspector();
    static void DrawParameters();
    static void DrawConfigEditorWindow();

    // ウェーブ設定用パネル
    static void DrawWaveConfigPanel(class GameParams& params);

    static ConfigViewMode currentConfigView;
    static Mode currentMode;
};