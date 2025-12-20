#include "GameSession.h"
#include "GameParams.h"
#include <algorithm>

void GameSession::ResetSession() {
    GameParams& params = GameParams::GetInstance();

    // 拠点の状態を初期設定からロード
    maxBaseHP = params.base.maxHealth;
    currentBaseHP = maxBaseHP;

    // 装備を初期設定のアクティブなものにする
    equippedGunPresetName = params.activeGunPresetName;

    // 初期ステータス
    currentMoney = 0;
    currentDay = 1;
    damageMultiplier = 1.0f;
    reloadSpeedBonus = 0.0f;
    movementSpeedBonus = 0.0f;

    std::cout << "Game Session Initialized." << std::endl;
}

void GameSession::DamageBase(int damage) {
    currentBaseHP -= damage;
    if (currentBaseHP < 0) currentBaseHP = 0;

    std::cout << "Base Damaged! HP: " << currentBaseHP << "/" << maxBaseHP << std::endl;

    if (currentBaseHP <= 0) {
        std::cout << "GAME OVER: The Gate has fallen." << std::endl;
        // シーン遷移やゲームオーバーイベントをここでトリガーする
    }
}

void GameSession::RepairBase(int amount) {
    currentBaseHP = std::min(maxBaseHP, currentBaseHP + amount);
    std::cout << "Base Repaired. HP: " << currentBaseHP << "/" << maxBaseHP << std::endl;
}

void GameSession::ChangeGun(const std::string& gunPresetName) {
    GameParams& params = GameParams::GetInstance();

    // プリセットが存在する場合のみ切り替える
    if (params.gunPresets.count(gunPresetName)) {
        equippedGunPresetName = gunPresetName;
        params.gun = params.gunPresets[gunPresetName];
        params.activeGunPresetName = gunPresetName;
        std::cout << "Gun Swapped to: " << gunPresetName << std::endl;
    }
}