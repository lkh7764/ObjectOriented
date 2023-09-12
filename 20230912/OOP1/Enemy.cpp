#include "Enemy.h"
#include "Canvas.h"

void Enemy::draw()
{
	if (n_frames > 0) {
		n_frames--;
		GameObject::draw();
		static char buffer[10];
		sprintf(buffer, "%2.1f", hp);
		canvas.draw(buffer, getPos() + 1, true);
	}
	else {
		GameObject::draw();
	}
}