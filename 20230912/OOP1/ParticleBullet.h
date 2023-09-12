#ifndef _PARTICLEBULLET_H_
#define _PARTICLEBULLET_H_

#include "Bullet.h"
class ParticleBullet :
    public Bullet
{
	int		n_frames_to_disappear;

public:

	ParticleBullet(int pos)
		: Bullet("*", pos, true, rand() % 2, 0.5f), n_frames_to_disappear(10)
	{}

	void onCollision(GameObject* collided) override;

	void update() override
	{
		Bullet::update();

		if (isAlive() == false || isVisible() == false) return;

		if (n_frames_to_disappear > 0) {
			--n_frames_to_disappear;
			if (n_frames_to_disappear <= 0) {
				setAlive(false);
			}
		}
	}

	~ParticleBullet()
	{
		n_frames_to_disappear = 0;
	}
};

#endif