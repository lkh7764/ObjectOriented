#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "GameObject.h"
class Player :
    public GameObject
{
public:

	Player(const char* shape, int pos, bool visible) : GameObject(shape, pos, visible)
	{}

	~Player()
	{}
};

#endif