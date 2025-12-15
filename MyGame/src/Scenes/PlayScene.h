#pragma once
#include "Scene.h"
#include <vector>
#include "../Objects/GameObject.h"
#include "../Objects/Player.h"
#include "../Objects/Bullet.h"
#include "../Core/Camera.h"

class PlayScene : public Scene {
public:
    // シーンに入った時
    void OnEnter(Game* game) override;

    // シーンを出る時
    void OnExit(Game* game) override;

    // 更新（移動など）
    void Update(Game* game) override;

    // 描画
    void Render(Game* game) override;

    // 入力
    void HandleEvents(Game* game) override;

private:
    // このシーンで使う変数たち
    Player* player;
    std::vector<GameObject*> gameObjects;

    Camera* camera;
    // テクスチャ
    SDL_Texture* playerTexture;
    SDL_Texture* bulletTexture;
};