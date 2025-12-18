#pragma once
#include <SDL.h>
#include <map>
#include <cstring>

enum class GameAction {
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight,
    Shoot,
    Reload,  // ★追加: リロードアクション
    Pause
};

class InputHandler {
public:
    InputHandler() {
        // --- キーボード設定 ---
        keyMap[GameAction::MoveUp] = SDL_SCANCODE_W;
        keyMap[GameAction::MoveDown] = SDL_SCANCODE_S;
        keyMap[GameAction::MoveLeft] = SDL_SCANCODE_A;
        keyMap[GameAction::MoveRight] = SDL_SCANCODE_D;
        keyMap[GameAction::Reload] = SDL_SCANCODE_R;      // ★追加: Rキーをリロードに割り当て
        keyMap[GameAction::Pause] = SDL_SCANCODE_ESCAPE;

        // --- マウス設定 ---
        mouseMap[GameAction::Shoot] = SDL_BUTTON_LEFT;


        // 初期化処理
        keyboardState = SDL_GetKeyboardState(&numKeys);
        prevKeyboardState = new Uint8[numKeys];
        std::memset(prevKeyboardState, 0, numKeys);

        currentMouseState = 0;
        prevMouseState = 0;
    }

    ~InputHandler() {
        if (prevKeyboardState) delete[] prevKeyboardState;
    }

    void Update() {
        // キーボード更新
        if (keyboardState) {
            std::memcpy(prevKeyboardState, keyboardState, numKeys);
        }
        keyboardState = SDL_GetKeyboardState(NULL);

        // マウス更新
        prevMouseState = currentMouseState;
        currentMouseState = SDL_GetMouseState(NULL, NULL); // 座標は不要なのでNULL
    }

    // 押しっぱなし判定
    bool IsPressed(GameAction action) {
        // キーボードチェック
        if (keyMap.count(action)) {
            if (keyboardState[keyMap[action]]) return true;
        }

        // マウスチェック
        if (mouseMap.count(action)) {
            int button = mouseMap[action];
            // ビット演算でボタンが押されているか判定
            if (currentMouseState & SDL_BUTTON(button)) return true;
        }

        return false;
    }

    // 押した瞬間判定
    bool IsJustPressed(GameAction action) {
        //  キーボードチェック
        if (keyMap.count(action)) {
            SDL_Scancode key = keyMap[action];
            if (keyboardState[key] && !prevKeyboardState[key]) return true;
        }

        // マウスチェック
        if (mouseMap.count(action)) {
            int button = mouseMap[action];
            bool isDown = (currentMouseState & SDL_BUTTON(button));
            bool wasDown = (prevMouseState & SDL_BUTTON(button));

            if (isDown && !wasDown) return true;
        }

        return false;
    }

private:
    // キーボード用マップ
    std::map<GameAction, SDL_Scancode> keyMap;
    // マウス用マップ
    std::map<GameAction, int> mouseMap;

    const Uint8* keyboardState = nullptr;
    Uint8* prevKeyboardState = nullptr;
    int numKeys = 0;

    // マウスの状態変数
    Uint32 currentMouseState;
    Uint32 prevMouseState;
};