#pragma once
#include <SDL.h>
#include <string> // ★追加：名前（タグ）を使うために必要

class Game; // 前方宣言

// すべてのゲーム内オブジェクトの親クラス
class GameObject {
public:
    GameObject(float x, float y, int w, int h, SDL_Texture* tex = nullptr)
        : x(x), y(y), width(w), height(h), texture(tex), angle(0),
        velX(0), velY(0), accX(0), accY(0),
        useGravity(false), isGrounded(false),
        isTrigger(false), // トリガー初期化
        isDead(false),    // ★追加：死亡フラグ初期化
        name("Object")    // ★追加：名前初期化（デフォルトはObject）
    {
    }

    virtual ~GameObject() {}
    virtual void Update(Game* game) = 0;
    virtual void Render(SDL_Renderer* renderer) = 0;

    // ★追加：何かに当たった時にPlaySceneから呼ばれる関数
    // 弾クラスなどでこれをオーバーライド（上書き）して使う
    virtual void OnTriggerEnter(GameObject* other) {
        // デフォルトでは何もしない
    }

    // 物理計算用メソッド（これをUpdateの最後に呼ぶ）
    void ApplyPhysics() {
        // 重力の適用
        if (useGravity) {
            velY += 0.5f; // 重力加速度（0.5ずつ加速）

            // 落下速度制限
            if (velY > 15.0f) {
                velY = 15.0f;
            }
        }

        // 加速度を速度に加算
        velX += accX;
        velY += accY;

        // 速度を位置に加算
        x += velX;
        y += velY;

        // 加速度リセット
        accX = 0;
        accY = 0;
    }

    // 座標修正用
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

    // 物理変数
    float velX, velY;
    float accX, accY;
    bool useGravity;
    bool isGrounded;

    // トリガーフラグ（trueなら物理的な押し戻しをしない）
    bool isTrigger;

    // ★追加機能
    bool isDead;      // trueになったらシーンが自動的に削除してくれる
    std::string name; // "Player", "Bullet", "Wall" などの識別用
};