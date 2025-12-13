#pragma once
#include "GameObject.h"
#include <cmath> 
#include "Bullet.h"

struct UnitStatus {
    int hp;
    int maxHp;
    float speed;     // 移動速度
    int attackPower; // 攻撃力
};

/// <summary>
/// プレイヤーのオブジェクト情報
/// </summary>
class Player : public GameObject {
public:
    // コンストラクタ
    Player(float x, float y, SDL_Texture* tex) : GameObject(x, y, 46, 128, tex) {
        // ★ステータスの初期値を設定
        status.hp = 100;
        status.maxHp = 100;
        status.speed = 5.0f;     // 今まで「5」と書いていた速度
        status.attackPower = 10;
        angle = 0;
    }

    void Update() override {
        // キー入力状態を取得
        const Uint8* state = SDL_GetKeyboardState(NULL);

        if (state[SDL_SCANCODE_UP])    y -= status.speed;
        if (state[SDL_SCANCODE_DOWN])  y += status.speed;
        if (state[SDL_SCANCODE_LEFT])  x -= status.speed;
        if (state[SDL_SCANCODE_RIGHT]) x += status.speed;

        // マウスの方向を向く計算
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        float centerX = x + width / 2;
        float centerY = y + height / 2;

        // 角度計算 (M_PI が使えない環境も考慮して数値直書きで安定させます)
        //angle = atan2(mouseY - centerY, mouseX - centerX) * 180.0 / 3.14159265;
    }

    void Render(SDL_Renderer* renderer) override {
        SDL_Rect destRect = { (int)x, (int)y, width, height };

        if (texture) {
            // 画像を回転させて描画
            SDL_RenderCopyEx(renderer, texture, NULL, &destRect, angle, NULL, SDL_FLIP_NONE);
        }
        else {
            // 画像がないなら赤い四角（デバッグ用）
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(renderer, &destRect);
        }

        // レーザー（緑色の線）
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        SDL_RenderDrawLine(renderer, (int)(x + width / 2), (int)(y + height / 2), mouseX, mouseY);
    }

    Bullet* Shoot(int mouseX, int mouseY, SDL_Texture* bulletTex) {

        // 1. 発射位置の計算（自分の中心）
        float spawnX = x + (width / 2) - 5;
        float spawnY = y + (height / 2) - 5;

        // 角度の計算
        float centerX = x + width / 2;
        float centerY = y + height / 2;
        double radian = atan2(mouseY - centerY, mouseX - centerX);
        double angle = radian * 180.0 / 3.14159265;

        // 弾を作って渡す（ここではリストに追加しない）
        return new Bullet(spawnX, spawnY, angle, bulletTex);
    }


    // --- 以下、便利機能 ---

    // ダメージを受ける処理
    void TakeDamage(int damage) {
        status.hp -= damage;
        if (status.hp < 0) status.hp = 0;
    }

    // HPを取得する（UI表示などで使う）
    int GetHP() const { return status.hp; }

    // 最大HPを取得する
    int GetMaxHP() const { return status.maxHp; }

public:
    // ステータスデータ
    // 外部（PlaySceneなど）から直接見れるように public にしています
    UnitStatus status;

private:
    double angle;
};