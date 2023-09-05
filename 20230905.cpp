// 2023.09.05 객체지향프로그래밍(2)
// 오늘의 목표1: 코드 리뷰
// 오늘의 목표2: 클래스들을 파일로 분리하는 작업하기
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib> // include rand()
#include <Windows.h>
#include <conio.h>

// Canvas라는 자료형의 존재가 있다는 것만 알려주는 용도.
class Canvas;
// extern 키워드를 사용해준다면? > extern class Canvas
// 어딘가에 Canvas라는 클래스가 존재한다는 의미.
// 실행을 해보면 오류는 아니지만 warning처리가 된다. > 해당 코드가 무시됨.
// 왜? 변수를 얘기한 게 아니라 자료형만 알려준 것이기 때문. > extern은 변수를 알려주는 용도?


class GameObject {
private:
	char* shape;
	int		pos;
	bool	visible;
	bool	alive;

	// 멤버 함수와 static 함수의 차이점
	// this 정보가 안 넘어간다는 것의 차이.
	// this란? 객체의 주소 정보. 포인터 값.
	// 단, 접근은 private로 해놨기 때문에 GameObject 클래스의 객체만 접근할 수 있다.
	static void RemoveDeadObjects();


	// protected: 해당 클래스를 상속받은 객체(클래스)가 멤버 변수처럼 사용할 수 있음.
	// 프로퍼티: 변수처럼 쓰기는 하지만 변수는 아님. 필드랑 다름.
	// 필드란? 멤버 변수 멤버 함수 이런 애들.
protected:
	static Canvas canvas;
	static int max_objs;
	static GameObject** objs;


public:
	// C는 기본적으로 값만 전달이 된다. >> call by value만 지원을 함.
	// C++는 레퍼런스 변수를 통해 call by reference(address)도 지원.
	// 원본 정보를 넘겨주는 경우(call by reference)에는 반드시 const처리 여부를 생각해줘야 함.
	GameObject(const char* shape, int pos, bool visible)
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
	// 디폴트 값 설정.
	// 여러 개 일 때는 맨 뒤쪽부터.
	void	setVisible(bool visible = true) { this->visible = visible; } // setter

	bool	isAlive() const { return alive; }
	void	setAlive(bool alive = true) {
		this->alive = alive;
		if (alive == false) setVisible(alive);
	}

	// virtual 함수
	// 외부에서 본문을 정의한 이유는? Canvas의 forward declaration.
	// Canvas에 존재하는 draw함수를 쓸거기 때문에 Canvas를 정의해준 후 draw함수를 정의해준다.
	virtual void draw();

	// move도 오브젝트마다 다르게 움직일 수 있기 때문에 virtual로 바꿔주고 기능을 추가해주는 것도 좋은 방향.
	void move(int speed) { pos += speed; }

	virtual void update() {}

	bool isColliding(GameObject* other) const {
		if (other == nullptr) return false;
		int myPos = getPos();
		int otherPos = other->getPos();
		// 관계 연산자가 비교 연산자보다 우위에 있기 때문에 아래처럼 코드를 작성해도 문제가 발생하지 않음.
		// 근데 난 그냥 괄호 사용할래
		return (otherPos >= myPos && otherPos <= myPos + getShapeSize()) ||
			(myPos >= otherPos && myPos <= otherPos + other->getShapeSize());
	}

	// 가상 소멸자: upcasting. 
	// 자식 클래스에서 부모 클래스에 있는 정보에 대해서도 소멸자를 실행시켜주기 위해 사용한다. 
	virtual ~GameObject() {
		delete[] this->shape;
		this->shape = nullptr;
		this->pos = 0;
		this->visible = false;
		this->alive = false;
	}

	static void Initialize();
	static void Add(GameObject* obj);
	static void Remove(GameObject* obj);
	static void ClearFrame();
	static void ProcessInput(bool& exit_flag);
	static void UpdateAll();
	static void DrawAll();
	static void RenderFrame();
	static void Deinitialize();
	static void ExamineCollision();
};

// 인터페이스 클래스: C#이나 JAVA에서는 기본적으로 제공하는 클래스. 클래스 이름 맨 앞에 I가 붙는다.
// 예시: C#의 List -> 배열과 유사. 
// 배열: Fixed size array 
// Dinamic array -> 사이즈를 유동적으로 변경할 수 있음. (크기에 따라 재할당)
// List는 배열 크기에 상관 없이 넣고 빼고 할 수 있고(빈공간 없이) 서칭도 할 수 있다.
// foreach같이 순회할 수 있는 애들을 Enumerable하다고 한다.
// 인터페이스는 멤버 메서드만 존재한다. 이런 기능을 제공한다를 알려주는 클래스.
// 그래서 순수 가상 함수로만 구성되어있어야 한다.
// 소멸자는 예외.
// C#에서는 소멸자 함수가 없다. 따라서 C#에서는 진짜 순수 가상 함수로만 구성되어 있음.
// 즉, 인터페이스는 순수 가상 함수로만 구성되어있는 클래스.
// 그래서 얘로만은 기능을 실행할 수 없다.
// 그럼 왜 쓸까?
// 표현 기법. 어떤 기능을 제공하겠다라는 것을 명시해놓은 것. > 기능을 묶어서 표현한다. 
// 이걸 상속받아서 각 클래스에 맞게 순수 가상 함수의 기능을 구현한다.
class ICollidable {

public:
	virtual ~ICollidable() {}
	virtual void onCollision(GameObject* collided) = 0; // pure virtual function

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
GameObject** GameObject::objs = new GameObject * [GameObject::max_objs];

void GameObject::draw() { if (alive == true) canvas.draw(shape, pos, visible); }


// player
class Player : public GameObject {

public:

	Player(const char* shape, int pos, bool visible) : GameObject(shape, pos, visible)
	{}

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
		: GameObject(shape, pos, visible), hp(5.0f), n_frames(0)
	{}

	void setTimeout(int n_frames)
	{
		this->n_frames = n_frames;
	}

	void onHit(float damage)
	{
		hp = hp - damage;
		if (hp <= 0.0f) {
			setAlive(false);
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
			// static 지역변수의 활용.
			// 정의되고 나서는 전역변수처럼 메모리상에서 계속 남아있다.
			// 그래서 주의해서 꼭 필요한 경우에만 사용하는 것이 좋음.
			static char buffer[10];
			sprintf(buffer, "%2.1f", hp);
			canvas.draw(buffer, getPos() + 1, true);
		}
		else {
			GameObject::draw();
		}
		// 여기도 else문 안쓰게 바꿔줄수도 있다.
		// 근데 다양한 상황에서 변형될 수 있으니까 무조건 축약하려고만 하지 않아도 된다.
	}

	void onCollision(GameObject* collided) override {};

	// 입력 변수를 받아올 때도 올바른 정보인지 확인해주는 작업도 해주면 좋다.
	// 불필요한 상황에서 빨리 빠져나올 수 있도록 코드를 작성해주는 습관.
	void update() {
		if (isAlive() == false || isVisible() == false) return;

		int prob = rand() % 100;
		if (prob < 80) return;
		if (prob < 90) move(-1);
		else move(1);
	}

	~Enemy() {}
};


// bullet
class Bullet : public GameObject, public ICollidable {
	int			direction;
	float		damagePower;

	static int nCreatedBullets;

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
			// 같은 과정을 두 번 호출하게 되는 경우.
			// 이런 경우에는 if else로 접근하는 것이 더 괜찮지 않을까?
			setPos(player_pos);
			setShape("<");
		}
	}

	float getDamagePower() const { return damagePower; }

	void move()
	{
		if (direction == 0) setPos(getPos() + 1);
		else setPos(getPos() - 1);
		if (getPos() < 0 || getPos() > canvas.getSize()) setAlive(false);
	}

	void onCollision(GameObject* collided) override;


	void update() override
	{
		if (isAlive() == false || isVisible() == false) return;

		move();
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

int Bullet::nCreatedBullets = 0;

// particle bullet
class ParticleBullet : public Bullet {
	int		n_frames_to_disappear;

public:

	ParticleBullet(int pos)
		: Bullet("*", pos, true, rand() % 2, 0.5f), n_frames_to_disappear(10)
	{}

	void onCollision(GameObject* collided) override;

	void update() override
	{
		Bullet::update();

		if (isAlive() == false || isVisible() == false) return;

		if (n_frames_to_disappear > 0) {
			--n_frames_to_disappear;
			if (n_frames_to_disappear <= 0) {
				setAlive(false);
			}
		}
	}

	~ParticleBullet()
	{
		n_frames_to_disappear = 0;
	}
};

void Bullet::onCollision(GameObject* collided)
{
	Enemy* enemy = dynamic_cast<Enemy*>(collided);
	if (enemy == nullptr) return;
	enemy->onHit(getDamagePower());

	GameObject::Add(new ParticleBullet(getPos() + rand() % 5 - 1));
	GameObject::Add(new ParticleBullet(getPos() + rand() % 5 - 1));

	setAlive(false);
}

void ParticleBullet::onCollision(GameObject* collided)
{
	Enemy* enemy = dynamic_cast<Enemy*>(collided);
	if (enemy == nullptr) return;
	enemy->onHit(getDamagePower());
	setAlive(false);
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
			GameObject::Add(new BlinkablePlayer("P", rand() % (canvas.getSize() - 5), true));
			break;
		case 1:
			GameObject::Add(new Enemy("E", rand() % (canvas.getSize() - 5), true));
			break;
		}
	}
}

void GameObject::Add(GameObject* obj)
{
	if (obj == nullptr) return;

	for (int i = 0; i < max_objs; i++)
	{
		if (objs[i] != nullptr) continue;
		objs[i] = obj;
		break;
	}
}

void GameObject::Remove(GameObject* obj)
{
	if (obj == nullptr) return;

	for (int i = 0; i < max_objs; i++)
	{
		if (obj == objs[i]) continue;

		delete objs[i];
		objs[i] = nullptr;
		return;
	}
}

void GameObject::ClearFrame()
{
	canvas.clear();
}
void GameObject::ProcessInput(bool& exit_flag)
{
	Bullet* bullet = nullptr;
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

			printf("[%2d: %s %2d %d %d] ", i, objs[i]->getShape(), objs[i]->getPos(), objs[i]->isVisible(), objs[i]->isAlive());
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
		if (player == nullptr) break;

		bullet = new Bullet(">", 0, false, 0, 1.0f);
		GameObject::Add(bullet);
		bullet->fire(player, rand() % 2);
		player->setBlinkingPeriod(30);
		break;

	case 'q':
		exit_flag = true;
		break;
	}
}

void GameObject::UpdateAll()
{
	for (int i = 0; i < max_objs; i++)
	{
		if (objs[i] == nullptr || objs[i]->isAlive() == false) continue;
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

void GameObject::RenderFrame()
{
	canvas.render();
	Sleep(100);
}

void GameObject::ExamineCollision()
{
	for (int i = 0; i < max_objs; i++)
	{
		if (objs[i] == nullptr) continue;
		if (objs[i]->isAlive() == false) continue;
		ICollidable* c_i = dynamic_cast<ICollidable*>(objs[i]);
		if (c_i == nullptr) continue;
		for (int j = i + 1; j < max_objs && objs[i]->isAlive() == true; j++)
		{
			if (objs[j] == nullptr) continue;
			if (objs[j]->isAlive() == false) continue;
			ICollidable* c_j = dynamic_cast<ICollidable*>(objs[j]);
			if (c_j == nullptr) continue;
			if (objs[i]->isColliding(objs[j]) == false)  continue;
			c_j->onCollision(objs[i]);
			c_i->onCollision(objs[j]);
		}
	}
	RemoveDeadObjects();
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

int main()
{
	GameObject::Initialize();
	bool exit_flag = false;
	while (exit_flag == false)
	{
		GameObject::ClearFrame();

		if (_kbhit()) GameObject::ProcessInput(exit_flag);

		GameObject::ExamineCollision();

		GameObject::UpdateAll();

		GameObject::DrawAll();

		GameObject::RenderFrame();
	}
	GameObject::Deinitialize();

	return 0;
}