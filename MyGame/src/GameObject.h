#pragma once
#include <SDL.h>

// すべてのゲーム内オブジェクトの親クラス
class GameObject {
public:
    // コンストラクタ（位置とサイズを指定）
    GameObject(float x, float y, int w, int h)
        : x(x), y(y), width(w), height(h) {
    }

    // 仮想デストラクタ
    virtual ~GameObject() {}

    virtual void Update() = 0;
    virtual void Render(SDL_Renderer* renderer) = 0;

    // 変数はアクセスしやすいようにpublic
    float x, y;
    int width, height;
};