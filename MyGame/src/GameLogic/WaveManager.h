#pragma once
#include <vector>
#include <string>
#include <queue>
#include "../Core/GameParams.h"

// 前方宣言
class Game;
struct SDL_Renderer;

/**
 * @brief ウェーブの進行状況と敵のスポーンを管理するクラス
 */
class WaveManager {
public:
    enum class State {
        PREPARING,      // ウェーブ開始前のカウントダウンなど
        SPAWNING,       // 敵を順番に生成している最中
        BATTLE,         // 敵は出し切ったが、画面内に敵が残っている状態
        WAVE_CLEAR,     // ウェーブ内の敵を全滅させた
        LEVEL_COMPLETED // 全てのウェーブをクリアした
    };

    WaveManager();
    ~WaveManager() = default;

    /**
     * @brief 指定したレベルのデータでマネージャーを初期化する
     */
    void Init(int levelID);

    /**
     * @brief 毎フレームの更新処理
     * @param game Gameクラスへのポインタ（InstantiateやRendererへのアクセス用）
     */
    void Update(Game* game);

    // 状態取得用
    State GetState() const { return currentState; }
    int GetCurrentWaveNumber() const { return currentWaveIndex + 1; }
    int GetTotalWaves() const { return totalWaves; }

private:
    /**
     * @brief 次のウェーブを開始する準備をする
     */
    void NextWave();

    /**
     * @brief 実際に敵をゲームに生成する
     */
    void SpawnEnemy(const std::string& presetName, Game* game);

    // 進行状態
    int currentLevelID = 1;
    int currentWaveIndex = -1;
    int totalWaves = 0;
    State currentState = State::PREPARING;

    // 出現管理
    std::queue<std::string> spawnQueue; // 今回のウェーブで出す敵プリセット名の列
    float spawnTimer = 0.0f;
    float spawnInterval = 1.0f;         // 敵が出る間隔（秒）

    float prepareTimer = 0.0f;
    const float PREPARE_DURATION = 3.0f; // ウェーブ間の待機時間
};