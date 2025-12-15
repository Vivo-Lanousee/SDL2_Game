#include "Time.h"
#include <SDL.h>

// static変数の実体を定義（初期化）
float Time::deltaTime = 0.0f;
unsigned int Time::lastFrameTime = 0;

void Time::Update() {
    // 現在の時刻を取得（ミリ秒）
    unsigned int current = SDL_GetTicks();
    if (lastFrameTime == 0) {
        lastFrameTime = current;
    }

    // 差分を計算 (ミリ秒 → 秒 に変換するために 1000.0f で割る)
    deltaTime = (current - lastFrameTime) / 1000.0f;

    lastFrameTime = current;

    if (deltaTime > 0.05f) {
        deltaTime = 0.05f;
    }
}