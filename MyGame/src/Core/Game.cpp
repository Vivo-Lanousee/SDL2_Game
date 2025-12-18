#include "Game.h"
#include <iostream>
#include <memory>

#include "../Objects/GameObject.h"
#include "../Scenes/Scene.h"
#include "../Scenes/PlayScene.h"
#include "InputHandler.h"
#include "../Scenes/TitleScene.h"
#include "../TextureManager.h"
#include "../UI/TextRenderer.h"
#include "imgui.h" 
#include "../Editor/EditorGUI.h"

Game::Game() : isRunning(false), window(nullptr), renderer(nullptr) {}
Game::~Game() { Clean(); }

bool Game::Init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
    int flags = fullscreen ? SDL_WINDOW_FULLSCREEN : 0;

    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        window.reset(SDL_CreateWindow(title, xpos, ypos, width, height, flags));
        renderer.reset(SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED));

        if (window && renderer) {
            SDL_SetRenderDrawColor(renderer.get(), 255, 255, 255, 255);

            // バックエンドの初期化のみ最初に行う
            EditorGUI::Init(window.get(), renderer.get());

            TextRenderer::Init("assets/fonts/PixelMplus10.ttf", 24);
            isRunning = true;
        }
    }
    else {
        return false;
    }

    inputHandler = std::make_unique<InputHandler>();
    ChangeScene(new TitleScene());
    return true;
}

void Game::ChangeScene(Scene* newScene) {
    if (currentScene) currentScene->OnExit(this);
    currentScene.reset(newScene);
    if (currentScene) currentScene->OnEnter(this);
}

void Game::HandleEvents() {
    if (inputHandler) inputHandler->Update();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) Quit();

        if (currentScene) {
            // 全てのイベント処理をシーンに任せる。
            // ImGuiを使うかどうかは各シーンが内部で判断する。
            currentScene->HandleEvents(this, &event);
        }
    }
}

void Game::Update() {
    if (currentScene) currentScene->Update(this);
}

void Game::Render() {
    // 背景
    SDL_SetRenderDrawColor(renderer.get(), 30, 30, 30, 255);
    SDL_RenderClear(renderer.get());

    // ゲーム本編
    if (currentScene) {
        currentScene->Render(this);
    }

    SDL_RenderPresent(renderer.get());
}

void Game::Clean() {
    if (isCleanedUp) return;

    if (currentScene) {
        currentScene->OnExit(this);
        currentScene.reset();
    }

    // 終了処理は一括で行う
    EditorGUI::Clean();
    TextRenderer::Clean();
    TextureManager::Clean();

    SDL_Quit();
    isCleanedUp = true;
}

void Game::DrawText(const char* text, int x, int y, SDL_Color color) {
    TextRenderer::Draw(renderer.get(), text, x, y, color);
}

std::vector<std::unique_ptr<GameObject>>& Game::GetCurrentSceneObjects() {
    if (currentScene) {
        return currentScene->GetObjects();
    }
    static std::vector<std::unique_ptr<GameObject>> emptyList;
    return emptyList;
}

SDL_Texture* Game::GetBulletTexture() {
    PlayScene* playScene = dynamic_cast<PlayScene*>(currentScene.get());
    if (playScene) {
        return playScene->GetBulletTexturePtr();
    }
    return nullptr;
}