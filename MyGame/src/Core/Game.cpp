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

Game::Game() : isRunning(false), window(nullptr), renderer(nullptr), nextScene(nullptr) {}
Game::~Game() { Clean(); }

bool Game::Init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
    int flags = fullscreen ? SDL_WINDOW_FULLSCREEN : 0;

    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        window.reset(SDL_CreateWindow(title, xpos, ypos, width, height, flags));
        renderer.reset(SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED));

        if (window && renderer) {
            SDL_SetRenderDrawColor(renderer.get(), 255, 255, 255, 255);

            EditorGUI::Init(window.get(), renderer.get());
            TextRenderer::Init("assets/fonts/PixelMplus10.ttf", 24);
            isRunning = true;
        }
    }
    else {
        return false;
    }

    inputHandler = std::make_unique<InputHandler>();

    // 初期シーンをセット
    currentScene.reset(new TitleScene());
    currentScene->OnEnter(this);

    return true;
}

void Game::ChangeScene(Scene* newScene) {
    // 既に予約がある場合は削除
    if (nextScene) {
        delete nextScene;
    }
    // 文字列ではなく、new されたポインタを代入する
    nextScene = newScene;
}

void Game::HandleEvents() {
    if (inputHandler) inputHandler->Update();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) Quit();

        if (currentScene) {
            currentScene->HandleEvents(this, &event);
        }
    }
}

void Game::Update() {
    // シーンの切り替え予約があるかチェック
    if (nextScene) {
        if (currentScene) {
            currentScene->OnExit(this);
        }

        // unique_ptr に新しいポインタを所有させる（古いのは自動破棄）
        currentScene.reset(nextScene);

        if (currentScene) {
            currentScene->OnEnter(this);
        }
        nextScene = nullptr;
    }

    if (currentScene) {
        currentScene->Update(this);
    }
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer.get(), 30, 30, 30, 255);
    SDL_RenderClear(renderer.get());

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

    if (nextScene) {
        delete nextScene;
        nextScene = nullptr;
    }

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