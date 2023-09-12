#include "Bullet.h"
#include "Canvas.h"
#include "Enemy.h"
#include "ParticleBullet.h"

int Bullet::nCreatedBullets = 0;

void Bullet::move() 
{
	if (direction == 0) setPos(getPos() + 1);
	else setPos(getPos() - 1);
	if (getPos() < 0 || getPos() > canvas.getSize()) setAlive(false);
}

void Bullet::onCollision(GameObject* collided)
{
	Enemy* enemy = dynamic_cast<Enemy*>(collided);
	if (enemy == nullptr) return;
	enemy->onHit(getDamagePower());

	GameObject::Add(new ParticleBullet(getPos() + rand() % 5 - 1));
	GameObject::Add(new ParticleBullet(getPos() + rand() % 5 - 1));

	setAlive(false);
}