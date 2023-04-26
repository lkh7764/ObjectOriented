#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib>
#include <Windows.h>

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

	const char* getShape() const { return this->shape; } // getter
	void	setShape(const char* shape) { strcpy(this->shape, shape); } // setter
	int		getShapeSize() const { return (int)strlen(this->shape); }

	bool	isVisible() const { return visible; } // gettter
	void	setVisible(bool visible = true) { this->visible = visible; } // setter

	void draw(Canvas& canvas) const { canvas.draw(shape, pos, visible); }

	void update() {}

	~GameObject() {
		delete[] this->shape;
		this->shape = nullptr;
		this->pos = 0;
		this->visible = false;
	}
};

// player
struct Player : public GameObject {

	Player(const char* shape, int pos, bool visible) : GameObject(shape, pos, visible)
	{}

	~Player()
	{}
};

// bullet
struct Bullet : public GameObject {
	int			direction; // 0: L2R, 1: R2L
	int			num_remaining_frames_to_fire;

	Bullet(const char* shape, int pos, bool visible, int direction, int n_frames)
		: GameObject(shape, pos, visible), direction(direction), num_remaining_frames_to_fire(n_frames)
	{}

	void move()
	{
		if (direction == 0) setPos(getPos() + 1);
		else setPos(getPos() - 1);
	}

	void update(const Player& player /* you need to fill something else */ )
	{
		num_remaining_frames_to_fire--;
		if (num_remaining_frames_to_fire == 0)
		{
			/* you need to do something else. */
			return;
		}
		if (isVisible() == false) return;

		move();
	}

	~Bullet()
	{
		direction = 0;
		num_remaining_frames_to_fire = 0;
	}
};


int main()
{
	Canvas* canvas = new Canvas(80);
	Player* player = new Player("(^___^)", 50, true);
	Bullet* bullet = new Bullet(">", 10, false, 0, 30);

	while (true)
	{
		canvas->clear();

		player->draw(*canvas);
		bullet->draw(*canvas); 

		player->update();
		// bullet->update( ???? )
		
		canvas->render();
		Sleep(100);
	}

	delete bullet;
	delete player;
	delete canvas;

	return 0;
}