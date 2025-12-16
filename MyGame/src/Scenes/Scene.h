#pragma once
#include <vector>  // ★必須
#include <memory>  // ★必須
#include "../Objects/GameObject.h" // ★GameObject型を知るために必要

class Game; // 前方宣言

class Scene {
public:
    virtual ~Scene() {}
    virtual void OnEnter(Game* game) = 0;
    virtual void OnExit(Game* game) = 0;
    virtual void Update(Game* game) = 0;
    virtual void Render(Game* game) = 0;
    virtual void HandleEvents(Game* game) = 0;

    // ★★★ この部分を追加してください！ ★★★
    // これがないと PlayScene で override できません。
    virtual std::vector<std::unique_ptr<GameObject>>& GetObjects() {
        // デフォルトでは空のリストを返す（タイトル画面など、オブジェクトがないシーン用）
        static std::vector<std::unique_ptr<GameObject>> emptyList;
        return emptyList;
    }
};