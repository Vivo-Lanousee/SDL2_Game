#pragma once
#include <SDL.h>

class GameObject;

class Camera {
public:
    Camera(int screenWidth, int screenHeight);

    void Follow(GameObject* target);
    SDL_Rect GetRect() const {
        return { (int)x, (int)y, w, h };
    }
    SDL_FPoint ScreenToWorld(int screenX, int screenY);

    // 座標プロパティ
    float x, y;
    int w, h;

    // マップの広さ制限
    int limitX, limitY;

    // ターゲットの中心からのオフセット量
    // 例: offsetX = 100 にすると、ターゲットより右側がより広く映るようになります
    float offsetX, offsetY;
};