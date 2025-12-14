#pragma once
#include "GameObject.h"
#include <cmath> 
#include "../Core/Game.h"
#include "Bullet.h"
#include "../Core/InputHandler.h"

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
        // ステータスの初期値を設定
        status.hp = 100;
        status.maxHp = 100;
        status.speed = 5.0f;
        status.attackPower = 10;
        angle = 0;

        useGravity = true;

        // ★重要：自分の名前をセットする
        // これがないと、Bulletが「これはプレイヤーだ」と認識できず、当たって消えてしまいます。
        name = "Player";
    }

    void Update(Game* game) override {
        InputHandler* input = game->GetInput();

        // ■ 左右移動
        velX = 0;
        if (input->IsPressed(GameAction::MoveLeft))  velX = -status.speed;
        if (input->IsPressed(GameAction::MoveRight)) velX = status.speed;

        // ■ ジャンプ
        if (input->IsPressed(GameAction::MoveUp)) {
            if (isGrounded) {
                velY = -14.0f; // ジャンプ力
                isGrounded = false;
            }
        }

        // ■ マウスの方向を向く処理
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        float centerX = x + width / 2;
        float centerY = y + height / 2;

        // ★コメントアウトされていた計算を復活させました
        double radian = atan2(mouseY - centerY, mouseX - centerX);
        angle = 0;// radian * 180.0 / 3.14159265;
    }

    void Render(SDL_Renderer* renderer) override {
        SDL_Rect destRect = { (int)x, (int)y, width, height };

        if (texture) {
            SDL_RenderCopyEx(renderer, texture, NULL, &destRect, angle, NULL, SDL_FLIP_NONE);
        }
        else {
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
        // 発射位置の計算
        float spawnX = x + (width / 2) - 5;
        float spawnY = y + (height / 2) - 5;

        // 角度の計算
        float centerX = x + width / 2;
        float centerY = y + height / 2;
        double radian = atan2(mouseY - centerY, mouseX - centerX);
        double angle = radian * 180.0 / 3.14159265;

        return new Bullet(spawnX, spawnY, angle, bulletTex);
    }

    // --- 便利機能 ---
    void TakeDamage(int damage) {
        status.hp -= damage;
        if (status.hp < 0) status.hp = 0;
    }

    int GetHP() const { return status.hp; }
    int GetMaxHP() const { return status.maxHp; }

public:
    UnitStatus status;

private:
    double angle;
};