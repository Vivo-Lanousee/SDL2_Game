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
        // 現在のパラメータをプリセットで上書き（RefreshConfig用）
        params.enemy = params.enemyPresets[presetName];
        params.activeEnemyPresetName = presetName;

        static std::random_device rd;
        static std::mt19937 gen(rd());

        // 地面(550px)より十分高い位置(50~300px)にスポーンさせる
        std::uniform_real_distribution<float> disY(50.0f, 300.0f);

        float startX = 1300.0f; // 画面右側
        float startY = disY(gen);
        int width = 64;
        int height = 64;

        SDL_Texture* tex = nullptr;
        if (!params.enemy.texturePath.empty()) {
            SharedTexturePtr sharedTex = TextureManager::LoadTexture(params.enemy.texturePath, game->GetRenderer());
            if (sharedTex) tex = sharedTex.get();
        }

        // シミュレーション用には空のパスを渡すが、Updateループが回れば物理で着地する
        std::vector<SDL_FPoint> path;

        auto newEnemy = std::make_unique<Enemy>(startX, startY, width, height, tex, path);
        newEnemy->RefreshConfig(game->GetRenderer());
        newEnemy->name = "Enemy_" + presetName;

        // 重要: SceneのgameObjectsに直接追加するか、game->Instantiate経由で追加されることを保証する
        // EditorSceneでは game->Instantiate が gameObjects リストに反映される設計である必要があります。
        game->Instantiate(std::move(newEnemy));
    }
    else {
        std::cerr << "Error: Enemy preset '" << presetName << "' not found." << std::endl;
    }
}