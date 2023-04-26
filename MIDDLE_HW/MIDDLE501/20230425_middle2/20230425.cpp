#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib>


// problem1
struct Rectangle {
	float pos1[2];
	float pos2[2];

	Rectangle(float p1[2], float p2[2]) : pos1{ p1[0], p1[1] }, pos2{ p2[0], p2[1] } {}

	float width = pos2[0] - pos1[0];
	float height = pos1[1] - pos2[1];
	float area = width * height;

	void print() {
		printf("[pos1] x: %f | y: %f\n", pos1[0], pos1[1]);
		printf("[pos2] x: %f | y: %f\n", pos2[0], pos2[1]);
		printf("width: %f\n", width);
		printf("height: %f\n", height);
		printf("area: %f\n", area);
	}

	~Rectangle() {
		for (int i = 0; i < 2; i++) {
			pos1[i] = 0;
			pos2[i] = 0;
		}
		width = 0;
		height = 0;
		area = 0;
	}
};

// problem1 실행
void problem1() {
	float pos1[2] = { 1.4, 9 };
	float pos2[2] = { 7.23, 4.1 };
	Rectangle* rectangle = new Rectangle(pos1, pos2);

	rectangle->print();

	delete(rectangle);
}


// problem2
struct Canvas;
struct GameObject {
	int		uniqueNum;
	char*	name;
	int		pos;
	int		direction;

	GameObject(int num, const char* name) : uniqueNum(num), name(new char[strlen(name) + 1]), 
		pos(0), direction(rand() % 2) {
		if (name != nullptr)
			strcpy(this->name, name);
	}

	int getUniqueNum() { return uniqueNum; }
	
	const char* getName() { return name; }
	int getNameSize() { return strlen(name); }

	int getPos() { return pos; }
	void setPos(int pos) { this->pos = pos; }

	int getDirection() { return direction; }
	void changeDirection() {
		if (direction == 0)
			direction = 1;
		else
			direction = 0;
	}

	void setPos(Canvas& canvas) {
		int canvasSize = canvas.getSize();
		pos = rand() % canvasSize;
		if (pos + strlen(name) > canvasSize) {
			pos -= strlen(name);
		}
			
	}

	void moveObject(Canvas& canvas) {

	}

	void update() {

	}

	~GameObject() {
		uniqueNum = 0;
		delete[] name;
		name = nullptr;
		pos = 0;
		direction = 0;
	}
};

struct Canvas {
	int		size;
	char*	frameBuffer;

	Canvas(int size) : size(size), frameBuffer(new char[size + 1]) {
		clear();
	}

	void clear() {
		for (int i = 0; i < size; i++)
			frameBuffer[i] = ' ';
		frameBuffer[size] = '\0';
	}

	int getSize() { return size; }

	void draw(const char* name, int pos) {
		for (int i = 0; i < strlen(name); i++) {
			if (pos + i < 0) continue;
			if (pos + i > size - 1) continue;

			frameBuffer[pos + i] = name[i];
		}
	}

	void render() const {
		printf("%s\r", frameBuffer);
	}

	~Canvas() {
		size = 0;
		delete[] frameBuffer;
		frameBuffer = nullptr;
	}
};

// problem2 실행
void problem2() {

}

int main() {
	problem1();

	return 0;
}