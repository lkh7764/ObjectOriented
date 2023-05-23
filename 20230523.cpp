#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib> // include rand()
#include <Windows.h>
#include <conio.h>


// 오늘 할 기능
// 총알과 enemy가 충돌할 때 총알의 파편이 튀어나온다. > particleBullet: *, 두개가 튀어나온다.
// 방향은 상관이 없다. particleBullet은 10frame동안 지정된 방향으로 이동한다.
// 이동 시 다른 enemy와 충돌할 경우, 충돌한 enemy hp-0.5 / particleBullet은 사라진다.
// i를 누르면 각 obj에 대한 정보가 출력되게끔

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

	virtual void draw(Canvas& canvas);

	void move(int speed) { pos += speed; }

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

class BlinkablePlayer : public Player {
	int n_frames;

public:
	BlinkablePlayer(const char* shape, int pos, bool visible)
		: Player(shape, pos, visible), n_frames(0)
	{}

	void setBlinkingPeriod(int n_frames) { this->n_frames = n_frames; }

	void draw(Canvas& canvas) override {
		if (n_frames > 0) {
			n_frames--;
			if (n_frames % 2 == 0) return;
		}
		GameObject::draw(canvas);
	}

	~BlinkablePlayer() {}
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

	void draw(Canvas& canvas) override
	{
		if (n_frames > 0) {
			n_frames--;
			GameObject::draw(canvas);

			static char buffer[10];
			sprintf(buffer, "%2.1f", hp);
			canvas.draw(buffer, getPos() + 1, true);
			return;
		}
		GameObject::draw(canvas);
	}

	// float 값으로 데미지를 주면 된다.
	void onHit(bool isBullet)
	{
		if (hp > 0) {
			if (isBullet == true)
				hp--;
			else
				hp -= 0.5;
			setTimeout(20);
		}
		if (hp <= 0) {
			setVisible(false);
		}
	}

	void update(GameObject* objs[], int max_objs) override
	{
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
// bullet에서 particle을 생성 가능하다. 근데 함수를 걍 뒤로 보내면 됨.
class Bullet : public GameObject {
	int			direction;
	int			particleCount;
	bool		particled;

public:

	Bullet(const char* shape, int pos, bool visible, int direction)
		: GameObject(shape, pos, visible), direction(direction), particleCount(1), particled(false)
	{}

	virtual void fire(GameObject* player, int direction)
	{
		if (player == nullptr) return;
		// player != nullptr
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

	void setDirection(int direction) { this->direction = direction; }

	int getParticleCount() { return particleCount; }
	void setParticleCount() { particleCount--; }

	bool isParticled() { return particled; }
	void setParticled(bool particled) { this->particled = particled; }

	void move()
	{
		if (direction == 0) setPos(getPos() + 1);
		else setPos(getPos() - 1);
	}

	void print() const override { printf("Bullet %p\n", this); }

	void update(GameObject* objs[], int max_objs) override
	{
		if (isVisible() == false) return;

		for (int i = 0; i < max_objs; i++)
		{
			if (objs[i] == nullptr) continue;
			Enemy* enemy = dynamic_cast<Enemy*>(objs[i]);
			if (enemy == nullptr) continue;
			if (enemy->isVisible() == false) continue;
			if (enemy->isColliding(getPos()) == true) {
				enemy->onHit(true);
				/*for (int j = 0; j < max_objs; j++) {
					if (objs[j] == nullptr) {
						ParticleBullet* obj = new ParticleBullet(getPos(), rand() % 2);
						objs[j] = obj;
						if (particleCount <= 0) break;
						particleCount--;
					}
					ParticleBullet* obj = dynamic_cast<ParticleBullet*>(objs[j]);
					if (obj == nullptr) continue;
					if (obj->isVisible() == false) {
						obj->setPos(getPos());
						obj->setDirection(rand() % 2);
						obj->setVisible(true);
						if (particleCount <= 0) break;
						particleCount--;
					}
				}*/
				setVisible(false);
				return;
			}
		}

		move();
	}

	~Bullet()
	{
		direction = 0;
		particleCount = 0;
	}
};

class ParticleBullet : public Bullet {
	int frameCount;
	
public: 
	ParticleBullet(int pos, int direction) : Bullet("*", pos, false, direction), frameCount(10) {}

	//void fire(GameObject* player, int direction) override
	//{
	//	if (player == nullptr) return;
	//	// player != nullptr
	//	int player_pos = player->getPos();
	//
	//	setVisible();
	//	setPos(player_pos + player->getShapeSize());
	//
	//	setDirection(direction);
	//	if (direction == 1)
	//		setPos(player_pos);
	//}

	void update(GameObject* objs[], int max_objs) override
	{
		if (isVisible() == false) return;

		if (frameCount <= 0) {
			setVisible(false);
			return;
		}

		for (int i = 0; i < max_objs; i++)
		{
			if (objs[i] == nullptr) continue;
			Enemy* enemy = dynamic_cast<Enemy*>(objs[i]);
			if (enemy == nullptr) continue;
			if (enemy->isVisible() == false) continue;
			if (enemy->isColliding(getPos()) == true) {
				enemy->onHit(false);
				setVisible(false);
				return;
			}
		}

		move();
		frameCount--;
	}

	~ParticleBullet() 
	{
		frameCount = 0;
	}
};


int main()
{
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
			objs[i] = new BlinkablePlayer("P", rand() % (canvas_size - 5), true);
			break;
		case 1:
			objs[i] = new Enemy("(*-*)", rand() % (canvas_size - 5), true);
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
			int randPos;

			int ch = _getch();

			switch (tolower(ch))
			{
			case 'a':
			case 'd':
			case ' ':
				// find a player closest to a random location.
				randPos = rand() % (canvas_size - 5);
				for (int i = 0; i < max_objs; i++)
				{
					BlinkablePlayer* obj;
					if (objs[i] == nullptr) continue;
					obj = dynamic_cast<BlinkablePlayer*>(objs[i]);
					if (obj == nullptr) continue;
					if (obj->isVisible() == false) continue;
					if (player == nullptr) {
						player = obj;
					}
					else {
						if (abs(obj->getPos() - randPos)
							< abs(player->getPos() - randPos)) {
							player = obj;
						}
					}
				}
				if (player == nullptr) break;
				for (int i = 0; i < max_objs; i++)
				{
					if (objs[i] == nullptr) {
						Bullet* obj = new Bullet(">", 0, false, 0);
						objs[i] = obj;
						found = obj;
						break;
					}
					Bullet* obj = dynamic_cast<Bullet*>(objs[i]);
					if (obj == nullptr) continue;
					if (obj->isVisible() == false) {
						found = obj;
						break;
					}
				}
				if (found == nullptr) break;
				found->fire(player, rand() % 2);
				player->setBlinkingPeriod(20);
				break;
			case 'q':
				exit_flag = true;
				break;
			}
		}

		for (int i = 0; i < max_objs; i++)
		{
			if (objs[i] == nullptr) continue;
			objs[i]->update(objs, max_objs);
		
			//Bullet* obj = dynamic_cast<Bullet*>(objs[i]);
			//if (obj != nullptr) {
			//	if (obj->isVisible() == false && obj->isParticled() == false) {
			//		for (int j = 0; j < max_objs; j++) {
			//			if (objs[j] == nullptr) {
			//				ParticleBullet* o = new ParticleBullet(obj->getPos(), rand() % 2);
			//				objs[j] = o;
			//				if (o->getParticleCount() <= 0) break;
			//				o->setParticleCount();
			//			}
			//			//ParticleBullet* obj = dynamic_cast<ParticleBullet*>(objs[j]);
			//			//if (obj == nullptr) continue;
			//			//if (obj->isVisible() == false) {
			//			//	obj->setPos(obj->getPos());
			//			//	obj->setDirection(rand() % 2);
			//			//	obj->setVisible(true);
			//			//	if (obj->getParticleCount() <= 0) break;
			//			//	obj->setParticleCount();
			//			//}
			//		}
			//	}
			//}
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