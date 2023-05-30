#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib> // include rand()
#include <Windows.h>
#include <conio.h>

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

	const char* getShape() const { return this->shape; } // getter
	void	setShape(const char* shape) { strcpy(this->shape, shape); } // setter
	int		getShapeSize() const { return (int)strlen(this->shape); }

	bool	isVisible() const { return visible; } // gettter
	void	setVisible(bool visible = true) { this->visible = visible; } // setter


	virtual void print() const { printf("GameObject %p\n", this); }

	virtual void draw(Canvas& canvas);

	void move(int speed) { pos += speed; }

	virtual void update(GameObject* objs[], int max_objs, int& nCreatedBullets) {}

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

class BlinkablePlayer : public Player {
	int n_frames;

public:
	BlinkablePlayer(const char* shape, int pos, bool visible) : Player(shape, pos, visible), n_frames(0)
	{}

	void draw(Canvas& canvas) override {
		if (n_frames > 0) {
			n_frames--;
			if (n_frames % 2 == 0) return;
		}
		GameObject::draw(canvas);
	}

	void setBlinkingPeriod(int n_frames)
	{
		this->n_frames = n_frames;
	}

};


// enemy
class Enemy : public GameObject {

	float hp;
	int n_frames;

public:

	Enemy(const char* shape, int pos, bool visible)
		: GameObject(shape, pos, visible), hp(10.0f), n_frames(0)
	{}

	void print() const override { printf("Enemy %p\n", this); }

	void setTimeout(int n_frames)
	{
		this->n_frames = n_frames;
	}

	void onHit(float damage)
	{
		hp = hp - damage;
		if (hp <= 0.0f) {
			setVisible(false);
		}
		else {
			setTimeout(10);
		}
	}

	void draw(Canvas& canvas) override
	{
		if (n_frames > 0) {
			n_frames--;
			GameObject::draw(canvas);
			static char buffer[10];
			sprintf(buffer, "%2.1f", hp);
			canvas.draw(buffer, getPos() + 1, true);
		}
		else {
			GameObject::draw(canvas);
		}
	}

	void update(GameObject* objs[], int max_objs, int& nCreatedBullets) {
		int prob = rand() % 100;
		if (prob < 80) return;
		if (prob < 90) move(-1);
		else move(1);
	}

	bool isColliding(int pos) const
	{
		int myPos = getPos();
		return pos >= myPos && pos <= myPos + getShapeSize();
	}

	~Enemy() {}
};


// bullet
class Bullet : public GameObject {
	int			direction;
	float		damagePower;

public:

	Bullet(const char* shape, int pos, bool visible, int direction, float damagePower)
		: GameObject(shape, pos, visible), direction(direction), damagePower(damagePower)
	{}

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

	void move()
	{
		if (direction == 0) setPos(getPos() + 1);
		else setPos(getPos() - 1);
	}

	virtual void onCollision(GameObject* objs[], int max_objs, int& nCreatedBullets);


	void update(GameObject* objs[], int max_objs, int& nCreatedBullets)
	{
		if (isVisible() == false) return;

		move();

		for (int i = 0; i < max_objs; i++)
		{
			if (objs[i] == nullptr) continue;
			Enemy* enemy = dynamic_cast<Enemy*>(objs[i]);
			if (enemy == nullptr) continue;
			if (enemy->isVisible() == false) continue;

			if (enemy->isColliding(getPos()) == true)
			{
				enemy->onHit(damagePower);
				onCollision(objs, max_objs, nCreatedBullets);
				return;
			}
		}
	}

	~Bullet()
	{
		direction = 0;
	}
};

// particle bullet
class ParticleBullet : public Bullet {
	int		n_frames_to_disappear;

public:

	ParticleBullet(int pos)
		: Bullet("*", pos, true, rand() % 2, 0.5f), n_frames_to_disappear(10)
	{}

	void onCollision(GameObject* objs[], int max_objs, int& nCreatedBullets) override {
		setVisible(false);
	}

	void update(GameObject* objs[], int max_objs, int& nCreatedBullets)
	{
		Bullet::update(objs, max_objs, nCreatedBullets);

		if (isVisible() == false) return;

		if (n_frames_to_disappear > 0) {
			--n_frames_to_disappear;
			if (n_frames_to_disappear <= 0) {
				setVisible(false);
			}
		}
	}

	~ParticleBullet()
	{
		n_frames_to_disappear = 0;
	}
};

void Bullet::onCollision(GameObject* objs[], int max_objs, int& nCreatedBullets) {
	int cnt = 0;
	for (int i = 0; i < max_objs; i++)
	{
		if (objs[i] != nullptr) continue;
		ParticleBullet* bullet = new ParticleBullet(getPos());
		nCreatedBullets++;
		objs[i] = bullet;
		if (++cnt == 2)
			break;
	}
	setVisible(false);
}

int main()
{
	const int canvas_size = 80;
	Canvas* canvas = new Canvas(canvas_size);
	const int max_objs = 50;
	GameObject* objs[max_objs];
	int nCreatedBullets = 0;

	for (int i = 0; i < max_objs; i++)
		objs[i] = nullptr;


	for (int i = 0; i < 5; i++)
	{
		switch (rand() % 2)
		{
		case 0:
			objs[i] = new BlinkablePlayer("P", rand() % (canvas_size - 5), true);
			break;
		case 1:
			objs[i] = new Enemy("E", rand() % (canvas_size - 5), true);
			break;
		}
	}

	bool exit_flag = false;
	while (exit_flag == false)
	{
		canvas->clear();

		if (_kbhit())
		{
			Bullet* found = nullptr;
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

					printf("[%2d: %s %2d %d] \nnumber of bullets: %d", i, objs[i]->getShape(), objs[i]->getPos(), objs[i]->isVisible(), nCreatedBullets);
				}
				printf("\n");
				break;

			case ' ':
				// find player
				randPos = rand() % (canvas_size - 5);
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

				if (player == nullptr) continue;


				for (int i = 0; i < max_objs; i++)
				{
					if (objs[i] == nullptr) {
						Bullet* bullet = new Bullet(">", 0, false, 0, 1.0f);
						nCreatedBullets++;
						objs[i] = bullet;
						found = bullet;
						break;
					}
				}
				if (found == nullptr) break;
				found->fire(player, rand() % 2);
				player->setBlinkingPeriod(30);
				break;
			case 'q':
				exit_flag = true;
				break;
			}
		}

		for (int i = 0; i < max_objs; i++)
		{
			if (objs[i] == nullptr) continue;
			objs[i]->update(objs, max_objs, nCreatedBullets);
		}

		for (int i = 0; i < max_objs; i++)
		{
			if (objs[i] == nullptr) continue;
			objs[i]->draw(*canvas);
		}

		canvas->render();
		Sleep(100);
	}

	for (int i = 0; i < max_objs; i++)
	{
		if (objs[i] == nullptr) continue;
		delete objs[i];
		objs[i] = nullptr;
	}
	delete canvas;

	return 0;
}