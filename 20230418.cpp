#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib>
#include <Windows.h>


// malloc으로 10byte를 동적할당 하라고 하면 14byte 크기의 공간을 만든다.
// 그래서 맨 앞에 얼만큼의 정보가 들어갈지에 대한 int 정보를 집어넣는다.
// 그래서 free 할때 몇 byte만큼의 정보가 들어가는지 알 수 있어서
// free 할 때 공간의 크기를 알려주지 않고도 주소정보만으로도 free가 가능하다.

// C++에서는 malloc 대신에
// char* p = new char; 을 사용한다.

// malloc과 new의 차이점?
// malloc은 공간만 생성한다.
// new는 생성자함수까지 실행한다. new: malloc + 생성자함수 호출.
// new를 실행하고나서는 free 대신 delete를 사용해준다.
// Player* players = new Player[2]{ Player(...), Player(...)}; 이렇게 하면 공간생성은 1번, 생성자함수는 2번 호출된다.
// delete players; 이렇게 하면 소멸자 함수는 1번 실행된다. 따라서 배열에 대해서 여러번 소멸자 함수를 실행해줘야 한다.
// 따라서 배열인 경우에는 일반적인 delete 말고
// delete[] players; 로 작성해준다. delete[]의 의미? 배열에 대해서 삭제해준다.

// GameObject 먼저 정의하고 Canvas 정의 -> Canvas 선언해주기
struct Canvas;
struct GameObject {
	// public&private 써줄 때 변수는 최대한 private로 해주고 함수는 public으로 해준다.
	// 변수 정보에 대한 접근은 함수로 접근하도록 설계해준다.
	char* shape;
	int		pos;
	bool	visible;

	// 멤버 초기화 리스트 사용하기: https://blockdmask.tistory.com/510
	GameObject(const char* shape, int pos, bool visible) 
		: shape( new char[strlen(shape) + 1]), pos(pos), visible(visible)
	{
		if (this->shape != nullptr)
			strcpy(this->shape, shape);
	}

	// pos 와 visible 정보는 gameObject가 가지고 있기 때문에
	// 해당 정보를 가지고 있는 GameObject에서 기능을 구현해주는 것이 좋다.

	int		getPos() const { return pos; } // getter
	void	setPos(int pos) { this->pos = pos; } // setter

	// 주소 정보를 넘겨줌 -> const를 붙이지 않으면 고쳐도 되는 정보로 오인할 수 있음.
	// 그래서 다른 데서 사용하다가 동적 생성해서 원래 정보가 날라갈 수 있음.
	// 그래서 고칠 수 없도록 const를 붙여줌.
	const char* getShape() const { return this->shape;  } // getter
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


// Canvas가 GameObject 아래에 정의되어있기 때문에 GameObject 정의할 때 안넣고 밖에다가 빼줌.
// 범위 지정 연산자 '::': https://kihyun3297.tistory.com/140 
void GameObject::draw(Canvas& canvas) const { canvas.draw(shape, pos, visible); }


// player
struct Player {
	GameObject gameObject;

	Player(const char* shape, int pos, bool visible) : gameObject(shape, pos, visible)
	{}

	// 똑같은 코드들이 반복적으로 나타남 > 상속을 통해 줄여줄 수 있다.
	void draw(Canvas& canvas) const 
	{ 
		gameObject.draw(canvas);
	}

	int getPos() const { return gameObject.getPos(); }

	int getShapeSize() const { return gameObject.getShapeSize(); }

	void update() { gameObject.update(); }

	~Player() 
	{}
};


// enemy
struct Enemy : public GameObject {
	// C++에서 상속의 의미? 코드의 재사용. (> 여기서 문제점이 발생한다.)
	// GameObject에 존재하는 기능들을 Player와 Enemy에서 재사용 할 수 있다.
	// 다중 상속: 부모가 여러개. C++는 다중 상속 지원. 
	// 복합기는 프린터일수도, 스캐너일수도, 팩스일수도 있는 느낌.
	// 다중 상속 모델의 단점: 상속되고 상속되고 상속되고가 반복되면 어느 정보가 어디에 있는지 논리적 모순이 발생할 수 있음.
	// 진짜 부모(주 기능)와 가짜 부모(부가 기능, 인터페이스(C#))을 나누는 것이 필요하다.
	// 그래서 JAVA 나 C#에서는 단일 부모 모델을 사용한다.
	// JAVA: class 복합기 extends 복사기, implements 스캐너, 팩시밀리 {}
	// C++: class 복합기 : public 복사기, public 스캐너, public 팩시밀리 {};
	// C#: class 복합기 : 복사기, I스캐너, I팩시밀리 {}
	// C#의 경우 단일 부모지만 C++의 형태와 매우 비슷, 부가 기능 앞에는 인터페이스의 약자인 I를 앞에 붙인다.
	// Parent: Base, SuperClass
	// Child: Derived, SubClass
	
	// 결론: 다중 상속을 지원하기는 하지만 되도록이면 단일로 만들자. 
	Enemy(const char* shape, int pos, bool visible) : GameObject(shape, pos, visible)
	{}
	// C#에서는 부모클래스 생성자 함수를 호출할 때 : base(shape, pos, visible)로 사용한다 > base라는 keyword를 사용.
	// JAVA는 멤버 초기화 리스트 사용하지 않고 함수 몸체에다가 super(shape, pos, visible)로 사용한다 > super라는 keyword를 사용.

	bool isColliding(int pos) const 
	{ 
		int myPos = getPos();
		return pos >= myPos && pos < myPos + getShapeSize();	
	}

	~Enemy() {}
};


// bullet
struct Bullet  {
	// 가지고 있다 > hasA. 
	// 근데 bullet, player, enemy가 gameObject에 포함되는 것 아닐까? > ~의 일종이다: isAKindOf > 상속
	// 상속이란, 부모가 가진 기능을 모두 받는다. 
	// 근데 정말 모든 걸 제공하고 싶을까? 해서 나온게 public과 private > class
	GameObject  gameObject;
	int			direction; // 0: L2R, 1: R2L
	int			num_remaining_frames_to_fire;

	// 상속으로 바꾸면 생성자 함수를 사용하여 오브젝트를 만들어줘야함 > 멤버 초기화 리스트 꼭 사용.
	Bullet(const char* shape, int pos, bool visible, int direction, int n_frames)
		: gameObject(shape, pos, visible), direction(direction), num_remaining_frames_to_fire(n_frames)
	{}

	// 상속 이후 삭제해도 됨.
	void draw(Canvas& canvas) const { gameObject.draw(canvas);	}

	void fire(const Player& player, const Enemy& enemy)
	{
		int enemy_pos = enemy.getPos();
		int player_pos = player.getPos();
		if (enemy.isColliding(player_pos)) return;

		// 상속 이후 gameObject 삭제 가능.
		gameObject.setVisible();

		// 원본 정보는 최대한 숨기고, 직접 정보에 접근하는 것이 아니라 호출해서 요청하는 형태로.
		// 내가 가지고 있지 않은 정보는 최대한 건들지 말자. 호출해서 요청하는 형태로 변형.
		gameObject.setPos(player_pos + player.getShapeSize());
		direction = 0;
		gameObject.setShape(">");
		if (enemy_pos < player_pos)
		{
			gameObject.setPos(player_pos);
			direction = 1;
			gameObject.setShape("<");
		}
	}

	void move() 
	{
		if (direction == 0) gameObject.setPos(gameObject.getPos()+1);
		else gameObject.setPos(gameObject.getPos()-1);
	}

	void update(const Player& player, const Enemy& enemy)
	{
		num_remaining_frames_to_fire--;
		if (num_remaining_frames_to_fire == 0)
		{
			fire(player, enemy);
			return;
		}
		if (gameObject.isVisible() == false) return;

		move();

		// check whether bullet collides with enemy
		if (enemy.isColliding(gameObject.getPos()))
			gameObject.setVisible(false);
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
	Enemy* enemy = new Enemy("(*-*)", 10, true);
	Bullet* bullet = new Bullet(">", 10, false, 0, 30);
	Bullet* bullet2 = new Bullet(">", 10, false, 0, 40);

	while (true)
	{
		canvas->clear();

		player->draw(*canvas);
		enemy->draw(*canvas);
		bullet->draw(*canvas); bullet2->draw(*canvas);

		player->update();
		enemy->update();
		bullet->update(*player, *enemy); bullet2->update(*player, *enemy);

		canvas->render();
		Sleep(100);
	}

	delete bullet2;
	delete bullet;
	delete enemy;
	delete player;
	delete canvas;
	
	return 0;
}

//여기까지 시험범위
