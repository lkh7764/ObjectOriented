#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib> // include rand()
#include <Windows.h>
#include <conio.h>

using namespace std;


// 최근 c++: #if 지원 안함, 대신 #ifdef는 됨.

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

	// virtual로 돌릴 때는 확장성을 생각해보자.
	virtual void draw(Canvas& canvas);

	void move(int speed) { pos += speed; }

	// bullet의 경우 입력패러미터가 필요함.
	// fire함수를 들여본다 > player, enemy 정보가 꼭 필요할까?
	// player: 플레이어의 위치에서 발사되어야 하기 때문에 필요. 근데 이건 GameObject기능
	// enemy: 방향을 찾기 위해 필요. 그럼 enemy보다는 방향으로 판단하도록 고친다.

	// 배열을 사용할 때는 배열 개수를 세팅해줘야함. 따라서 사이즈정보도 넘겨주기
	// 전역상수는 되도록이면 지양
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

class BlinckablePlayer : public Player {
	int n_frames;

public:
	BlinckablePlayer(const char* shape, int pos, bool visible) : Player(shape, pos, visible), n_frames(0) {}

	void setBlinckingPeriod(int n_frames) { this->n_frames = n_frames; }

	void draw(Canvas& canvas) override {
		if (n_frames > 0) {
			if (n_frames % 2 == 0) {
				n_frames--;
				return;
			}
		}
		GameObject::draw(canvas);
	}
};


// enemy
class Enemy : public GameObject {
	int hp;
	int n_frames;

public:
	Enemy(const char* shape, int pos, bool visible) 
		: GameObject(shape, pos, visible), hp(10), n_frames(0)
	{}

	void print() const override { printf("Enemy %p\n", this); }

	void setTimeout(int n) { n_frames = n; }

	// 오류 발생한 이유 > 부모클래스에서 const였기 때문. 앞으로는 바뀔 거기 때문에 부모클래스에서 const를 빼준다.
	void draw(Canvas& canvas) override {
		if (n_frames > 0) {
			n_frames--;
			// hp를 최상단에서 출력하기 위해 먼저 선언.
			GameObject::draw(canvas);

			// 삭제되지 않는 공간을 만들어서 재사용하며 성능을 고려한다.
			static char buffer[10];
			// printf를 buffer에 집어넣어주는 함수. > sprintf
			sprintf(buffer, "%d", hp);
			canvas.draw(buffer, getPos() + 1, true);
		}
	}

	void onHit() {
		if (hp > 0) {
			hp--;
			setTimeout(20);
		}
		if (hp <= 0) setVisible(false);
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
		return pos >= myPos && pos < myPos + getShapeSize();
	}

	~Enemy() {}
};


// bullet
class Bullet : public GameObject {
	int			direction;
	// 코드 단순화.
	// int			num_remaining_frames_to_fire;

public:
	Bullet(const char* shape, int pos, bool visible, int direction)
		: GameObject(shape, pos, visible), direction(direction)
	{}

	void fire(GameObject* player, int direction)
	{
		if (player == nullptr) return;
		// player != nullptr
		int player_pos = player->getPos();

		setVisible();
		setPos(player_pos + player->getShapeSize());

		this->direction = direction;
		setShape(">");
		if (this->direction == 1)
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

	void print() const override { printf("Bullet %p\n", this); }

	void update(GameObject* objs[], int max_objs)
	{
		/*
		// 프레임 변수ㅡㄹㄹ 삭제해줬기 때문에 필요 없음.
		num_remaining_frames_to_fire--;
			if (num_remaining_frames_to_fire == 0)
			{
				fire(player, enemy);
				return;
			}	*/	
		if (isVisible() == false) return;

		for (int i = 0; i < max_objs; i++) {
			if (objs[i] == nullptr) continue;
			Enemy* e = dynamic_cast<Enemy*>(objs[i]);
			if (e == nullptr) continue;
			if (e->isVisible() == false)continue;
			if (e->isColliding(getPos()) == true) {
				e->onHit();
				setVisible(false);
				return;
			}
		}

		move();
	}

	~Bullet()
	{
		direction = 0;
	}
};


class GO {
public:
	GO() { cout << "GO constructor" << endl; }

	virtual ~GO() { cout << "GO distructor" << endl; }
};

class P : public GO {
public:
	P() :GO() { cout << "P constructor" << endl; }

	~P() { cout << "P distructor" << endl; }
};

int main()
{
// #define EX 0

#ifdef EX
	GO* g = new GO();
	// P생성자 호출 > G생성자 호출 > G생성자 실행 > P생성자 실행
	printf("\n");
	P* p = new P();

	printf("\n");

	delete g;
	// P소멸자 호출 > P소멸자 실행 > G소멸자 호출 > G소멸자 실행
	printf("\n");
	delete p;


	GO* go = new P();
	// 다운캐스팅
	P* p2 = static_cast<P*>(go);
	// 소멸자를 virtual로 만든다면? 이름도 다르고 구성요소가 다름 그래서 불러와서 리셋하고 이런 과정이 들어가게됨.
	// >> 소멸자만 예외상황으로 둘 수 없을까? > 소멸자는 항상 virtual로
	// virtual이란? 자식 클래스에서 부모 클래스에 있는 함수를 업캐스팅하는 것.
	delete p2;
#endif

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
			objs[i] = new BlinckablePlayer("P", rand()%(canvas_size-5), true);
			// 다운캐스팅해주기
			// objs[i]->setBlinckingPeriod(20);
			break;
		case 1:
			objs[i] = new Enemy("(*-*)", rand () % (canvas_size - 5), true);
			break;
		}
	}

	while (true) {

		canvas->clear();

		if (_kbhit()) {
			Bullet* found = nullptr;
			Player* player = nullptr;
			int randPos;

			int ch = _getch();

			switch (tolower(ch)) {
			case 'a':
			case 'd':
			case ' ':
				randPos = rand() % (canvas_size - 5);
				for (int i = 0; i < max_objs; i++) {
					Player* obj;
					if (!objs[i]) continue;
					
					obj = dynamic_cast<Player*>(objs[i]);
					if (!obj) continue;
					if (!player) player = obj;
					else
						if (abs(obj->getPos() - randPos) < abs(player->getPos() - randPos))
							player = obj;
				}
				if (!player) break;
				for (int i = 0; i < max_objs; i++) {
					if (!objs[i]) {
						Bullet* obj = new Bullet(">", 0, false, 0);
						objs[i] = obj;
						found = obj;
						break;
					}

					Bullet* obj = dynamic_cast<Bullet*>(objs[i]);
					if (!obj) continue;
					if (obj->isVisible() == false) {
						found = obj;
						break;
					}
					
					if (!found) break;
					found->fire(player, rand() % 2);

					// 나눠서 기능을 구현해보기. 
				}
			}
		}

		for (int i = 0; i < max_objs; i++) {
			if (objs[i] != nullptr)
				objs[i]->update(objs, max_objs);
		}

		for (int i = 0; i < max_objs; i++) {
			if (objs[i] != nullptr)
				objs[i]->draw(*canvas);
		}

		canvas->render();
		Sleep(100);
	}

#define TEST_GAME 0

#if TEST_GAME
	for (int i = 0; i < max_objs; i++)
	{
		// examine whether current object is a bullet
		Bullet* b = dynamic_cast<Bullet*>(objs[i]);

		// if not, continue
		if (b == nullptr) continue;

		// if so, set a random position.
		int randPos = rand() % canvas_size;

		// find the closest enemy from the random position
		Enemy* closestEnemy = nullptr;
		int closestDistance = canvas_size;
		for (int j = 0; j < max_objs; j++)
		{
			Enemy* e = dynamic_cast<Enemy*>(objs[j]);
			if (e == nullptr) continue;
			// e != nullptr
			int distance = abs(e->getPos() - randPos);
			if (distance < closestDistance)
			{
				closestEnemy = e;
				closestDistance = distance;
			}
		}

		// find the closest player from the random position
		Player* closestPlayer = nullptr;
		closestDistance = canvas_size;
		for (int j = 0; j < max_objs; j++)
		{
			Player* p = dynamic_cast<Player*>(objs[j]);
			if (p == nullptr) continue;
			int distance = abs(p->getPos() - randPos);
			if (distance < closestDistance)
			{
				closestPlayer = p;
				closestDistance = distance;
			}
		}

		// if found the closest enemy and the closest player, fire from the player to the enemy.
		if (closestPlayer != nullptr && closestEnemy != nullptr)
		{
			b->fire(*closestPlayer, *closestEnemy);
		}
	}
#endif

#if TEST_GAME
	Player* player = new Player("(^___^)", 50, true); /* hello world */
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
				player->move(1); break;
			case 'd':
				player->move(0); break;
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

		/*player->draw(*canvas);
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
		}*/

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