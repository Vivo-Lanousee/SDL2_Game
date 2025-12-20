#pragma once
#include <vector>
#include <memory>
#include <SDL.h>

// 前方宣言: 実体は Scene.cpp でインクルードする Game.h にあります
class Game;
class GameObject;

/**
 * @brief 全シーンの基底クラス
 */
class Scene {
public:
    virtual ~Scene() = default;
    virtual void OnEnter(Game* game) = 0;
    virtual void OnExit(Game* game) = 0;
    virtual void HandleEvents(Game* game, SDL_Event* event) = 0;
    virtual void Render(Game* game) = 0;

    /**
     * @brief シーンの共通更新フロー（物理・衝突・クリーンアップ）
     * 実装は Scene.cpp に記述し、Game クラスの関数を呼び出せるようにします
     */
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