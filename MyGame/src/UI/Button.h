#pragma once
#include <SDL.h>
#include <string>
#include <functional>

class Button {
public:
    // コンストラクタ
    Button(int x, int y, int w, int h, std::string label);

    // イベント処理
    bool HandleEvents(SDL_Event* event);

    // 描画
    void Render(SDL_Renderer* renderer);

    std::function<void()> OnClick;

private:
    SDL_Rect rect;
    std::string text;
    bool isHovered;
};