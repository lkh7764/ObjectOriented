#include <cstdlib>
#include <Windows.h>
#include <conio.h>

#include "GameObject.h"
// 외부 함수가 필요한 경우에, 헤더파일에 인클루드 하지 말고 소스파일에 인클루드 하기.
#include "Canvas.h"
#include "BlinkablePlayer.h"
#include "Enemy.h"
#include "Bullet.h"

Canvas GameObject::canvas(80, 20);
int GameObject::max_objs = 50;
GameObject** GameObject::objs = new GameObject * [GameObject::max_objs];

void GameObject::draw() { if (alive == true) canvas.draw(shape, pos, visible); }

void GameObject::Initialize()
{
	for (int i = 0; i < max_objs; i++)
		objs[i] = nullptr;

	for (int i = 0; i < 5; i++)
	{
		switch (rand() % 2)
		{
		case 0:
			GameObject::Add(new BlinkablePlayer("P", rand() % (canvas.getSize() - 5), true));
			break;
		case 1:
			GameObject::Add(new Enemy("E", rand() % (canvas.getSize() - 5), true));
			break;
		}
	}
}

void GameObject::Add(GameObject* obj)
{
	if (obj == nullptr) return;

	for (int i = 0; i < max_objs; i++)
	{
		if (objs[i] != nullptr) continue;
		objs[i] = obj;
		break;
	}
}

void GameObject::Remove(GameObject* obj)
{
	if (obj == nullptr) return;

	for (int i = 0; i < max_objs; i++)
	{
		if (obj == objs[i]) continue;

		delete objs[i];
		objs[i] = nullptr;
		return;
	}
}

void GameObject::ClearFrame()
{
	canvas.clear();
}
void GameObject::ProcessInput(bool& exit_flag)
{
	Bullet* bullet = nullptr;
	BlinkablePlayer* player = nullptr;
	int ch = _getch();
	int randPos;

	switch (tolower(ch))
	{
	case 'i':
		printf("\n");
		for (int i = 0; i < max_objs; i++)
		{
			if (objs[i] == nullptr) continue;

			printf("[%2d: %s %2d %d %d] ", i, objs[i]->getShape(), objs[i]->getPos(), objs[i]->isVisible(), objs[i]->isAlive());
		}
		Bullet::printNumberOfCreatedBullets();
		break;

	case ' ':
		// find player
		randPos = rand() % (canvas.getSize() - 5);
		for (int i = 0; i < max_objs; i++)
		{
			if (objs[i] == nullptr) continue;
			BlinkablePlayer* obj = dynamic_cast<BlinkablePlayer*>(objs[i]);
			if (obj == nullptr) continue;
			if (obj->isVisible() == false) continue;
			if (player == nullptr) {
				player = obj;
				continue;
			}
			if (abs(obj->getPos() - randPos) < abs(player->getPos() - randPos)) {
				player = obj;
			}
		}
		if (player == nullptr) break;

		bullet = new Bullet(">", 0, false, 0, 1.0f);
		GameObject::Add(bullet);
		bullet->fire(player, rand() % 2);
		player->setBlinkingPeriod(30);
		break;

	case 'q':
		exit_flag = true;
		break;
	}
}

void GameObject::UpdateAll()
{
	for (int i = 0; i < max_objs; i++)
	{
		if (objs[i] == nullptr || objs[i]->isAlive() == false) continue;
		objs[i]->update();
	}
	RemoveDeadObjects();
}

void GameObject::DrawAll()
{
	for (int i = 0; i < max_objs; i++)
	{
		if (objs[i] == nullptr) continue;
		objs[i]->draw();
	}
}

void GameObject::RenderFrame()
{
	canvas.render();
	Sleep(100);
}

void GameObject::ExamineCollision()
{
	for (int i = 0; i < max_objs; i++)
	{
		if (objs[i] == nullptr) continue;
		if (objs[i]->isAlive() == false) continue;
		ICollidable* c_i = dynamic_cast<ICollidable*>(objs[i]);
		if (c_i == nullptr) continue;
		for (int j = i + 1; j < max_objs && objs[i]->isAlive() == true; j++)
		{
			if (objs[j] == nullptr) continue;
			if (objs[j]->isAlive() == false) continue;
			ICollidable* c_j = dynamic_cast<ICollidable*>(objs[j]);
			if (c_j == nullptr) continue;
			if (objs[i]->isColliding(objs[j]) == false)  continue;
			c_j->onCollision(objs[i]);
			c_i->onCollision(objs[j]);
		}
	}
	RemoveDeadObjects();
}

void GameObject::RemoveDeadObjects()
{
	for (int i = 0; i < max_objs; i++)
	{
		if (objs[i] == nullptr) continue;
		if (objs[i]->isAlive() == true) continue;
		delete objs[i];
		objs[i] = nullptr;
	}
}

void GameObject::Deinitialize()
{
	for (int i = 0; i < max_objs; i++)
	{
		if (objs[i] == nullptr) continue;
		delete objs[i];
		objs[i] = nullptr;
	}
	delete[] objs;
}