#ifndef __SHRAPNEL_H__
#define __SHRAPNEL_H__

#include "GameObject.h"

using namespace cocos2d;

class Shrapnel : public GameObject {

private:

	float m_blastPower;
	float m_density;
	bool active;
	
public:


	b2Body* ghostBody;
	b2Body* shrapnelBody;
	cocos2d::Sprite * shrapnelSprite;
	
	Shrapnel( Layer * layer, b2World* world, float blastPower, float density, float worldStartX, float worldEndX, float perspectiveX )
	: GameObject(  layer, world,  worldStartX, worldEndX, perspectiveX,  new std::string("SHRAPNEL") )
	{
		m_density = density;
		m_blastPower = blastPower;
	
		active = true;
		
		ghostBody = NULL;
		shrapnelBody = NULL;
		shrapnelSprite = NULL;
	}

	void createShrapnel( b2Vec2 startPosition, b2Vec2 rayDir );

	~Shrapnel();
		
	void updateSprites();
	
	void reduceShrapnelSpeed( float );
	
	void dissapear();
};

#endif // __SHRAPNEL_H__
