#include "Player.h"
#include "../Core/Game.h"
#include "../Core/InputHandler.h"
#include "../Core/Camera.h"
#include "../Core/Time.h"
#include "../Core/GameParams.h" 
#include "Bullet.h"
#include <cmath>
#include <memory>
// Player.h, Animator.h, Timer.h が全てインクルードされている前提です

// プレイヤーコンストラクタ
Player::Player(float x, float y, SDL_Texture* tex, SDL_Texture* bulletTex, Camera* cam)
	: GameObject(x, y, 46, 128, tex),
	// ★★★ 修正1: currentHealth の初期化を有効化 ★★★
	currentHealth(GameParams::GetInstance().player.maxHealth) // Player.hで currentHealth が定義されている前提
{
	const float initialFireInterval = 0.15f;
	// shootTimer.SetInterval(initialFireInterval); // shootTimerが定義されている前提

	angle = 0;
	useGravity = true;
	name = "Player";
	this->bulletTexture = bulletTex;
	this->camera = cam;
	isFlipLeft = false;

	animator = std::make_unique<Animator>();
	animator->LoadFromJson("assets/data/player.json");
}

void Player::Update(Game* game) {
	InputHandler* input = game->GetInput();
	GameParams& params = GameParams::GetInstance();

	animator->Update();
	// shootTimer.Update();

	// ★★★ 修正2 (重要): 移動速度の計算から Time::deltaTime を削除 ★★★
	// velX には純粋な速度 (px/s) をセットする。PhysicsでdeltaTimeが適用される。
	float moveSpeed = params.player.moveSpeed;

	velX = 0;
	if (input->IsPressed(GameAction::MoveLeft)) {
		velX = -moveSpeed; // velX = -300.0f (px/s)
		isFlipLeft = true;
	}
	if (input->IsPressed(GameAction::MoveRight)) {
		velX = moveSpeed; // velX = 300.0f (px/s)
		isFlipLeft = false;
	}

	// アニメーション切り替え (変更なし)
	if (velX != 0) {
		animator->Play("Run");
	}
	else {
		animator->Play("Idle");
	}

	// ジャンプ
	if (input->IsJustPressed(GameAction::MoveUp)) {
		if (isGrounded) {
			// ★★★ 修正3 (重要): ジャンプ力の設定から Time::deltaTime を削除 ★★★
			// velY には純粋な速度 (px/s) をセットする 
			velY = -params.player.jumpVelocity;
			isGrounded = false;
		}
	}

	// マウス処理 (変更なし)
	int screenMouseX, screenMouseY;
	SDL_GetMouseState(&screenMouseX, &screenMouseY);
	SDL_FPoint worldMouse = camera->ScreenToWorld(screenMouseX, screenMouseY);

	// 射撃処理 (変更なし)
	if (input->IsPressed(GameAction::Shoot) /* && shootTimer.IsReady() */) {
		auto newBullet = Shoot(worldMouse.x, worldMouse.y, bulletTexture);

		if (newBullet) {
			game->Instantiate(std::move(newBullet));
			// shootTimer.Reset();
		}
	}
}

// Render (変更なし)
void Player::OnRender(SDL_Renderer* renderer, int drawX, int drawY) {
	SDL_Rect destRect = { drawX, drawY, width, height };

	SDL_Rect srcRect = animator->GetSrcRect(width, height);
	SDL_RendererFlip flip = isFlipLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

	if (texture) {
		SDL_RenderCopyEx(renderer, texture, &srcRect, &destRect, angle, NULL, flip);
	}
	else {
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderFillRect(renderer, &destRect);
	}

	// レーザー描画 (変更なし)
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	int mx, my;
	SDL_GetMouseState(&mx, &my);
	int lineStartX = drawX + width / 2;
	int lineStartY = drawY + height / 2;
	SDL_RenderDrawLine(renderer, lineStartX, lineStartY, mx, my);
}

// Shoot
std::unique_ptr<Bullet> Player::Shoot(float targetX, float targetY, SDL_Texture* bulletTex) {
	// ★★★ 修正4: ローカル変数を明示的に宣言 (前回残ったエラーを解消) ★★★
	float spawnX = x + (width / 2) - 5;
	float spawnY = y + (height / 2) - 5;

	float centerX = x + width / 2;
	float centerY = y + height / 2;

	double radian = atan2(targetY - centerY, targetX - centerX);
	// M_PI の代わりに定数を使用
	double angleDeg = radian * 180.0 / 3.14159265358979323846;

	// Bulletの4引数コンストラクタ (Player用) を使用
	return std::make_unique<Bullet>(spawnX, spawnY, angleDeg, bulletTex);
}

// TakeDamage 
void Player::TakeDamage(int damage) {
	// ★★★ 修正5: currentHealth を使用して HP を処理 ★★★
	currentHealth -= damage;

	// MaxHealth の取得も GameParams から行う
	float maxHp = GameParams::GetInstance().player.maxHealth;

	if (currentHealth < 0) currentHealth = 0;
	if (currentHealth > maxHp) currentHealth = maxHp; // 念の為クランプ
}