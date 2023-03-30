#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib>
#include <Windows.h>


//canvas
//char* 를 새로운 자료형 t라고 생각하면 편함.
void canvas_init(char*& canvas,int size) {
	canvas = (char*)malloc(sizeof(char) * (size + 1));
}

void canvas_clear(char* canvas, int size) {
	for (int i = 0; i < size; i++)
		canvas[i] = ' ';
	canvas[size] = '\0';
}

//바뀌지 않는 정보에는 반드시 const 붙여주기.
void canvas_draw(char* canvas, int size, const char* shape, int pos) {
	//shpae이 nullptr이면 strlen(shape) 값이 0이 되기 때문에 문제가 일어나지 않음.
	//문제상황1. canvas가 nullptr을 가리키는 경우.
	//문제상황2. pos가 음수인 경우.
	//문제상황3. shape의 사이즈가 canvas보다 큰 경우.
	//비정상적인 상황에 대처하는 것이 중요!
	if (canvas == nullptr) return;

	for (int i = 0; i < strlen(shape); i++) {
		if (pos + i < 0) continue;
		if (pos + i > size - 1) break;

		canvas[pos + i] = shape[i];
	}
}

void canvas_render(char* canvas) {
	printf("%s\r", canvas);
}

void canvas_deinit(char* canvas) {
	free(canvas);
}


//player
void player_init(char*& player_shape, const char* shape, int& pos) {
	player_shape = (char*)malloc(sizeof(char) * (strlen(shape) + 1));
	for (int i = 0; i < strlen(shape); i++)
		player_shape[i] = shape[i];
	player_shape[strlen(shape)] = '\0';

	pos = 50;
}

void player_draw(const char*& player_shape, int pos, char* canvas, int canvas_size) {
	canvas_draw(canvas, canvas_size, player_shape, pos);
}

void player_update(char* player_shape, int& pos) {
	
}

void player_deinit(char* player_shape) {
	free(player_shape);
}


//enemy
void enemy_init(char*& enemy_shape, const char* shape, int& pos) {
	enemy_shape = (char*)malloc(sizeof(char) * (strlen(shape) + 1));
	for (int i = 0; i < strlen(shape); i++)
		enemy_shape[i] = shape[i];
	enemy_shape[strlen(shape)] = '\0';

	pos = 10;
}

void enemy_draw(const char*& enemy_shape, int pos, char* canvas, int canvas_size) {
	canvas_draw(canvas, canvas_size, enemy_shape, pos);
}

void enemy_update(char* enemy_shape, int& pos) {

}

void enemy_deinit(char* enemy_shape) {
	free(enemy_shape);
}


//bullet
void bullet_init(char*& bullet_shape, const char* shape, int& pos, bool& visible, int& direction) {
	bullet_shape = (char*)malloc(sizeof(char) * (strlen(shape) + 1));
	for (int i = 0; i < strlen(shape); i++)
		bullet_shape[i] = shape[i];
	bullet_shape[strlen(shape)] = '\0';
	visible = false;

	pos = 0;
	direction = 0;
}

void bullet_draw(const char* bullet_shape, int pos, bool visible, char* canvas, int canvas_size) {
	if (visible == false) return;
	canvas_draw(canvas, canvas_size, bullet_shape, pos);
}

void bullet_update(char* bullet_shape, int& pos, bool& visible, int player_pos, int enemy_pos, int& n_frames, int& direction) {
	//증가 전 값이 리턴
	if (n_frames++ == 30) {
		visible = true;
		pos = player_pos;
		if (player_pos < enemy_pos)
			direction = 0;
		else if (enemy_pos < player_pos){
			direction = 1;
			const char* shape = "<";
			for (int i = 0; i < strlen(bullet_shape); i++)
				bullet_shape[i] = shape[i];
			bullet_shape[strlen(shape)] = '\0';
		}
		else
			visible = false;
		return;
	}
	if (visible == false) return;

	//update bullet position
	if (direction == 0) pos++;
	else pos--;

	// check whether bullet collides with enemy
	if (pos == enemy_pos)
		visible = false;
}

void bullet_deinit(char* bullet_shape) {
	free(bullet_shape);
}


int main()
{
	//배열 공간으로 포인터 정보 ->  nullptr이 들어있음, 0번째 위치. > 이 위치에 운영체제 공간이 있는데, 그걸 고칠 수 없음.
	char* canvas = nullptr;
	int canvas_size = 80;

	char* player_shape = nullptr;
	int player_pos;
	
	char* enemy_shape = nullptr;
	int enemy_pos;

	char* bullet_shape = nullptr;
	int bullet_pos;
	bool bullet_visible;
	int bullet_direction; //0: left to right, 1: right to left
	int n_frames = 0;

	//포인터 정보를 넘겨줌.  
	canvas_init(canvas, canvas_size);
	
	player_init(player_shape, "(^--^)", player_pos);
	enemy_init(enemy_shape, "(*--*)", enemy_pos);
	bullet_init(bullet_shape, ">", bullet_pos, bullet_visible, bullet_direction);

	while (true) {
		canvas_clear(canvas, canvas_size);

		player_draw(player_shape, player_pos, canvas, canvas_size);
		enemy_draw(enemy_shape, enemy_pos, canvas, canvas_size);
		bullet_draw(bullet_shape, bullet_pos, bullet_visible, canvas, canvas_size);

		player_update(player_shape, player_pos);
		enemy_update(enemy_shape, enemy_pos);
		bullet_update(bullet_shape, bullet_pos, bullet_visible, player_pos, enemy_pos, n_frames, bullet_direction);

		canvas_render(canvas);
		Sleep(100);
	}

	bullet_deinit(bullet_shape);
	enemy_deinit(enemy_shape);
	player_deinit(player_shape);

	canvas_deinit(canvas);

	return 0;
}