#pragma once

// ★★★ 修正箇所 1: JSONライブラリのインクルードと名前空間の使用 ★★★
#include <nlohmann/json.hpp> 
using json = nlohmann::json;
// ★★★ 修正終わり ★★★


// ★★★ 修正箇所 2: PhysicsSettings 名前空間とスケーリング定数を定義 ★★★
namespace PhysicsSettings {
    // GUIに表示される値を内部のピクセル値に変換するための係数
    constexpr float GravityScale = 100.0f;
}


struct PlayerParams {
    float moveSpeed = 300.0f;
    float jumpVelocity = 600.0f;
    float maxHealth = 100.0f;

    // JSON シリアライズ/デシリアライズ用関数
    friend void to_json(json& j, const PlayerParams& p) {
        j = json{
            {"moveSpeed", p.moveSpeed},
            {"jumpVelocity", p.jumpVelocity},
            {"maxHealth", p.maxHealth}
        };
    }
    friend void from_json(const json& j, PlayerParams& p) {
        j.at("moveSpeed").get_to(p.moveSpeed);
        j.at("jumpVelocity").get_to(p.jumpVelocity);
        j.at("maxHealth").get_to(p.maxHealth);
    }
};


struct PhysicsParams {
    // ★★★ 修正箇所 2: gravity の初期値を 9.8f に戻し、GUI表示と一致させる ★★★
    float gravity = 9.8f; // GUI表示用の重力加速度 (m/s^2 相当)
    // ★★★ 修正箇所 3: 終端速度を維持 ★★★
    float terminalVelocity = 1500.0f; // 終端速度 (px/s)

    // JSON シリアライズ/デシリアライズ用関数
    friend void to_json(json& j, const PhysicsParams& p) {
        j = json{
            {"gravity", p.gravity},
            {"terminalVelocity", p.terminalVelocity}
        };
    }
    friend void from_json(const json& j, PhysicsParams& p) {
        j.at("gravity").get_to(p.gravity);
        j.at("terminalVelocity").get_to(p.terminalVelocity);
    }
};


struct EnemyParams {
    float baseSpeed = 50.0f;
    int baseHealth = 100;

    // JSON シリアライズ/デシリアライズ用関数
    friend void to_json(json& j, const EnemyParams& p) {
        j = json{
            {"baseSpeed", p.baseSpeed},
            {"baseHealth", p.baseHealth}
        };
    }
    friend void from_json(const json& j, EnemyParams& p) {
        // baseHealth は int ですが、JSONはintとfloatの変換を自動で行うため、get_toで問題ありません。
        j.at("baseSpeed").get_to(p.baseSpeed);
        j.at("baseHealth").get_to(p.baseHealth);
    }
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

    // GameParams全体を JSON に変換する関数
    friend void to_json(json& j, const GameParams& p) {
        j = json{
            {"Player", p.player},
            {"Physics", p.physics},
            {"Enemy", p.enemy}
        };
    }

    // JSON から GameParams 全体をロードする関数
    friend void from_json(const json& j, GameParams& p) {
        j.at("Player").get_to(p.player);
        j.at("Physics").get_to(p.physics);
        j.at("Enemy").get_to(p.enemy);
    }

private:
    GameParams() = default; // シングルトン化
    GameParams(const GameParams&) = delete;
    GameParams& operator=(const GameParams&) = delete;
};