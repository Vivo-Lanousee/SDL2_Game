#pragma once
#include <SDL.h>
#include <string>
#include <functional> // ★追加：関数を代入できる型 std::function を使うため

class Button {
public:
    // コンストラクタ
    Button(int x, int y, int w, int h, std::string label);

    // イベント処理
    // 戻り値は「クリックされたかどうか」ではなく、「イベントを消費したか」などを返すのが一般的ですが、
    // 今回は OnClick で処理するので、bool の意味合いは薄れます
    bool HandleEvents(SDL_Event* event);

    // 描画
    // ★修正：Gameクラスへの依存を削除し、純粋にレンダラーだけを受け取る
    void Render(SDL_Renderer* renderer);

    // ★追加：クリックされた時に実行したい関数を入れる変数
    // "void型で引数なしの関数" を保持できます
    std::function<void()> OnClick;

private:
    SDL_Rect rect;
    std::string text;
    bool isHovered;
    // isClicked は OnClick で即時実行するなら不要になることが多いですが、
    // 状態として持ちたい場合は残してもOKです。今回はシンプルに削除します。
};