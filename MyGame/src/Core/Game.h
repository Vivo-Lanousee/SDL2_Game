#pragma once
#include <SDL.h>
#include <vector>
#include "../Objects/GameObject.h"
#include "../Objects/Player.h"
#include <SDL_ttf.h>

class Scene;
class Game {
public:
    Game();
    ~Game();

    bool Init(const char* title, int x, int y, int width, int height, bool fullscreen);
    void HandleEvents();
    void Update();
    void Render();
    void Clean();

    bool Running() { return isRunning; }

    void Quit() { isRunning = false; }

    // シーンを切り替える重要関数
    void ChangeScene(Scene* newScene);

    // 各シーンからアクセスするゲッター
    SDL_Renderer* GetRenderer() const { return renderer; }

    // 共通で使う便利関数
    void DrawText(const char* text, int x, int y, SDL_Color color);

private:
    bool isRunning;
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;

    // 現在のシーン（タイトル画面、プレイ画面など）
    Scene* currentScene;
};