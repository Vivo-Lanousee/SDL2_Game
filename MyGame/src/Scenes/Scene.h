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
    virtual void HandleEvents(Game* game, SDL_Event* event) = 0;
    virtual void Render(Game* game) = 0;

    void Update(Game* game);

    virtual bool ShowImGui() const { return false; }
    virtual std::vector<std::unique_ptr<GameObject>>& GetObjects() = 0;

protected:
    // 各シーン固有のロジック
    virtual void OnUpdate(Game* game) = 0;

private:
    // AABBによる重なり判定
    bool CheckOverlap(GameObject* a, GameObject* b);
};