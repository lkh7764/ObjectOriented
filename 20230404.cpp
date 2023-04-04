#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <Windows.h>


// 1. ����ü ����
// 2. main���� �Լ��� �Ѱ��� �μ� ���� ��ü
// 3. �ٲ� ���� ������ ���� �Լ� ���� ���� ��ü
// 4. const ��� ���� ����: �Լ��� �����ϸ� ������ ��ü �Ǵ°�?

// c���� �����ϴ� �Լ��� �˾Ƶδ� �� ����. Ư�� string����

// canvas
struct Canvas {
	char* frameBuffer;	//��ǻ�� �׷��Ƚ�: ������ �ΰ�, �׸��� �����Ӱ� ������� �׸��� �������ִ� ������. �ΰ��� �����ư��鼭 �۾��Ѵ�. �ϳ��� ����� �ϳ��� �׸���
	int		size;
};

// �Լ� function ������Ÿ��(�Լ��� �ڷ���, �Լ��� �̸�, ������ �ڷ���.)�� ��򰡿� �ִٴ� ���� �˷��ش�. �ٵ� �ǵ����̸� �Լ� �ۿ��ٰ� ����.
extern void canvas_clear(Canvas&);
// �Լ��� ����: void canvas_init(Canvas&) > �Լ� �ٵ� �������� ����.
// extern, ���α׷� ��򰡿� �� �Լ��� �����Ѵٴ� ���� �˷���. �ǵ����̸� �ٿ��� ����ϴ� �� ����.

// size�� �Ѱ��ٶ� �ι��ڵ� �����ؼ� �Ѱ������� �� ����. �ٵ� ����?
void canvas_init(Canvas& canvas, int size)
{
	canvas.frameBuffer = (char*)malloc(sizeof(char) * (size + 1));
	canvas.size = size;
	canvas_clear(canvas);
}

// �Լ��� ���ǵǾ� �ִ� > �Լ� �ٵ�(�߰�ȣ)�� ���� 
void canvas_clear(Canvas& canvas)
{
	// canvas_init�� ȣ������ �ʰ� ����ȴٸ�? �����Ⱚ�� ���鼭 ���� �߻�. �̸� �ذ��� �� �ִ� �ڵ� �߰�
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
	// �ʿ�� ������ ���� �ʱ�ȭ���شٴ� �ǹ̿���
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
	// player.shape�� !nullptr���� ����
	player.shape = (char*)malloc(sizeof(char) * (strlen(shape) + 1));

	// �� ������ strcpy�� ��ü ����. > strcpy(player.shape, shape); strcpy: �ι��ڱ��� �ڵ����� ��������.
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
	// player ��ü�� �Ѱ��ָ�? �������� ���� ���� ������ �ٸ� �� ����. �������� �������� �Ѱ��ֱ⺸�ٴ� ����� ������ �Ѱ��ֵ��� canvas_draw�� ������ ����.
	// -> �׷� gameObject Ư¡�� ���� ����ü�� ���� ������شٸ�? -> ���
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


// Enemy�� Player ����ü�� �ڷ����� �ʿ��� ���� ���� ��� ��ġ, �׷� GameObject�� �����شٸ�?
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
	// �̹� ���� ��Ȳ�̸� �Լ��� ����Ǳ� ������ ���� else�� �� �ʿ�� ����.
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
	// �굵 �Լ�ȭ �������� ������?
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
	// fire bullet, default ������ ���� -> if else���� �ٿ��ش�.
	bullet.pos = player.pos + strlen(player.shape);
	bullet.direction = 0;
	strcpy(bullet.shape, ">");
	if (player.pos > enemy.pos) {
		bullet.pos = player.pos;
		bullet.direction = 1;
		strcpy(bullet.shape, "<");
	}

	// ���� �� �غ� �Ǹ� �����ִ� ����
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

// ��Ƽ�� ���� - 4byte, ���۷��� ���� - 4byte �� ������ ũ�� ������ ���ص� ��.
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
	// ��ü, instance / ����ü �ȿ� �Լ����� �����Ѵ� -> �̰��� ��ü����: ����) canvas_init() -> cnavas.init()
	Canvas	canvas;

	Player	player;
	Enemy	enemy;
	Bullet	bullet;
	Bullet	bullet2;

	// �Լ��� ���� �� -> �� �Լ��� ��� ����� ���ΰ�? �� �������� �����ؼ�, � ������ �ʿ��������� ����ָ� ����.
	// �Լ��� �ٲ��ִ� �� ���ٴ� �Լ��� �Ѱ��ִ� ������ �ٲ��ִ� ����.
	// �ʱⰪ�� �����ϴ� ����, �� ����ü�� �ʱ�ȭ ������ �����ϴ� �ൿ�� init�Լ��� �����ϸ� ����. -> unity Start()
	canvas_init(canvas, 80);
	player_init(player, "(^____^)", 10, true);
	enemy_init(enemy, "(*_*)", 50, true);
	bullet_init(bullet, ">", 0, false, 0, 30);
	bullet_init(bullet2, ">", 0, false, 0, 40);

	while (true)
	{
		canvas_clear(canvas);

		// �̷��� ����ü�� ����ϸ� �Ѱ������ ������ Ȯ �ش�.
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