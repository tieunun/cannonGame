#include "GameLayer.h"

USING_NS_CC;

Scene * GameLayer::createScene()
{
	auto scene = Scene::create();
	auto layer = GameLayer::create();
	scene->addChild(layer);
	return scene;
}

bool GameLayer::init()
{
	//NULLIFY!
	ground = NULL;
	m_world = NULL;
	
	//SET WIN SIZE
	winSize = Director::getInstance()->getVisibleSize();

	if ( !Layer::init() ) return false;

	//INIT BOX2D
	initBox2D();

	//CREATE GROUND
	initGround();

	this->setTouchEnabled(true);
	
	this->schedule( schedule_selector(GameLayer::tick) );
	
	return true;
}

void GameLayer::initBox2D(){

	b2Vec2 gravity;
	gravity.Set( 0.0f , -10.0f );
	m_world = new b2World( gravity );		

}

void GameLayer::initGround(){
	ground = new Ground(this, m_world);
	ground->initGround();
}

void GameLayer::onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{
	for( auto touch : touches )
	{
		Point location = touch->getLocationInView();
		location = Director::getInstance()->convertToGL(location);
		
		if( location.x < 500 ) {
			if( ground ){ ground->setOffsetX( -100 ); }
		}
		else{
			if( ground ){ ground->setOffsetX( 100 ); }
		};
	}		
}


void GameLayer::onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{

	Point location = touches[0]->getLocationInView();
	location = Director::getInstance()->convertToGL(location);
}

void GameLayer::tick(float dt){
	
	int velocityIterations = 8;
	int positionIterations = 1;
	
	m_world->Step(dt, velocityIterations, positionIterations);			
			
	if( ground != NULL ){
		ground->update();
	}
	
}

