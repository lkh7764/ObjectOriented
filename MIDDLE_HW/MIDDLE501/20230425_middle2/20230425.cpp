#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib>
#include <Windows.h>


// problem1
struct Rect {
	float pos1[2];
	float pos2[2];

	Rect(float p1[2], float p2[2]) : pos1{ p1[0], p1[1] }, pos2{ p2[0], p2[1] } {}

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

	~Rect() {
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
	Rect* rect = new Rect(pos1, pos2);

	rect->print();

	delete(rect);
}


// problem2
struct GameObject;
struct Canvas {
	int		size;
	char* frameBuffer;

	Canvas(int size) : size(size), frameBuffer(new char[size + 1]) {
		clear();
	}

	void clear() {
		for (int i = 0; i < size; i++)
			frameBuffer[i] = ' ';
		frameBuffer[size] = '\0';
	}

	int getSize() { return size; }

	void draw(const GameObject& obj);

	void render() const {
		printf("%s\r", frameBuffer);
	}

	~Canvas() {
		size = 0;
		delete[] frameBuffer;
		frameBuffer = nullptr;
	}
};

struct GameObject {
	int		uniqueNum;

	int		scissorsNum;
	int		rocksNum;
	int		papersNum;
	int		thisCount;

	char* name;
	int		pos;
	int		direction;

	GameObject(int uniqueNum, int scissorsNum, int rocksNum, int papersNum, int thisCount, const char* name) :
		uniqueNum(uniqueNum), scissorsNum(scissorsNum), rocksNum(rocksNum), papersNum(papersNum),
		thisCount(thisCount), name(new char[strlen(name) + 1]), pos(0), direction(rand() % 2) {
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

	void setPos(Canvas& canvas, GameObject* scissors, GameObject* rocks, GameObject* papers) {
		int canvasSize = canvas.getSize();
		pos = rand() % canvasSize;
		if (pos + strlen(name) > canvasSize) {
			pos -= strlen(name);
		}
		for (int i = 0; i < scissorsNum; i++) {
			if (comparePos(scissors[i].pos) == true) {
				if (uniqueNum == 0 && thisCount == i)
					continue;
				setPos(canvas, scissors, rocks, papers);
			}
		}
		for (int i = 0; i < rocksNum; i++) {
			if (comparePos(rocks[i].pos) == true) {
				if (uniqueNum == 1 && thisCount == i)
					continue;
				setPos(canvas, scissors, rocks, papers);
			}
		}
		for (int i = 0; i < papersNum; i++) {
			if (comparePos(papers[i].pos) == true) {
				if (uniqueNum == 1 && thisCount == i)
					continue;
				setPos(canvas, scissors, rocks, papers);
			}
		}
	}

	void moveObject(Canvas& canvas) {
		if (direction == 0) pos++;
		else pos--;

		if (pos + 1 >= canvas.getSize() - 1)
			changeDirection();
		else if (pos - 1 <= 0)
			changeDirection();
	}

	void collideWithOther(GameObject* scissors, GameObject* rocks, GameObject* papers) {
		if (uniqueNum == 0) {
			// other이 scissor일 때
			for (int i = 0; i < scissorsNum; i++)
				if (i != thisCount && comparePos(scissors[i].pos) == true) changeDirection();
			// other이 rock일 때
			for (int i = 0; i < rocksNum; i++)
				if (comparePos(rocks[i].pos) == true) delete(this);
		}
		if (uniqueNum == 1) {
			for (int i = 0; i < rocksNum; i++)
				if (i != thisCount && comparePos(rocks[i].pos) == true) changeDirection();
			for (int i = 0; i < papersNum; i++)
				if (comparePos(papers[i].pos) == true) delete(this);
		}
		if (uniqueNum == 2) {
			for (int i = 0; i < papersNum; i++)
				if (i != thisCount && comparePos(papers[i].pos) == true) changeDirection();
			for (int i = 0; i < scissorsNum; i++)
				if (comparePos(scissors[i].pos) == true) delete(this);
		}
	}

	bool comparePos(int pos) {
		return pos >= (this->pos - 1) && pos < (this->pos + 1) + getNameSize();
	}

	void draw(Canvas& canvas) {
		canvas.draw(*this);
	}

	void update(Canvas& canvas, GameObject* scissors, GameObject* rocks, GameObject* papers) {
		moveObject(canvas);
		collideWithOther(scissors, rocks, papers);
	}

	~GameObject() {
		uniqueNum = 0;
		delete[] name;
		name = nullptr;
		pos = 0;
		direction = 0;
	}
};

void Canvas::draw(const GameObject& obj) {
	for (int i = 0; i < strlen(obj.name); i++) {
		if (obj.pos + i < 0) continue;
		if (obj.pos + i > size - 1) continue;

		frameBuffer[obj.pos + i] = obj.name[i];
	}
}

struct Scissors : public GameObject {
	Scissors(int scissorsNum, int rocksNum, int papersNum, int thisCount) :
		GameObject(0, scissorsNum, rocksNum, papersNum, thisCount, "scissors") {}

	~Scissors() {}
};

struct Rock : public GameObject {
	Rock(int scissorsNum, int rocksNum, int papersNum, int thisCount) :
		GameObject(1, scissorsNum, rocksNum, papersNum, thisCount, "rock") {}

	~Rock() {}
};

struct Paper : public GameObject {
	Paper(int scissorsNum, int rocksNum, int papersNum, int thisCount) :
		GameObject(2, scissorsNum, rocksNum, papersNum, thisCount, "paper") {}

	~Paper() {}
};

// problem2 실행
void problem2() {
	Canvas* canvas = new Canvas(80);

	Scissors** scissors;
	Rock** rocks;
	Paper** papers;

	int canvasSize;

	// 0: scissors, 1: rock, 2: paper
	int countNum[3] = { 0, 0, 0 };
	int num;

	// 각 객체가 몇 개 나올지 랜덤 지정
	for (int i = 0; i < 7; i++) {
		num = rand() % 3;
		countNum[num]++;
	}

	// 지정된 수만큼 객체 생성: scissors
	scissors = (Scissors**)malloc(sizeof(Scissors*) * countNum[0]);
	for (int j = 0; j < countNum[0]; j++) 
		scissors[j] = new Scissors(countNum[0], countNum[1], countNum[2], j);
	// 지정된 수만큼 객체 생성: rock
	rocks = (Rock**)malloc(sizeof(Rock*) * countNum[1]);
	for (int j = 0; j < countNum[1]; j++) 
		rocks[j] = new Rock(countNum[0], countNum[1], countNum[2], j);
	// 지정된 수만큼 객체 생성: paper
	papers = (Paper**)malloc(sizeof(Paper*) * countNum[2]);
	for (int j = 0; j < countNum[2]; j++) 
		papers[j] = new Paper(countNum[0], countNum[1], countNum[2], j);

	// setPos
	for (int j = 0; j < countNum[0]; j++) 
		scissors[j]->setPos(*canvas, *scissors, *rocks, *papers);
	for (int j = 0; j < countNum[1]; j++) 
		rocks[j]->setPos(*canvas, *scissors, *rocks, *papers);
	for (int j = 0; j < countNum[2]; j++) 
		papers[j]->setPos(*canvas, *scissors, *rocks, *papers);

	// 작동
	while (true) {
		canvas->clear();

		for (int j = 0; j < countNum[0]; j++)
			scissors[j]->draw(*canvas);
		for (int j = 0; j < countNum[1]; j++)
			rocks[j]->draw(*canvas);
		for (int j = 0; j < countNum[2]; j++)
			papers[j]->draw(*canvas);

		for (int j = 0; j < countNum[0]; j++)
			scissors[j]->update(*canvas, *scissors, *rocks, *papers);
		for (int j = 0; j < countNum[1]; j++)
			rocks[j]->update(*canvas, *scissors, *rocks, *papers);
		for (int j = 0; j < countNum[2]; j++)
			papers[j]->update(*canvas, *scissors, *rocks, *papers);

		canvas->render();
		Sleep(100);
	}

	free(scissors);
	free(rocks);
	free(papers);

	free(canvas);
}

int main() {
	problem2();

	return 0;
}