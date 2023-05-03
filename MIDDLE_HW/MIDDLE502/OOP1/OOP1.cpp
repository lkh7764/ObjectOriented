#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib>
#include <Windows.h>


struct GameObject;
// canvas
struct Canvas {
	char* frameBuffer;
	int		size;

	Canvas(int size) : size(size), frameBuffer(new char[size + 1]) {
		clear();
	}

	void clear() {
		for (int i = 0; i < size; i++)
			frameBuffer[i] = ' ';
		frameBuffer[size] = '\0';
	}

	int getSize() { return size; }

	void draw(const GameObject& obj);

	void render() const {
		printf("%s\r", frameBuffer);
	}

	~Canvas() {
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

	GameObject* me() { return this; }

	void draw(Canvas& canvas) {
		canvas.draw(*me());
		// canvas.draw(shape, pos, visible);
	}

	void update() {}

	~GameObject() {
		delete[] this->shape;
		this->shape = nullptr;
		this->pos = 0;
		this->visible = false;
	}
};

void Canvas::draw(const GameObject& obj) {
	if (obj.visible == false) return;

	for (int i = 0; i < strlen(obj.shape); i++)
	{
		if (obj.pos + i < 0) continue;
		if (obj.pos + i > size - 1) continue;

		frameBuffer[obj.pos + i] = obj.shape[i];
	}
}

// enemy
struct Enemy : public GameObject {
	float hp;
	int frameCount;
	char* emergencyShape;
	char* originShape;

	Enemy(const char* shape, int pos, bool visible) : GameObject(shape, pos, visible), hp(10.0), frameCount(0),
		emergencyShape(new char[getShapeSize() + 1]), originShape(new char[getShapeSize() + 1]) {
		if (emergencyShape != nullptr) {
			for (int i = 0; i < getShapeSize(); i++)
				emergencyShape[i] = ' ';
		}
		emergencyShape[getShapeSize()] = '\0';

		if (originShape != nullptr)
			strcpy(originShape, getShape());
	}

	bool isColliding(int pos) const {
		int myPos = getPos();
		return pos >= (myPos - 1) && pos < (myPos + 1) + getShapeSize();
	}

	void damage() {
		hp -= 0.3;
	}

	void emergency() {
		frameCount++;
		if (frameCount % 2 == 1)
			setShape(emergencyShape);
		else
			setShape(originShape);
	}

	void update() {
		if (hp <= 3.0) { emergency(); }
		if (hp <= 0.0) { setVisible(false); }
	}

	~Enemy() {
		hp = 0;
		delete[] emergencyShape;
		emergencyShape = nullptr;
		delete[] originShape;
		originShape = nullptr;
	}
};

// laser
struct Laser : public GameObject {

	Laser(int pos) : GameObject("-", pos, true) {}

	void attack(Enemy& enemy) {
		bool e_visible = isVisible();
		if (e_visible == true)
			enemy.damage();
	}

	void update(Enemy& enemy) {
		int pos = getPos();
		if (enemy.isColliding(pos))
			attack(enemy);
	}

	~Laser() {}
};

// player
struct Player : public GameObject {
	int frameCount;
	int direction;	// LtoR: 0, RtoL: 1
	int distance;
	int time_ray;

	int rayCount;
	int rayInterval;

	Player(const char* shape, int pos, bool visible) : GameObject(shape, pos, visible),
		frameCount(0), direction(0), distance(0), time_ray(0), rayCount(0), rayInterval(30) {}

	void setDirection(Enemy& enemy, Canvas& canvas) {
		int p_pos = getPos();
		int e_pos = enemy.getPos();
		int p_shapeSize = getShapeSize();
		int e_shapeSize = enemy.getShapeSize();

		if (enemy.isVisible() == true) {
			if (p_pos < e_pos) {
				direction = 0;
				distance = e_pos - p_pos - p_shapeSize;
			}
			else {
				direction = 1;
				distance = p_pos - e_pos - e_shapeSize;
			}
		}
		else {
			direction = rand() % 2;
			if (direction == 0)
				distance = canvas.getSize() - p_pos - p_shapeSize;
			else
				distance = p_pos;
		}
	}

	void shootRay(Enemy& enemy, Canvas& canvas) {
		int p_pos = getPos();
		int p_shapeSize = getShapeSize();

		Laser* lasers = (Laser*)malloc(sizeof(Laser) * distance);
		for (int i = 0; i < distance; i++) {
			Laser* laser = &lasers[i];
			if (direction == 0)
				laser = new Laser(p_pos + p_shapeSize + i);
			else
				laser = new Laser(p_pos - i - 1);
			laser->draw(canvas);
			laser->update(enemy);
		}

		time_ray++;

		if (time_ray >= 20) {
			free(lasers);
			direction = 0;
			distance = 0;
			time_ray = 0;
			frameCount = 0;
			rayCount++;
		}
	}

	void update(Enemy& enemy, Canvas& canvas) {
		frameCount++;
		if (rayCount != 0)
			rayInterval = 50;
		if (frameCount >= rayInterval) {
			if (frameCount == rayInterval)
				setDirection(enemy, canvas);
			shootRay(enemy, canvas);
		}
	}

	~Player() {
		frameCount = 0;
		direction = 0;
		distance = 0;
		time_ray = 0;
		rayCount = 0;
		rayInterval = 0;
	}
};


int main()
{
	Canvas* canvas = new Canvas(80);
	Player* player = new Player("(^___^)", 10, true);
	Enemy* enemy = new Enemy("(*-*)", 50, true);

	while (true)
	{
		canvas->clear();

		player->draw(*canvas);
		enemy->draw(*canvas);

		player->update(*enemy, *canvas);
		enemy->update();

		canvas->render();
		Sleep(100);
	}

	delete enemy;
	delete player;
	delete canvas;

	return 0;
}