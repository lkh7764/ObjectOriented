#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>

//https://youtu.be/MBbI48P0bxg

void PlaceCharacter(char place[], char character[], int char_size, int char_pos) {
	for (int i = 0; i < char_size; i++)
		place[char_pos + i] = character[i];
}

int main() 
{
	char canvas[100 + 1];

	char player[] = "(^-^)";
	int player_pos = 10;

	char enemy[] = "(#-O)";
	char enemy_dead[] = "(#-T)";
	int enemy_pos = 50;

	char bullet = '>';
	int bullet_pos = player_pos + strlen(player) + 1;
	int pos_term = 1;

	int frame_set = 100;
	int frame_count = 0;

	while (true) {
		for (int i = 0; i < 100; i++)
			canvas[i] = ' ';
		canvas[100] = '\0';

		PlaceCharacter(canvas, player, strlen(player), player_pos);
		PlaceCharacter(canvas, enemy, strlen(enemy), enemy_pos);
		
		if (frame_count*frame_set >= 3000) {
			canvas[bullet_pos] = bullet;
			bullet_pos += pos_term;

			if (bullet_pos == enemy_pos - 1) {
				canvas[bullet_pos] = ' ';
				pos_term = 0;
				strcpy(enemy, enemy_dead);
			}
		}

		printf("%s\r", canvas);

		Sleep(frame_set);
		frame_count ++;
	}

	return 0;
}