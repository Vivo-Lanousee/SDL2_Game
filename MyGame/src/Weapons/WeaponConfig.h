#pragma once
#include <string>

struct WeaponConfig {
    std::string name;       // 武器の表示名
    int damage;             // 弾丸が与えるダメージ
    float fireRate;         // 毎秒の発射回数 
    float range;            // 射程距離
    float bulletSpeed;      // 弾丸の移動速度 
    float spreadAngle;      // 集弾性の角度 -0.0fで完全な精度。
    int   bulletWidth;      // 弾丸の幅
    int   bulletHeight;     // 弾丸の高さ
    // 弾丸のテクスチャIDなどを追加する
};

// 武器のプロトタイプ定義例 
namespace WeaponPrototypes {
    const WeaponConfig BasicCannon = {
        "Basic Cannon",
        20,     // ダメージ
        2.0f,   // 発射レート (毎秒2発)
        200.0f, // 射程
        15.0f,  // 弾速
        10.0f,  // 集弾性 (±5度のブレ)
        4, 4    // 弾サイズ
    };

    const WeaponConfig HeavyLaser = {
        "Heavy Laser",
        100,
        0.5f,   // 毎秒0.5発 (2秒に1発)
        350.0f,
        30.0f,  // 超弾速
        0.0f,   // 完全な集弾性
        8, 8
    };
}