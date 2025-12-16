#pragma once

// ★★★ 修正箇所 1: PhysicsSettings 名前空間とスケーリング定数を定義 ★★★
namespace PhysicsSettings {
    // GUIに表示される値を内部のピクセル値に変換するための係数
    constexpr float GravityScale = 100.0f;
}

struct PlayerParams {
    float moveSpeed = 300.0f;
    float jumpVelocity = 600.0f;
    float maxHealth = 100.0f;
};

struct PhysicsParams {
    // ★★★ 修正箇所 2: gravity の初期値を 9.8f に戻し、GUI表示と一致させる ★★★
    // 内部では PhysicsSettings::GravityScale (100.0f) を乗算して 980.0f として使用されます。
    float gravity = 9.8f; // GUI表示用の重力加速度 (m/s^2 相当)

    // ★★★ 修正箇所 3: 終端速度を維持 ★★★
    float terminalVelocity = 1500.0f; // 終端速度 (px/s)
};

struct EnemyParams {
    float baseSpeed = 50.0f;
    int baseHealth = 100;
};

// 全てのパラメータを管理するシングルトン的な構造体
struct GameParams {
    static GameParams& GetInstance() {
        static GameParams instance;
        return instance;
    }

    // パラメータ
    PlayerParams player;
    PhysicsParams physics;
    EnemyParams enemy;

private:
    GameParams() = default; // シングルトン化
    GameParams(const GameParams&) = delete;
    GameParams& operator=(const GameParams&) = delete;
};