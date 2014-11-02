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
	background = NULL;
	backgroundLight = NULL;
	
	//SET WIN SIZE
	winSize = Director::getInstance()->getVisibleSize();

	worldStartX = 0;
	worldEndX = 5*winSize.width/3;
	worldHeight = worldEndX;
	perspectiveX = (3*winSize.width)/4;

	trajectoryFrames = 0;

	if ( !Layer::init() ) return false;

	directionPoint.Set( 100, 100 );
	globalScale = 0.5;

	//INIT BACKGROUND
	initBackground();

	//INIT BOX2D
	initBox2D();

	//CREATE GROUND
	initGround();
	
	//GENERATE CLOUDS
	generateClouds();
	
	//CREATE CANNON
	initCannon();
	
	//GENERATE ENEMIES
	generateEnemies();

	this->setTouchEnabled(true);
	
	this->schedule( schedule_selector(GameLayer::tick) );
	
	return true;
}

void GameLayer::initBox2D(){

	g_screenLog->log( LL_INFO , "Initing Box2D" );

	b2Vec2 gravity;
	gravity.Set( 0.0f , -10.0f );
	m_world = new b2World( gravity );	
	
	m_world->SetContactListener(&bulletContactListenerInstance);	

}

void GameLayer::initBackground(){
	if(background){
		this->removeChild(background);
		background = NULL;
	}
	
	Texture2D::TexParams params = {GL_LINEAR, GL_LINEAR, GL_REPEAT,  GL_REPEAT};
	
	background = Sprite::create("sky.png",  Rect(0, 0, worldEndX, worldHeight));
	
	background->getTexture()->setTexParameters(params);
	background->setPosition( worldEndX/2, worldHeight/4-50 );
	
	this->addChild( background , -50 );

	backgroundLight = Sprite::create("sky2.png",  Rect(0, 0, worldEndX, 256));
	
	backgroundLight->getTexture()->setTexParameters(params);
	backgroundLight->setPosition( worldEndX/2, 128 );
	
	this->addChild( backgroundLight , -50 );

}
	
void GameLayer::initGround(){
	
	ground = new Ground(this, m_world, worldStartX, worldEndX, perspectiveX );
	ground->initGround( Point( worldStartX ,0) , Point( worldEndX, winSize.height/6) );
	
}

void GameLayer::generateClouds(){
	
	//random number of clouds
	int cloudsCount = 15;
	
	for(int i = 0 ; i < cloudsCount ; i++ )
	{
		Cloud * cloud = new Cloud( this, m_world, worldStartX, worldEndX, perspectiveX );
		cloud->createCloud( Point( -worldEndX/3+rand()%(int)(1.3*(int)worldEndX), 350+(rand()%(int)(winSize.height-200)) ) );
		clouds.push_back(cloud);
	}
}

void GameLayer::generateEnemies(){
	
	int enemiesCount = 1;
	
	for(int i = 0 ; i < enemiesCount ; i++ )
	{
		Enemy * enemy = new Enemy( this, m_world, worldStartX, worldEndX, perspectiveX );
		enemy->createTank( 2*globalScale );
		enemies.push_back(enemy);
	}
}


void GameLayer::initCannon(){


	cannon = new Cannon(this, m_world, worldStartX, worldEndX, perspectiveX );
	cannon->createCannon( b2Vec2( 2 , 2 ) , globalScale );
	
	if( ground ) cannon->createDistanceJoint( ground->getBody() );
}

void GameLayer::createBullet(){

	resetTrajectory();

	Bullet * bullet = new Bullet(this, m_world, 1 , 1, worldStartX, worldEndX, perspectiveX );
	bullet->createBullet( cannon->GetBarrelExit(),  globalScale  );
	bullet->shoot( cannon->GetShootingDirection() , 1 );
	
	bullets.push_back(bullet);

}

void GameLayer::resetTrajectory(){
	
	for(int i = 0; i < trajectory.size(); i++)
	{
		this->removeChild( trajectory[i] );
		trajectory[i] = NULL;
	}	
	
	trajectory.clear();
	trajectoryFrames = 0;
}

void GameLayer::addTrajectoryPoint( Point p ){
	
	//Only add points far away from cannon
	if( p.x < 500 ) return;
	
	//Trajectory point
	Sprite * tP = Sprite::create("kolko.png",  Rect(0, 0, 50, 50));
	tP->setPosition( p.x , p.y );
	
	float perspectiveScale = 1.0;
	
	//After perspective point
	if( p.x > perspectiveX ){
		float a = 0.8 / (worldEndX - perspectiveX);
		perspectiveScale = 1 - a*(p.x - perspectiveX); 
		
	}
	
	tP->setScale( 0.2 * perspectiveScale );
	
	trajectory.push_back( tP );
	
	this->addChild( tP , 0 );

}

void GameLayer::onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{
	for( auto touch : touches )
	{
		Point location = touch->getLocationInView();
		location = Director::getInstance()->convertToGL(location);
		
		if( location.x > 300 )
		{
			cannon->soldierToWheel();
			directionPoint.Set( location.x/PTM_RATIO , location.y/PTM_RATIO );
		}
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
	else cannon->resetSoldiers();
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

	//Update enemies
	for(int i = 0; i < enemies.size(); i++)
	{
		enemies[i]->updateSprites();
	}
	
	//Update clouds
	for(int i = 0; i < clouds.size(); i++)
	{
		clouds[i]->updateSprites();
	}
	
	//Explode bullets
	for(int i = 0; i < bullets.size(); i++)
	{
		Bullet * bullet = bullets[i];
		bullet->updateSprites();
		
		if( trajectoryFrames % 15 == 0 ){
			trajectoryFrames = 0;
			Sprite * bulletSprite = bullet->getMainSprite();
			if( bulletSprite )	addTrajectoryPoint( bulletSprite->getPosition() );
		}
		
		//bullet exploded and explosion finished
		if( bullet->disposeBullet() )
		{
			bullets.erase(bullets.begin()+i);
			delete bullet;
			
			this->runAction( Follow::create( cannon->getMainSprite() ,  Rect(worldStartX*this->getScale(), worldStartX*this->getScale(), worldEndX*this->getScale() , worldEndX*this->getScale() ) ));
	
			if(cannon) cannon->resetSoldiers();
		}
	}
	
	trajectoryFrames++;
	
}
