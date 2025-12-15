#include "Animator.h"
#include "Time.h"
#include <fstream>  // ファイル操作用
#include <iostream> // エラー表示用

Animator::Animator()
    : currentClip(nullptr), timer(0), currentFrameIndex(0)
{
}


bool Animator::LoadFromJson(std::string filePath) {
    // ファイルを開く
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cout << "[Error] Failed to open animation file: " << filePath << std::endl;
        return false;
    }

    try {
        // JSONデータをパース（解析）
        json data;
        file >> data;

        // "animations" 配列の中身を順番に取り出す
        if (data.contains("animations")) {
            for (auto& anim : data["animations"]) {
                // 必須項目の取得
                std::string name = anim["name"];
                int row = anim["row"];
                int count = anim["frameCount"];
                float speed = anim["speed"];

                // オプション項目（省略時はtrue）
                bool loop = true;
                if (anim.contains("loop")) {
                    loop = anim["loop"];
                }

                // 登録関数を呼び出してメモリに保存
                AddAnimation(name, row, count, speed, loop);
            }
        }
        std::cout << "[System] Loaded animations from " << filePath << std::endl;
        return true;
    }
    catch (json::parse_error& e) {
        std::cout << "[Error] JSON Parse Error in " << filePath << ": " << e.what() << std::endl;
        return false;
    }
}

// 今までの手動登録関数（LoadFromJsonから呼ばれる）
void Animator::AddAnimation(std::string name, int row, int frameCount, float speed, bool loop) {
    AnimationClip clip;
    clip.name = name;
    clip.row = row;
    clip.frameCount = frameCount;
    clip.speed = speed;
    clip.loop = loop;

    animations[name] = clip;

    // 最初のアニメーションをデフォルトで再生する
    if (currentClip == nullptr) {
        Play(name);
    }
}

void Animator::Play(std::string name) {
    // すでに同じアニメーションが再生中ならリセットしない
    if (currentClip && currentClip->name == name) {
        return;
    }

    // 新しいアニメーションが見つかったら切り替える
    if (animations.count(name) > 0) {
        currentClip = &animations[name];
        currentFrameIndex = 0;
        timer = 0;
    }
}

void Animator::Update() {
    if (!currentClip) return;

    timer += Time::deltaTime;

    // 設定された速度を超えたら次のコマへ
    if (timer >= currentClip->speed) {
        timer = 0;
        currentFrameIndex++;

        // 最後のコマまで行ったら
        if (currentFrameIndex >= currentClip->frameCount) {
            if (currentClip->loop) {
                currentFrameIndex = 0; // ループする
            }
            else {
                currentFrameIndex = currentClip->frameCount - 1; // 最後の絵で止める
            }
        }
    }
}

SDL_Rect Animator::GetSrcRect(int w, int h) {
    SDL_Rect src;
    src.w = w;
    src.h = h;

    if (currentClip) {
        src.x = currentFrameIndex * w;
        src.y = currentClip->row * h;
    }
    else {
        // アニメーションがない時は左上を表示
        src.x = 0;
        src.y = 0;
    }

    return src;
}