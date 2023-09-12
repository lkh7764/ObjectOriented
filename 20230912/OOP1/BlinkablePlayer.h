#ifndef _BLINKABLEPLAYER_H_
#define _BLINKABLEPLAYER_H_

#include "Player.h"
class BlinkablePlayer :
    public Player
{
	int n_frames;

public:
	BlinkablePlayer(const char* shape, int pos, bool visible) : Player(shape, pos, visible), n_frames(0)
	{}

	void draw() override {
		if (n_frames > 0) {
			n_frames--;
			if (n_frames % 2 == 0) return;
		}
		GameObject::draw();
	}

	void setBlinkingPeriod(int n_frames)
	{
		this->n_frames = n_frames;
	}

};

#endif