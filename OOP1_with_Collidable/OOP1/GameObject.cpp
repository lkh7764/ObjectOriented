#include "GameObject.h"
// �ܺ� �Լ��� �ʿ��� ��쿡, ������Ͽ� ��Ŭ��� ���� ���� �ҽ����Ͽ� ��Ŭ��� �ϱ�.
#include "Canvas.h"

Canvas GameObject::canvas(80);
int GameObject::max_objs = 50;
GameObject** GameObject::objs = new GameObject * [GameObject::max_objs];

void GameObject::draw() { if (alive == true) canvas.draw(shape, pos, visible); }