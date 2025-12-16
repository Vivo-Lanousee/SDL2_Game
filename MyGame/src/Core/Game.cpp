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

// ★★★ 修正箇所 1: ImGuiの状態チェックに必要 ★★★
#include "imgui.h" 

// ★ここが重要：Editorフォルダのヘッダーを読み込む
#include "../Editor/EditorGUI.h"

Game::Game() : isRunning(false), window(nullptr), renderer(nullptr) {}

// ★★★ 修正箇所: デストラクタの実装を復活させる ★★★
Game::~Game() { Clean(); }

bool Game::Init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
    int flags = fullscreen ? SDL_WINDOW_FULLSCREEN : 0;

    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        window.reset(SDL_CreateWindow(title, xpos, ypos, width, height, flags));
        renderer.reset(SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED));

        if (window && renderer) {
            SDL_SetRenderDrawColor(renderer.get(), 255, 255, 255, 255);

            // ★ImGuiの初期化を委譲
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

// ★★★ 修正箇所 2: HandleEvents で ImGui のマウス制御を実装 ★★★
void Game::HandleEvents() {
    if (inputHandler) inputHandler->Update();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {

        // ★イベントをエディタに渡す (ImGuiがイベントを消費)
        EditorGUI::HandleEvents(&event);

        // ImGuiがマウス入力を必要としているかチェック
        ImGuiIO& io = ImGui::GetIO();
        bool guiWantsMouse = io.WantCaptureMouse;

        if (event.type == SDL_QUIT) Quit();

        if (currentScene) {
            bool isMouseEvent = (event.type == SDL_MOUSEBUTTONDOWN ||
                event.type == SDL_MOUSEBUTTONUP ||
                event.type == SDL_MOUSEMOTION);

            if (isMouseEvent) {
                // ImGuiがマウスを掴んでいない場合のみ、ゲーム内のマウスイベントを処理
                if (!guiWantsMouse) {
                    currentScene->HandleEvents(this);
                }
            }
            else {
                // キーボードなど、その他のイベントは常に処理を試みる
                currentScene->HandleEvents(this);
            }
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
    if (currentScene) currentScene->Render(this);

    // ★エディタUIの描画
    EditorGUI::Render(renderer.get(), currentScene.get());

    SDL_RenderPresent(renderer.get());
}

void Game::Clean() {
    if (isCleanedUp) return;

    if (currentScene) {
        currentScene->OnExit(this);
        currentScene.reset();
    }

    // エディタの終了処理
    EditorGUI::Clean();

    TextRenderer::Clean();
    TextureManager::Clean();

    SDL_Quit();

    // ★ 追加：終了処理完了フラグを設定
    isCleanedUp = true;
}

void Game::DrawText(const char* text, int x, int y, SDL_Color color) {
    TextRenderer::Draw(renderer.get(), text, x, y, color);
}

// ★★★ [Turret機能向け 追加メソッドの実装] ★★★

std::vector<std::unique_ptr<GameObject>>& Game::GetCurrentSceneObjects() {
    if (currentScene) {
        // Scene::GetObjects() (virtual) の実体が PlayScene で実装されている必要あり
        return currentScene->GetObjects();
    }
    // pendingObjects は Game::Update の外部で使われることが想定されるため、
    // Gameクラスのメンバとして定義されている前提
    // return pendingObjects; 

    // pendingObjects が Gameクラスのメンバとして定義されている前提で、ここでは仮の空リストを返します。
    // 実際の pendingObjects へのアクセスロジックに合わせて修正してください。
    static std::vector<std::unique_ptr<GameObject>> emptyList;
    return emptyList;
}

SDL_Texture* Game::GetBulletTexture() {
    PlayScene* playScene = dynamic_cast<PlayScene*>(currentScene.get());

    if (playScene) {
        // ★★★ 修正箇所: PlayScene::GetBulletTexturePtr() を呼び出す ★★★
        return playScene->GetBulletTexturePtr();
    }
    return nullptr;
}