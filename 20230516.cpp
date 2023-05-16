#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib> // include rand()
#include <Windows.h>
#include <conio.h>

using namespace std;


// �ֱ� c++: #if ���� ����, ��� #ifdef�� ��.

class Canvas;
class GameObject {
	char* shape;
	int		pos;
	bool	visible;

public:
	GameObject(const char* shape, int pos, bool visible)
		: shape(new char[strlen(shape) + 1]), pos(pos), visible(visible)
	{
		if (this->shape != nullptr)
			strcpy(this->shape, shape);
	}

	int		getPos() const { return pos; } // getter
	void	setPos(int pos) { this->pos = pos; } // setter

	const char* getShape() const { this->shape; } // getter
	void	setShape(const char* shape) { strcpy(this->shape, shape); } // setter
	int		getShapeSize() const { return (int)strlen(this->shape); }

	bool	isVisible() const { return visible; } // gettter
	void	setVisible(bool visible = true) { this->visible = visible; } // setter


	virtual void print() const { printf("GameObject %p\n", this); }

	// virtual�� ���� ���� Ȯ�强�� �����غ���.
	virtual void draw(Canvas& canvas);

	void move(int speed) { pos += speed; }

	// bullet�� ��� �Է��з����Ͱ� �ʿ���.
	// fire�Լ��� �鿩���� > player, enemy ������ �� �ʿ��ұ�?
	// player: �÷��̾��� ��ġ���� �߻�Ǿ�� �ϱ� ������ �ʿ�. �ٵ� �̰� GameObject���
	// enemy: ������ ã�� ���� �ʿ�. �׷� enemy���ٴ� �������� �Ǵ��ϵ��� ��ģ��.

	// �迭�� ����� ���� �迭 ������ �����������. ���� ������������ �Ѱ��ֱ�
	// ��������� �ǵ����̸� ����
	virtual void update(GameObject* objs[], int max_objs) {}

	virtual ~GameObject() {
		delete[] this->shape;
		this->shape = nullptr;
		this->pos = 0;
		this->visible = visible;
	}
};

// canvas
class Canvas {
	char* frameBuffer;
	int		size;

public:
	Canvas(int size) : size(size), frameBuffer(new char[(size_t)size + 1])
	{
		clear();
	}

	void clear()
	{
		for (int i = 0; i < size; i++)
			frameBuffer[i] = ' ';
		frameBuffer[size] = '\0';
	}

	void draw(const char* shape, int pos, bool visible)
	{
		if (visible == false) return;

		for (int i = 0; i < strlen(shape); i++)
		{
			if (pos + i < 0) continue;
			if (pos + i > size - 1) continue;

			frameBuffer[pos + i] = shape[i];
		}
	}

	void render() const
	{
		printf("%s\r", frameBuffer);
	}

	~Canvas()
	{
		delete[] frameBuffer;
		frameBuffer = nullptr;
		size = 0;
	}
};


void GameObject::draw(Canvas& canvas) { canvas.draw(shape, pos, visible); }


// player
class Player : public GameObject {

public:
	Player(const char* shape, int pos, bool visible) : GameObject(shape, pos, visible)
	{}

	void print() const override { printf("Player %p\n", this); }

	~Player()
	{}
};

class BlinckablePlayer : public Player {
	int n_frames;

public:
	BlinckablePlayer(const char* shape, int pos, bool visible) : Player(shape, pos, visible), n_frames(0) {}

	void setBlinckingPeriod(int n_frames) { this->n_frames = n_frames; }

	void draw(Canvas& canvas) override {
		if (n_frames > 0) {
			if (n_frames % 2 == 0) {
				n_frames--;
				return;
			}
		}
		GameObject::draw(canvas);
	}
};


// enemy
class Enemy : public GameObject {
	int hp;
	int n_frames;

public:
	Enemy(const char* shape, int pos, bool visible) 
		: GameObject(shape, pos, visible), hp(10), n_frames(0)
	{}

	void print() const override { printf("Enemy %p\n", this); }

	void setTimeout(int n) { n_frames = n; }

	// ���� �߻��� ���� > �θ�Ŭ�������� const���� ����. �����δ� �ٲ� �ű� ������ �θ�Ŭ�������� const�� ���ش�.
	void draw(Canvas& canvas) override {
		if (n_frames > 0) {
			n_frames--;
			// hp�� �ֻ�ܿ��� ����ϱ� ���� ���� ����.
			GameObject::draw(canvas);

			// �������� �ʴ� ������ ���� �����ϸ� ������ ����Ѵ�.
			static char buffer[10];
			// printf�� buffer�� ����־��ִ� �Լ�. > sprintf
			sprintf(buffer, "%d", hp);
			canvas.draw(buffer, getPos() + 1, true);
		}
	}

	void onHit() {
		if (hp > 0) {
			hp--;
			setTimeout(20);
		}
		if (hp <= 0) setVisible(false);
	}

	void update() {
		int prob = rand() % 100;
		if (prob < 80) return;
		if (prob < 90) move(-1);
		else move(1);
	}

	bool isColliding(int pos) const
	{
		int myPos = getPos();
		return pos >= myPos && pos < myPos + getShapeSize();
	}

	~Enemy() {}
};


// bullet
class Bullet : public GameObject {
	int			direction;
	// �ڵ� �ܼ�ȭ.
	// int			num_remaining_frames_to_fire;

public:
	Bullet(const char* shape, int pos, bool visible, int direction)
		: GameObject(shape, pos, visible), direction(direction)
	{}

	void fire(GameObject* player, int direction)
	{
		if (player == nullptr) return;
		// player != nullptr
		int player_pos = player->getPos();

		setVisible();
		setPos(player_pos + player->getShapeSize());

		this->direction = direction;
		setShape(">");
		if (this->direction == 1)
		{
			setPos(player_pos);
			setShape("<");
		}
	}

	void move()
	{
		if (direction == 0) setPos(getPos() + 1);
		else setPos(getPos() - 1);
	}

	void print() const override { printf("Bullet %p\n", this); }

	void update(GameObject* objs[], int max_objs)
	{
		/*
		// ������ �����Ѥ��� ��������� ������ �ʿ� ����.
		num_remaining_frames_to_fire--;
			if (num_remaining_frames_to_fire == 0)
			{
				fire(player, enemy);
				return;
			}	*/	
		if (isVisible() == false) return;

		for (int i = 0; i < max_objs; i++) {
			if (objs[i] == nullptr) continue;
			Enemy* e = dynamic_cast<Enemy*>(objs[i]);
			if (e == nullptr) continue;
			if (e->isVisible() == false)continue;
			if (e->isColliding(getPos()) == true) {
				e->onHit();
				setVisible(false);
				return;
			}
		}

		move();
	}

	~Bullet()
	{
		direction = 0;
	}
};


class GO {
public:
	GO() { cout << "GO constructor" << endl; }

	virtual ~GO() { cout << "GO distructor" << endl; }
};

class P : public GO {
public:
	P() :GO() { cout << "P constructor" << endl; }

	~P() { cout << "P distructor" << endl; }
};

int main()
{
// #define EX 0

#ifdef EX
	GO* g = new GO();
	// P������ ȣ�� > G������ ȣ�� > G������ ���� > P������ ����
	printf("\n");
	P* p = new P();

	printf("\n");

	delete g;
	// P�Ҹ��� ȣ�� > P�Ҹ��� ���� > G�Ҹ��� ȣ�� > G�Ҹ��� ����
	printf("\n");
	delete p;


	GO* go = new P();
	// �ٿ�ĳ����
	P* p2 = static_cast<P*>(go);
	// �Ҹ��ڸ� virtual�� ����ٸ�? �̸��� �ٸ��� ������Ұ� �ٸ� �׷��� �ҷ��ͼ� �����ϰ� �̷� ������ ���Ե�.
	// >> �Ҹ��ڸ� ���ܻ�Ȳ���� �� �� ������? > �Ҹ��ڴ� �׻� virtual��
	// virtual�̶�? �ڽ� Ŭ�������� �θ� Ŭ������ �ִ� �Լ��� ��ĳ�����ϴ� ��.
	delete p2;
#endif

	const int canvas_size = 80;
	Canvas* canvas = new Canvas(canvas_size);

	const int max_objs = 50;
	GameObject* objs[max_objs];
	for (int i = 0; i < max_objs; i++)
		objs[i] = nullptr;

	for (int i = 0; i < 5; i++)
	{
		switch (rand() % 2)
		{
		case 0:
			objs[i] = new BlinckablePlayer("P", rand()%(canvas_size-5), true);
			// �ٿ�ĳ�������ֱ�
			// objs[i]->setBlinckingPeriod(20);
			break;
		case 1:
			objs[i] = new Enemy("(*-*)", rand () % (canvas_size - 5), true);
			break;
		}
	}

	while (true) {

		canvas->clear();

		if (_kbhit()) {
			Bullet* found = nullptr;
			Player* player = nullptr;
			int randPos;

			int ch = _getch();

			switch (tolower(ch)) {
			case 'a':
			case 'd':
			case ' ':
				randPos = rand() % (canvas_size - 5);
				for (int i = 0; i < max_objs; i++) {
					Player* obj;
					if (!objs[i]) continue;
					
					obj = dynamic_cast<Player*>(objs[i]);
					if (!obj) continue;
					if (!player) player = obj;
					else
						if (abs(obj->getPos() - randPos) < abs(player->getPos() - randPos))
							player = obj;
				}
				if (!player) break;
				for (int i = 0; i < max_objs; i++) {
					if (!objs[i]) {
						Bullet* obj = new Bullet(">", 0, false, 0);
						objs[i] = obj;
						found = obj;
						break;
					}

					Bullet* obj = dynamic_cast<Bullet*>(objs[i]);
					if (!obj) continue;
					if (obj->isVisible() == false) {
						found = obj;
						break;
					}
					
					if (!found) break;
					found->fire(player, rand() % 2);

					// ������ ����� �����غ���. 
				}
			}
		}

		for (int i = 0; i < max_objs; i++) {
			if (objs[i] != nullptr)
				objs[i]->update(objs, max_objs);
		}

		for (int i = 0; i < max_objs; i++) {
			if (objs[i] != nullptr)
				objs[i]->draw(*canvas);
		}

		canvas->render();
		Sleep(100);
	}

#define TEST_GAME 0

#if TEST_GAME
	for (int i = 0; i < max_objs; i++)
	{
		// examine whether current object is a bullet
		Bullet* b = dynamic_cast<Bullet*>(objs[i]);

		// if not, continue
		if (b == nullptr) continue;

		// if so, set a random position.
		int randPos = rand() % canvas_size;

		// find the closest enemy from the random position
		Enemy* closestEnemy = nullptr;
		int closestDistance = canvas_size;
		for (int j = 0; j < max_objs; j++)
		{
			Enemy* e = dynamic_cast<Enemy*>(objs[j]);
			if (e == nullptr) continue;
			// e != nullptr
			int distance = abs(e->getPos() - randPos);
			if (distance < closestDistance)
			{
				closestEnemy = e;
				closestDistance = distance;
			}
		}

		// find the closest player from the random position
		Player* closestPlayer = nullptr;
		closestDistance = canvas_size;
		for (int j = 0; j < max_objs; j++)
		{
			Player* p = dynamic_cast<Player*>(objs[j]);
			if (p == nullptr) continue;
			int distance = abs(p->getPos() - randPos);
			if (distance < closestDistance)
			{
				closestPlayer = p;
				closestDistance = distance;
			}
		}

		// if found the closest enemy and the closest player, fire from the player to the enemy.
		if (closestPlayer != nullptr && closestEnemy != nullptr)
		{
			b->fire(*closestPlayer, *closestEnemy);
		}
	}
#endif

#if TEST_GAME
	Player* player = new Player("(^___^)", 50, true); /* hello world */
	Enemy* enemy = new Enemy("(*-*)", 10, true);
	const int max_bullets = 100;
	Bullet* bullets[max_bullets];
	for (int i = 0; i < max_bullets; i++)
		bullets[i] = nullptr;

	bool exit_flag = false;
	while (exit_flag == false)
	{
		canvas->clear();

		if (_kbhit())
		{
			Bullet* found = nullptr;
			int ch = _getch();

			switch (tolower(ch))
			{
			case 'a':
				player->move(1); break;
			case 'd':
				player->move(0); break;
			case ' ':
				for (int i = 0; i < max_bullets; i++) {
					if (bullets[i] == nullptr) {
						bullets[i] = new Bullet(">", 0, false, 0, 0);
						found = bullets[i];
						break;
					}
					if (bullets[i]->isVisible() == false) {
						found = bullets[i];
						break;
					}
				}
				// if found, fire
				if (found != nullptr) {
					found->fire(*player, *enemy);
				}
				break;
			case 'q':
				exit_flag = true;
				break;
			}
		}

		/*player->draw(*canvas);
		enemy->draw(*canvas);
		for (int i = 0; i < max_bullets; i++)
		{
			if (bullets[i] == nullptr) continue;
			bullets[i]->draw(*canvas);
		}

		player->update();
		enemy->update();
		for (int i = 0; i < max_bullets; i++)
		{
			if (bullets[i] == nullptr) continue;
			bullets[i]->update(*player, *enemy);
		}*/

		canvas->render();
		Sleep(100);
	}

	for (int i = 0; i < max_bullets; i++)
		delete bullets[i];
	delete enemy;
	delete player;
	delete canvas;
#endif


	return 0;
}