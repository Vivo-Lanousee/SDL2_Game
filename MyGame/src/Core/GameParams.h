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
        if (j.contains("moveSpeed")) j.at("moveSpeed").get_to(p.moveSpeed);
        if (j.contains("jumpVelocity")) j.at("jumpVelocity").get_to(p.jumpVelocity);
        if (j.contains("maxHealth")) j.at("maxHealth").get_to(p.maxHealth);
    }
};


// --- 銃のパラメータ構造体 (同時発射数、オフセット、弾倉、リロードを追加) ---
struct GunParams {
    float fireRate = 0.2f;      // 発射間隔（秒）
    float bulletSpeed = 800.0f; // 弾の速度
    int damage = 10;            // 攻撃力
    float spreadAngle = 5.0f;   // 集弾率（拡散角度 0〜45度想定）

    int shotCount = 1;          // 同時発射数（ショットガン等）
    float offsetX = 0.0f;       // 銃の表示位置オフセットX
    float offsetY = 0.0f;       // 銃の表示位置オフセットY

    // ★追加パラメータ
    int magazineSize = 30;      // 最大装弾数
    float reloadTime = 1.5f;    // リロード時間（秒）

    std::string texturePath = "assets/images/guns/default_gun.png"; // 銃の画像パス

    // JSON シリアライズ用関数
    friend void to_json(json& j, const GunParams& p) {
        j = json{
            {"fireRate", p.fireRate},
            {"bulletSpeed", p.bulletSpeed},
            {"damage", p.damage},
            {"spreadAngle", p.spreadAngle},
            {"shotCount", p.shotCount},
            {"offsetX", p.offsetX},
            {"offsetY", p.offsetY},
            {"magazineSize", p.magazineSize}, // ★
            {"reloadTime", p.reloadTime},     // ★
            {"texturePath", p.texturePath}
        };
    }
    // JSON デシリアライズ用関数
    friend void from_json(const json& j, GunParams& p) {
        if (j.contains("fireRate")) j.at("fireRate").get_to(p.fireRate);
        if (j.contains("bulletSpeed")) j.at("bulletSpeed").get_to(p.bulletSpeed);
        if (j.contains("damage")) j.at("damage").get_to(p.damage);

        // 追加された項目は contains で存在チェックを行うと古いJSONとの互換性が保てます
        if (j.contains("spreadAngle")) j.at("spreadAngle").get_to(p.spreadAngle);
        if (j.contains("shotCount"))   j.at("shotCount").get_to(p.shotCount);
        if (j.contains("offsetX"))     j.at("offsetX").get_to(p.offsetX);
        if (j.contains("offsetY"))     j.at("offsetY").get_to(p.offsetY);

        // ★リロード関連のデシリアライズ
        if (j.contains("magazineSize")) j.at("magazineSize").get_to(p.magazineSize);
        if (j.contains("reloadTime"))   j.at("reloadTime").get_to(p.reloadTime);

        if (j.contains("texturePath")) j.at("texturePath").get_to(p.texturePath);
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
        if (j.contains("gravity")) j.at("gravity").get_to(p.gravity);
        if (j.contains("terminalVelocity")) j.at("terminalVelocity").get_to(p.terminalVelocity);
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
        if (j.contains("baseSpeed")) j.at("baseSpeed").get_to(p.baseSpeed);
        if (j.contains("baseHealth")) j.at("baseHealth").get_to(p.baseHealth);
    }
};


// 全てのパラメータを管理するシングルトン構造体
struct GameParams {
public:
    static GameParams& GetInstance() {
        static GameParams instance;
        return instance;
    }

    // 各パラメータ
    PlayerParams player;
    GunParams gun;
    PhysicsParams physics;
    EnemyParams enemy;

    // プリセット管理
    std::map<std::string, PlayerParams> playerPresets;
    std::string activePlayerPresetName;

    std::map<std::string, GunParams> gunPresets;
    std::string activeGunPresetName;

    std::map<std::string, EnemyParams> enemyPresets;
    std::string activeEnemyPresetName;


    // GameParams全体を JSON に変換する関数
    friend void to_json(json& j, const GameParams& p) {
        j = json{
            {"Player", p.player},
            {"Gun", p.gun},
            {"Physics", p.physics},
            {"Enemy", p.enemy},

            {"PlayerPresets", p.playerPresets},
            {"ActivePlayerPreset", p.activePlayerPresetName},

            {"GunPresets", p.gunPresets},
            {"ActiveGunPreset", p.activeGunPresetName},

            {"EnemyPresets", p.enemyPresets},
            {"ActiveEnemyPreset", p.activeEnemyPresetName}
        };
    }

    // JSON から GameParams 全体をロードする関数
    friend void from_json(const json& j, GameParams& p) {
        if (j.contains("Player")) j.at("Player").get_to(p.player);
        if (j.contains("Gun")) j.at("Gun").get_to(p.gun);
        if (j.contains("Physics")) j.at("Physics").get_to(p.physics);
        if (j.contains("Enemy")) j.at("Enemy").get_to(p.enemy);

        if (j.contains("PlayerPresets")) j.at("PlayerPresets").get_to(p.playerPresets);
        if (j.contains("ActivePlayerPreset")) j.at("ActivePlayerPreset").get_to(p.activePlayerPresetName);

        if (j.contains("GunPresets")) j.at("GunPresets").get_to(p.gunPresets);
        if (j.contains("ActiveGunPreset")) j.at("ActiveGunPreset").get_to(p.activeGunPresetName);

        if (j.contains("EnemyPresets")) j.at("EnemyPresets").get_to(p.enemyPresets);
        if (j.contains("ActiveEnemyPreset")) j.at("ActiveEnemyPreset").get_to(p.activeEnemyPresetName);

        // ロード後、アクティブなプリセットを現在のパラメータに適用
        p.applyActivePresets();
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

        // Gun 適用 
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

private:
    GameParams() {
        // 初期状態として「Default」プリセットを登録
        playerPresets["Default"] = player;
        activePlayerPresetName = "Default";

        gunPresets["Default"] = gun;
        activeGunPresetName = "Default";

        enemyPresets["Default"] = enemy;
        activeEnemyPresetName = "Default";
    }

    GameParams(const GameParams&) = delete;
    GameParams& operator=(const GameParams&) = delete;
};