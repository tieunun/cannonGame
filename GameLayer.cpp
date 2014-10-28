#include "GameLayer.h"

USING_NS_CC;

Scene * GameLayer::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();
	
	// 'layer' is an autorelease object
	auto layer = GameLayer::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool GameLayer::init()
{
	ground = NULL;
	
	if ( !Layer::init() )
	{
		return false;
	}

	winSize = Director::getInstance()->getVisibleSize();

	initBox2D();

	//Generate Ground
	ground = new Ground(this, m_world);
	ground->initGround();
			
	this->setTouchEnabled(true);
	
	this->schedule( schedule_selector(GameLayer::tick) );
	
	return true;
}

void GameLayer::initBox2D(){

	b2Vec2 gravity;
	gravity.Set( 0.0f , -10.0f );
	m_world = new b2World( gravity );		

}

void GameLayer::onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{
	for( auto touch : touches )
	{
		Point location = touch->getLocationInView();
		location = Director::sharedDirector()->convertToGL(location);

	}		
}


void GameLayer::onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{

	Point location = touches[0]->getLocationInView();
	location = Director::sharedDirector()->convertToGL(location);
	
}

void GameLayer::tick(float dt){
	
	int velocityIterations = 8;
	int positionIterations = 1;
	
	m_world->Step(dt, velocityIterations, positionIterations);
			
			
	if( ground != NULL ){
		ground->update();
	}
	
}

