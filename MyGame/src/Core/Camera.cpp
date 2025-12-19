#include "Camera.h"
#include "../Objects/GameObject.h"
#include "../Core/GameParams.h" // 追加：パラメータ取得用

// コンストラクタの実装
Camera::Camera(int screenWidth, int screenHeight)
    : x(0), y(0), w(screenWidth), h(screenHeight),
    limitX(2000), limitY(1000),
    offsetX(0.0f), offsetY(0.0f)
{
}

// パラメータ同期関数の実装
void Camera::SyncWithParams() {
    // シングルトンから最新のエディタ設定を取得
    auto& cp = GameParams::GetInstance().camera;

    this->offsetX = cp.offsetX;
    this->offsetY = cp.offsetY;
    this->limitX = cp.limitX;
    this->limitY = cp.limitY;
}

// Follow関数の実装
void Camera::Follow(GameObject* target) {
    if (!target) return;

    // 追従計算の前に、エディタで変更された可能性のある値を反映
    SyncWithParams();

    // ターゲットが画面のど真ん中に来るようにカメラ位置を計算 + オフセット
    x = (target->x + target->width / 2.0f) - (w / 2.0f) + offsetX;
    y = (target->y + target->height / 2.0f) - (h / 2.0f) + offsetY;

    // マップの外側を映さないように制限 (SyncWithParamsで更新されたlimitX/Yを使用)
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