#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib> // include rand()
#include <Windows.h>
#include <conio.h>

class Canvas;
class GameObject;

class ICollidable {
public:
	virtual ~ICollidable() {}
	// 본체가 없는 함수를 생성 -> 순수가상함수 pure virtual function
	virtual void onCollision(GameObject* other) = 0;
	// virtual void onCollision(GameObject& other) PURE; >> 비주얼스튜디오에서만 적용된다.
};

class GameObject {
	char*	shape;
	int		pos;
	bool	visible;
	bool	alive;
	// 소유권 문제: 만든 애가 삭제까지 하면 좋은데 bullet과 particle bullet의 경우 애매해짐
	// -> 게임오브젝트 배열에 넣고, 이걸 관리해주는 스태틱함수로 관리를 해주는 방식으로 구현해준다.
	// 말고 다양한 방식으로도 구현할 수 ㅇㅆ음.

protected:

	static Canvas canvas;
	static int max_objs;
	static GameObject** objs;

public:

	GameObject(const char* shape, int pos = 0, bool visible = true)
		: shape(new char[strlen(shape) + 1]), pos(pos), visible(visible), alive(true)
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

	bool	isAlive() const { return alive; }
	bool	isDead() const { return !alive; }
	// 디폴트 매개변수 값이 있는 함수
	// 일반적인 함수를 실행할때는 setAlive(true); setAlive(false); 이런 식으로 매개변수를 꼭 넣어줘야함.
	// 근데 디폴트 값을 넣어주면 setAlive();로 실행하면 디폴트값이 자동으로 넘어감. setAlive() == setAlive(true)
	// 여러 매개변수를 받아야 할 때는 규칙이 따로 존재한다.
	void	setAlive(bool alive = true) { 
		this->alive = alive; 
		if (!alive) setVisible(alive);
	}
	void	destroy(GameObject* obj = nullptr) { 
		if (!obj) this->setAlive();
		else obj->setAlvie(false); 
	}

	virtual void print() const { printf("GameObject %p\n", this); }

	virtual void draw();

	void move(int speed) { pos += speed; }

	virtual void update() {}

	virtual ~GameObject() {
		delete[] this->shape;
		this->shape = nullptr;
		this->pos = 0;
		this->visible = visible;
	}

	static void Initialize();
	static void Add(GameObject* obj);
	static void Deinitialize();
	static void ClearFrame();
	static void RenderFrame();
	static void ExamineCollisions();
	static void UpdateAll();
	static void DrawAll();
	static void ProcessInput(bool& exit_flag);
	static void RemoveDeadObjects();
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

	int getSize() const { return size; }

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


Canvas GameObject::canvas(80);
int GameObject::max_objs = 50;
GameObject** GameObject::objs = new GameObject*[GameObject::max_objs];

void GameObject::draw() { canvas.draw(shape, pos, visible); }


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
	BlinkablePlayer(const char* shape, int pos, bool visible) : Player(shape, pos, visible), n_frames(0)
	{}

	void draw() override {
		if (n_frames > 0) {
			n_frames--;
			if (n_frames % 2 == 0) return;
		}
		GameObject::draw();
	}

	void setBlinkingPeriod(int n_frames)
	{
		this->n_frames = n_frames;
	}

};


// enemy
class Enemy : public GameObject, public ICollidable {

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

	void onHit(float damage)
	{
		hp = hp - damage;
		if (hp <= 0.0f) {
			destroy();
		}
		else {
			setTimeout(10);
		}
	}

	void draw() override
	{
		if (n_frames > 0) {
			n_frames--;
			GameObject::draw();
			static char buffer[10];
			sprintf(buffer, "%2.1f", hp);
			canvas.draw(buffer, getPos() + 1, true);
		}
		else {
			GameObject::draw();
		}
	}

	void update() {
		int prob = rand() % 100;
		if (prob < 80) return;
		if (prob < 90) move(-1);
		else move(1);
	}

	bool isCollision(const GameObject* other) override {};
	~Enemy() {}
};


// bullet
class Bullet : public GameObject, public ICollidable {
	int			direction;
	float		damagePower;

	static int	nCreatedBullets;  // declare

public:

	Bullet(const char* shape, int pos, bool visible, int direction, float damagePower)
		: GameObject(shape, pos, visible), direction(direction), damagePower(damagePower)
	{
		nCreatedBullets++;
	}

	void fire(GameObject* player, int direction)
	{
		if (player == nullptr) return;

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

	void move()
	{
		if (direction == 0) setPos(getPos() + 1);
		else setPos(getPos() - 1);
		if (getPos() < 0 || getPos() + getShapeSize() > canvas.getSize()) destroy();
	}

	virtual void onCollision(GameObject* other) override;

	float getDamagePower() { return damagePower; }

	void update()
	{
		if (isDead()) return;

		move();

		for (int i = 0; i < max_objs; i++)
		{
			if (objs[i] == nullptr) continue;
			Enemy* enemy = dynamic_cast<Enemy*>(objs[i]);
			if (enemy == nullptr) continue;
			if (enemy->isVisible() == false) continue;

			if (enemy->isColliding(getPos()) == true)
			{
				enemy->onHit(damagePower);
				onCollision();
				return;
			}
		}
	}

	~Bullet()
	{
		direction = 0;
	}


	static void printNumberOfCreatedBullets()
	{
		printf("\n# of Created Bullets = %d\n", nCreatedBullets);
	}
};

int Bullet::nCreatedBullets = 0;  // define and initialize

// particle bullet
class ParticleBullet : public Bullet {
	int		n_frames_to_disappear;

public:

	ParticleBullet(int pos)
		: Bullet("*", pos, true, rand() % 2, 0.5f), n_frames_to_disappear(10)
	{}

	void onCollision(GameObject* other) override;

	void update()
	{
		Bullet::update();

		if (isDead()) return;

		if (n_frames_to_disappear > 0) {
			--n_frames_to_disappear;
			if (n_frames_to_disappear <= 0) {
				destroy();
			}
		}
	}

	~ParticleBullet()
	{
		n_frames_to_disappear = 0;
	}
};

void Bullet::onCollision(GameObject* other) {
	Enemy* enemy = dynamic_cast<Enemy*>(other);
	if (!enemy) return;
	enemy->onHit(getDamagePower());

	// particle bullet의 위치를 랜덤하게 설정을 해주면 생성되는 것까지 보임.
	Add(new ParticleBullet(getPos();));
	Add(new ParticleBullet(getPos();));
	//int cnt = 0;
	//for (int i = 0; i < max_objs; i++)
	//{
	//	if (objs[i] != nullptr) continue;
	//	ParticleBullet* bullet = new ParticleBullet(getPos());
	//	objs[i] = bullet;
	//	if (++cnt == 2)
	//		break;
	//}
	destroy();
}

void ParticleBullet::onCollision(GameObject* other) {
	Enemy* enemy = dynamic_cast<Enemy*>(other);
	if (!enemy) return;
	enemy->onHit(getDamagePower());

	destroy();
}

// 문제점: 포인터가 중복이 될 수 있음 > 중복된 오브젝트가 생성될 수 있음. >> 추가 코드 작성 필요.
// 집합의 개념을 사용할 수 있다. 세트 자료형? 중복되면 안 집어넣고 아니면 집어넣고.
void GameObject::Add(GameObject* obj) {
	if (!obj) return;
	for (int i = 0; i < max_objs; i++) {
		if (objs[i]) continue;

		objs[i] = obj;
		return;
	}
}

void GameObject::Initialize()
{
	for (int i = 0; i < max_objs; i++)
		objs[i] = nullptr;

	for (int i = 0; i < 5; i++)
	{
		switch (rand() % 2)
		{
		case 0:
			Add(new BlinkablePlayer("P", rand() % (canvas.getSize() - 5), true));
			// objs[i] = new BlinkablePlayer("P", rand() % (canvas.getSize() - 5), true);
			break;
		case 1:
			Add(new Enemy("E", rand() % (canvas.getSize() - 5), true));
			// objs[i] = new Enemy("E", rand() % (canvas.getSize() - 5), true);
			break;
		}
	}
}

void GameObject::Deinitialize()
{
	for (int i = 0; i < max_objs; i++)
	{
		if (objs[i] == nullptr) continue;
		delete objs[i];
		objs[i] = nullptr;
	}
	delete[] objs;
}
void GameObject::ClearFrame()
{
	canvas.clear();
}
void GameObject::RenderFrame()
{
	canvas.render();
	Sleep(100);
}

void ExamineCollisions() {
	for (int i = 0; i < max_objs; i++) {
		GameObject* o_i = objs[i];
		if (!o_i) continue;
		if (o_i->isDead()) continue;
		ICollidable* c_i = dynamic_cast<ICollidable*>(o_i);
		if (!c_i) continue;

		for (int j = i + 1; j < max_objs; j++) {
			GameObject* o_j = objs[j];
			if (!o_j) continue;
			if (o_j->isDead()) continue;
			ICollidable* c_j = dynamic_cast<ICollidable*>(o_j);
			if (!c_j) continue;

			if (o_j->isColliding(*o_i)==true) {
				c_j->onCollision(o_i);
				// 위 과정에서 o_i가 죽는다면? 다음 루프를 돌 필요가 없음. 따라서 o_i->isAlive() 조건을 추가
				c_i->onCollision(o_j);
			}
		}
	}
}

void GameObject::UpdateAll()
{
	for (int i = 0; i < max_objs; i++)
	{
		if (objs[i] == nullptr) continue;
		objs[i]->update();
	}
	RemoveDeadObjects();
}

void GameObject::DrawAll()
{
	for (int i = 0; i < max_objs; i++)
	{
		if (objs[i] == nullptr) continue;
		objs[i]->draw();
	}
}

void GameObject::ProcessInput(bool& exit_flag)
{
	Bullet* found = nullptr;
	BlinkablePlayer* player = nullptr;
	int ch = _getch();
	int randPos;

	switch (tolower(ch))
	{
	case 'i':
		printf("\n");
		for (int i = 0; i < max_objs; i++)
		{
			if (objs[i] == nullptr) continue;

			printf("[%2d: %s %2d %d] ", i, objs[i]->getShape(), objs[i]->getPos(), objs[i]->isVisible());
		}
		Bullet::printNumberOfCreatedBullets();
		break;

	case ' ':
		// find player
		randPos = rand() % (canvas.getSize() - 5);
		for (int i = 0; i < max_objs; i++)
		{
			if (objs[i] == nullptr) continue;
			BlinkablePlayer* obj = dynamic_cast<BlinkablePlayer*>(objs[i]);
			if (obj == nullptr) continue;
			if (obj->isVisible() == false) continue;
			if (player == nullptr) {
				player = obj;
				continue;
			}
			if (abs(obj->getPos() - randPos) < abs(player->getPos() - randPos)) {
				player = obj;
			}
		}

		if (player == nullptr) return;


		for (int i = 0; i < max_objs; i++)
		{
			if (objs[i] == nullptr) {
				Bullet* bullet = new Bullet(">", 0, false, 0, 1.0f);
				objs[i] = bullet;
				found = bullet;
				break;
			}
		}
		if (found == nullptr) break;
		found->fire(player, rand() % 2);
		player->setBlinkingPeriod(30);
		break;

	case 'q':
		exit_flag = true;
		break;
	}
}

void GameObject::RemoveDeadObjects()
{
	for (int i = 0; i < max_objs; i++)
	{
		if (objs[i] == nullptr) continue;
		if (objs[i]->isAlive() == true) continue;
		delete objs[i];
		objs[i] = nullptr;
	}
}

int main()
{
	GameObject::Initialize();

	bool exit_flag = false;
	while (exit_flag == false)
	{
		GameObject::ClearFrame();

		if (_kbhit()) GameObject::ProcessInput(exit_flag);

		GameObject::ExamineCollisions();

		GameObject::UpdateAll();		

		GameObject::DrawAll();		

		GameObject::RenderFrame();
	}
	GameObject::Deinitialize();

	return 0;
}