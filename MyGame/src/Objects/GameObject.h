#pragma once
#include <SDL.h>

// すべてのゲーム内オブジェクトの親クラス
class GameObject {
public:
    // コンストラクタ（位置とサイズを指定）
    // texture = nullptr なら四角形を描画する
    GameObject(float x, float y, int w, int h, SDL_Texture* tex = nullptr)
        : x(x), y(y), width(w), height(h), texture(tex), angle(0) {
    }

    virtual ~GameObject() {}
    virtual void Update() = 0;

    virtual void Render(SDL_Renderer* renderer) = 0;
    float x, y;
    int width, height;

    // 
    SDL_Texture* texture; // 画像データへの参照
    double angle;         // 回転角度（度数法）
};