#pragma once
#include <SDL.h>

class Game;
// すべてのゲーム内オブジェクトの親クラス
class GameObject {
public:
    GameObject(float x, float y, int w, int h, SDL_Texture* tex = nullptr)
        : x(x), y(y), width(w), height(h), texture(tex), angle(0),
        velX(0), velY(0), accX(0), accY(0), useGravity(false), isGrounded(false) {
    }

    virtual ~GameObject() {}
    virtual void Update(Game* game) = 0;
    virtual void Render(SDL_Renderer* renderer) = 0;

    // 物理計算用メソッド（これをUpdateの最後に呼ぶ）
    void ApplyPhysics() {
        // 重力の適用
        if (useGravity) {
            accY = 0.5f; // 重力の強さ（お好みで調整）
        }

        // 速度に加速度を足す
        velX += accX;
        velY += accY;

        // 位置に速度を足す
        x += velX;
        y += velY;

        // 加速度は毎フレームリセットしないと加速し続けるので注意
        // （ただし重力のような定数は毎回セットするか、ここでリセットしない設計にする）
        // 今回はシンプルに「重力は毎回セット」「入力による加速度は直接速度をいじる」方針にします
    }

    // 座標修正用（当たり判定で押し戻すときに使う）
    void SetPos(float newX, float newY) {
        x = newX;
        y = newY;
    }

    float x, y;
    int width, height;
    SDL_Texture* texture;
    double angle;

    // 物理演算用変数
    float velX, velY; // 速度 (Velocity)
    float accX, accY; // 加速度 (Acceleration)
    bool useGravity;  // 重力を使うか？
    bool isGrounded;  // 地面に足がついているか？
};