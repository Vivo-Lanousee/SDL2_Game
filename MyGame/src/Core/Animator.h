#pragma once
#include <SDL.h>
#include <string>
#include <map>
#include <vector>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

// 1つのアニメーションの定義
struct AnimationClip {
    std::string name;
    int row;          // スプライトシートの何行目か
    int frameCount;   // 何コマあるか
    float speed;      // 切り替え速度（秒）
    bool loop;        // ループするかどうか
};

class Animator {
public:
    Animator();
    bool LoadFromJson(std::string filePath);
    void AddAnimation(std::string name, int row, int frameCount, float speed, bool loop = true);
    // 再生するアニメーションを切り替える
    void Play(std::string name);

    // 毎フレーム更新
    void Update();

    // 現在のコマの画像範囲を取得
    SDL_Rect GetSrcRect(int w, int h);

private:
    std::map<std::string, AnimationClip> animations;
    std::string currentAnimName;
    AnimationClip* currentClip;

    float timer;
    int currentFrameIndex;
};