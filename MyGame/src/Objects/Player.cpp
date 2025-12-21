#include "Player.h"
#include "../Core/Game.h"
#include "../Core/InputHandler.h"
#include "../Core/Camera.h"
#include "../Core/Time.h"
#include "../Core/GameParams.h" 
#include "Bullet.h"
#include <cmath>
#include <memory>
#include <iostream>
#include <random>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Player::Player(float x, float y, SDL_Texture* tex, SDL_Texture* bulletTex, Camera* cam)
    : GameObject(x, y, 46, 128, tex),
    currentHealth(GameParams::GetInstance().player.maxHealth),
    fireCooldown(0.0f),
    reloadTimer(0.0f),
    isReloading(false)
{
    angle = 0;
    useGravity = true;

    // --- 物理挙動の設定 ---
    // Physics::ResolveCollision 内の aIsCharacter 判定にヒットさせるため "Player" を含めます
    this->name = "Player";

    // isTrigger を false にすることで、地面（Block）に対しては物理的に衝突（着地）します。
    // キャラクター同士の通り抜けは Physics 側のロジックで自動的に行われます。
    this->isTrigger = true;

    this->bulletTexture = bulletTex;
    this->camera = cam;
    this->isFlipLeft = false;

    // 初期弾数を設定
    currentAmmo = GameParams::GetInstance().gun.magazineSize;

    animator = std::make_unique<Animator>();
    animator->LoadFromJson("assets/data/player.json");
}

void Player::Update(Game* game) {
    InputHandler* input = game->GetInput();
    GameParams& params = GameParams::GetInstance();

    animator->Update();

    //移動ロジック (GameParamsの数値を即時反映)
    float moveSpeed = params.player.moveSpeed;
    velX = 0;
    if (input->IsPressed(GameAction::MoveLeft)) {
        velX = -moveSpeed;
        isFlipLeft = false;
    }
    if (input->IsPressed(GameAction::MoveRight)) {
        velX = moveSpeed;
        isFlipLeft = true;
    }

    if (velX != 0) {
        animator->Play("Run");
    }
    else {
        animator->Play("Idle");
    }

    // ジャンプロジック
    if (input->IsJustPressed(GameAction::MoveUp)) {
        if (isGrounded) {
            velY = -params.player.jumpVelocity;
            isGrounded = false;
        }
    }

    // 銃テクスチャがなければロードを試みる
    if (!gunTexture) {
        RefreshGunConfig(game->GetRenderer());
    }

    // --- リロード処理 ---
    bool wantsReload = input->IsJustPressed(GameAction::Reload);

    if ((wantsReload || currentAmmo <= 0) && !isReloading && currentAmmo < params.gun.magazineSize) {
        isReloading = true;
        reloadTimer = params.gun.reloadTime;
        std::cout << "Player Reloading..." << std::endl;
    }

    if (isReloading) {
        reloadTimer -= Time::deltaTime;
        if (reloadTimer <= 0) {
            currentAmmo = params.gun.magazineSize;
            isReloading = false;
            std::cout << "Player Reload Complete!" << std::endl;
        }
    }

    // 連射クールダウンの更新
    if (fireCooldown > 0) {
        fireCooldown -= Time::deltaTime;
    }

    // 射撃処理
    int screenMouseX, screenMouseY;
    SDL_GetMouseState(&screenMouseX, &screenMouseY);
    SDL_FPoint worldMouse = camera->ScreenToWorld(screenMouseX, screenMouseY);

    // 射撃条件: ボタン押下中 且つ クールダウン終了 且つ リロード中でない 且つ 残弾がある
    if (input->IsPressed(GameAction::Shoot) && fireCooldown <= 0.0f && !isReloading && currentAmmo > 0) {

        // 残弾を減らす
        currentAmmo--;

        // shotCount の数だけ弾を同時に生成する
        for (int i = 0; i < params.gun.shotCount; ++i) {
            auto newBullet = Shoot(worldMouse.x, worldMouse.y, bulletTexture);
            if (newBullet) {
                game->Instantiate(std::move(newBullet));
            }
        }

        // クールダウンを設定
        fireCooldown = params.gun.fireRate;
    }
}

void Player::OnRender(SDL_Renderer* renderer, int drawX, int drawY) {
    SDL_Rect destRect = { drawX, drawY, width, height };
    SDL_Rect srcRect = animator->GetSrcRect(width, height);
    SDL_RendererFlip flip = isFlipLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

    // プレイヤー本体の描画
    if (texture) {
        SDL_RenderCopyEx(renderer, texture, &srcRect, &destRect, angle, NULL, flip);
    }

    // 銃の描画
    if (gunTexture) {
        GameParams& params = GameParams::GetInstance();
        int mx, my;
        SDL_GetMouseState(&mx, &my);

        float centerX = (float)drawX + (float)width / 2.0f + params.gun.offsetX;
        float centerY = (float)drawY + (float)height / 2.0f + params.gun.offsetY;

        double gunAngle = atan2(my - centerY, mx - centerX) * 180.0 / M_PI;

        int gunW = 64;
        int gunH = 32;
        SDL_Rect gunDest = { (int)centerX - gunW / 4, (int)centerY - gunH / 2, gunW, gunH };

        SDL_RendererFlip gunFlip = (gunAngle > 90 || gunAngle < -90) ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;

        if (isReloading) {
            SDL_SetTextureAlphaMod(gunTexture.get(), 128);
        }
        else {
            SDL_SetTextureAlphaMod(gunTexture.get(), 255);
        }

        SDL_RenderCopyEx(renderer, gunTexture.get(), NULL, &gunDest, gunAngle, NULL, gunFlip);
    }
}

std::unique_ptr<Bullet> Player::Shoot(float targetX, float targetY, SDL_Texture* bulletTex) {
    GameParams& params = GameParams::GetInstance();

    float spawnX = x + (width / 2.0f) + params.gun.offsetX;
    float spawnY = y + (height / 2.0f) + params.gun.offsetY;

    float dx = targetX - spawnX;
    float dy = targetY - spawnY;
    float baseAngleRad = atan2(dy, dx);

    if (dx == 0 && dy == 0) return nullptr;

    static std::random_device rd;
    static std::mt19937 gen(rd());

    float spreadRad = params.gun.spreadAngle * (M_PI / 180.0f);
    std::uniform_real_distribution<float> dist(-spreadRad / 2.0f, spreadRad / 2.0f);

    double finalAngleRad = baseAngleRad + dist(gen);

    float vx = (float)cos(finalAngleRad) * params.gun.bulletSpeed;
    float vy = (float)sin(finalAngleRad) * params.gun.bulletSpeed;

    return std::make_unique<Bullet>(
        spawnX - 5, spawnY - 5,
        10, 10,
        vx, vy,
        params.gun.damage,
        bulletTex,
        BulletSide::Player
    );
}

void Player::TakeDamage(int damage) {
    currentHealth -= (float)damage;
    float maxHp = GameParams::GetInstance().player.maxHealth;
    if (currentHealth < 0) currentHealth = 0;
    if (currentHealth > maxHp) currentHealth = maxHp;
}

int Player::GetMaxHP() const {
    return (int)GameParams::GetInstance().player.maxHealth;
}

void Player::RefreshGunConfig(SDL_Renderer* renderer) {
    std::string path = GameParams::GetInstance().gun.texturePath;
    if (!path.empty()) {
        gunTexture = TextureManager::LoadTexture(path.c_str(), renderer);
    }
    currentAmmo = GameParams::GetInstance().gun.magazineSize;
    isReloading = false;
}