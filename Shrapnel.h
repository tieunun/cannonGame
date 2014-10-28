#ifndef __SHRAPNEL_H__
#define __SHRAPNEL_H__

#include "GameObject.h"

class Shrapnel : public GameObject {
	
	private:
	
		float m_blastPower;
		float m_density;
		
	public:

		b2Body* shrapnelBody;
		cocos2d::Sprite * shrapnelSprite;
		
	Shrapnel( Layer * layer, b2World* world, float blastPower, float density)
	: GameObject(  layer, world, new std::string("SHRAPNEL") )
	{
		m_density = density;
		m_blastPower = blastPower;
		
		shrapnelBody = NULL;
		shrapnelSprite = NULL;
	}

	void createShrapnel( b2Vec2 startPosition, b2Vec2 rayDir )
	{

		b2BodyDef shrapnelBodyDef;
		shrapnelBodyDef.type = b2_dynamicBody;
		shrapnelBodyDef.fixedRotation = true; 
		shrapnelBodyDef.bullet = true; // prevent tunneling at high speed
		shrapnelBodyDef.linearDamping = 1; // drag due to moving through air
		shrapnelBodyDef.position = startPosition; 
		shrapnelBodyDef.linearVelocity = m_blastPower * rayDir;
		shrapnelBody = m_world->CreateBody( &shrapnelBodyDef );

		b2CircleShape circleShape;
		circleShape.m_radius = 0.05; // very small

		b2FixtureDef shrapnelFixtureDef;
		shrapnelFixtureDef.shape = &circleShape;
		shrapnelFixtureDef.density = m_density; // very high - shared across all particles
		shrapnelFixtureDef.friction = 0; // friction not necessary
		shrapnelFixtureDef.restitution = 0.99f; // high restitution to reflect off obstacles
		
		//set Colliding
		shrapnelFixtureDef.filter.categoryBits = SHRAPNEL;
		shrapnelFixtureDef.filter.maskBits = GROUND | ENEMY | CANNON;

		
		shrapnelBody->CreateFixture( &shrapnelFixtureDef );

		shrapnelSprite = cocos2d::Sprite::create("shrapnel.png",  cocos2d::CCRectMake(0, 0, 15, 15));
		shrapnelSprite->setPosition( cocos2d::Vec2( (startPosition.x+0.0)*PTM_RATIO , (startPosition.y+0.0)*PTM_RATIO ) );
		layer->addChild( shrapnelSprite , 0 );
		
	}

	~Shrapnel(){
		m_world->DestroyBody( shrapnelBody );
		layer->removeChild( shrapnelSprite );
	};
		
		
	void updateSprites(){
		shrapnelSprite->setPosition( cocos2d::ccp( shrapnelBody->GetPosition().x * PTM_RATIO, shrapnelBody->GetPosition().y * PTM_RATIO) );
		shrapnelSprite->setRotation( -1 * CC_RADIANS_TO_DEGREES( shrapnelBody->GetAngle()) );
	};
};

#endif // __SHRAPNEL_H__
