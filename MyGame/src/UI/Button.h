#pragma once
#include <SDL.h>
#include <string>
#include <functional>
#include "../Core/Game.h"

class Button {
public:
    // コンストラクタ：位置、サイズ、表示テキストを設定
    Button(int x, int y, int w, int h, std::string label)
        : rect{ x, y, w, h }, text(label), isHovered(false), isClicked(false)
    {
    }

    // イベント処理：クリック判定
    // TitleSceneのHandleEventsから呼ぶ
    bool HandleEvents(SDL_Event* event) {
        if (event->type == SDL_MOUSEMOTION) {
            // マウスが動いたら「ホバー判定」
            int mx = event->motion.x;
            int my = event->motion.y;
            isHovered = (mx >= rect.x && mx <= rect.x + rect.w &&
                my >= rect.y && my <= rect.y + rect.h);
        }

        if (event->type == SDL_MOUSEBUTTONDOWN) {
            // 左クリック かつ ホバー中なら「クリックされた！」
            if (event->button.button == SDL_BUTTON_LEFT && isHovered) {
                return true; // クリックされたことを通知
            }
        }
        return false;
    }

    // 描画処理
    void Render(Game* game) {
        SDL_Renderer* r = game->GetRenderer();

        // 1. 色を決める（ホバー中は明るく、普段は暗く）
        if (isHovered) {
            SDL_SetRenderDrawColor(r, 100, 100, 255, 255); // 青（明るめ）
        }
        else {
            SDL_SetRenderDrawColor(r, 50, 50, 150, 255);   // 青（暗め）
        }

        // 2. 四角形を描画
        SDL_RenderFillRect(r, &rect);

        // 3. 枠線を描画（白）
        SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
        SDL_RenderDrawRect(r, &rect);

        // 4. テキストを描画（ボタンの中央っぽく配置）
        // ※簡易的にボタンの左上+余白の位置に描画します
        SDL_Color white = { 255, 255, 255, 255 };
        game->DrawText(text.c_str(), rect.x + 20, rect.y + 15, white);
    }

private:
    SDL_Rect rect;    // ボタンの形
    std::string text; // ボタンの文字
    bool isHovered;   // マウスが乗っているか
    bool isClicked;   // クリックされたか
};