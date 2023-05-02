#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib>
#include <Windows.h>
#include <conio.h> // console, getch & getche�� �����ϰ��ִ�.

// ���� ������ ����: ����� �Է� �ޱ� & ���� ��� ����
// �������� ���� ����: ��� / ��ĳ���� & �ٿ�ĳ���� > ĳ��Ʈ ������: �迭�� ���� �۾�


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

	void draw(Canvas& canvas) const;

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

	Canvas(int size) : size(size), frameBuffer(new char[size + 1])
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

	~Player()
	{}
};


// enemy
struct Enemy : public GameObject {

	Enemy(const char* shape, int pos, bool visible) : GameObject(shape, pos, visible)
	{}

	bool isColliding(int pos) const
	{
		int myPos = getPos();
		return pos >= myPos && pos < myPos + getShapeSize();
	}

	// enemy�� �����̰� ����� ��� �߰�
	void update() {
		int moving_prob = rand() % 100;

		if (moving_prob < 80) return;

		// setPos(getPos() + (moving_prob < 90) ? -1 : 1); ���� �Ʒ� �� �ڵ带 �����ų �� �ִ�.
		if (moving_prob < 90)setPos(getPos() - 1);
		else setPos(getPos() + 1);
	}

	~Enemy() {}
};


// bullet
struct Bullet : public GameObject {
	int			direction; // 0: L2R, 1: R2L
	int			num_remaining_frames_to_fire;

	Bullet(const char* shape, int pos, bool visible, int direction, int n_frames)
		: GameObject(shape, pos, visible), direction(direction), num_remaining_frames_to_fire(n_frames)
	{}

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

int main() {
	Canvas* canvas = new Canvas(80);
	Player* player = new Player("(^___^)", 50, true);
	Enemy* enemy = new Enemy("(*-*)", 10, true);
	const int max_bullets = 100;
	Bullet* bullets[max_bullets];
	for (int i = 0; i < max_bullets; i++)
		bullets[i] = nullptr;

	Bullet* found;

	// stdin: standard input > Ű����� �Է¹޴� ���� ���Ϸ� ǥ���� ��
	
	// stdio�� ����
	// extern FILE* stdin;
	// extern FILE* stdout;
	// extern FILE* stderr;
	
	// int getchar() {
	//		return getc(stdin);
	// }


	// while (true) {
		// �����ڵ带 �ޱ� ���� �������� ���� �� �ֵ��� unsigned char�� �ƴ� int�� ��ȯ.
		// getchar > ���� ���, ���� ����. ���۸� ������� ������ �Է¹��ڸ��� �ٷ� ����.
		// getche > echo, �Է��� ���� ���� �ð������� ������ �ٷ� ��.
		// functionŰ���� 0���� ����, Ű���� �������� 224�� ����.
		// int ch = _getch();
		// printf("ch = %d %x %c\n", ch, ch, ch);
	// }

	while (true)
	{
		canvas->clear();

		// blocking�Լ�, �Է��� ���� ������ ���� �ڵ尡 ������ �ȵ�.
		// �׷��� Ű������Ʈ �Լ�(_kbhit())�� ����Ѵ� > ����ڰ� �Է��� �ߴ��� ���ߴ����� Ȯ�ι޴� �Լ�(bool)
		if (_kbhit()) {
			int ch = _getch();
			switch (ch) {
			// switch�� ������ ��� ��������. break���� ������.
			case 'A':
			case 'a':
				player->setPos(player->getPos() - 1);
				break;
			case 'D':
			case 'd':
				player->setPos(player->getPos() + 1);
				break;
			case ' ':
				// find an empty (if so, allocate a new bullet) or unsued bullet from "bullets".
				// if found, set fire.
				// for (int i = 0; i < max_bullets; i++) {
				// 	 if (bullets[i] != nullptr) {
				//		 if (bullets[i]->isVisible())  continue;
				//		 bullets[i]->fire(*player, *enemy);
				//	 }
				//	 bullets[i]->draw(*canvas);
				// }

				//�����ڵ�
				// find an empty (if so, allocate a new bullet) or unsued bullet from "bullets".
				found = nullptr;
				for (int i = 0; i < max_bullets; i++) {
					if (bullets[i] == nullptr) {
						bullets[i] = new Bullet(">", 0, false, 0, 0);
						found = bullets[i];
						break;
					}
					if (bullets[i]->isVisible() == false) {
						bullets[i]->fire(*player, *enemy);
						break;
					}
				}
				if (found != nullptr)
					found->fire(*player, *enemy);
				break;
			}
			// �� �ڵ��� �ܼ�ȭ: toupper(char)&tolower(char)�Լ� ���
			// switch (toupper(ch)) {
			// case 'A':
			// 	player->setPos(player->getPos() - 1);
			// 	break;
			// case 'D':
			//	player->setPos(player->getPos() + 1);
			//	break;d
		}

		player->draw(*canvas);
		enemy->draw(*canvas);
		for (int i = 0; i < max_bullets; i++) {
			if (bullets[i] == nullptr) continue;
			bullets[i]->draw(*canvas);
		}

		player->update();
		enemy->update();
		for (int i = 0; i < max_bullets; i++) {
			if (bullets[i] == nullptr) continue;
			bullets[i]->update(*player, *enemy);
		}

		canvas->render();
		Sleep(100);
	}

	for (int i = 0; i < max_bullets; i++) {
		if (bullets[i] == nullptr) continue;
		delete bullets[i];
	}
	delete enemy;
	delete player;
	delete canvas;

	return 0;
}