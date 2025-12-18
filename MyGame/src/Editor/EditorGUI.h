#pragma once
#include <SDL.h>
#include "../Scenes/Scene.h"
#include "../Objects/GameObject.h"

/**
 * @brief エディタ用のGUIを管理する静的クラス
 * ImGuiの初期化、描画、イベント処理、および各設定パネルの管理を行います。
 */
class EditorGUI {
public:
    /**
     * @brief エディタの動作状態
     */
    enum class Mode {
        GAME,    // 通常のゲーム実行モード
        EDITOR   // オブジェクト配置やパラメータ編集が可能なエディタモード
    };

    /**
     * @brief 現在どの設定パネルを表示しているか
     */
    enum class ConfigViewMode {
        NONE,    // 非表示
        PLAYER,  // プレイヤー設定
        GUN,     // 銃の設定（追加）
        ENEMY,   // 敵の設定
        PHYSICS  // 物理法則の設定
    };

    // --- 基本ライフサイクル ---

    // 初期化（コンテキスト作成、バックエンド接続、Configロード）
    static void Init(SDL_Window* window, SDL_Renderer* renderer);

    // イベント処理（SDLイベントをImGuiに渡す）
    static void HandleEvents(SDL_Event* event);

    // 描画（NewFrameからRenderDrawDataまでを一括で行う）
    static void Render(SDL_Renderer* renderer, Scene* currentScene);

    // 終了処理（バックエンド切断、コンテキスト破棄）
    static void Clean();

    // モード切り替え
    static void SetMode(Mode newMode);

    // --- 公開メンバ変数 ---

    // 現在Inspectorで詳細を表示・編集しているオブジェクト
    static GameObject* selectedObject;

    // エディタ内でプレイヤーを動かして数値を試せる「テストモード」のフラグ
    static bool isTestMode;

private:
    // --- 内部描画関数 ---

    // シーン内のオブジェクト一覧を表示するウィンドウ
    static void DrawHierarchy(Scene* currentScene);

    // 選択中のオブジェクトの座標や物理値を編集するウィンドウ
    static void DrawInspector();

    // 設定パネルを切り替えるためのランチャーボタン
    static void DrawParameters();

    // プレイヤー、銃、敵、物理の各パラメータを編集するメインウィンドウ
    static void DrawConfigEditorWindow();

    // --- 内部状態管理変数 ---

    // 現在どの設定を表示しているか
    static ConfigViewMode currentConfigView;

    // 現在の動作モード（GAME / EDITOR）
    static Mode currentMode;
};