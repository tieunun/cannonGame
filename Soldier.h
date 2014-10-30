#ifndef __SOLDIER_H__
#define __SOLDIER_H__

#include "GameObject.h"

using namespace cocos2d;

class Soldier : public GameObject {

private:

	b2Body * soldierBody;
	Sprite * soldierSprite;
	
	b2Vec2 desiredPosition;
	
public:
	
	Soldier( Layer * layer, b2World* world, float worldStartX, float worldEndX, float perspectiveX )
	: GameObject(  layer, world,  worldStartX, worldEndX, perspectiveX,  new std::string("SOLDIER") )
	{
		soldierBody = NULL;
		soldierSprite = NULL;
	}
	
	~Soldier();
	
	void createSoldier( float );
	
	void moveToPosition( b2Vec2 );
		
	void updateSprites();
	
	void moveSoldier( );

	
};

#endif // __SOLDIER_H__

