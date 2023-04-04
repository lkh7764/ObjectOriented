#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <Windows.h>


// 1. 구조체 생성
// 2. main에서 함수에 넘겨줄 인수 정보 교체
// 3. 바뀐 변수 정보에 따라 함수 변수 정보 교체
// 4. const 사용 여부 결정: 함수를 실행하면 정보가 교체 되는가?

// c에서 제공하는 함수는 알아두는 게 좋다. 특히 string관련

// canvas
struct Canvas {
	char* frameBuffer;	//컴퓨터 그래픽스: 프레임 두개, 그리는 프레임과 만들어진 그림을 내보내주는 프레임. 두개가 번갈아가면서 작업한다. 하나가 만들면 하나는 그리고
	int		size;
};

// 함수 function 프로토타입(함수의 자료형, 함수의 이름, 변수의 자료형.)이 어딘가에 있다느 것을 알려준다. 근데 되도록이면 함수 밖에다가 선언.
extern void canvas_clear(Canvas&);
// 함수의 선언: void canvas_init(Canvas&) > 함수 바디가 존재하지 않음.
// extern, 프로그램 어딘가에 이 함수가 존재한다는 것을 알려줌. 되도록이면 붙여서 사용하는 게 좋다.

// size를 넘겨줄때 널문자도 포함해서 넘겨줄지는 내 결정. 근데 굳이?
void canvas_init(Canvas& canvas, int size)
{
	canvas.frameBuffer = (char*)malloc(sizeof(char) * (size + 1));
	canvas.size = size;
	canvas_clear(canvas);
}

// 함수가 정의되어 있다 > 함수 바디(중괄호)가 존재 
void canvas_clear(Canvas& canvas)
{
	// canvas_init을 호출하지 않고 실행된다면? 쓰레기값이 들어가면서 문제 발생. 이를 해결할 수 있는 코드 추가
	if (canvas.frameBuffer == nullptr || canvas.size <= 0) return;

	for (int i = 0; i < canvas.size; i++)
		canvas.frameBuffer[i] = ' ';
	canvas.frameBuffer[canvas.size] = '\0';
}

void canvas_draw(Canvas& canvas, const char* shape, int pos, bool visible)
{
	if (shape == nullptr || visible == false) return;

	for (int i = 0; i < strlen(shape); i++)
	{
		if (pos + i < 0) continue;
		if (pos + i > canvas.size - 1) continue;

		canvas.frameBuffer[pos + i] = shape[i];
	}
}

void canvas_render(const Canvas& canvas)
{
	printf("%s\r", canvas.frameBuffer);
}

void canvas_deinit(Canvas& canvas)
{
	free(canvas.frameBuffer);
	// 필요는 없지만 전부 초기화해준다는 의미에서
	canvas.size = 0;
}


// player
struct Player {
	char* shape;
	int		pos;
	bool	visible;
};

void player_init(Player& player, const char* shape, int pos, bool visible)
{
	if (shape == nullptr) {
		printf("player shape should be determined");
		exit(0);
	}
	// player.shape이 !nullptr임을 보장
	player.shape = (char*)malloc(sizeof(char) * (strlen(shape) + 1));

	// 이 과정을 strcpy로 대체 가능. > strcpy(player.shape, shape); strcpy: 널문자까지 자동으로 복사해줌.
	/*for (int i = 0; i < strlen(shape); i++)
		player.shape[i] = shape[i];
	player.shape[strlen(shape)] = '\0';*/
	if (player.shape != nullptr)
		strcpy(player.shape, shape);
	player.pos = pos;
	player.visible = visible;
}

void player_draw(const Player& player, Canvas& canvas)
{
	// player 자체를 넘겨주면? 세가지가 가진 정보 종류가 다를 수 있음. 쓸데없는 정보까지 넘겨주기보다는 공통된 정보만 넘겨주도록 canvas_draw의 변수를 설정.
	// -> 그럼 gameObject 특징을 가진 구조체를 따로 만들어준다면? -> 상속
	canvas_draw(canvas, player.shape, player.pos, player.visible);
}

void player_update(Player& player)
{

}

void player_deinit(Player& player)
{
	free(player.shape);
	player.shape = nullptr;
	player.pos = 0;
	player.visible = false;
}


// Enemy와 Player 구조체는 자료형과 필요한 변수 정보 모두 일치, 그럼 GameObject로 묶어준다면?
// enemy
struct Enemy {
	char* shape;
	int		pos;
	bool	visible;
};

void enemy_init(Enemy& enemy, const char* shape, int pos, bool visible)
{
	if (shape == nullptr) {
		printf("enemy shape should be determined");
		exit(0);
	}
	enemy.shape = (char*)malloc(sizeof(char) * (strlen(shape) + 1));
	if(enemy.shape != nullptr)
		strcpy(enemy.shape, shape);
	enemy.pos = pos;
	enemy.visible = visible;
}

void enemy_draw(const Enemy& enemy, Canvas& canvas)
{
	canvas_draw(canvas, enemy.shape, enemy.pos, enemy.visible);
}

bool enemy_isColliding(const Enemy& enemy, int pos)
{
	if (pos >= enemy.pos && pos < enemy.pos + strlen(enemy.shape)) {
		strcpy(enemy.shape, "(T_T)");
		return true;
	}
	// 이미 같은 상황이면 함수가 종료되기 때문에 굳이 else를 쓸 필요는 없음.
	return false;
}

void enemy_update(Enemy& enemy)
{

}

void enemy_deinit(Enemy& enemy)
{
	free(enemy.shape);
	enemy.shape = nullptr;
	enemy.pos = 0;
	enemy.visible = false;
}


// bullet
struct Bullet {
	char*	shape;
	int		pos;
	bool	visible;
	int		direction; // 0: L2R, 1: R2L
	int		num_remaining_frames_to_fire;
};

void bullet_init(Bullet& bullet, const char* shape, int pos, bool visible, int direction, int num_frame)
{
	// 얘도 함수화 가능하지 않을까?
	if (shape == nullptr) {
		printf("bullet input shape is not defined");
		exit(0);
	}
	bullet.shape = (char*)malloc(sizeof(char) * (strlen(shape) + 1));
	if (bullet.shape != nullptr)
		strcpy(bullet.shape, shape);
	bullet.pos = pos;
	bullet.visible = visible;
	bullet.direction = direction;
	bullet.num_remaining_frames_to_fire = num_frame;
}

void bullet_draw(const Bullet& bullet, Canvas& canvas)
{
	canvas_draw(canvas, bullet.shape, bullet.pos, bullet.visible);
}

void bullet_fire(Bullet& bullet, const Player& player, const Enemy& enemy)
{
	// fire bullet, default 값으로 설정 -> if else문을 줄여준다.
	bullet.pos = player.pos + strlen(player.shape);
	bullet.direction = 0;
	strcpy(bullet.shape, ">");
	if (player.pos > enemy.pos) {
		bullet.pos = player.pos;
		bullet.direction = 1;
		strcpy(bullet.shape, "<");
	}

	// 값이 다 준비가 되면 보여주는 순서
	bullet.visible = true;
}

void bullet_move(Bullet& bullet)
{
	if (bullet.direction == 0) {
		bullet.pos++;
	}
	else {
		bullet.pos--;
	}
}

// 인티저 정보 - 4byte, 레퍼런스 정보 - 4byte 기 때문에 크기 걱정은 안해도 됨.
void bullet_update(Bullet& bullet, const Player& player, const Enemy& enemy)
{
	bullet.num_remaining_frames_to_fire--;
	if (bullet.num_remaining_frames_to_fire == 0) {
		if (player.pos == enemy.pos) return;

		bullet_fire(bullet, player, enemy);
		return;
	}

	if (bullet.visible == false) return;

	// update bullet_pos
	bullet_move(bullet);

	// check whether bullet pos is equal to enemy pos
	if (enemy_isColliding(enemy, bullet.pos))
		bullet.visible = false;
}

void bullet_deinit(Bullet& bullet)
{
	free(bullet.shape);
	bullet.shape = nullptr;
	bullet.pos = 0;
	bullet.visible = false;
	bullet.direction = 0;
	bullet.num_remaining_frames_to_fire = 0;
}


// main
int main()
{
	// 객체, instance / 구조체 안에 함수까지 포함한다 -> 이것이 객체지향: 예시) canvas_init() -> cnavas.init()
	Canvas	canvas;

	Player	player;
	Enemy	enemy;
	Bullet	bullet;
	Bullet	bullet2;

	// 함수를 만들 때 -> 이 함수를 어떻게 사용할 것인가? 의 관점으로 시작해서, 어떤 정보가 필요한지부터 잡아주면 좋다.
	// 함수를 바꿔주는 것 보다는 함수에 넘겨주는 정보만 바꿔주는 방향.
	// 초기값을 전달하는 과정, 각 구조체에 초기화 정보를 저장하는 행동은 init함수를 실행하며 들어간다. -> unity Start()
	canvas_init(canvas, 80);
	player_init(player, "(^____^)", 10, true);
	enemy_init(enemy, "(*_*)", 50, true);
	bullet_init(bullet, ">", 0, false, 0, 30);
	bullet_init(bullet2, ">", 0, false, 0, 40);

	while (true)
	{
		canvas_clear(canvas);

		// 이렇게 구조체를 사용하면 넘겨줘야할 변수가 확 준다.
		player_draw(player, canvas);
		enemy_draw(enemy, canvas);
		bullet_draw(bullet, canvas);
		bullet_draw(bullet2, canvas);

		player_update(player);
		enemy_update(enemy);
		bullet_update(bullet, player, enemy);
		bullet_update(bullet2, player, enemy);

		canvas_render(canvas);
		Sleep(100);
	}

	bullet_deinit(bullet);
	bullet_deinit(bullet2);
	enemy_deinit(enemy);
	player_deinit(player);
	canvas_deinit(canvas);

	return 0;
}