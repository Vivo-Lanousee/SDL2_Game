#pragma once

#include <nlohmann/json.hpp> 
#include <map>
#include <string>

using json = nlohmann::json;

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


// --- 追加: 銃のパラメータ構造体 ---
struct GunParams {
    float fireRate = 0.2f;      // 発射間隔（秒）
    float bulletSpeed = 800.0f; // 弾の速度
    int damage = 10;            // 攻撃力
    std::string texturePath = "assets/images/gun.png"; // 銃の画像パス

    // JSON シリアライズ/デシリアライズ用関数
    friend void to_json(json& j, const GunParams& p) {
        j = json{
            {"fireRate", p.fireRate},
            {"bulletSpeed", p.bulletSpeed},
            {"damage", p.damage},
            {"texturePath", p.texturePath}
        };
    }
    friend void from_json(const json& j, GunParams& p) {
        j.at("fireRate").get_to(p.fireRate);
        j.at("bulletSpeed").get_to(p.bulletSpeed);
        j.at("damage").get_to(p.damage);
        // パスがJSONにない場合の安全策
        if (j.contains("texturePath")) {
            j.at("texturePath").get_to(p.texturePath);
        }
    }
};


struct PhysicsParams {
    float gravity = 9.8f; // GUI表示用の重力加速度 (m/s^2 相当)
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
        j.at("baseSpeed").get_to(p.baseSpeed);
        j.at("baseHealth").get_to(p.baseHealth);
    }
};


// 全てのパラメータを管理するシングルトン構造体
struct GameParams {
    static GameParams& GetInstance() {
        static GameParams instance;
        return instance;
    }

    // 各パラメータ
    PlayerParams player;
    GunParams gun;      // ★追加: 現在の銃設定
    PhysicsParams physics;
    EnemyParams enemy;

    // プリセット管理
    std::map<std::string, PlayerParams> playerPresets;
    std::string activePlayerPresetName;

    std::map<std::string, GunParams> gunPresets;       // ★追加: 銃のプリセット
    std::string activeGunPresetName;

    std::map<std::string, EnemyParams> enemyPresets;
    std::string activeEnemyPresetName;


    // GameParams全体を JSON に変換する関数
    friend void to_json(json& j, const GameParams& p) {
        j = json{
            {"Player", p.player},
            {"Gun", p.gun},       // ★追加
            {"Physics", p.physics},
            {"Enemy", p.enemy},

            {"PlayerPresets", p.playerPresets},
            {"ActivePlayerPreset", p.activePlayerPresetName},

            {"GunPresets", p.gunPresets},        // ★追加
            {"ActiveGunPreset", p.activeGunPresetName}, // ★追加

            {"EnemyPresets", p.enemyPresets},
            {"ActiveEnemyPreset", p.activeEnemyPresetName}
        };
    }

    // JSON から GameParams 全体をロードする関数
    friend void from_json(const json& j, GameParams& p) {
        j.at("Player").get_to(p.player);
        if (j.contains("Gun")) j.at("Gun").get_to(p.gun); // ★追加
        j.at("Physics").get_to(p.physics);
        j.at("Enemy").get_to(p.enemy);

        if (j.contains("PlayerPresets")) j.at("PlayerPresets").get_to(p.playerPresets);
        if (j.contains("ActivePlayerPreset")) j.at("ActivePlayerPreset").get_to(p.activePlayerPresetName);

        if (j.contains("GunPresets")) j.at("GunPresets").get_to(p.gunPresets); // ★追加
        if (j.contains("ActiveGunPreset")) j.at("ActiveGunPreset").get_to(p.activeGunPresetName); // ★追加

        if (j.contains("EnemyPresets")) j.at("EnemyPresets").get_to(p.enemyPresets);
        if (j.contains("ActiveEnemyPreset")) j.at("ActiveEnemyPreset").get_to(p.activeEnemyPresetName);

        // ロード後、アクティブなプリセットを現在のパラメータに適用
        p.applyActivePresets();
    }

private:
    GameParams() {
        // 初期状態として「Default」プリセットを登録
        playerPresets["Default"] = player;
        activePlayerPresetName = "Default";

        gunPresets["Default"] = gun;       // ★追加
        activeGunPresetName = "Default";   // ★追加

        enemyPresets["Default"] = enemy;
        activeEnemyPresetName = "Default";
    }

    void applyActivePresets() {
        // Player 適用
        if (playerPresets.count(activePlayerPresetName)) {
            player = playerPresets.at(activePlayerPresetName);
        }
        else {
            activePlayerPresetName = "Default";
            if (playerPresets.count("Default")) player = playerPresets.at("Default");
        }

        // Gun 適用 ★追加
        if (gunPresets.count(activeGunPresetName)) {
            gun = gunPresets.at(activeGunPresetName);
        }
        else {
            activeGunPresetName = "Default";
            if (gunPresets.count("Default")) gun = gunPresets.at("Default");
        }

        // Enemy 適用
        if (enemyPresets.count(activeEnemyPresetName)) {
            enemy = enemyPresets.at(activeEnemyPresetName);
        }
        else {
            activeEnemyPresetName = "Default";
            if (enemyPresets.count("Default")) enemy = enemyPresets.at("Default");
        }
    }

    GameParams(const GameParams&) = delete;
    GameParams& operator=(const GameParams&) = delete;
};