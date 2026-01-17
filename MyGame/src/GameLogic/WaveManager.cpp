#include "WaveManager.h"
#include "../Core/Game.h"
#include "../Core/Time.h"
#include "../Core/GameParams.h"
#include "../Objects/Enemy.h"
#include "../TextureManager.h"
#include <SDL.h>
#include <iostream>
#include <vector>
#include <random>

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
        params.enemy = params.enemyPresets[presetName];
        params.activeEnemyPresetName = presetName;

        static std::random_device rd;
        static std::mt19937 gen(rd());

        // スポーン位置の決定（画面右端の外側）
        std::uniform_real_distribution<float> disY(50.0f, 400.0f);
        float startX = 1300.0f;
        float startY = disY(gen);


        std::vector<SDL_FPoint> dummyPath;
        auto newEnemy = std::make_unique<Enemy>(startX, startY, 64, 64, nullptr, dummyPath);
        newEnemy->RefreshConfig(game->GetRenderer());
        newEnemy->name = "Enemy";

        game->Instantiate(std::move(newEnemy));
    }
    else {
        std::cerr << "Error: Enemy preset '" << presetName << "' not found." << std::endl;
    }
}