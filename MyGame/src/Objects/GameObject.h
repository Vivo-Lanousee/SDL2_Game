#pragma once
#include <SDL.h>

class Game; // 前方宣言

// すべてのゲーム内オブジェクトの親クラス
class GameObject {
public:
    GameObject(float x, float y, int w, int h, SDL_Texture* tex = nullptr)
        : x(x), y(y), width(w), height(h), texture(tex), angle(0),
        velX(0), velY(0), accX(0), accY(0),
        useGravity(false), isGrounded(false),
        isTrigger(false) // ★追加：初期化（デフォルトは実体あり）
    {
    }

    virtual ~GameObject() {}
    virtual void Update(Game* game) = 0;
    virtual void Render(SDL_Renderer* renderer) = 0;

    // 物理計算用メソッド（これをUpdateの最後に呼ぶ）
    void ApplyPhysics() {
        // 重力の適用
        if (useGravity) {
            velY += 0.5f; // 重力加速度（0.5ずつ加速）

            // 落下速度制限（これがないと無限に加速してすり抜けます）
            if (velY > 15.0f) {
                velY = 15.0f;
            }
        }

        // 加速度を速度に加算（横移動の慣性などをつけたい場合に使用）
        velX += accX;
        velY += accY;

        // 速度を位置に加算（実際に移動させる）
        x += velX;
        y += velY;

        // 加速度は毎フレームリセットするのが一般的
        accX = 0;
        accY = 0;
    }

    // 座標修正用（Physicsクラスから押し戻すときに使う）
    void SetPos(float newX, float newY) {
        x = newX;
        y = newY;
    }

    // 座標・サイズ
    float x, y;
    int width, height;

    // 見た目
    SDL_Texture* texture;
    double angle;

    // Physics.h がアクセスする変数
    float velX, velY; // 速度 (Velocity)
    float accX, accY; // 加速度 (Acceleration)
    bool useGravity;  // 重力フラグ
    bool isGrounded;  // 着地フラグ

    // ★追加：トリガーフラグ
    // trueにすると、Physics::ResolveCollision（押し戻し）の対象にならず、すり抜けます。
    // （弾、コイン、ゴール判定などに使用）
    bool isTrigger;
};