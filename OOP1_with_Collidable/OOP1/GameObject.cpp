#include "GameObject.h"
// 외부 함수가 필요한 경우에, 헤더파일에 인클루드 하지 말고 소스파일에 인클루드 하기.
#include "Canvas.h"

Canvas GameObject::canvas(80);
int GameObject::max_objs = 50;
GameObject** GameObject::objs = new GameObject * [GameObject::max_objs];

void GameObject::draw() { if (alive == true) canvas.draw(shape, pos, visible); }