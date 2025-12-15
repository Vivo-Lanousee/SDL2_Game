#pragma once
#include "Time.h"

class Timer {
public:
    // コンストラクタ（設定時間を渡す）
    Timer(float seconds = 0.0f) {
        maxTime = seconds;
        currentTime = 0.0f;
    }

    // 時間を進める（Updateで呼ぶ）
    void Update() {
        if (currentTime > 0.0f) {
            currentTime -= Time::deltaTime;
        }
    }

    // 0秒以下になったらtrue
    bool IsReady() {
        return currentTime <= 0.0f;
    }

    void Reset() {
        currentTime = maxTime;
    }

    // 設定時間を変更する
    void SetInterval(float seconds) {
        maxTime = seconds;
    }

private:
    float currentTime; // 残り時間
    float maxTime;     // 設定された間隔
};