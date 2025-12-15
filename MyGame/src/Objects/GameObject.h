#pragma once
#include <SDL.h>
#include <string>
#include "../Core/Camera.h" // カメラの座標(x,y)を使うためインクルードが必要

class Game;

// すべてのゲーム内オブジェクトの親クラス
class GameObject {
public:
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

    // ★変更点1：Renderは「仮想関数」ではなく「普通の関数」にする
    // ここでカメラ座標の引き算をまとめて行う（テンプレートメソッド）
    void Render(SDL_Renderer* renderer, Camera* camera) {
        // 1. ワールド座標をコピー
        int drawX = (int)x;
        int drawY = (int)y;

        // 2. カメラがある場合、ズラす（共通処理）
        if (camera) {
            drawX -= (int)camera->x;
            drawY -= (int)camera->y;
        }

        // 3. 計算済みの座標を使って、実際の描画処理を呼び出す
        OnRender(renderer, drawX, drawY);
    }

    // 弾クラスなどでこれをオーバーライド（上書き）して使う
    virtual void OnTriggerEnter(GameObject* other) {
        // デフォルトでは何もしない
    }

    // 物理計算用メソッド
    void ApplyPhysics() {
        if (useGravity) {
            velY += 0.5f;
            if (velY > 15.0f) velY = 15.0f;
        }

        velX += accX;
        velY += accY;

        x += velX;
        y += velY;

        accX = 0;
        accY = 0;
    }

    void SetPos(float newX, float newY) {
        x = newX;
        y = newY;
    }

protected:
    // ★変更点2：子クラスが実装するのはこっち！
    // drawX, drawY はすでにカメラ補正済みの座標が入ってくる
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

    // トリガーフラグ
    bool isTrigger;

    bool isDead;
    std::string name;
};