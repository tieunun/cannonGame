#include "Soldier.h"

using namespace cocos2d;

void Soldier::createSoldier( float scale ){
	
	setScale(scale);

	b2BodyDef soldierDef;
	soldierDef.position.Set( worldStartX/PTM_RATIO + 1 + (rand()%10)/10 , 2 );
	soldierDef.type = b2_dynamicBody;

	soldierBody = m_world->CreateBody(&soldierDef);
	
	b2PolygonShape soldierShape;
	soldierShape.SetAsBox( 0.5 , 0.8 );

	b2FixtureDef soldierFixtureDef;
	soldierFixtureDef.density = 1;
	soldierFixtureDef.friction = 0.3;

	//set Colliding
	soldierFixtureDef.filter.categoryBits = CANNON;
	soldierFixtureDef.filter.maskBits = GROUND | ENEMY ;

	soldierFixtureDef.shape = &soldierShape;
	soldierBody->CreateFixture(&soldierFixtureDef);

	soldierSprite = cocos2d::Sprite::create("soldier.png",  cocos2d::CCRectMake(0, 0, 533 , 966));
	soldierSprite->setPosition( cocos2d::Vec2( 2*PTM_RATIO , 2*PTM_RATIO ) );
	float ratio = 1.6*PTM_RATIO/966;
	soldierSprite->setScale(scale*ratio);
	layer->addChild( soldierSprite , 10 );
		
};

void Soldier::moveToPosition( b2Vec2 position ){
	this->desiredPosition = position;
	
	b2Vec2 currentPosition = soldierBody->GetPosition();
	if(desiredPosition.x - currentPosition.x < 0) flipSoldier();
			
};

void Soldier::flipSoldier()
{
	soldierSprite->setScaleX( -1*soldierSprite->getScaleX() );
}

void Soldier::updateSprites(){
	
	if( !soldierSprite || !soldierBody ) return;
	
	moveSoldier();
	
	soldierSprite->setPosition( cocos2d::ccp( soldierBody->GetPosition().x * PTM_RATIO , soldierBody->GetPosition().y * PTM_RATIO) );
	soldierSprite->setRotation( -1 * CC_RADIANS_TO_DEGREES( soldierBody->GetAngle()) );

};


void Soldier::moveSoldier(){
	b2Vec2 currentPosition = soldierBody->GetPosition();
	
	if( fabsf(desiredPosition.x - currentPosition.x) > 0.2 )
	{
		//if( soldierBody->GetLinearVelocity().x < 2.0 )
		//{
			b2Vec2 direction = desiredPosition - currentPosition;
			direction.Normalize();
			
			//float desiredV0 = 2.0-soldierBody->GetLinearVelocity().x;
			//float desiredForce = soldierBody->GetMass()*desiredV0;
			
			float desiredVelocity = desiredPosition.x - currentPosition.x;
			if( desiredVelocity > 3.0 ) desiredVelocity = 3.0;
			
			//soldierBody->ApplyLinearImpulse( 2*desiredForce*b2Vec2(direction.x,0) ,  soldierBody->GetWorldCenter() , NULL );
			
			soldierBody->SetLinearVelocity( b2Vec2( desiredVelocity , 0 ) );
		//}
	}
}

Soldier::~Soldier(){
	if( soldierBody != NULL ) m_world->DestroyBody( soldierBody );
	if( soldierSprite != NULL ) layer->removeChild( soldierSprite );
};
