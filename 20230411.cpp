#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib>
#include <Windows.h>

// canvas
struct Canvas {
	char* frameBuffer;
	int		size;

	// canvas의 실체 공간을 만들어 반환하는 함수.
	Canvas(int size)	// constructor, 생성자함수. 메모리상에 올라간 후 호출되는 함수.
	{
		// canvas라는 공간을 실체화 하는 과정이기때문에 생략이 가능.
		// Canvas* canvas = (Canvas*)malloc(sizeof(Canvas));
		// if (canvas == nullptr) return canvas;

		this->size = size;
		// frameBuffer와 clear의 경우, 함수 내의 지역변수와 겹치지 않기 때문에 this->를 삭제해도 된다.
		frameBuffer = (char*)malloc(sizeof(char) * (size + 1));
		// 원래는 전방위선언 되어야함. 하지만 같은 구조체 안의 멤버 변수와 멤버 함수 사이에서는 필요가 없다. 왜? this->의 역할.
		// 단, 다른 구조체 간의 호출에서는 필요할 수 있다. 
		clear();

		// 반환할 필요도 없으므로 void로 수정해준다.
		// return canvas;
	}

	// Canvas* canvas 이런거 다 삭제해도됨, 왜? this로 실체가 이미 넘어왔기 때문.
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

	// const가 붙은 입력변수? >> 함수 본체 앞에 const, 함수가 실행될 때 관련된 멤버변수는 바뀌지 않는다는 것을 명시.
	void render() const
	{
		printf("%s\r", frameBuffer);
	}

	~Canvas()	// destructor, 메모리 상에서 사라질 때 호출되는 함수. 리턴 필요 없음. 직접호출 안함.
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

	// A() {}, 기본 생성자 함수, 내가 만들지 않아도 컴파일러가 알아서 만들어주는 형태.
};

A* bc = new A(); // 그래서 생성자 함수를 미리 만들어 놓지 않아도 해당 코드에서 오류가 발생하지 않는다.



int main()
{
	Canvas* canvas = new Canvas(80);	// 생성자 함수.
	// 또는
	// Canvas* canvas = (Canvas*)malloc(sizeof(Canvas)); >> Canvas* canvas = new Canvas();
	// canvas->canvas_init(80);

	// 포인터를 사용하면 더 복잡하게 코드를 짤 수 있다. 
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

	// 소멸자 함수는 절대! 직접 호출하지 말기.
	delete bullet2;
	delete bullet;
	delete enemy;
	delete player;

	delete canvas;

	return 0;
}