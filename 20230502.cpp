#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib>
#include <Windows.h>
#include <conio.h> // console, getch & getche를 포함하고있다.

// 오늘 진행할 내용: 사용자 입력 받기 & 연사 기능 구현
// 앞으로의 수업 일정: 상속 / 업캐스팅 & 다운캐스팅 > 캐스트 연산자: 배열을 통해 작업


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

	// enemy가 움직이게 만드는 기능 추가
	void update() {
		int moving_prob = rand() % 100;

		if (moving_prob < 80) return;

		// setPos(getPos() + (moving_prob < 90) ? -1 : 1); 으로 아래 두 코드를 단축시킬 수 있다.
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

	// stdin: standard input > 키보드로 입력받는 것을 파일로 표현한 것
	
	// stdio의 형태
	// extern FILE* stdin;
	// extern FILE* stdout;
	// extern FILE* stderr;
	
	// int getchar() {
	//		return getc(stdin);
	// }


	// while (true) {
		// 에러코드를 받기 위해 음수값을 받을 수 있도록 unsigned char가 아닌 int로 반환.
		// getchar > 버퍼 사용, 수정 가능. 버퍼를 사용하지 않으면 입력받자마자 바로 나옴.
		// getche > echo, 입력한 값에 대해 시각적으로 반응이 바로 옴.
		// function키들은 0으로 시작, 키보드 오른쪽은 224로 시작.
		// int ch = _getch();
		// printf("ch = %d %x %c\n", ch, ch, ch);
	// }

	while (true)
	{
		canvas->clear();

		// blocking함수, 입력을 받지 않으면 다음 코드가 실행이 안됨.
		// 그래서 키보드히트 함수(_kbhit())를 사용한다 > 사용자가 입력을 했는지 안했는지를 확인받는 함수(bool)
		if (_kbhit()) {
			int ch = _getch();
			switch (ch) {
			// switch는 밑으로 계속 내려간다. break나올 때까지.
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

				//교수코드
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
			// 위 코드의 단순화: toupper(char)&tolower(char)함수 사용
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