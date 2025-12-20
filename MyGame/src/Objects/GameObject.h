#pragma once
#include <SDL.h>
#include <string>
#include "../Core/Camera.h" // Camera クラスの宣言が既に存在

// 前方宣言
class Game;

// すべてのゲーム内オブジェクトの親
class GameObject {
public:
    // コンストラクタ (変更なし)
    GameObject(float x, float y, int w, int h, SDL_Texture* tex = nullptr)
        : x(x), y(y), width(w), height(h), texture(tex), angle(0),
        velX(0), velY(0), accX(0), accY(0),
        useGravity(false), isGrounded(false),
        isTrigger(false),
        isDead(false),
        name("Object")
    {
    }

    virtual ~GameObject() {}
    virtual void Update(Game* game) = 0;

    void RenderWithCamera(SDL_Renderer* renderer, Camera* camera) {
        int drawX = (int)x;
        int drawY = (int)y;

        if (camera) {
            drawX -= (int)camera->x;
            drawY -= (int)camera->y;
        }
        OnRender(renderer, drawX, drawY);
    }

    // 衝突時のコールバック
    virtual void OnTriggerEnter(GameObject* other) {
    }

    void SetPos(float newX, float newY) {
        x = newX;
        y = newY;
    }

protected:
    // 子クラスで具体的な描画処理を書く
    virtual void OnRender(SDL_Renderer* renderer, int drawX, int drawY) = 0;

public:
    // 座標・サイズ
    float x, y;
    int width, height;

    // 見た目
    SDL_Texture* texture;
    double angle;

    // 物理変数
    float velX, velY;
    float accX, accY;
    bool useGravity;
    bool isGrounded;

    // フラグ関連
    bool isTrigger;
    bool isDead;

    // GUI表示用の名前
    std::string name;
};