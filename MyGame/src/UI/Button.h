#pragma once
#include <SDL.h>
#include <string>

// 前方宣言（Game.hをインクルードしない！）
class Game;

class Button {
public:
    Button(int x, int y, int w, int h, std::string label);

    // ★修正1: "override" を削除しました（継承していないため）
    // ★修正2: 関数の中身は .cpp に移動しました
    bool HandleEvents(SDL_Event* event);

    void Render(Game* game);

private:
    SDL_Rect rect;
    std::string text;
    bool isHovered;
    bool isClicked;
};