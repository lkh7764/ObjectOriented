// ���������� ���ǵǾ� ���������� ���ǵǰ� ���ش�.
// ����: �ߺ��Ǽ� ��Ŭ��� �Ǵ� ��Ȳ. �� ó���� ����Ǹ� �� ��ũ�ΰ� �������� ����.
// �ٵ� �� ��° ����(��Ŭ���)�Ǹ� ��ũ�ΰ� �����ϰ� ��.
// c���� ����ϴ� ������� ��Ģ�� �̰Ŷ� �ٸ���.
// ���־� ��Ʃ����� #pragma one���� ��ĥ �� �ִ�. 
#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <iostream>

class Canvas;
// ��������� ����: �ִٴ� �͸� �˷��ֱ�.
// ��Ŭ��� ���ִ� �� �ҽ��ڵ忡�� �Ѵ�.

class GameObject
{
	// �� Ŭ���������� ���Ǵ� �޼���� ����� �־ ������.
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