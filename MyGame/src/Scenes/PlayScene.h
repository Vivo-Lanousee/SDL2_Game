#pragma once
#include "Scene.h"
#include <vector>
#include "../Objects/GameObject.h"
#include "../Objects/Player.h"
#include "../Objects/Bullet.h"
#include "../Core/Camera.h"
#include <memory>
#include "../TextureManager.h"

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
    // Playerは所有権をリスト(gameObjects)に渡すので、ここは参照用の生ポインタでOK
    Player* player;

    // GameObjectのリスト（所有権あり）
    std::vector<std::unique_ptr<GameObject>> gameObjects;

    // カメラ（所有権あり）
    std::unique_ptr<Camera> camera;
    TexturePtr playerTexture;
    TexturePtr bulletTexture;
};