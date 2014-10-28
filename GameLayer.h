#ifndef __GAME_LAYER_H__
#define __GAME_LAYER_H__

#include "Box2D/Box2D.h"
#include "cocos2d.h"
#include "Ground.h"

using namespace cocos2d;

class GameLayer : public cocos2d::Layer
{
		
private:

	//texture data
	int nStripes;
	float textureWidth;
	float textureHeight;

	Size winSize;
	Ground * ground;

    b2World * m_world;

public:

	CustomCommand drawStripesCommand;

	Sprite * noiseSprite;
	Sprite * stripes;

    static cocos2d::Scene* createScene();

    // implement the "static create()" method manually
    CREATE_FUNC(GameLayer);

	bool init();
    
	void initBox2D();
	
	Texture2D * createTexture( Color4F bgColor, float textureWidth, float textureHeight, int nStripes );

	void drawStripes();

	Color4F randomBrightColor();
	
	void genBackground();
	
	void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);

	void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);

	void tick(float dt);

};

#endif // __GAME_LAYER_H__
