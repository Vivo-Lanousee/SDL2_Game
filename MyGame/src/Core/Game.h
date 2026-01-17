#pragma once
#include <SDL.h>
#include <vector>
#include <memory> 

class Scene;
class InputHandler;
class GameObject;
class Bullet;
struct SDL_Texture;

struct WindowDestroyer {
    void operator()(SDL_Window* w) const {
        if (w) SDL_DestroyWindow(w);
    }
};

struct RendererDestroyer {
    void operator()(SDL_Renderer* r) const {
        if (r) SDL_DestroyRenderer(r);
    }
};

using WindowPtr = std::unique_ptr<SDL_Window, WindowDestroyer>;
using RendererPtr = std::unique_ptr<SDL_Renderer, RendererDestroyer>;

class Game {
public:
    Game();
    ~Game();

    bool Init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
    void HandleEvents();
    void Update();
    void Render();
    void Clean();
    bool Running() { return isRunning; }
    void Quit() { isRunning = false; }

    // シーン遷移を予約する
    void ChangeScene(Scene* newScene);

    SDL_Renderer* GetRenderer() const { return renderer.get(); }
    InputHandler* GetInput() const { return inputHandler.get(); }

    std::vector<std::unique_ptr<GameObject>>& GetPendingObjects() { return pendingObjects; }
    void ClearPendingObjects() { pendingObjects.clear(); }
    void Instantiate(std::unique_ptr<GameObject> obj) { pendingObjects.push_back(std::move(obj)); }

    std::vector<std::unique_ptr<GameObject>>& GetCurrentSceneObjects();
    SDL_Texture* GetBulletTexture();
    void DrawText(const char* text, int x, int y, SDL_Color color);

private:
    bool isRunning;
    bool isCleanedUp = false;

    WindowPtr window;
    RendererPtr renderer;

    std::unique_ptr<InputHandler> inputHandler;
    std::unique_ptr<Scene> currentScene;

    // 次のフレームで切り替えるためのシーン保持
    Scene* nextScene = nullptr;

    std::vector<std::unique_ptr<GameObject>> pendingObjects;
};