#pragma once
#include <SDL.h>
#include <vector>
#include "GameObject.h"
#include "Player.h"

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

private:
    bool isRunning;
    SDL_Window* window;
    SDL_Renderer* renderer;


    Player* player;

    //ゲーム全オブジェクト
    std::vector<GameObject*> gameObjects;

    SDL_Texture* playerTexture;
    SDL_Texture* bulletTexture;
};