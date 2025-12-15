#pragma once

class Time {
public:
    // Unityのように Time::deltaTime でどこからでもアクセス可能にする
    static float deltaTime;

    // フレームの最初に呼び出して時間を更新する関数
    static void Update();

private:
    // 前回のフレームの時刻（ミリ秒）
    static unsigned int lastFrameTime;
};