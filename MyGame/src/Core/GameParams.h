#pragma once

#include <nlohmann/json.hpp> 
#include <map>
#include <string>

using json = nlohmann::json;

namespace PhysicsSettings {
    // GUIに表示される値を内部のピクセル値に変換するための係数
    constexpr float GravityScale = 100.0f;
}

// 移動モードの定義
enum class MovementType {
    Linear = 0,    // 拠点（目標）へ向かって一直線
    PathFollow = 1 // ウェイポイント（経路）を辿る
};

// 移動スタイルの定義 (追加)
enum class LocomotionType {
    Ground = 0,  // 地上を歩く
    Flying = 1,  // 空中を浮遊移動
    Jumping = 2  // 跳ねながら移動
};

// 攻撃方法の定義
enum class AttackType {
    Melee = 0,    // 近接（接触攻撃）
    Ranged = 1,   // 遠距離（弾を発射）
    Kamikaze = 2  // 自爆
};

struct PlayerParams {
    float moveSpeed = 300.0f;
    float jumpVelocity = 600.0f;
    float maxHealth = 100.0f;

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

struct GunParams {
    float fireRate = 0.2f;
    float bulletSpeed = 800.0f;
    int damage = 10;
    float spreadAngle = 5.0f;
    int shotCount = 1;
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    int magazineSize = 30;
    float reloadTime = 1.5f;
    std::string texturePath = "assets/images/guns/default_gun.png";

    friend void to_json(json& j, const GunParams& p) {
        j = json{
            {"fireRate", p.fireRate},
            {"bulletSpeed", p.bulletSpeed},
            {"damage", p.damage},
            {"spreadAngle", p.spreadAngle},
            {"shotCount", p.shotCount},
            {"offsetX", p.offsetX},
            {"offsetY", p.offsetY},
            {"magazineSize", p.magazineSize},
            {"reloadTime", p.reloadTime},
            {"texturePath", p.texturePath}
        };
    }
    friend void from_json(const json& j, GunParams& p) {
        if (j.contains("fireRate")) j.at("fireRate").get_to(p.fireRate);
        if (j.contains("bulletSpeed")) j.at("bulletSpeed").get_to(p.bulletSpeed);
        if (j.contains("damage")) j.at("damage").get_to(p.damage);
        if (j.contains("spreadAngle")) j.at("spreadAngle").get_to(p.spreadAngle);
        if (j.contains("shotCount"))   j.at("shotCount").get_to(p.shotCount);
        if (j.contains("offsetX"))     j.at("offsetX").get_to(p.offsetX);
        if (j.contains("offsetY"))     j.at("offsetY").get_to(p.offsetY);
        if (j.contains("magazineSize")) j.at("magazineSize").get_to(p.magazineSize);
        if (j.contains("reloadTime"))   j.at("reloadTime").get_to(p.reloadTime);
        if (j.contains("texturePath")) j.at("texturePath").get_to(p.texturePath);
    }
};

struct PhysicsParams {
    float gravity = 9.8f;
    float terminalVelocity = 1500.0f;

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
    int baseHealth = 100;
    int attackPower = 10;
    float baseSpeed = 50.0f;
    float attackRange = 100.0f;
    float attackInterval = 1.5f;
    MovementType moveMethod = MovementType::Linear;
    LocomotionType locomotionStyle = LocomotionType::Ground; // 追加
    AttackType attackMethod = AttackType::Melee;
    std::string texturePath = "assets/images/enemies/default_enemy.png";
    std::string bulletTexturePath = "assets/images/enemies/enemy_bullet.png"; // 追加

    friend void to_json(json& j, const EnemyParams& p) {
        j = json{
            {"baseHealth", p.baseHealth},
            {"attackPower", p.attackPower},
            {"baseSpeed", p.baseSpeed},
            {"attackRange", p.attackRange},
            {"attackInterval", p.attackInterval},
            {"moveMethod", static_cast<int>(p.moveMethod)},
            {"locomotionStyle", static_cast<int>(p.locomotionStyle)},
            {"attackMethod", static_cast<int>(p.attackMethod)},
            {"texturePath", p.texturePath},
            {"bulletTexturePath", p.bulletTexturePath}
        };
    }
    friend void from_json(const json& j, EnemyParams& p) {
        if (j.contains("baseHealth")) j.at("baseHealth").get_to(p.baseHealth);
        if (j.contains("attackPower")) j.at("attackPower").get_to(p.attackPower);
        if (j.contains("baseSpeed")) j.at("baseSpeed").get_to(p.baseSpeed);
        if (j.contains("attackRange")) j.at("attackRange").get_to(p.attackRange);
        if (j.contains("attackInterval")) j.at("attackInterval").get_to(p.attackInterval);
        if (j.contains("moveMethod")) p.moveMethod = static_cast<MovementType>(j.at("moveMethod").get<int>());
        if (j.contains("locomotionStyle")) p.locomotionStyle = static_cast<LocomotionType>(j.at("locomotionStyle").get<int>());
        if (j.contains("attackMethod")) p.attackMethod = static_cast<AttackType>(j.at("attackMethod").get<int>());
        if (j.contains("texturePath")) j.at("texturePath").get_to(p.texturePath);
        if (j.contains("bulletTexturePath")) j.at("bulletTexturePath").get_to(p.bulletTexturePath);
    }
};

struct CameraParams {
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    int limitX = 2000;
    int limitY = 1000;

    friend void to_json(json& j, const CameraParams& p) {
        j = json{
            {"offsetX", p.offsetX},
            {"offsetY", p.offsetY},
            {"limitX", p.limitX},
            {"limitY", p.limitY}
        };
    }
    friend void from_json(const json& j, CameraParams& p) {
        if (j.contains("offsetX")) j.at("offsetX").get_to(p.offsetX);
        if (j.contains("offsetY")) j.at("offsetY").get_to(p.offsetY);
        if (j.contains("limitX")) j.at("limitX").get_to(p.limitX);
        if (j.contains("limitY")) j.at("limitY").get_to(p.limitY);
    }
};

struct BaseParams {
    int maxHealth = 1000;
    float defense = 0.0f;
    std::string texturePath = "assets/images/base/gate.png";

    friend void to_json(json& j, const BaseParams& p) {
        j = json{
            {"maxHealth", p.maxHealth},
            {"defense", p.defense},
            {"texturePath", p.texturePath}
        };
    }
    friend void from_json(const json& j, BaseParams& p) {
        if (j.contains("maxHealth")) j.at("maxHealth").get_to(p.maxHealth);
        if (j.contains("defense")) j.at("defense").get_to(p.defense);
        if (j.contains("texturePath")) j.at("texturePath").get_to(p.texturePath);
    }
};

struct GameParams {
public:
    static GameParams& GetInstance() {
        static GameParams instance;
        return instance;
    }

    PlayerParams player;
    GunParams gun;
    PhysicsParams physics;
    EnemyParams enemy;
    CameraParams camera;
    BaseParams base;

    std::map<std::string, PlayerParams> playerPresets;
    std::string activePlayerPresetName;

    std::map<std::string, GunParams> gunPresets;
    std::string activeGunPresetName;

    std::map<std::string, EnemyParams> enemyPresets;
    std::string activeEnemyPresetName;

    std::map<std::string, CameraParams> cameraPresets;
    std::string activeCameraPresetName;

    friend void to_json(json& j, const GameParams& p) {
        j = json{
            {"Player", p.player},
            {"Gun", p.gun},
            {"Physics", p.physics},
            {"Enemy", p.enemy},
            {"Camera", p.camera},
            {"Base", p.base},
            {"PlayerPresets", p.playerPresets},
            {"ActivePlayerPreset", p.activePlayerPresetName},
            {"GunPresets", p.gunPresets},
            {"ActiveGunPreset", p.activeGunPresetName},
            {"EnemyPresets", p.enemyPresets},
            {"ActiveEnemyPreset", p.activeEnemyPresetName},
            {"CameraPresets", p.cameraPresets},
            {"ActiveCameraPreset", p.activeCameraPresetName}
        };
    }

    friend void from_json(const json& j, GameParams& p) {
        if (j.contains("Player")) j.at("Player").get_to(p.player);
        if (j.contains("Gun")) j.at("Gun").get_to(p.gun);
        if (j.contains("Physics")) j.at("Physics").get_to(p.physics);
        if (j.contains("Enemy")) j.at("Enemy").get_to(p.enemy);
        if (j.contains("Camera")) j.at("Camera").get_to(p.camera);
        if (j.contains("Base")) j.at("Base").get_to(p.base);
        if (j.contains("PlayerPresets")) j.at("PlayerPresets").get_to(p.playerPresets);
        if (j.contains("ActivePlayerPreset")) j.at("ActivePlayerPreset").get_to(p.activePlayerPresetName);
        if (j.contains("GunPresets")) j.at("GunPresets").get_to(p.gunPresets);
        if (j.contains("ActiveGunPreset")) j.at("ActiveGunPreset").get_to(p.activeGunPresetName);
        if (j.contains("EnemyPresets")) j.at("EnemyPresets").get_to(p.enemyPresets);
        if (j.contains("ActiveEnemyPreset")) j.at("ActiveEnemyPreset").get_to(p.activeEnemyPresetName);
        if (j.contains("CameraPresets")) j.at("CameraPresets").get_to(p.cameraPresets);
        if (j.contains("ActiveCameraPreset")) j.at("ActiveCameraPreset").get_to(p.activeCameraPresetName);
        p.applyActivePresets();
    }

    void applyActivePresets() {
        if (playerPresets.count(activePlayerPresetName)) player = playerPresets.at(activePlayerPresetName);
        if (gunPresets.count(activeGunPresetName)) gun = gunPresets.at(activeGunPresetName);
        if (enemyPresets.count(activeEnemyPresetName)) enemy = enemyPresets.at(activeEnemyPresetName);
        if (cameraPresets.count(activeCameraPresetName)) camera = cameraPresets.at(activeCameraPresetName);
    }

private:
    GameParams() {
        playerPresets["Default"] = player;
        activePlayerPresetName = "Default";
        gunPresets["Default"] = gun;
        activeGunPresetName = "Default";
        enemyPresets["Default"] = enemy;
        activeEnemyPresetName = "Default";
        cameraPresets["Default"] = camera;
        activeCameraPresetName = "Default";
    }
    GameParams(const GameParams&) = delete;
    GameParams& operator=(const GameParams&) = delete;
};