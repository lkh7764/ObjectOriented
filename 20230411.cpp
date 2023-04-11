#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib>
#include <Windows.h>

// canvas
struct Canvas {
	char* frameBuffer;
	int		size;

	// canvas�� ��ü ������ ����� ��ȯ�ϴ� �Լ�.
	Canvas(int size)	// constructor, �������Լ�. �޸𸮻� �ö� �� ȣ��Ǵ� �Լ�.
	{
		// canvas��� ������ ��üȭ �ϴ� �����̱⶧���� ������ ����.
		// Canvas* canvas = (Canvas*)malloc(sizeof(Canvas));
		// if (canvas == nullptr) return canvas;

		this->size = size;
		// frameBuffer�� clear�� ���, �Լ� ���� ���������� ��ġ�� �ʱ� ������ this->�� �����ص� �ȴ�.
		frameBuffer = (char*)malloc(sizeof(char) * (size + 1));
		// ������ ���������� �Ǿ����. ������ ���� ����ü ���� ��� ������ ��� �Լ� ���̿����� �ʿ䰡 ����. ��? this->�� ����.
		// ��, �ٸ� ����ü ���� ȣ�⿡���� �ʿ��� �� �ִ�. 
		clear();

		// ��ȯ�� �ʿ䵵 �����Ƿ� void�� �������ش�.
		// return canvas;
	}

	// Canvas* canvas �̷��� �� �����ص���, ��? this�� ��ü�� �̹� �Ѿ�Ա� ����.
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

	// const�� ���� �Էº���? >> �Լ� ��ü �տ� const, �Լ��� ����� �� ���õ� ��������� �ٲ��� �ʴ´ٴ� ���� ���.
	void render() const
	{
		printf("%s\r", frameBuffer);
	}

	~Canvas()	// destructor, �޸� �󿡼� ����� �� ȣ��Ǵ� �Լ�. ���� �ʿ� ����. ����ȣ�� ����.
	{
		free(frameBuffer);
		frameBuffer = nullptr;
		size = 0;
	}
};


// player
struct Player {
	char* shape;
	int		pos;
	bool	visible;

	Player(const char* shape, int pos, bool visible)
	{
		this->shape = (char*)malloc(sizeof(char) * (strlen(shape) + 1));
		if (this->shape != nullptr)
			strcpy(this->shape, shape);

		this->pos = pos;
		this->visible = visible;
	}

	void draw(Canvas& canvas) const
	{
		canvas.draw(shape, pos, visible);
	}

	void update()
	{
	}

	~Player()
	{
		free(shape);
		shape = nullptr;
		pos = 0;
		visible = false;
	}
};


// enemy
struct Enemy {
	char* shape;
	int		pos;
	bool	visible;

	Enemy(const char* shape, int pos, bool visible)
	{
		this->shape = (char*)malloc(sizeof(char) * (strlen(shape) + 1));
		if (this->shape != nullptr)
			strcpy(this->shape, shape);

		this->pos = pos;
		this->visible = visible;
	}

	void draw(Canvas& canvas) const
	{
		canvas.draw(shape, pos, visible);
	}

	void update()
	{
	}

	bool isColliding(int pos) const
	{
		return (pos >= this->pos && pos < this->pos + strlen(shape));
	}

	~Enemy()
	{
		free(shape);
		shape = nullptr;
		pos = 0;
		visible = false;
	}
};


// bullet
struct Bullet {
	char* shape;
	int		pos;
	bool	visible;
	int		direction; // 0: L2R, 1: R2L
	int		num_remaining_frames_to_fire;

	Bullet(const char* shape, int pos, bool visible, int direction, int n_frames)
	{
		this->shape = (char*)malloc(sizeof(char) * (strlen(shape) + 1));
		if (this->shape != nullptr) strcpy(this->shape, shape);

		this->visible = visible;
		this->pos = pos;
		this->direction = direction;
		num_remaining_frames_to_fire = n_frames;
	}

	void draw(Canvas& canvas) const
	{
		canvas.draw(shape, pos, visible);
	}

	void fire(const Player& player, const Enemy& enemy)
	{
		if (pos == player.pos) return;

		visible = true;

		pos = player.pos + strlen(player.shape);
		direction = 0;
		strcpy(shape, ">");
		if (enemy.pos < player.pos)
		{
			pos = player.pos;
			direction = 1;
			strcpy(shape, "<");
		}
	}

	void move()
	{
		if (direction == 0) pos++;
		else pos--;
	}

	void update(const Player& player, const Enemy& enemy)
	{
		num_remaining_frames_to_fire--;
		if (num_remaining_frames_to_fire == 0)
		{
			fire(player, enemy);
			return;
		}
		if (visible == false) return;

		move();

		// check whether bullet collides with enemy
		if (enemy.isColliding(pos)) {
			visible = false;
		}
	}

	~Bullet()
	{
		free(shape);
		shape = nullptr;
		pos = 0;
		visible = 0;
		direction = 0;
		num_remaining_frames_to_fire = 0;
	}
};


struct A {
	int b;
	int c;

	// A() {}, �⺻ ������ �Լ�, ���� ������ �ʾƵ� �����Ϸ��� �˾Ƽ� ������ִ� ����.
};

A* bc = new A(); // �׷��� ������ �Լ��� �̸� ����� ���� �ʾƵ� �ش� �ڵ忡�� ������ �߻����� �ʴ´�.



int main()
{
	Canvas* canvas = new Canvas(80);	// ������ �Լ�.
	// �Ǵ�
	// Canvas* canvas = (Canvas*)malloc(sizeof(Canvas)); >> Canvas* canvas = new Canvas();
	// canvas->canvas_init(80);

	// �����͸� ����ϸ� �� �����ϰ� �ڵ带 © �� �ִ�. 
	Player* player = new Player("(^___^)", 50, true);
	Enemy* enemy = new Enemy("(*-*)", 10, true);
	Bullet* bullet = nullptr; 
	Bullet* bullet2 = nullptr; 

	// (int a = 30) == (int a(30)) == (int a{30})
	int n_frames = 0;

	while (true)
	{
		canvas->clear();

		player->draw(*canvas);
		enemy->draw(*canvas);
		if(bullet != nullptr) bullet->draw(*canvas);
		if(bullet2 != nullptr) bullet2->draw(*canvas);

		n_frames++;
		if (n_frames == 10) {
			bullet = new Bullet(">", 10, true, 0, 0 /* 30 frames to be fired (corresponding to 3 seconds */);
			bullet->fire(*player, *enemy);
		}
		if (n_frames == 30) {
			bullet2 = new Bullet(">", 10, true, 0, 0 /* 40 frames to be fired (corresponding to 4 seconds */);
			bullet2->fire(*player, *enemy);
		}

		player->update();
		enemy->update();
		if (bullet != nullptr) bullet->update(*player, *enemy);
		if (bullet2 != nullptr) bullet2->update(*player, *enemy);

		canvas->render();
		Sleep(100);
	}

	// �Ҹ��� �Լ��� ����! ���� ȣ������ ����.
	delete bullet2;
	delete bullet;
	delete enemy;
	delete player;

	delete canvas;

	return 0;
}