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

protected:
	static Canvas canvas;
	static int max_objs;
	static GameObject** objs;

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

	virtual void draw();

	void move(int speed) { pos += speed; }

	virtual void update() {}

	virtual ~GameObject() {
		delete[] this->shape;
		this->shape = nullptr;
		this->pos = 0;
		this->visible = visible;
	}

	static void Initialize();
	static void Deinitialize();
	static void ClearFrame();
	static void RenderFrame();
	static void UpdateAll();
	static void RemoveUnused();
	static void DrawAll();
	static void ProcessInput(bool& exit_flag);
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

	int getSize() { return size; }

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

Canvas GameObject::canvas(80);
int GameObject::max_objs = 50;
GameObject** GameObject::objs = new GameObject*[GameObject::max_objs];

void GameObject::draw() { canvas.draw(shape, pos, visible); }


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

	void draw() override {
		if (n_frames > 0) {
			n_frames--;
			if (n_frames % 2 == 0) return;
		}
		GameObject::draw();
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

	void draw() override
	{
		if (n_frames > 0) {
			n_frames--;
			GameObject::draw();
			static char buffer[10];
			sprintf(buffer, "%2.1f", hp);
			draw();
		}
		else {
			GameObject::draw();
		}
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
		return pos >= myPos && pos <= myPos + getShapeSize();
	}

	~Enemy() {}
};


// bullet
class Bullet : public GameObject {
	int			direction;
	float		damagePower;
	static int	bulletNum;

public:

	Bullet(const char* shape, int pos, bool visible, int direction, float damagePower)
		: GameObject(shape, pos, visible), direction(direction), damagePower(damagePower)
	{
		bulletNum++;
	}

	static int getBulletNum() { return bulletNum; }

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
		//if (getPos() < 0 || getPos() > canvas.getSize()) {

		//}
	}

	virtual void onCollision();


	void update()
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
				onCollision();
				return;
			}
		}
	}

	~Bullet()
	{
		direction = 0;
	}
};

// Bullet �ȿ��� �� static int bulletNum�� ���� �ѰŰ� ���Ǵ� ���� ����� �Ѵ�. 
// �Ϲ����� class�� ����Լ��� �ν��Ͻ�ȭ�� �� �Բ� ���ǵȴ�.
// �ٵ� static�� class �� 1�� ���� �����Ѵ�. ���� ���Ǵ� �� �ǰ� ���� �Ǵ� ������ �۵��ȴ�. 
// ���������� ���, ���� �ʱ�ȭ���� �ʾƵ� 0���� �ڵ� �ʱ�ȭ�ȴ�. ���� ��͵� ���� �ʱ�ȭ �� ���൵ �� �۵���.
// class ���� static�� ������ ���� �����ϴ�. �ٵ� Ŭ���� �ܿ����� �������� ���Ѵٴ� �������� ����.
int Bullet::bulletNum = 0;

// main���� ���������� ����ؼ� �ڵ带 �ۼ��ϸ�? �����ؾ� �� �κ��� ����...
// 1. particleBullet�� Bullet ���� onCollsion �Լ��� ����� �� ���� > onCollision�� ���������� ������ �Ѱ���� ��.
// 2. �׷� onCollision�� ����Ǵ� Bullet�� update���� ���������� ������ �Ѱ���� ��.
// 3. �ٵ� GameObject�� update�� override�ؼ� ���ϱ� GameObject�� update�� ���������� ������ �Ѱ��ִ� ������� �ٲ�� ��.
// 4. �׷� GameObject�� update�� override�ؼ� ���� ��� update�Լ��鵵 �� ���������� ������ �Ѱ��ִ� ������� �ٲ�� ��.


// particle bullet
class ParticleBullet : public Bullet {
	int		n_frames_to_disappear;

public:

	ParticleBullet(int pos)
		: Bullet("*", pos, true, rand() % 2, 0.5f), n_frames_to_disappear(10)
	{}

	void onCollision() override {
		setVisible(false);
	}

	void update(GameObject* objs[], int max_objs)
	{
		Bullet::update();

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

void Bullet::onCollision() {
	int cnt = 0;
	for (int i = 0; i < max_objs; i++)
	{
		if (objs[i] != nullptr) continue;
		ParticleBullet* bullet = new ParticleBullet(getPos());
		objs[i] = bullet;
		if (++cnt == 2)
			break;
	}
	setVisible(false);
}


void GameObject::Initialize() {
	for (int i = 0; i < max_objs; i++)
		objs[i] = nullptr;

	for (int i = 0; i < 5; i++)
	{
		switch (rand() % 2)
		{
		case 0:
			objs[i] = new BlinkablePlayer("P", rand() % (canvas.getSize() - 5), true);
			break;
		case 1:
			objs[i] = new Enemy("E", rand() % (canvas.getSize() - 5), true);
			break;
		}
	}
}
void GameObject::Deinitialize() {
	for (int i = 0; i < max_objs; i++)
	{
		if (objs[i] == nullptr) continue;
		delete objs[i];
		objs[i] = nullptr;
	}
	delete[] objs;
}
void GameObject::ClearFrame() {
	canvas.clear();
}
void GameObject::RenderFrame() {
	canvas.render();
	Sleep(100);
}
void GameObject::UpdateAll() {
	for (int i = 0; i < max_objs; i++)
	{
		if (objs[i] == nullptr) continue;
		objs[i]->update();
	}
}
void GameObject::RemoveUnused() {
	for (int i = 0; i < max_objs; i++) {
		if (objs[i] == nullptr) continue;
		if (objs[i]->isVisible() == true) continue;
		delete objs[i];
	}
}
void GameObject::DrawAll() {
	for (int i = 0; i < max_objs; i++)
	{
		if (objs[i] == nullptr) continue;
		objs[i]->draw();
	}
}
void GameObject::ProcessInput(bool& exit_flag) {
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

			int bulletNum = Bullet::getBulletNum();
			printf("[%2d: %s %2d %d], number of bullet: %d ", i, objs[i]->getShape(), objs[i]->getPos(), objs[i]->isVisible(), bulletNum);
		}
		printf("\n");
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

		if (player == nullptr) return;


		for (int i = 0; i < max_objs; i++)
		{
			if (objs[i] == nullptr) {
				Bullet* bullet = new Bullet(">", 0, false, 0, 1.0f);
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

int main()
{
	GameObject::Initialize();

	bool exit_flag = false;
	while (exit_flag == false)
	{
		GameObject::ClearFrame();

		if (_kbhit()) GameObject::ProcessInput(exit_flag);

		GameObject::UpdateAll();
		GameObject::RemoveUnused();

		GameObject::DrawAll();

		GameObject::RenderFrame();
	}

	GameObject::Deinitialize();

	return 0;
}