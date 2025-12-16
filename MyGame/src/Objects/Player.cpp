#include "Player.h"
#include "../Core/Game.h"
#include "../Core/InputHandler.h"
#include "../Core/Camera.h"
#include "../Core/Time.h"
#include "Bullet.h"
#include <cmath>
#include <memory> // std::make_uniqueを使うために念のため

Player::Player(float x, float y, SDL_Texture* tex, SDL_Texture* bulletTex, Camera* cam)
	: GameObject(x, y, 46, 128, tex)
{
	status.hp = 100;
	status.maxHp = 100;
	status.speed = 300.0f;
	status.attackPower = 10;
	status.fireInterval = 0.15f;

	shootTimer.SetInterval(status.fireInterval);

	angle = 0;
	useGravity = true;
	name = "Player";
	this->bulletTexture = bulletTex;
	this->camera = cam;
	isFlipLeft = false;

	animator = std::make_unique<Animator>();

	// JSONファイルから読み込み
	animator->LoadFromJson("assets/data/player.json");
}

void Player::Update(Game* game) {
	InputHandler* input = game->GetInput();

	// アニメーターとタイマーの時間を進める
	// unique_ptr は -> 演算子でそのままメンバ関数を呼べます
	animator->Update();
	shootTimer.Update();

	// 移動計算
	float moveAmount = status.speed * Time::deltaTime;

	velX = 0;
	if (input->IsPressed(GameAction::MoveLeft)) {
		velX = -moveAmount;
		isFlipLeft = true;
	}
	if (input->IsPressed(GameAction::MoveRight)) {
		velX = moveAmount;
		isFlipLeft = false;
	}

	// アニメーション切り替え
	if (velX != 0) {
		animator->Play("Run");
	}
	else {
		animator->Play("Idle");
	}

	// ジャンプ
	if (input->IsJustPressed(GameAction::MoveUp)) {
		if (isGrounded) {
			velY = -14.0f;
			isGrounded = false;
		}
	}

	// マウス処理
	int screenMouseX, screenMouseY;
	SDL_GetMouseState(&screenMouseX, &screenMouseY);
	SDL_FPoint worldMouse = camera->ScreenToWorld(screenMouseX, screenMouseY);

	// 射撃処理
	if (input->IsPressed(GameAction::Shoot) && shootTimer.IsReady()) {
		// ★修正: unique_ptr で Bullet を受け取る
		auto newBullet = Shoot(worldMouse.x, worldMouse.y, bulletTexture);

		if (newBullet) {
			// ★修正: std::move で所有権を Game に完全に移動する
			game->Instantiate(std::move(newBullet));
			shootTimer.Reset();
		}
	}
}

void Player::OnRender(SDL_Renderer* renderer, int drawX, int drawY) {
	SDL_Rect destRect = { drawX, drawY, width, height };

	// 現在のコマを取得
	SDL_Rect srcRect = animator->GetSrcRect(width, height);

	// 向きに応じて反転
	SDL_RendererFlip flip = isFlipLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

	if (texture) {
		SDL_RenderCopyEx(renderer, texture, &srcRect, &destRect, angle, NULL, flip);
	}
	else {
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderFillRect(renderer, &destRect);
	}

	// レーザー描画
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	int mx, my;
	SDL_GetMouseState(&mx, &my);
	int lineStartX = drawX + width / 2;
	int lineStartY = drawY + height / 2;
	SDL_RenderDrawLine(renderer, lineStartX, lineStartY, mx, my);
}

// ★修正: 戻り値を unique_ptr<Bullet> に変更
std::unique_ptr<Bullet> Player::Shoot(float targetX, float targetY, SDL_Texture* bulletTex) {
	float spawnX = x + (width / 2) - 5;
	float spawnY = y + (height / 2) - 5;

	float centerX = x + width / 2;
	float centerY = y + height / 2;
	double radian = atan2(targetY - centerY, targetX - centerX);
	double angleDeg = radian * 180.0 / 3.14159265;

	// ★修正: new Bullet(...) ではなく make_unique を使って生成
	return std::make_unique<Bullet>(spawnX, spawnY, angleDeg, bulletTex);
}

void Player::TakeDamage(int damage) {
	status.hp -= damage;
	if (status.hp < 0) status.hp = 0;
}