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

    // セッションの初期化
    GameSession::GetInstance().ResetSession();

    // テクスチャ読み込み
    playerTexture = TextureManager::LoadTexture("assets/images/player.png", game->GetRenderer());
    bulletTexture = TextureManager::LoadTexture("assets/images/bullet.png", game->GetRenderer());

    // カメラ設定
    camera = std::make_unique<Camera>(800, 600);
    camera->limitX = 5000;
    camera->limitY = 1200;

    // 1. 拠点の生成
    auto baseObj = std::make_unique<Base>(80, 300, 80, 250);
    baseObj->name = "Base Gate";
    // 拠点の初期設定を反映
    baseObj->RefreshConfig(game->GetRenderer());
    gameObjects.push_back(std::move(baseObj));

    //  地面の生成
    auto ground = std::make_unique<Block>(0, 550, 5000, 50);
    ground->name = "Block";
    ground->useGravity = false;
    gameObjects.push_back(std::move(ground));

    // プレイヤーの生成
    auto pPtr = std::make_unique<Player>(400, 100, playerTexture.get(), bulletTexture.get(), camera.get());
    pPtr->name = "Player";
    player = pPtr.get();
    gameObjects.push_back(std::move(pPtr));

    // ウェーブマネージャーの開始（レベル1から）
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
        return;
    }

    // --- ゲームオーバー判定 ---
    if (GameSession::GetInstance().currentBaseHP <= 0) {
        std::cout << "GATE DESTROYED... GAME OVER" << std::endl;
        game->ChangeScene(new TitleScene());
        return;
    }

    // --- ウェーブマネージャーの更新 ---
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

    // 背景色
    SDL_SetRenderDrawColor(renderer, 30, 35, 50, 255);
    SDL_RenderClear(renderer);

    // 全オブジェクトの描画
    for (const auto& obj : gameObjects) {
        if (obj) obj->RenderWithCamera(renderer, camera.get());
    }

    // --- UI 描画エリア ---

    // 1. 拠点HPバー
    GameSession& session = GameSession::GetInstance();
    float hpRatio = (session.maxBaseHP > 0) ? (float)session.currentBaseHP / session.maxBaseHP : 0;

    SDL_Rect barBG = { 200, 30, 400, 15 };
    SDL_Rect barFG = { 200, 30, (int)(400 * hpRatio), 15 };

    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderFillRect(renderer, &barBG);

    if (hpRatio > 0.5f) SDL_SetRenderDrawColor(renderer, 0, 255, 100, 255);
    else if (hpRatio > 0.2f) SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);
    else SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255);

    SDL_RenderFillRect(renderer, &barFG);
    TextRenderer::Draw(renderer, "GATE INTEGRITY", 200, 12, { 255, 255, 255, 255 });

    // ウェーブ（日付）表示
    std::string dayText = "SURVIVAL DAY: " + std::to_string(waveManager.GetCurrentWaveNumber());
    TextRenderer::Draw(renderer, dayText, 20, 20, { 255, 255, 0, 255 });

    std::string statusText = "";
    switch (waveManager.GetState()) {
    case WaveManager::State::PREPARING: statusText = "NEXT WAVE APPROACHING..."; break;
    case WaveManager::State::SPAWNING:  statusText = "ENEMY DETECTED!"; break;
    case WaveManager::State::BATTLE:    statusText = "ELIMINATE REMAINING HOSTILES"; break;
    case WaveManager::State::LEVEL_COMPLETED: statusText = "MISSION ACCOMPLISHED!"; break;
    }
    TextRenderer::Draw(renderer, statusText, 20, 50, { 200, 200, 200, 255 });

    // プレイヤーUI
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

        // 体力バー(y=30, h=15)の下、y=52付近に表示
        TextRenderer::Draw(renderer, ammoStr, 200, 52, ammoCol);
    }
}