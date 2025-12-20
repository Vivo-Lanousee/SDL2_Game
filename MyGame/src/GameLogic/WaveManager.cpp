#include "WaveManager.h"
#include "../Core/Game.h"
#include "../Core/Time.h"
#include "../Core/GameParams.h"
#include "../Objects/Enemy.h"
#include "../TextureManager.h"
#include <SDL.h>
#include <iostream>
#include <vector>

WaveManager::WaveManager() {
}

void WaveManager::Init(int levelID) {
    currentLevelID = levelID;
    currentWaveIndex = -1;

    auto& params = GameParams::GetInstance();
    if (params.levelConfigs.count(currentLevelID)) {
        totalWaves = static_cast<int>(params.levelConfigs[currentLevelID].waves.size());
    }
    else {
        totalWaves = 0;
        std::cerr << "Warning: Level ID " << levelID << " not found in GameParams." << std::endl;
    }

    NextWave();
}

void WaveManager::NextWave() {
    currentWaveIndex++;

    if (currentWaveIndex >= totalWaves) {
        currentState = State::LEVEL_COMPLETED;
        return;
    }

    // キューをクリアして、新しいウェーブの敵を詰め込む
    while (!spawnQueue.empty()) spawnQueue.pop();

    auto& params = GameParams::GetInstance();
    const auto& waveData = params.levelConfigs[currentLevelID].waves[currentWaveIndex];

    for (const auto& entry : waveData.spawns) {
        for (int i = 0; i < entry.count; ++i) {
            spawnQueue.push(entry.enemyPresetName);
        }
    }

    currentState = State::PREPARING;
    prepareTimer = PREPARE_DURATION;
    std::cout << "Wave " << (currentWaveIndex + 1) << " Preparing..." << std::endl;
}

void WaveManager::Update(Game* game) {
    if (!game) return;

    float dt = Time::deltaTime;

    switch (currentState) {
    case State::PREPARING:
        prepareTimer -= dt;
        if (prepareTimer <= 0) {
            currentState = State::SPAWNING;
            spawnTimer = 0;
            std::cout << "Wave " << (currentWaveIndex + 1) << " Start!" << std::endl;
        }
        break;

    case State::SPAWNING:
        spawnTimer -= dt;
        if (spawnTimer <= 0) {
            if (!spawnQueue.empty()) {
                std::string preset = spawnQueue.front();
                spawnQueue.pop();
                SpawnEnemy(preset, game);
                spawnTimer = spawnInterval;
            }
            else {
                currentState = State::BATTLE;
            }
        }
        break;

    case State::BATTLE:
    {
        bool enemyExists = false;
        auto& objects = game->GetCurrentSceneObjects();
        for (const auto& obj : objects) {
            if (obj && !obj->isDead && dynamic_cast<Enemy*>(obj.get())) {
                enemyExists = true;
                break;
            }
        }

        if (!enemyExists) {
            currentState = State::WAVE_CLEAR;
            std::cout << "Wave " << (currentWaveIndex + 1) << " Clear!" << std::endl;
        }
    }
    break;

    case State::WAVE_CLEAR:
        NextWave();
        break;

    case State::LEVEL_COMPLETED:
        break;
    }
}

void WaveManager::SpawnEnemy(const std::string& presetName, Game* game) {
    auto& params = GameParams::GetInstance();

    if (params.enemyPresets.count(presetName)) {
        // 出現させる敵のパラメータをプリセットから一時的に適用
        params.enemy = params.enemyPresets[presetName];
        params.activeEnemyPresetName = presetName;

        // 1. コンストラクタに必要な引数を準備
        float startX = 1300.0f; // 画面右端の外側
        float startY = 500.0f;  // 地面の高さ
        int width = 64;         // デフォルトサイズ
        int height = 64;

        // テクスチャの取得
        SDL_Texture* tex = nullptr;
        if (!params.enemy.texturePath.empty()) {
            SharedTexturePtr sharedTex = TextureManager::LoadTexture(params.enemy.texturePath, game->GetRenderer());
            if (sharedTex) {
                tex = sharedTex.get();
            }
        }

        // 移動パスの準備（現在はLinear移動想定なので空のリストを渡す）
        std::vector<SDL_FPoint> path;

        // 2. Enemyのコンストラクタを正しい引数で呼び出す
        auto newEnemy = std::make_unique<Enemy>(startX, startY, width, height, tex, path);

        // 3. パラメータの最終適用（内部変数のセットと弾丸テクスチャのロード）
        newEnemy->RefreshConfig(game->GetRenderer());

        newEnemy->name = "Enemy_" + presetName;

        // 4. Gameクラスの生成キューに送る
        game->Instantiate(std::move(newEnemy));
    }
    else {
        std::cerr << "Error: Enemy preset '" << presetName << "' not found." << std::endl;
    }
}