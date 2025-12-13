#pragma once
#include <SDL.h>
class Game;

class Scene {
public:
    virtual ~Scene() {}

    // 初期化（入った瞬間）
    virtual void OnEnter(Game* game) = 0;

    // 更新
    virtual void Update(Game* game) = 0;

    // 描画
    virtual void Render(Game* game) = 0;

    // 終了（出る瞬間）
    virtual void OnExit(Game* game) = 0;
};