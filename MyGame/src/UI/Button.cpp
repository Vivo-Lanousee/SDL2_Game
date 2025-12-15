#include "Button.h"
#include "../Core/Game.h" // ★ここでGameの中身を読み込む

Button::Button(int x, int y, int w, int h, std::string label)
    : rect{ x, y, w, h }, text(label), isHovered(false), isClicked(false)
{
}

bool Button::HandleEvents(SDL_Event* event) {
    if (event->type == SDL_MOUSEMOTION) {
        int mx = event->motion.x;
        int my = event->motion.y;
        isHovered = (mx >= rect.x && mx <= rect.x + rect.w &&
            my >= rect.y && my <= rect.y + rect.h);
    }

    if (event->type == SDL_MOUSEBUTTONDOWN) {
        if (event->button.button == SDL_BUTTON_LEFT && isHovered) {
            return true;
        }
    }
    return false;
}

void Button::Render(Game* game) {
    SDL_Renderer* r = game->GetRenderer();

    if (isHovered) {
        SDL_SetRenderDrawColor(r, 100, 100, 255, 255);
    }
    else {
        SDL_SetRenderDrawColor(r, 50, 50, 150, 255);
    }

    SDL_RenderFillRect(r, &rect);
    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
    SDL_RenderDrawRect(r, &rect);

    SDL_Color white = { 255, 255, 255, 255 };
    game->DrawText(text.c_str(), rect.x + 20, rect.y + 15, white);
}