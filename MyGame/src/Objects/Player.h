#pragma once
#include "GameObject.h"
#include <cmath>
#include "../Core/Game.h"
#include "Bullet.h"
#include "../Core/InputHandler.h"
#include "../Core/Camera.h" 

struct UnitStatus {
    int hp;
    int maxHp;
    float speed;
    int attackPower;
};

class Player : public GameObject {
public:
    Player(float x, float y, SDL_Texture* tex, SDL_Texture* bulletTex, Camera* cam)
        : GameObject(x, y, 46, 128, tex)
    {
        status.hp = 100;
        status.maxHp = 100;
        status.speed = 5.0f;
        status.attackPower = 10;
        angle = 0;
        useGravity = true;
        name = "Player";
        this->bulletTexture = bulletTex;

        this->camera = cam;
    }

    void Update(Game* game) override {
        InputHandler* input = game->GetInput();

        // 移動処理
        velX = 0;
        if (input->IsPressed(GameAction::MoveLeft))  velX = -status.speed;
        if (input->IsPressed(GameAction::MoveRight)) velX = status.speed;

        // ジャンプ
        if (input->IsJustPressed(GameAction::MoveUp)) {
            if (isGrounded) {
                velY = -14.0f;
                isGrounded = false;
            }
        }

        // マウス座標
        int screenMouseX, screenMouseY;
        SDL_GetMouseState(&screenMouseX, &screenMouseY);
        SDL_FPoint worldMouse = camera->ScreenToWorld(screenMouseX, screenMouseY);

        // プレイヤーの中心
        float centerX = x + width / 2;
        float centerY = y + height / 2;
        double radian = atan2(worldMouse.y - centerY, worldMouse.x - centerX);
        //angle = radian * 180.0 / 3.14159265;


        // 射撃処理
        if (input->IsJustPressed(GameAction::Shoot)) {
            // 変換済みのワールド座標を渡す
            Bullet* newBullet = Shoot(worldMouse.x, worldMouse.y, bulletTexture);

            if (newBullet) {
                game->Instantiate(newBullet);
            }
        }
    }

    // 描画処理（OnRender）
    void OnRender(SDL_Renderer* renderer, int drawX, int drawY) override {
        // プレイヤーの描画
        SDL_Rect destRect = { drawX, drawY, width, height };

        if (texture) {
            SDL_RenderCopyEx(renderer, texture, NULL, &destRect, angle, NULL, SDL_FLIP_NONE);
        }
        else {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(renderer, &destRect);
        }

        // レーザー（緑色の線）
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        int mx, my;
        SDL_GetMouseState(&mx, &my);
        int lineStartX = drawX + width / 2;
        int lineStartY = drawY + height / 2;

        SDL_RenderDrawLine(renderer, lineStartX, lineStartY, mx, my);
    }

    Bullet* Shoot(float targetX, float targetY, SDL_Texture* bulletTex) {
        // 発射位置 (ワールド座標)
        float spawnX = x + (width / 2) - 5;
        float spawnY = y + (height / 2) - 5;

        // 角度計算
        float centerX = x + width / 2;
        float centerY = y + height / 2;
        double radian = atan2(targetY - centerY, targetX - centerX);
        double angle = radian * 180.0 / 3.14159265;

        return new Bullet(spawnX, spawnY, angle, bulletTex);
    }

    // ... 便利機能 ...
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
    SDL_Texture* bulletTexture;
    Camera* camera;
};