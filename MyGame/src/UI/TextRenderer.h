#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <iostream>

class TextRenderer {
public:
    // フォントの読み込みを行う初期化関数
    // 引数: フォントのパス, 文字サイズ
    static bool Init(const char* fontPath, int fontSize);

    // 終了処理（フォントを閉じる）
    static void Clean();

    // 文字を描画する関数
    // 引数: レンダラー, 表示する文字, X座標, Y座標, 文字色
    static void Draw(SDL_Renderer* renderer, std::string text, int x, int y, SDL_Color color);

private:
    static TTF_Font* font;
};