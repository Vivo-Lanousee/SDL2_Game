#include "PlayScene.h"
#include "../Core/Game.h"
#include "../Core/Time.h"
#include "../Core/InputHandler.h"
#include "../Core/Physics.h"
#include "../Core/GameParams.h"
#include "../Core/GameSession.h"
#include "../Objects/Block.h"
#include "../Objects/Enemy.h"
#include "../Objects/Base.h"
#include "../TextureManager.h"
#include "../UI/TextRenderer.h"
#include "TitleScene.h"
#include <iostream>
#include <string>
#include <algorithm>

void PlayScene::OnEnter(Game* game) {
    std::cout << "Entering PlayScene... Start Defense." << std::endl;

    // 1. セッションの初期化
    GameSession::GetInstance().ResetSession();

    // 2. テクスチャ読み込み
    playerTexture = TextureManager::LoadTexture("assets/images/player.png", game->GetRenderer());
    bulletTexture = TextureManager::LoadTexture("assets/images/bullet.png", game->GetRenderer());

    // 3. カメラ設定
    camera = std::make_unique<Camera>(800, 600);
    camera->limitX = 5000;
    camera->limitY = 1200;

    // 4. 拠点の生成
    auto baseObj = std::make_unique<Base>(80, 300, 80, 250);
    baseObj->name = "Base Gate";
    // 拠点のテクスチャサイズ反映とパラメータ適用
    baseObj->RefreshConfig(game->GetRenderer());
    gameObjects.push_back(std::move(baseObj));

    // 5. 地面の生成
    auto ground = std::make_unique<Block>(0, 550, 5000, 50);
    ground->name = "Block"; // 物理演算対象にするための固定名
    ground->useGravity = false; // 地面自体は落下させない
    gameObjects.push_back(std::move(ground));

    // 6. プレイヤーの生成
    auto pPtr = std::make_unique<Player>(400, 100, playerTexture.get(), bulletTexture.get(), camera.get());
    pPtr->name = "Player";

    // プレイヤーの画像サイズを自動取得して当たり判定を補正
    if (playerTexture) {
        int realW, realH;
        if (SDL_QueryTexture(playerTexture.get(), NULL, NULL, &realW, &realH) == 0) {
            pPtr->width = realW;
            pPtr->height = realH;
        }
    }

    player = pPtr.get();
    gameObjects.push_back(std::move(pPtr));

    // 7. ウェーブマネージャーの開始（レベル1から）
    waveManager.Init(1);
}

void PlayScene::OnExit(Game* game) {
    player = nullptr;
    gameObjects.clear();
}

void PlayScene::HandleEvents(Game* game, SDL_Event* event) {
    if (event->type == SDL_QUIT) {
        game->Quit();
    }
}

void PlayScene::OnUpdate(Game* game) {
    // --- タイトルへの離脱（Escapeキー） ---
    if (game->GetInput()->IsJustPressed(GameAction::Pause)) {
        game->ChangeScene(new TitleScene());
        return; // 重要：シーン切り替え後は即座に処理を中断する
    }

    // --- ゲームオーバー判定 ---
    if (GameSession::GetInstance().currentBaseHP <= 0) {
        std::cout << "GATE DESTROYED... GAME OVER" << std::endl;
        game->ChangeScene(new TitleScene());
        return;
    }

    // --- ウェーブマネージャーの更新 ---
    // ここで生成される敵（Enemy）は Enemy::RefreshConfig 内で画像サイズ補正が行われます
    waveManager.Update(game);

    // --- カメラの追従 ---
    if (player && !player->isDead) {
        camera->Follow(player);
    }
    else {
        camera->Follow(nullptr);
    }
}

void PlayScene::Render(Game* game) {
    SDL_Renderer* renderer = game->GetRenderer();

    // 背景色（サバイバル感のある暗い紺色）
    SDL_SetRenderDrawColor(renderer, 30, 35, 50, 255);
    SDL_RenderClear(renderer);

    // 全オブジェクトの描画（カメラ位置を考慮）
    for (const auto& obj : gameObjects) {
        if (obj) obj->RenderWithCamera(renderer, camera.get());
    }

    // --- UI 描画エリア ---

    // 拠点HPバー（中央上部）
    GameSession& session = GameSession::GetInstance();
    float hpRatio = (session.maxBaseHP > 0) ? (float)session.currentBaseHP / session.maxBaseHP : 0;

    SDL_Rect barBG = { 200, 30, 400, 15 };
    SDL_Rect barFG = { 200, 30, (int)(400 * hpRatio), 15 };

    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255); // バーの背景
    SDL_RenderFillRect(renderer, &barBG);

    // HP残量に応じた色変化
    if (hpRatio > 0.5f) SDL_SetRenderDrawColor(renderer, 0, 255, 100, 255); // 安全：緑
    else if (hpRatio > 0.2f) SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255); // 警告：黄
    else SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255); // 危険：赤

    SDL_RenderFillRect(renderer, &barFG);
    TextRenderer::Draw(renderer, "GATE INTEGRITY", 200, 12, { 255, 255, 255, 255 });

    // ウェーブ（生存日数）表示（左上）
    std::string dayText = "SURVIVAL DAY: " + std::to_string(waveManager.GetCurrentWaveNumber());
    TextRenderer::Draw(renderer, dayText, 20, 20, { 255, 255, 0, 255 });

    // 状況説明テキスト
    std::string statusText = "";
    switch (waveManager.GetState()) {
    case WaveManager::State::PREPARING: statusText = "NEXT WAVE APPROACHING..."; break;
    case WaveManager::State::SPAWNING:  statusText = "ENEMY DETECTED!"; break;
    case WaveManager::State::BATTLE:    statusText = "ELIMINATE REMAINING HOSTILES"; break;
    case WaveManager::State::LEVEL_COMPLETED: statusText = "MISSION ACCOMPLISHED!"; break;
    }
    TextRenderer::Draw(renderer, statusText, 20, 50, { 200, 200, 200, 255 });

    // プレイヤーUI (体力バーの下に弾数を表示)
    if (player && !player->isDead) {
        int currentAmmo = player->GetCurrentAmmo();
        int maxAmmo = GameParams::GetInstance().gun.magazineSize;
        std::string ammoStr = "AMMO: " + std::to_string(currentAmmo) + " / " + std::to_string(maxAmmo);

        SDL_Color ammoCol = { 255, 255, 255, 255 };
        if (player->GetIsReloading()) {
            ammoStr = "RELOADING...";
            ammoCol = { 255, 100, 0, 255 };
        }
        else if (currentAmmo == 0) {
            ammoCol = { 255, 50, 50, 255 };
        }

        // 中央の体力バー(y=30, h=15)のすぐ下、y=52に配置
        TextRenderer::Draw(renderer, ammoStr, 200, 52, ammoCol);
    }
}