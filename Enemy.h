#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "GameObject.h"

using namespace cocos2d;

class Enemy : public GameObject {

private:

	b2Body * enemyBody;
	Sprite * enemySprite;
	
	b2Vec2 desiredPosition;
	
public:
	
	Enemy( Layer * layer, b2World* world, float worldStartX, float worldEndX, float perspectiveX )
	: GameObject(  layer, world,  worldStartX, worldEndX, perspectiveX,  new std::string("SOLDIER") )
	{
		enemyBody = NULL;
		enemySprite = NULL;
	}
	
	~Enemy();
	
	void createEnemy( float );
	
	void moveToPosition( b2Vec2 );
		
	void updateSprites();
	
	void moveEnemy( );
	
	void flipEnemy();

	void disposeEnemy();
	
	void reduceEnemySpeed( float );
};

#endif // __SOLDIER_H__

