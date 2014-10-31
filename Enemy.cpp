#include "Enemy.h"

using namespace cocos2d;

void Enemy::createEnemy( float scale ){
	
	setScale(scale);

	b2BodyDef enemyDef;
	enemyDef.position.Set( worldEndX/PTM_RATIO - 5 , 2 );
	enemyDef.type = b2_dynamicBody;

	enemyBody = m_world->CreateBody(&enemyDef);
	
	b2PolygonShape enemyShape;
	enemyShape.SetAsBox( 0.5 , 0.8 );

	b2FixtureDef enemyFixtureDef;
	enemyFixtureDef.density = 1;
	enemyFixtureDef.friction = 0.3;

	//set Colliding
	enemyFixtureDef.filter.categoryBits = ENEMY;
	enemyFixtureDef.filter.maskBits = GROUND | SHRAPNEL | BULLET ;

	enemyFixtureDef.shape = &enemyShape;
	enemyBody->CreateFixture(&enemyFixtureDef);

	enemySprite = cocos2d::Sprite::create("soldier.png",  cocos2d::CCRectMake(0, 0, 1*PTM_RATIO , 1.6*PTM_RATIO));
	enemySprite->setPosition( cocos2d::Vec2( 2*PTM_RATIO , 2*PTM_RATIO ) );
	enemySprite->setScale(scale);
	
	this->desiredPosition = b2Vec2( worldStartX + 15 , 0 );
	
	layer->addChild( enemySprite , 10 );
		
	flipEnemy();
};

void Enemy::moveToPosition( b2Vec2 position ){
	this->desiredPosition = position;
	
	b2Vec2 currentPosition = enemyBody->GetPosition();
	if(desiredPosition.x - currentPosition.x < 0) flipEnemy();
			
};

void Enemy::flipEnemy()
{
	enemySprite->setScaleX( -1*enemySprite->getScaleX() );
}

void Enemy::moveEnemy(){
	b2Vec2 currentPosition = enemyBody->GetPosition();
	
	if( fabsf(desiredPosition.x - currentPosition.x) > 0.2 )
	{
		b2Vec2 direction = desiredPosition - currentPosition;
		direction.Normalize();
			
		float desiredVelocity = 1.0;//desiredPosition.x - currentPosition.x;
		//if( desiredVelocity > 2.0 ) desiredVelocity = 2.0;
		
		float frictionForce = enemyBody->GetMass()*10.0*0.3;
		float force = enemyBody->GetMass()*desiredVelocity;
		
//		enemyBody->SetLinearVelocity( b2Vec2( desiredVelocity , 0 ) );

		enemyBody->ApplyForce( b2Vec2( 6*frictionForce*direction.x , 0 ) , enemyBody->GetWorldCenter() , NULL );
	}
}

Enemy::~Enemy(){
	disposeEnemy();
};

void Enemy::disposeEnemy(){
	if( enemyBody != NULL ) m_world->DestroyBody( enemyBody );
	if( enemySprite != NULL ) layer->removeChild( enemySprite );
}


void Enemy::updateSprites(){
	
	if( !enemySprite || !enemyBody ) return;
	
	b2Vec2 currentPosition = enemyBody->GetPosition();
	if( currentPosition.x < worldStartX + 16 ){
		disposeEnemy();
		createEnemy(scale);
		return;
	}
	
	moveEnemy();
	
	enemySprite->setPosition( cocos2d::ccp( enemyBody->GetPosition().x * PTM_RATIO , enemyBody->GetPosition().y * PTM_RATIO) );
	enemySprite->setRotation( -1 * CC_RADIANS_TO_DEGREES( enemyBody->GetAngle()) );

	//setPerspective
	float perspectiveFactor = getPerspectiveFactor( enemyBody->GetPosition().x * PTM_RATIO );
	enemySprite->setScale( perspectiveFactor * scale );

	//realistic perspective body movement
	reduceEnemySpeed( perspectiveFactor );
};


void Enemy::reduceEnemySpeed( float perspectiveFactor )
{
	enemyBody->SetLinearVelocity( perspectiveFactor*enemyBody->GetLinearVelocity() );
}

