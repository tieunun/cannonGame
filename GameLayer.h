#ifndef __GAME_LAYER_H__
#define __GAME_LAYER_H__

#include "Box2D/Box2D.h"
#include "cocos2d.h"
#include "Ground.h"
#include "Cannon.h"
#include "Bullet.h"
#include "BulletContactListener.h"
#include "ScreenLog.h"
#include "Cloud.h"
#include "Enemy.h"

using namespace cocos2d;

class GameLayer : public cocos2d::Layer
{
		
private:

	float worldStartX, worldEndX, worldHeight, perspectiveX;

	b2Vec2 directionPoint;
	
	Size winSize;
	
    b2World * m_world;
    
	Ground * ground;

    Cannon * cannon;

	std::vector<Bullet*> bullets;
	std::vector<Cloud*> clouds;

	std::vector<Enemy*> enemies;

	Sprite * background;
	Sprite * backgroundLight;

	BulletContactListener bulletContactListenerInstance;
	
	float globalScale;

public:

    static cocos2d::Scene* createScene();

    // implement the "static create()" method manually
    CREATE_FUNC(GameLayer);

	bool init();
    
	void initBackground();
	
	void initBox2D();
	
	void initGround();
	
	void generateClouds();
	
	void initCannon();
	
	void generateEnemies();
	
	void createBullet();
	
	void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);

	void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);

	void tick(float dt);

};

#endif // __GAME_LAYER_H__
