#pragma once

#include <nlohmann/json.hpp> 
#include <map>
using json = nlohmann::json;

namespace PhysicsSettings {
    // GUIに表示される値を内部のピクセル値に変換するための係数
    constexpr float GravityScale = 100.0f;
}


struct PlayerParams {
    float moveSpeed = 300.0f;
    float jumpVelocity = 600.0f;
    float maxHealth = 100.0f;

    // JSON シリアライズ/デシリアライズ用関数 (変更なし)
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
    float gravity = 9.8f; // GUI表示用の重力加速度 (m/s^2 相当)
    float terminalVelocity = 1500.0f; // 終端速度 (px/s)

    // JSON シリアライズ/デシリアライズ用関数 (変更なし)
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

    // JSON シリアライズ/デシリアライズ用関数 (変更なし)
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

    std::map<std::string, PlayerParams> playerPresets;
    std::string activePlayerPresetName;

    std::map<std::string, EnemyParams> enemyPresets;
    std::string activeEnemyPresetName;


    // GameParams全体を JSON に変換する関数
    friend void to_json(json& j, const GameParams& p) {
        j = json{
            {"Player", p.player},
            {"Physics", p.physics},
            {"Enemy", p.enemy},

            // ★★★ 修正箇所 4: プリセット関連をJSONに含める ★★★
            {"PlayerPresets", p.playerPresets},
            {"ActivePlayerPreset", p.activePlayerPresetName},
            {"EnemyPresets", p.enemyPresets},
            {"ActiveEnemyPreset", p.activeEnemyPresetName}
        };
    }

    // JSON から GameParams 全体をロードする関数
    friend void from_json(const json& j, GameParams& p) {
        j.at("Player").get_to(p.player);
        j.at("Physics").get_to(p.physics);
        j.at("Enemy").get_to(p.enemy);

        if (j.contains("PlayerPresets")) j.at("PlayerPresets").get_to(p.playerPresets);
        if (j.contains("ActivePlayerPreset")) j.at("ActivePlayerPreset").get_to(p.activePlayerPresetName);

        if (j.contains("EnemyPresets")) j.at("EnemyPresets").get_to(p.enemyPresets);
        if (j.contains("ActiveEnemyPreset")) j.at("ActiveEnemyPreset").get_to(p.activeEnemyPresetName);

        // ロード後、アクティブなプリセットを現在のパラメータに適用
        p.applyActivePresets();
    }

private:
    GameParams() {
        // 初期状態として、現在のplayer/enemy値を「Default」プリセットとして登録
        playerPresets["Default"] = player;
        activePlayerPresetName = "Default";

        enemyPresets["Default"] = enemy;
        activeEnemyPresetName = "Default";
    }

    void applyActivePresets() {
        if (playerPresets.count(activePlayerPresetName)) {
            player = playerPresets.at(activePlayerPresetName);
        }
        else {
            // 見つからなかった場合、Defaultをアクティブにする
            activePlayerPresetName = "Default";
            if (playerPresets.count("Default")) {
                player = playerPresets.at("Default");
            }
        }

        if (enemyPresets.count(activeEnemyPresetName)) {
            enemy = enemyPresets.at(activeEnemyPresetName);
        }
        else {
            activeEnemyPresetName = "Default";
            if (enemyPresets.count("Default")) {
                enemy = enemyPresets.at("Default");
            }
        }
    }

    GameParams(const GameParams&) = delete;
    GameParams& operator=(const GameParams&) = delete;
};