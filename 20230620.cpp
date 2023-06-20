#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib>
#include <Windows.h>
#include <conio.h>


class Canvas;
class GameObject {
	char*	shape;
	int		pos;
	bool	visible;
	bool	alive;

	static void RemoveDeadObjects();


protected:
	// GameObject���� ������ ��� GameObject�� ����(destroy������)
	static Canvas	canvas;
	static int			max_objs;
	static GameObject** objs;


public:
	GameObject(const char* shape, int pos, bool visible)
		: shape(new char[strlen(shape) + 1]), pos(pos), visible(visible), alive(true) 
	{
		if (this->shape)
			strcpy(this->shape, shape);
	}

	// �Լ� ���� �ڿ� ���� const: �ش� �Լ��� ���� class�� ��������� �ٲ��� �ʰڴٴ� �ǹ�
	// -> ��� ������ �ٲٴ�(const�� ���� ����) �Լ��� ȣ�⵵ ����.
	int		getPos() const { return pos; }
	void		setPos(int pos) { this->pos = pos; }

	// ��ȯ���� const, �� ���.
	const char*	getShape() const { return shape; }
	void		setShape(const char* shape) { strcpy(this->shape, shape); }
	int		getShapeSize() const { return (int)strlen(this->shape); }

	bool	isVisible() const { return visible; }
	void		setVisible(bool visible = true) { this->visible = visible; }

	bool	isAlive() const { return alive; }
	void		setAlive(bool alive = true) { 
		this->alive = alive; 
		if (alive == false) setVisible(alive);
	}

	virtual void	draw();

	void		move(int speed) { pos += speed; }

	virtual void	update() {}

	// ��� �ܼ��� �ε����� �� �ε����ĸ� ��ȯ�ϴ� �Լ�.
	bool	isColliding(GameObject* other) const {
		if (!other) return false;
		int otherPos = other->getPos();
		return (otherPos>=pos&&otherPos<=pos+getShapeSize())||
			(pos >= otherPos && pos <= otherPos + other->getShapeSize());
	}

	// virtual�� ����ϴ� ����? GameObject a = new Player �̷� ������ ��ü�� �����
	// �̰� �����ϴ� ���������� Player�� �Ҹ��ڰ� ������� �ʴ´�.
	// �ٵ� virtual�� �ٿ��ָ� Player�� �Ҹ��ڱ����� ����ȴ�.
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


class ICollidable {
public:
	// pure virtual function -> ��ü�� ���� �Լ�.
	virtual void onCollision(GameObject* collided) = 0;
	virtual ~ICollidable() {}
};


class Canvas {
	char*	frameBuffer;
	int		size;


public:
	// ���� ������ ���� �ʵ��� �̿��� ����
	Canvas(int size) : frameBuffer(new char[(size_t)size + 1]), size(size) {
		clear();
	}

	void clear() {
		for (int i = 0; i < size; i++)
			frameBuffer[i] = ' ';
		frameBuffer[size] = '\0';
	}

	int getSize() const { return size; }

	void draw(const char* shape, int pos, bool visible) {
		if (visible == false) return;
		for (int i = 0; i < strlen(shape); i++) {
			if (pos + i < 0) continue;
			if (pos + i > size - 1) break;
			frameBuffer[pos + i] = shape[i];
		}
	}

	void render() const {
		printf("%s\r", frameBuffer);
	}

	~Canvas() {
		delete[] frameBuffer;
		frameBuffer = nullptr;
		size = 0;
	}
};


Canvas	GameObject::canvas(80);
int			GameObject::max_objs = 50;
GameObject**	GameObject::objs = new GameObject * [GameObject::max_objs];

void GameObject:: draw() {
	if (alive == true)
		canvas.draw(shape, pos, visible);
}


class Player : public GameObject {
public:
	Player(const char* shape, int pos, bool visible) : GameObject(shape, pos, visible) {}
	~Player() {}
};


class BlinkablePlayer : public Player {
	int n_frames;

	
public:
	BlinkablePlayer(const char* shape, int pos, bool visible) 
		: Player(shape, pos, visible), n_frames(0) 
	{}

	void setBlinkingPeriod(int n_frames) {
		this->n_frames = n_frames;
	}

	void draw() override {
		if (n_frames > 0) {
			n_frames--;
			if (n_frames % 2 == 0) return;
		}
		// ����ߴµ�? �ٽ� �ҷ����� �� �����Ѱ�?
		GameObject::draw();
	}

	~BlinkablePlayer() {
		n_frames = 0;
	}
};


class Enemy : public GameObject, public ICollidable {
	float	hp;
	int		n_frames;


public:
	Enemy(const char* shape, int pos, bool visible)
		: GameObject(shape, pos, visible), hp(5.0f), n_frames(0)
	{}

	void setTimeout(int n_frames) {
		this->n_frames = n_frames;
	}

	void onHit(float damage) {
		hp = hp - damage;
		if (hp <= 0.0f)
			setAlive(false);
		else
			setTimeout(10);
	}
	
	// ����? -> ExamineCollision���� ���� ���� �۵��ϱ� ���� �ۼ���.
	// �� �´� ICollidable�� ���� �Լ�������
	void onCollision(GameObject* collided) override {};

	void draw() override {
		if (n_frames > 0) {
			n_frames--;
			GameObject::draw();
			static char buffer[10];
			// sprintf -> 1P: �Է��� ��ġ / 2P: �Է��� ��(������ ���ڸ�����/�Ҽ��� ���ڸ�����)
			// 3P: ���������ڿ� �´� ����
			sprintf(buffer, "%2.1f", hp);
			canvas.draw(buffer, getPos() + 1, true);
		}
		else
			GameObject::draw();
	}

	// ��� �� �������̵带 ���ϴ°�?
	void update() {
		if (isAlive() == false || isVisible() == false) return;

		int prob = rand() % 100;
		if (prob < 60) return;
		if (prob < 80) move(-1);
		else move(1);
	}

	~Enemy() {
		// ���� �ڵ忡���� �� �ȿ� �ʱ�ȭ �۾��� ����. ����? �׳� ������ �ǰ�?
		hp = 0.0f;
		n_frames = 0;
	}
};


class Bullet : public GameObject, public ICollidable {
	int		direction;
	float	damagePower;

	static int nCreatedBullets;


public:
	Bullet(const char* shape, int pos, bool visible, int direction, float damagePower)
		: GameObject(shape, pos, visible), direction(direction), damagePower(damagePower)
	{
		nCreatedBullets++;
	}

	// fire�� �� �ʿ��Ѱ�?�� ���� ����
	void fire(GameObject* player, int direction) {
		if (!player) return;

		int playerPos = player->getPos();

		setVisible();
		setPos(playerPos + player->getShapeSize());

		this->direction = direction;
		setShape(">");
		if (direction == 1) {
			setPos(playerPos);
			setShape("<");
		}
	}

	float getDamagePower() const { return damagePower; }

	void move() {
		if (direction == 0) setPos(getPos() + 1);
		else setPos(getPos() - 1);
		if (getPos() < 0 || getPos() > canvas.getSize()) setAlive(false);
	}

	void onCollision(GameObject* collided) override;

	void update() override {
		if (isAlive() == false || isVisible() == false) return;
		move();
	}

	~Bullet() {
		direction = 0;
		damagePower = 0.0f;
	}

	static void printNumberOfCreatedBullets() {
		printf("\n# of Created Bullets = %d\n", nCreatedBullets);
	}
};


int Bullet::nCreatedBullets = 0;


class ParticleBullet : public Bullet {
	int		n_frames_to_disappear;


public:
	ParticleBullet(int pos)
		: Bullet("*", pos, true, rand() % 2, 0.5f), n_frames_to_disappear(10)
	{}

	void onCollision(GameObject* collided) override;

	void update() override {
		Bullet::update();

		if (isAlive() == false || isVisible() == false) return;

		if (n_frames_to_disappear > 0) {
			--n_frames_to_disappear;
			if (n_frames_to_disappear <= 0)
				setAlive(false);
		}
	}

	~ParticleBullet() {
		n_frames_to_disappear = 0;
	}
};


void Bullet::onCollision(GameObject* collided) {
	Enemy* enemy = dynamic_cast<Enemy*>(collided);
	if (!enemy) return;
	enemy->onHit(getDamagePower());

	GameObject::Add(new ParticleBullet(getPos() + rand() % 5 - 1));
	GameObject::Add(new ParticleBullet(getPos() + rand() % 5 - 1));

	setAlive(false);
}


void ParticleBullet::onCollision(GameObject* collided) {
	Enemy* enemy = dynamic_cast<Enemy*>(collided);
	if (!enemy) return;
	enemy->onHit(getDamagePower());

	setAlive(false);
}


void GameObject::Initialize() {
	for (int i = 0; i < max_objs; i++)
		objs[i] = nullptr;

	for (int i = 0; i < 4; i++) {
		switch (rand() % 2) {
		case 0:
			GameObject::Add(new BlinkablePlayer("P", rand() % (canvas.getSize() - 5), true));
			break;
		case 1:
			GameObject::Add(new Enemy("E", rand() % (canvas.getSize() - 5), true));
			break;
		}
	}
}

void GameObject::Add(GameObject* obj) {
	if (!obj) return;
	for (int i = 0; i < max_objs; i++) {
		if (objs[i]) continue;
		objs[i] = obj;
		break;
	}
}

void GameObject::Remove(GameObject* obj) {
	if (!obj) return;
	for (int i = 0; i < max_objs; i++) {
		if (obj == objs[i]) continue;
		delete objs[i];
		objs[i] = nullptr;
		return;
	}
}

void GameObject::ClearFrame() {
	canvas.clear();
}

void GameObject::ProcessInput(bool& exit_flag) {
	Bullet* bullet = nullptr;
	BlinkablePlayer* player = nullptr;
	int ch = _getch();
	int randPos;

	switch (tolower(ch)) {
	case 'i':
		printf("\n");
		for (int i = 0; i < max_objs; i++) {
			if (!objs[i]) continue;
			printf("%2d: %s %2d %d %d",
				i, objs[i]->getShape(), objs[i]->getPos(), objs[i]->isVisible(), objs[i]->isAlive());
		}
		Bullet::printNumberOfCreatedBullets();
		break;

	case' ':
		randPos = rand() % (canvas.getSize() - 5);
		for (int i = 0; i < max_objs; i++) {
			if (!objs[i]) continue;
			BlinkablePlayer* obj = dynamic_cast<BlinkablePlayer*>(objs[i]);
			if (!obj) continue;
			if (obj->isVisible() == false) continue;
			if (!player) {
				player = obj;
				continue;
			}
			if (abs(obj->getPos() - randPos) < abs(player->getPos() - randPos))
				player = obj;
		}
		if (!player) break;

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

void GameObject::UpdateAll() {
	for (int i = 0; i < max_objs; i++) {
		if (!objs[i] || objs[i]->isAlive() == false) continue;
		objs[i]->update();
	}
	RemoveDeadObjects();
}

void GameObject::DrawAll() {
	for (int i = 0; i < max_objs; i++) {
		if (!objs[i]) continue;
		objs[i]->draw();
	}
}

void GameObject::RenderFrame() {
	canvas.render();
	Sleep(100);
}

void GameObject::ExamineCollision() {
	for (int i = 0; i < max_objs; i++) {
		if (!objs[i]) continue;
		if (objs[i]->isAlive() == false) continue;
		ICollidable* c_i = dynamic_cast<ICollidable*>(objs[i]);
		if (!c_i) continue;
		for (int j = i + 1; j < max_objs && objs[i]->isAlive() == true; j++) {
			if (!objs[j]) continue;
			if (objs[j]->isAlive() == false) continue;
			ICollidable* c_j = dynamic_cast<ICollidable*>(objs[j]);
			if (!c_j) continue;
			if (objs[i]->isColliding(objs[j]) == false) continue;
			c_j->onCollision(objs[i]);
			c_i->onCollision(objs[j]);
		}
	}
	RemoveDeadObjects();
}

void GameObject::RemoveDeadObjects() {
	for (int i = 0; i < max_objs; i++) {
		if (!objs[i]) continue;
		if (objs[i]->isAlive() == true) continue;
		delete objs[i];
		objs[i] = nullptr;
	}
}

void GameObject::Deinitialize() {
	for (int i = 0; i < max_objs; i++) {
		if (!objs[i]) continue;
		delete objs[i];
		objs[i] = nullptr;
	}
	delete[] objs;
}


int main() {
	GameObject::Initialize();
	bool exit_flag = false;
	while (exit_flag == false) {
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