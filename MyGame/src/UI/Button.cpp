#include "Button.h"
#include "TextRenderer.h"
#include "imgui.h" // ★★★ 修正箇所 1: ImGuiのステータスチェックに必要 ★★★

// コンストラクタ
Button::Button(int x, int y, int w, int h, std::string label)
    : rect{ x, y, w, h }, text(label), isHovered(false)
{
}

// イベント処理
bool Button::HandleEvents(SDL_Event* event) {

    // ★★★ 修正箇所 2: ImGuiがマウスを掴んでいるかチェック ★★★
    // ImGuiがマウスをキャプチャしている（つまり、ImGuiウィンドウ上でクリックやドラッグが行われている）場合、
    // SDLのUI要素のイベント処理はスキップし、イベントを消費しない（falseを返す）。
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) {
        // マウスが動いていても、ImGuiが優先されるため、isHoveredの更新も行いません。
        return false;
    }

    if (event->type == SDL_MOUSEMOTION) {
        int mx = event->motion.x;
        int my = event->motion.y;
        isHovered = (mx >= rect.x && mx <= rect.x + rect.w &&
            my >= rect.y && my <= rect.y + rect.h);
    }

    if (event->type == SDL_MOUSEBUTTONDOWN) {
        if (event->button.button == SDL_BUTTON_LEFT && isHovered) {
            if (OnClick) {
                OnClick();
            }
            return true; // イベントを処理した
        }
    }
    return false;
}

// 描画処理
void Button::Render(SDL_Renderer* renderer) {
    if (isHovered) {
        SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 50, 50, 150, 255);
    }

    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &rect);

    SDL_Color white = { 255, 255, 255, 255 };

    TextRenderer::Draw(renderer, text.c_str(), rect.x + 20, rect.y + 15, white);
}