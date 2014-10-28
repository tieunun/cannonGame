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
	cannon = NULL;
	
	//SET WIN SIZE
	winSize = Director::getInstance()->getVisibleSize();

	if ( !Layer::init() ) return false;

	directionPoint.Set( 100, 100 );

	//INIT BOX2D
	initBox2D();

	//CREATE GROUND
	initGround();
	
	//CREATE CANNON
	initCannon();

	this->setTouchEnabled(true);
	
	this->schedule( schedule_selector(GameLayer::tick) );
	
	return true;
}

void GameLayer::initBox2D(){

	b2Vec2 gravity;
	gravity.Set( 0.0f , -10.0f );
	m_world = new b2World( gravity );	
	
	m_world->SetContactListener(&bulletContactListenerInstance);	

}

void GameLayer::initGround(){
	ground = new Ground(this, m_world);
	ground->initGround();
}

void GameLayer::initCannon(){
	cannon = new Cannon(this, m_world);
	cannon->createCannon( b2Vec2( 5 , 20 ) );
}

void GameLayer::createBullet(){

	Bullet * bullet = new Bullet(this, m_world, 0.5 , 1);
	bullet->createBullet( cannon->GetBarrelExit() );
	bullet->shoot( cannon->GetShootingDirection() , 2 );
	
	bullets.push_back(bullet);

}

void GameLayer::onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{
	for( auto touch : touches )
	{
		Point location = touch->getLocationInView();
		location = Director::getInstance()->convertToGL(location);
		
		if( location.x > 300 )
			directionPoint.Set( location.x/PTM_RATIO , location.y/PTM_RATIO );
	
	}		
}

void GameLayer::onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{
	Point location = touches[0]->getLocationInView();
	location = Director::getInstance()->convertToGL(location);
	
	if( location.x <= 300 )
	{
		cannon->shoot();
		createBullet();
	}
}

void GameLayer::tick(float dt){
	
	int velocityIterations = 8;
	int positionIterations = 1;
	
	m_world->Step(dt, velocityIterations, positionIterations);			
			
	if( ground != NULL ){
		ground->update();
	}
	
	if( cannon != NULL ){
		cannon->updateSprites();
		cannon->rotateBarrel( directionPoint );
	}
	
	//Explode bullets
	for(int i = 0; i < bullets.size(); i++)
	{
		Bullet * bullet = bullets[i];
		if( !bullet->updateSprites() )
		{
			bullets.erase(bullets.begin()+i);
			delete bullet;
			
			this->runAction( Follow::create( cannon->getMainSprite() ,  Rect(0, 0, this->getScale()*20000, this->getScale()*1300) ));
	
		}
	}
	
}
