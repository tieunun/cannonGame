#include "Shrapnel.h"

using namespace cocos2d;

void Shrapnel::createShrapnel( b2Vec2 startPosition, b2Vec2 rayDir )
{

	b2BodyDef shrapnelBodyDef;
	shrapnelBodyDef.type = b2_dynamicBody;
	shrapnelBodyDef.fixedRotation = true; 
	shrapnelBodyDef.bullet = true; // prevent tunneling at high speed
	shrapnelBodyDef.linearDamping = 10*(1+scale); // drag due to moving through air
	shrapnelBodyDef.position = startPosition; 
	shrapnelBodyDef.linearVelocity = scale*m_blastPower * rayDir;
	
	shrapnelBody = m_world->CreateBody( &shrapnelBodyDef );
	ghostBody = m_world->CreateBody( &shrapnelBodyDef );

	b2CircleShape circleShape;
	circleShape.m_radius = scale*0.05; // very small

	b2FixtureDef shrapnelFixtureDef;
	shrapnelFixtureDef.shape = &circleShape;
	shrapnelFixtureDef.density = m_density; // very high - shared across all particles
	shrapnelFixtureDef.friction = 0; // friction not necessary
	shrapnelFixtureDef.restitution = 0.99f; // high restitution to reflect off obstacles
	
	//set Colliding
	shrapnelFixtureDef.filter.categoryBits = SHRAPNEL;
	shrapnelFixtureDef.filter.maskBits = GROUND | ENEMY | CANNON;

	
	shrapnelBody->CreateFixture( &shrapnelFixtureDef );
	
	//ghost doesn`t collide
	shrapnelFixtureDef.filter.maskBits = 0;
	ghostBody->CreateFixture( &shrapnelFixtureDef );

	shrapnelSprite = cocos2d::Sprite::create("shrapnel.png",  cocos2d::CCRectMake(0, 0, 15, 15));
	shrapnelSprite->setPosition( cocos2d::Vec2( (startPosition.x+0.0)*PTM_RATIO , (startPosition.y+0.0)*PTM_RATIO ) );
	shrapnelSprite->setScale(scale);
	layer->addChild( shrapnelSprite , 0 );
	
}

Shrapnel::~Shrapnel(){
	if( ghostBody != NULL ) m_world->DestroyBody( ghostBody );
	if( shrapnelBody != NULL ) m_world->DestroyBody( shrapnelBody );
	if( shrapnelSprite != NULL ) layer->removeChild( shrapnelSprite );
};
	
	
void Shrapnel::updateSprites(){

	if(!active) return; //ONLY ACTIVE SHRAPNELS
	
	shrapnelSprite->setPosition( cocos2d::ccp( shrapnelBody->GetPosition().x * PTM_RATIO, shrapnelBody->GetPosition().y * PTM_RATIO) );
	shrapnelSprite->setRotation( -1 * CC_RADIANS_TO_DEGREES( shrapnelBody->GetAngle()) );
	
	//setPerspective
	float perspectiveFactor = getPerspectiveFactor( shrapnelBody->GetPosition().x * PTM_RATIO );
	shrapnelSprite->setScale( perspectiveFactor * scale );
	
	reduceShrapnelSpeed( perspectiveFactor );
};

void Shrapnel::reduceShrapnelSpeed( float perspectiveFactor )
{
		
	float minVelocity = 50.0;
	//TOO SLOW - KILL SHRAPNEL
	if( shrapnelBody->GetLinearVelocity().Length() <= minVelocity ){
		dissapear();
		return;
	}
	
	//get velocity direction
	b2Vec2 normalizedVelocityVector = ghostBody->GetLinearVelocity();
	normalizedVelocityVector.Normalize();
	
	//get velocity values
	float currentVelocity = ghostBody->GetLinearVelocity().Length();
	float desiredVelocity = perspectiveFactor*currentVelocity;
	
	// set velocity
	shrapnelBody->SetLinearVelocity(desiredVelocity*normalizedVelocityVector);

}

void Shrapnel::dissapear(){
	
	active = false;
	
	if( ghostBody != NULL ) m_world->DestroyBody( ghostBody );
	if( shrapnelBody != NULL ) m_world->DestroyBody( shrapnelBody );
	if( shrapnelSprite != NULL ) layer->removeChild( shrapnelSprite );

	ghostBody = NULL;
	shrapnelBody = NULL;
	shrapnelSprite = NULL;
	
}
