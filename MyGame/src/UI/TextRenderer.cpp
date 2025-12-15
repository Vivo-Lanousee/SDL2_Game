#include "TextRenderer.h"

// static変数の実体を定義
TTF_Font* TextRenderer::font = nullptr;

// ★修正: ヘッダーに合わせて引数 (path, size) を追加
bool TextRenderer::Init(const char* fontPath, int fontSize) {
    // 文字システムの初期化
    if (TTF_Init() == -1) {
        std::cout << "TTF_Init Error: " << TTF_GetError() << std::endl;
        return false;
    }

    // フォント読み込み
    // ★修正: 引数で渡されたパスとサイズを使う
    font = TTF_OpenFont(fontPath, fontSize);

    if (!font) {
        std::cout << "Failed to load font: " << fontPath << std::endl;
        return false;
    }

    return true;
}

void TextRenderer::Clean() {
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    TTF_Quit();
}

// ★修正: ヘッダーに合わせて std::string に変更
void TextRenderer::Draw(SDL_Renderer* renderer, std::string text, int x, int y, SDL_Color color) {
    if (!font || !renderer) return;
    if (text.empty()) return; // 空文字チェック

    // 文字からサーフェスを作成
    // ★修正: std::string を C言語形式の文字列に変換するために .c_str() を使う
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!surface) return;

    // サーフェスからテクスチャを作成
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    // 描画範囲の設定
    SDL_Rect destRect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &destRect);

    // メモリ解放
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}