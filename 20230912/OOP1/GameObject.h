// 내부적으로 정의되어 있지않으면 정의되게 해준다.
// 예시: 중복되서 인클루드 되는 상황. 맨 처음에 실행되면 이 매크로가 존재하지 않음.
// 근데 두 번째 실행(인클루드)되면 매크로가 존재하게 됨.
// c에서 사용하는 헤더파일 규칙은 이거랑 다르다.
// 비주얼 스튜디오는 #pragma one으로 퉁칠 수 있다. 
#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <iostream>

class Canvas;
// 헤더파일의 목적: 있다는 것만 알려주기.
// 인클루드 해주는 건 소스코드에서 한다.

class GameObject
{
	// 내 클래스에서만 사용되는 메서드는 헤더에 넣어도 괜찮다.
private:

	char* shape;
	int		pos;
	bool	visible;
	bool	alive;

	static void RemoveDeadObjects();

protected:

	static Canvas canvas;
	static int max_objs;
	static GameObject** objs;

public:

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
	void	setVisible(bool visible = true) { this->visible = visible; } // setter

	bool	isAlive() const { return alive; }
	void	setAlive(bool alive = true) {
		this->alive = alive;
		if (alive == false) setVisible(alive);
	}

	virtual void draw();

	void move(int speed) { pos += speed; }

	virtual void update() {}

	bool isColliding(GameObject* other) const {
		if (other == nullptr) return false;
		int myPos = getPos();
		int otherPos = other->getPos();
		return (otherPos >= myPos && otherPos <= myPos + getShapeSize()) ||
			(myPos >= otherPos && myPos <= otherPos + other->getShapeSize());
	}

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

#endif