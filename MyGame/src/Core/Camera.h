#pragma once
#include <SDL.h>

class GameObject;

class Camera {
public:
    Camera(int screenWidth, int screenHeight);

    /**
     * @brief ターゲットを追従する。
     * 内部で GameParams から最新のオフセットとマップ制限を読み込みます。
     */
    void Follow(GameObject* target);

    /**
     * @brief 最新のパラメータ設定を同期する
     */
    void SyncWithParams();

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
    float offsetX, offsetY;
};