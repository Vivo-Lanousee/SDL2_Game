#include "Game.h"
#include <iostream>
#include <memory>

#include "InputHandler.h"
#include "../Scenes/Scene.h"
#include "../UI/TextRenderer.h"
#include "../Scenes/TitleScene.h" 
#include "../Scenes/PlayScene.h"

Game::Game() : isRunning(false), window(nullptr), renderer(nullptr) {}

Game::~Game() {
    Clean();
}

bool Game::Init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
    int flags = 0;
    if (fullscreen) {
        flags = SDL_WINDOW_FULLSCREEN;
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        std::cout << "Subsystems Initialized!..." << std::endl;

        window.reset(SDL_CreateWindow(title, xpos, ypos, width, height, flags));

        if (window) {
            std::cout << "Window created!" << std::endl;
        }

        renderer.reset(SDL_CreateRenderer(window.get(), -1, 0));

        if (renderer) {
            // renderer.get() で中身にアクセス
            SDL_SetRenderDrawColor(renderer.get(), 255, 255, 255, 255);
            std::cout << "Renderer created!" << std::endl;
        }

        TextRenderer::Init("assets/fonts/PixelMplus10.ttf", 24);

        isRunning = true;
    }
    else {
        return false;
    }

    inputHandler = std::make_unique<InputHandler>();

    // タイトルシーンへ
    ChangeScene(new TitleScene());

    return true;
}

void Game::ChangeScene(Scene* newScene) {
    if (currentScene) {
        currentScene->OnExit(this);
    }
    currentScene.reset(newScene);
    if (currentScene) {
        currentScene->OnEnter(this);
    }
}

void Game::HandleEvents() {
    if (inputHandler) {
        inputHandler->Update();
    }
    if (currentScene) {
        currentScene->HandleEvents(this);
    }
}

void Game::Update() {
    if (currentScene) {
        currentScene->Update(this);
    }
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);
    SDL_RenderClear(renderer.get());

    if (currentScene) {
        currentScene->Render(this);
    }

    SDL_RenderPresent(renderer.get());
}

void Game::Clean() {
    std::cout << "Cleaning game..." << std::endl;

    if (currentScene) {
        currentScene->OnExit(this);
        currentScene.reset();
    }

    TextRenderer::Clean();

    SDL_Quit();
}

void Game::DrawText(const char* text, int x, int y, SDL_Color color) {
    TextRenderer::Draw(renderer.get(), text, x, y, color);
}