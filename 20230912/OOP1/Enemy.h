#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "GameObject.h"
#include "ICollidable.h"
class Enemy :
    public GameObject, public ICollidable
{
	float hp;
	int n_frames;

public:

	Enemy(const char* shape, int pos, bool visible)
		: GameObject(shape, pos, visible), hp(5.0f), n_frames(0)
	{}

	void setTimeout(int n_frames)
	{
		this->n_frames = n_frames;
	}

	void onHit(float damage)
	{
		hp = hp - damage;
		if (hp <= 0.0f) {
			setAlive(false);
		}
		else {
			setTimeout(10);
		}
	}

	void draw() override;

	void onCollision(GameObject* collided) override {};

	void update() {
		if (isAlive() == false || isVisible() == false) return;

		int prob = rand() % 100;
		if (prob < 80) return;
		if (prob < 90) move(-1);
		else move(1);
	}

	~Enemy() {}
};

#endif