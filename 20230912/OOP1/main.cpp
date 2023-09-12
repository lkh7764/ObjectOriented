// 2023.09.08 객체지향프로그래밍(2)
// 오늘 할 내용: 클래스파일 분할
// 2023.09.12 객체지향프로그래밍(2)
// 오늘 할 내용: 클래스파일 분할 완료 / 플레이어입력 기능 추가
#include <iostream>
#include <cstdlib> // include rand()
#include <Windows.h>
#include <conio.h>

#include "GameObject.h"
#include "Utils.h"

int main()
{
	GameObject::Initialize();
	bool exit_flag = false;
	while (exit_flag == false)
	{
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