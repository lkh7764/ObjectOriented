#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib> // include rand()
#include <Windows.h>
#include <conio.h>
#include <time.h>


// ���ݱ��� �ߴ� ��> ����ü ����� �������&�Լ������
// ����Լ��� �޸� �󿡼� ��� �� ������?
// �����Լ��� ȣ�����ִ� ���. �����Ϸ��� �ּ������� �̰����� �� ó������.
// ����ü?�� ������ ���̺��� ������ �ִ�.
// �ϴ� ������ ����ü�� print()�� ������ش�.
// ���� Bullet�� print()�� ���ְ� Bullet�� print()�� �����ϸ�?
// �����Ϸ��� �۾��� �� �� �� �� > �θ� ����ü�� ���̺��� Ȯ���ؼ� print������ ���� > �ڵ� ����

// �ڵ� © �� ���� �ڵ� �ۼ����� ���� ���� ����� ��� �������� ���� �����غ���.
// �����ϰ� �پ��� ��Ȳ���� ��ü���� ������ �����ؼ� Ǯ���.
// �̷��� �ذ��ϴ� �� �ͼ������� �ܰ迡�� �������� ���� ����غ���.
// ������ �� ����� ����(��ü�� ��� ���� �Ǵ�)�� �ذ��� �� �ִ� ���?

// ���������� draw&render�غ���

// ���
// upcasting & downcasting: downcasting�� ����� �� �� ����.
// dynamic_cast: upcasting�� ������ ���������� �����ֱ�. 3�� ��ӵ� �ŵ� �غ���? *��Ӱ��迩�� �Ѵٴ� �ſ� ����

struct Canvas;
struct GameObject {
	char* shape;
	int		pos;
	bool	visible;

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

	// virtual: �θ� Ŭ���������� ���� ����.
	// virtual �����ϸ� �ڽ� Ŭ���������� ���� ���� > ����Ǿ��ִ�.
	// ���? virtual �����ϸ� v table�� ���� ���������. ���⿡ print()�� ��ϵ�.
	// �̷��� �Ǹ� �ڽ� Ŭ�������� ������ print()�Լ��� ���� �˾ƺ�.
	// ������ > override
	// �̷��� �ϸ� ������ ������ ��������ϱ� �������?
	virtual void print() const { printf("GameObject %p\n", this); }

	void draw(Canvas& canvas) const;

	void move(int speed) { pos += speed; }

	void update() {}

	~GameObject() {
		delete[] this->shape;
		this->shape = nullptr;
		this->pos = 0;
		this->visible = visible;
	}
};

// canvas
struct Canvas {
	char* frameBuffer;
	int		size;

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


void GameObject::draw(Canvas& canvas) const { canvas.draw(shape, pos, visible); }


// player
struct Player : public GameObject {

	Player(const char* shape, int pos, bool visible) : GameObject(shape, pos, visible)
	{}

	void print() const override { printf("Player %p\n", this); }

	~Player()
	{}
};


// enemy
struct Enemy : public GameObject {

	Enemy(const char* shape, int pos, bool visible) : GameObject(shape, pos, visible)
	{}

	void print() const override { printf("Enemy %p\n", this); }

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
struct Bullet : public GameObject {
	int			direction;
	int			num_remaining_frames_to_fire;

	Bullet(const char* shape, int pos, bool visible, int direction, int n_frames)
		: GameObject(shape, pos, visible), direction(direction), num_remaining_frames_to_fire(n_frames)
	{}

	void print() const override { printf("Bullet %p\n", this); }

	void fire(const Player& player, const Enemy& enemy)
	{
		int enemy_pos = enemy.getPos();
		int player_pos = player.getPos();
		if (enemy.isColliding(player_pos)) return;

		setVisible();
		setPos(player_pos + player.getShapeSize());

		direction = 0;
		setShape(">");
		if (enemy_pos < player_pos)
		{
			setPos(player_pos);
			direction = 1;
			setShape("<");
		}
	}

	void move()
	{
		if (direction == 0) setPos(getPos() + 1);
		else setPos(getPos() - 1);
	}

	void update(const Player& player, const Enemy& enemy)
	{
		num_remaining_frames_to_fire--;
		if (num_remaining_frames_to_fire == 0)
		{
			fire(player, enemy);
			return;
		}
		if (isVisible() == false) return;

		move();

		// check whether bullet collides with enemy
		if (enemy.isColliding(getPos()))
			setVisible(false);
	}

	~Bullet()
	{
		direction = 0;
		num_remaining_frames_to_fire = 0;
	}
};

int main()
{
	const int canvasSize = 80;
	srand((unsigned int)time(NULL));

	Canvas* canvas = new Canvas(canvasSize);

	const int max_obj = 10;
	GameObject* objs[max_obj];


	// �� �������� �ϸ� rock scissors paper ����..
	// JAVA������ �� virtual�� �����Ѵ�. 
	for (int i = 0; i < max_obj; i++) {
		switch (rand() % 3) {
		case 0:
			objs[i] = new Player("(^___^)", 50, true);
			break;
		case 1:
			objs[i] = new Enemy("(*-*)", 10, true);
			break;
		case 2:
			// �̷��� �ϸ� fire()�� �� �� ����.
			objs[i] = new Bullet(">", 0, false, 0, 0);
			break;
		}
	}

	for (int i = 0; i < max_obj; i++) {
		// dynamic_cast: objs[i]�� bullet�̸� b, �ƴϸ� nullptr��ȯ.
		// ���Ѱɷ� typeID�� �ִµ� ���� �ȴٷ�.
		Bullet* b = dynamic_cast<Bullet*>(objs[i]);
		if (b != nullptr) printf("%d obj is a bullet\n", i);
		Player* p = dynamic_cast<Player*>(objs[i]);
		if (p != nullptr) printf("%d obj is a player\n", i);
		Enemy* e = dynamic_cast<Enemy*>(objs[i]);
		if (e != nullptr) printf("%d obj is an enemy\n", i);

		objs[i]->print();
		printf("\n");
	}

	// Bullet�� ã�Ƽ� randPosition�� �ְ�, ���� ����� player�� enemy���� �Ѿ��� ��� �ڵ�.
	for (int i = 0; i < max_obj; i++) {
		// examine whether current object is a bullet.
		Bullet* b = dynamic_cast<Bullet*>(objs[i]);

		// if not, continue.
		if (b == nullptr) continue;

		// if so, set a random position.
		int randPos = rand() % canvasSize;

		// find the closest enemy from the random position. > �̰Ÿ� �Ϸ��� ��׵� ��ġ������ �־���ϴ°� �ƴѰ�?
		Enemy* closestEnemy = nullptr;
		int closestDistance = canvasSize;
		for (int j = 0; j < max_obj; j++) {
			Enemy* e = dynamic_cast<Enemy*>(objs[j]);
			if (e == nullptr) continue;

			int distance = abs(e->getPos() - randPos);
			if (distance < closestDistance) {
				closestEnemy = e;
				closestDistance = distance;
			}
		} // Enemy�� ������ �������� ����.

		// find the closest player from the random position.
		Player* closestPlayer = nullptr;
		closestDistance = canvasSize;
		for (int j = 0; j < max_obj; j++) {
			Player* p = dynamic_cast<Player*>(objs[j]);
			if (p == nullptr) continue;

			int distance = abs(p->getPos() - randPos);
			if (distance < closestDistance) {
				closestPlayer = p;
				closestDistance = distance;
			}
		} // closestEnemy�� ã�� �ڵ�� �ʹ� �Ȱ���. �̰� ��ĥ �� �ִ� �����?

		// if found the closest enemy and the closest player, fire from the player to the enemy.
		if (closestPlayer != nullptr && closestEnemy != nullptr) {
			// visible ���� ������ֵ��� ���׷��̵� �� �� �ִ�.
			// ���� obj���� �迭�� ���ܵ� ������? ���� ���� ���� ������?
			// ���� ���� �� �Ѿ˿� p&e�� �±� ����, Ÿ������ p&e�� �׾��ٸ�? 
			// > update()�� p&e�� �������鼭 �浹�� ���� p&e�� �ִ��� Ȯ���ϴ� ������ ���׷��̵� ����.
			b->fire(*closestPlayer, *closestEnemy);
		}
	}


#if 0
	Bullet* bullet = new Bullet(">", 0, false, 0, 0);
	bullet->print();
	// Bullet�� �ִ� print()�� �ּ�ó���ϰ� �����ϸ� ��µǴ� this���� �޶���
	// int�� ���������� float�� �ٲٰ� ������� ���� ���� ����.

	// upcasting
	GameObject* obj = bullet;
	obj->print();

	// �̰� ����� ����ȯ�� �ʼ��� �ʿ�.
	Enemy* enemy = (Enemy*)bullet;
	enemy->print();

	Enemy* e = (Enemy*)obj;
	e->print();

	// downcasting�� ����� ����ȯ�� �ʿ�. > �����Ϸ��� �ڽ��� �� ���� ������ �츮 ���� ��ġ�� ��.
	Bullet* another = (Bullet*)obj;

	// ��� �ּ������� �ٸ� ��ü�� �ϳ� ����� �ذ�. upcasting �ƴ� ����.
	// upcasting�� downcasting�� �����ͺ����θ� ����Ѵ�.
	// ���۷����� �Ǳ� �ϴµ� �� ����� ���ְ� ���� �� ���� ������ �����͸� ����ϴ� ���� ����.
	GameObject another = *bullet;


	// visual studio������ �ּ��� �ȿ� �ּ����� ���� �� ���� (/**/ ���)
	// ��? ��ȣó�� �ν��� �ϴ� �� �ƴ϶� �׳� ���������� �˾Ƶ���.
	// �ּ��� ��� ����� �� �ִ� �� > condition, ���Ǻ� ������.
#endif

#define TEST_GAME 0

	//ifdef TEST_GAME > TEST_GAME�� ���ǵǾ� �ִٸ� ����. �̰� ���� �����ϱ� ������ #define �ּ� ó��.
#if TEST_GAME
	Player* player = new Player("(^___^)", 50, true);
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
				player->move(-1); break;
			case 'd':
				player->move(1); break;
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

		player->draw(*canvas);
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
		}

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