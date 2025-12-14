#pragma once
#include <SDL.h>
#include <map>


enum class GameAction {
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight,
    Shoot,
    Pause
};

class InputHandler {
public:
    InputHandler() {
        keyMap[GameAction::MoveUp] = SDL_SCANCODE_UP;
        keyMap[GameAction::MoveDown] = SDL_SCANCODE_DOWN;
        keyMap[GameAction::MoveLeft] = SDL_SCANCODE_LEFT;
        keyMap[GameAction::MoveRight] = SDL_SCANCODE_RIGHT;
        keyMap[GameAction::Shoot] = SDL_SCANCODE_Z;
        keyMap[GameAction::Pause] = SDL_SCANCODE_ESCAPE;
    }

    // 毎フレーム呼ぶ（キーボードの状態更新）
    void Update() {
        keyboardState = SDL_GetKeyboardState(NULL);
    }

    bool IsPressed(GameAction action) {
        // そのアクションに対応するキーを探す
        SDL_Scancode key = keyMap[action];

        // そのキーが押されているか調べる
        if (keyboardState) {
            return keyboardState[key];
        }
        return false;
    }

    void RemapKey(GameAction action, SDL_Scancode newKey) {
        keyMap[action] = newKey;
    }

private:
    // 「アクション」と「キー」の対応表
    std::map<GameAction, SDL_Scancode> keyMap;

    // SDLのキーボード状態
    const Uint8* keyboardState = nullptr;
};