#pragma once
#include <vector>
#include <memory>
#include <SDL.h>

class Game;
class GameObject;

class Scene {
public:
    virtual ~Scene() = default;
    virtual void OnEnter(Game* game) = 0;
    virtual void OnExit(Game* game) = 0;

    // SDL_Event* を受け取るように変更
    virtual void HandleEvents(Game* game, SDL_Event* event) = 0;

    virtual void Update(Game* game) = 0;
    virtual void Render(Game* game) = 0;

    virtual bool ShowImGui() const { return false; }
    virtual std::vector<std::unique_ptr<GameObject>>& GetObjects() = 0;
};