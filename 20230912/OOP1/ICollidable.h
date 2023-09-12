#ifndef _ICOLLIDABLE_H_
#define _ICOLLIDABLE_H_

class GameObject;

class ICollidable
{
public:
	virtual ~ICollidable() {}
	virtual void onCollision(GameObject* collided) = 0; // pure virtual function

};

#endif