#ifndef __CLOUD_H__
#define __CLOUD_H__

#define MAX_CLOUD_SPRITES 6

#include "GameObject.h"

using namespace cocos2d;

class Cloud : public GameObject {

private:

	Point startPosition;
	
	cocos2d::Sprite * cloudSprites[MAX_CLOUD_SPRITES];
	Point cloudBottomLeft[MAX_CLOUD_SPRITES];
	Point cloudTopRight[MAX_CLOUD_SPRITES];
	int spriteCount;
	int baseMoveSpeed;
	int cloudWidth, cloudHeight;
	
public:
	
	Cloud( Layer * layer, b2World* world, float worldStartX, float worldEndX, float perspectiveX )
	: GameObject(  layer, world,  worldStartX, worldEndX, perspectiveX,  new std::string("CLOUD") )
	{
		//NULLIFY clouds
		for( int i = 0; i < MAX_CLOUD_SPRITES ; i++ )
			cloudSprites[i] = NULL;
			
		//SET BASE SPEED
		baseMoveSpeed = 1.5 + (rand()%15)/10;
	}
	
	void generateRandomCloud( Point startPosition );

	void createCloud( Point startPosition );

	~Cloud();
	
	void disposeCloud();
		
	void updateSprites();
	
	float getCloudPerspectiveFactor( float );
	
};

#endif // __CLOUD_H__

