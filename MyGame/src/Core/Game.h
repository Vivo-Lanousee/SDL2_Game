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

    // text:表示したい文字, x,y:座標, color:色
    void DrawText(const char* text, int x, int y, SDL_Color color);

    bool Running() { return isRunning; }

private:
    bool isRunning;
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;

    //プレイヤーデータ
    Player* player;

    //ゲーム全オブジェクト
    std::vector<GameObject*> gameObjects;

    SDL_Texture* playerTexture;
    SDL_Texture* bulletTexture;
};