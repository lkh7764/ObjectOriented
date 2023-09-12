#ifndef _BULLET_H_
#define _BULLET_H_

#include "GameObject.h"
#include "ICollidable.h"
class Bullet :
    public GameObject, public ICollidable
{
	int			direction;
	float		damagePower;

	static int nCreatedBullets;

public:

	Bullet(const char* shape, int pos, bool visible, int direction, float damagePower)
		: GameObject(shape, pos, visible), direction(direction), damagePower(damagePower)
	{
		nCreatedBullets++;
	}

	void fire(GameObject* player, int direction)
	{
		if (player == nullptr) return;

		int player_pos = player->getPos();

		setVisible();
		setPos(player_pos + player->getShapeSize());

		this->direction = direction;
		setShape(">");
		if (direction == 1)
		{
			setPos(player_pos);
			setShape("<");
		}
	}

	float getDamagePower() const { return damagePower; }

	void move();

	void onCollision(GameObject* collided) override;


	void update() override
	{
		if (isAlive() == false || isVisible() == false) return;

		move();
	}

	~Bullet()
	{
		direction = 0;
	}

	static void printNumberOfCreatedBullets()
	{
		printf("\n# of Created Bullets = %d\n", nCreatedBullets);
	}
};

#endif