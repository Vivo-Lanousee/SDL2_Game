#pragma once
#include <string>
#include <iostream>

/**
 * @brief ゲーム中の実行時の状態（拠点HP、所持金、装備状況等）を管理するシングルトン
 */
class GameSession {
public:
    static GameSession& GetInstance() {
        static GameSession instance;
        return instance;
    }

    // --- 拠点 (Gate) の動的状況 ---
    int currentBaseHP;
    int maxBaseHP;

    // --- プレイヤーのプレイ状況 ---
    std::string equippedGunPresetName; // 現在装備している銃のプリセット名
    int currentMoney;                  // 所持金
    int currentDay;                    // 現在の日数 (Wave数)

    // --- プレイヤー能力値 (カード等による恒久的な強化) ---
    float damageMultiplier;            // ダメージ倍率
    float reloadSpeedBonus;            // リロード速度ボーナス(割合)
    float movementSpeedBonus;          // 移動速度ボーナス

    /**
     * @brief セッションの初期化（ニューゲーム時）
     */
    void ResetSession();

    /**
     * @brief 拠点にダメージを与える
     */
    void DamageBase(int damage);

    /**
     * @brief 拠点を修理する
     */
    void RepairBase(int amount);

    /**
     * @brief 銃の装備を変更する
     */
    void ChangeGun(const std::string& gunPresetName);

private:
    GameSession() { ResetSession(); }
    GameSession(const GameSession&) = delete;
    GameSession& operator=(const GameSession&) = delete;
};