#include "Camera.h"
#include "../Objects/GameObject.h"

// コンストラクタの実装
Camera::Camera(int screenWidth, int screenHeight)
    : x(0), y(0), w(screenWidth), h(screenHeight),
    limitX(2000), limitY(1000)
{
}

// Follow関数の実装
void Camera::Follow(GameObject* target) {
    if (!target) return;

    // ターゲットが画面のど真ん中に来るようにカメラ位置を計算
    // カメラX = (プレイヤーX + プレイヤー半幅) - (画面半幅)
    x = (target->x + target->width / 2) - (w / 2);
    y = (target->y + target->height / 2) - (h / 2);

    // マップの外側を映さないように制限
    if (x < 0) x = 0;
    if (y < 0) y = 0;

    if (x > limitX - w) x = limitX - w;
    if (y > limitY - h) y = limitY - h;
}

SDL_FPoint Camera::ScreenToWorld(int screenX, int screenY) {
    return {
        (float)screenX + x,
        (float)screenY + y
    };
}