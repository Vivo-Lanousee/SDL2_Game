#include "Camera.h"
#include "../Objects/GameObject.h"

// コンストラクタの実装
Camera::Camera(int screenWidth, int screenHeight)
    : x(0), y(0), w(screenWidth), h(screenHeight),
    limitX(2000), limitY(1000),
    offsetX(0.0f), offsetY(0.0f) // オフセットを0で初期化
{
}

// Follow関数の実装
void Camera::Follow(GameObject* target) {
    if (!target) return;

    // ターゲットが画面のど真ん中に来る位置を基準に、offsetX/offsetYを加算
    // カメラX = (ターゲットの中心) - (画面の中心) + オフセット
    x = (target->x + target->width / 2.0f) - (w / 2.0f) + offsetX;
    y = (target->y + target->height / 2.0f) - (h / 2.0f) + offsetY;

    // マップの外側を映さないように制限
    if (x < 0) x = 0;
    if (y < 0) y = 0;

    if (x > (float)limitX - w) x = (float)limitX - w;
    if (y > (float)limitY - h) y = (float)limitY - h;
}

SDL_FPoint Camera::ScreenToWorld(int screenX, int screenY) {
    return {
        (float)screenX + x,
        (float)screenY + y
    };
}