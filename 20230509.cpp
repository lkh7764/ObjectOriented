#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib> // include rand()
#include <Windows.h>
#include <conio.h>
#include <time.h>


// 지금까지 했던 거> 구조체 만들고 멤버변수&함수만들기
// 멤버함수는 메모리 상에서 어떻게 들어가 있을까?
// 전역함수를 호출해주는 방식. 컴파일러가 주소정보랑 이것저것 다 처리해줌.
// 구조체?는 각각의 테이블을 가지고 있다.
// 일단 각각의 구조체에 print()를 만들어준다.
// 만약 Bullet에 print()를 없애고 Bullet의 print()를 실행하면?
// 컴파일러가 작업을 한 번 더 함 > 부모 구조체의 테이블을 확인해서 print있으면 실행 > 코드 재사용

// 코드 짤 때 먼저 코드 작성하지 말고 무슨 기능을 어떻게 구현할지 부터 생각해보자.
// 복잡하고 다양한 상황에서 객체지향 개념을 적용해서 풀어보자.
// 이렇게 해결하는 게 익숙해지는 단계에서 성능적인 면을 고려해보기.
// 변수를 안 만들고도 문제(객체의 사망 여부 판단)를 해결할 수 있는 방법?

// 개인적으로 draw&render해보기

// 요약
// upcasting & downcasting: downcasting은 맘대로 할 수 없다.
// dynamic_cast: upcasting된 정보를 원본정보로 돌려주기. 3번 상속된 거도 해볼까? *상속관계여야 한다는 거에 유의

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

	// virtual: 부모 클래스에서만 선언 가능.
	// virtual 선언하면 자식 클래스에서도 조작 가능 > 연결되어있다.
	// 어떻게? virtual 선언하면 v table이 따로 만들어진다. 여기에 print()가 등록됨.
	// 이렇게 되면 자식 클래스에서 정의한 print()함수가 서로 알아봄.
	// 덮어씌운다 > override
	// 이렇게 하면 선언할 때마다 덮어씌워지니까 상관없나?
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


	// 이 로직으로 하면 rock scissors paper 쉽다..
	// JAVA에서는 다 virtual로 생각한다. 
	for (int i = 0; i < max_obj; i++) {
		switch (rand() % 3) {
		case 0:
			objs[i] = new Player("(^___^)", 50, true);
			break;
		case 1:
			objs[i] = new Enemy("(*-*)", 10, true);
			break;
		case 2:
			// 이렇게 하면 fire()는 할 수 없다.
			objs[i] = new Bullet(">", 0, false, 0, 0);
			break;
		}
	}

	for (int i = 0; i < max_obj; i++) {
		// dynamic_cast: objs[i]가 bullet이면 b, 아니면 nullptr반환.
		// 편한걸로 typeID가 있는데 아직 안다룸.
		Bullet* b = dynamic_cast<Bullet*>(objs[i]);
		if (b != nullptr) printf("%d obj is a bullet\n", i);
		Player* p = dynamic_cast<Player*>(objs[i]);
		if (p != nullptr) printf("%d obj is a player\n", i);
		Enemy* e = dynamic_cast<Enemy*>(objs[i]);
		if (e != nullptr) printf("%d obj is an enemy\n", i);

		objs[i]->print();
		printf("\n");
	}

	// Bullet만 찾아서 randPosition을 넣고, 제일 가까운 player나 enemy에게 총알을 쏘는 코드.
	for (int i = 0; i < max_obj; i++) {
		// examine whether current object is a bullet.
		Bullet* b = dynamic_cast<Bullet*>(objs[i]);

		// if not, continue.
		if (b == nullptr) continue;

		// if so, set a random position.
		int randPos = rand() % canvasSize;

		// find the closest enemy from the random position. > 이거를 하려면 얘네도 위치정보가 있어야하는거 아닌가?
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
		} // Enemy가 없으면 설정되지 않음.

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
		} // closestEnemy를 찾는 코드와 너무 똑같다. 이걸 합칠 수 있는 방법은?

		// if found the closest enemy and the closest player, fire from the player to the enemy.
		if (closestPlayer != nullptr && closestEnemy != nullptr) {
			// visible 값을 고려해주도록 업그레이드 할 수 있다.
			// 죽은 obj들을 배열에 남겨둘 이유는? 없앨 수도 있지 않을까?
			// 만약 내가 쏜 총알에 p&e가 맞기 전에, 타겟팅한 p&e가 죽었다면? 
			// > update()에 p&e를 뒤져보면서 충돌될 만한 p&e가 있는지 확인하는 식으로 업그레이드 가능.
			b->fire(*closestPlayer, *closestEnemy);
		}
	}


#if 0
	Bullet* bullet = new Bullet(">", 0, false, 0, 0);
	bullet->print();
	// Bullet에 있는 print()를 주석처리하고 실행하면 출력되는 this값이 달라짐
	// int를 임의적으로 float로 바꾸고 출력했을 때와 같은 원리.

	// upcasting
	GameObject* obj = bullet;
	obj->print();

	// 이건 명시적 형변환이 필수로 필요.
	Enemy* enemy = (Enemy*)bullet;
	enemy->print();

	Enemy* e = (Enemy*)obj;
	e->print();

	// downcasting은 명시적 형변환이 필요. > 컴파일러가 자신할 수 없기 때문에 우리 손을 거치는 거.
	Bullet* another = (Bullet*)obj;

	// 요건 주소정보가 다른 객체를 하나 만들어 준거. upcasting 아님 주의.
	// upcasting과 downcasting은 포인터변수로만 사용한다.
	// 레퍼런스도 되긴 하는데 내 맘대로 없애고 만들 수 없기 때문에 포인터를 사용하는 편이 낫다.
	GameObject another = *bullet;


	// visual studio에서는 주석문 안에 주석문을 넣을 수 없다 (/**/ 요거)
	// 왜? 괄호처럼 인식을 하는 게 아니라 그냥 끝맺음으로 알아들음.
	// 주석문 대신 사용할 수 있는 것 > condition, 조건부 컴파일.
#endif

#define TEST_GAME 0

	//ifdef TEST_GAME > TEST_GAME이 정의되어 있다면 실행. 이거 쓰고 실행하기 싫으면 #define 주석 처리.
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