#pragma once
#include "GameObject.h"
#include "../Core/Timer.h"
#include "../Core/Animator.h"
#include <memory> // ★必須

// 前方宣言
class Game;
class Camera;
class Bullet;
struct SDL_Texture;

struct UnitStatus {
	int hp;
	int maxHp;
	float speed;
	int attackPower;
	float fireInterval;
};

class Player : public GameObject {
public:
	Player(float x, float y, SDL_Texture* tex, SDL_Texture* bulletTex, Camera* cam);


	void Update(Game* game) override;
	void OnRender(SDL_Renderer* renderer, int drawX, int drawY) override;

	void TakeDamage(int damage);
	int GetHP() const { return status.hp; }
	int GetMaxHP() const { return status.maxHp; }

private:
	// ★修正: unique_ptr<Bullet> を返すように変更
	std::unique_ptr<Bullet> Shoot(float targetX, float targetY, SDL_Texture* bulletTex);

public:
	UnitStatus status;

private:
	double angle;
	SDL_Texture* bulletTexture;
	Camera* camera;

	Timer shootTimer;

	std::unique_ptr<Animator> animator;

	bool isFlipLeft = false;
};