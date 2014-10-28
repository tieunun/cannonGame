#ifndef __GAME_LAYER_H__
#define __GAME_LAYER_H__

#include "Box2D/Box2D.h"
#include "cocos2d.h"
#include "Ground.h"

using namespace cocos2d;

class GameLayer : public cocos2d::Layer
{
		
private:

	Size winSize;
	
	Ground * ground;

    b2World * m_world;

public:

    static cocos2d::Scene* createScene();

    // implement the "static create()" method manually
    CREATE_FUNC(GameLayer);

	bool init();
    
	void initBox2D();
	
	void initGround();
	
	void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);

	void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);

	void tick(float dt);

};

#endif // __GAME_LAYER_H__
