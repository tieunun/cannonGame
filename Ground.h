#ifndef __GROUND_H__
#define __GROUND_H__

#include "GameObject.h"

#define maxHillPoints  1000
#define hillSegmentWidth  10
#define maxVertexPoints 5000

#define maxMountains 10

using namespace cocos2d;

class Ground : public GameObject , public Node{
	
private:
	
	Size winSize;
	
	// MOUNTAINS
	Sprite * mountains[maxMountains];
	
	// GROUND SPRITE
	Sprite * groundSprite;
	
	// B2GROUND BODY
	b2Body * groundBody;
	
	// TEXTURE
	CustomCommand drawStripesCommand;
	Sprite * noiseSprite;
	Sprite * textureSprite;
	int nStripes;
	float textureWidth;
	float textureHeight;
	
public:
	
	Ground( Layer * layer , b2World * world, float worldStartX, float worldEndX, float perspectiveX ) 
	:GameObject( layer , world,  worldStartX, worldEndX, perspectiveX, new std::string("GROUND") ) {

		winSize = Director::getInstance()->getVisibleSize();
		
		textureSprite = NULL;
		groundBody = NULL;
		noiseSprite = NULL;
		
		for( int i = 0 ; i < maxMountains ; i++ ){
			mountains[i] = NULL;
		}
				
	}
	
	//UPDATE
	void update();
	
	//RANDOM GROUND GENERATION
	void initGround( Point bottomLeft , Point topRight );

	//B2BODY
	void setBox2DBody( Point bottomLeft , Point topRight );
	
	void generateMountains();
	
	//TEXTURING
	Color4F randomBrightColor();
	Texture2D * createTexture( Color4F bgColor, float textureWidth, float textureHeight, int nStripes );
	void drawTexture();
	
	b2Body * getBody(){ return groundBody; };
};

#endif // __GROUND_H__
