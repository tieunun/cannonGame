#include "Cannon.h"

using namespace cocos2d;

void Cannon::createCannon( b2Vec2 startPoint , float scale ){
	
	setScale( scale );
	
	b2BodyDef cannonDef;
	cannonDef.type = b2_dynamicBody;
	
	b2FixtureDef cannonFixtureDef;
	
	//set Colliding
	cannonFixtureDef.filter.categoryBits = CANNON;
	cannonFixtureDef.filter.maskBits = GROUND | BULLET;

	//Create cannon barrel
	cannonDef.position.Set( scale*13.5, scale*7.0);
	cannonFixtureDef.density = 0.6;
	cannonFixtureDef.friction = 0.2;
	
	m_cannonBarrel = m_world->CreateBody( &cannonDef );
	b2PolygonShape cannonBarrelShape;
	cannonBarrelShape.SetAsBox(scale*5, scale*0.5);
		
	cannonFixtureDef.shape = &cannonBarrelShape;
	m_cannonBarrel->CreateFixture(&cannonFixtureDef);
	
	cannonBarrelSprite = cocos2d::Sprite::create("lufa.png",  Rect(0, 0, 640, 104));
	cannonBarrelSprite->setPosition( cocos2d::Vec2( (scale*13.5)*PTM_RATIO , (scale*7.0)*PTM_RATIO ) );
	cannonBarrelSprite->setScale(scale*0.5);
	layer->addChild( cannonBarrelSprite , 0 );
	
	//Create cannon frame
	b2PolygonShape cannonFrameShape;
	cannonFrameShape.SetAsBox( scale*4.5, scale*1.0 );
	
	cannonFixtureDef.density = 5;
	cannonFixtureDef.friction = 0.9;

	cannonDef.position.Set(scale*5, scale*5);		
	m_cannonFrame = m_world->CreateBody( &cannonDef );
	
	cannonFixtureDef.shape = &cannonFrameShape;
	m_cannonFrame->CreateFixture(&cannonFixtureDef);
	
	cannonFrameSprite = Sprite::create("baza.png",  Rect(0, 0, 576, 128));
	cannonFrameSprite->setPosition( Vec2( (scale*5.0)*PTM_RATIO , (scale*5.0)*PTM_RATIO ) );
	cannonFrameSprite->setScale(scale*0.5);
	layer->addChild( cannonFrameSprite , 0 );
	
	m_cannonFrame->SetUserData(this);
	
	//Create cannon wheel
	cannonDef.position.Set(scale*7.5, scale*5.0);
	b2CircleShape cannonWheelShape;
	cannonWheelShape.m_radius = scale*2; 
	
	m_cannonWheel = m_world->CreateBody( &cannonDef );
	
	cannonFixtureDef.density = 4;
	cannonFixtureDef.friction = 0.8;
	cannonFixtureDef.shape = &cannonWheelShape;
	m_cannonWheel->CreateFixture(&cannonFixtureDef);
	
	cannonWheelSprite = cocos2d::Sprite::create("kolo.png",  Rect(0, 0, 256, 256));
	cannonWheelSprite->setPosition( cocos2d::Vec2( (scale*7.5)*PTM_RATIO  , (scale*5.0)*PTM_RATIO  ) );
	cannonWheelSprite->setScale(scale*0.5);
	layer->addChild( cannonWheelSprite , 0 );
	
	//Join Frame and Barrel
	b2RevoluteJointDef revoluteJointDef;
	revoluteJointDef.bodyA = m_cannonFrame;
	revoluteJointDef.bodyB = m_cannonBarrel;
	revoluteJointDef.collideConnected = false;
	revoluteJointDef.localAnchorA.Set(scale*3.5,scale*0.75);
	revoluteJointDef.localAnchorB.Set(-scale*2.5,0);

	revoluteJointDef.enableMotor = false;
	revoluteJointDef.enableLimit = false;
	revoluteJointDef.lowerAngle = -20 * DEGTORAD;
	revoluteJointDef.upperAngle =  90 * DEGTORAD;

	m_cannonBarrelJoint = (b2RevoluteJoint*)m_world->CreateJoint( &revoluteJointDef );

	//Join Frame and Wheel
	revoluteJointDef.bodyA = m_cannonFrame;
	revoluteJointDef.bodyB = m_cannonWheel;
	revoluteJointDef.collideConnected = false;
	revoluteJointDef.localAnchorA.Set(scale*2.5,scale*0.25);
	revoluteJointDef.localAnchorB.Set(0,0);

	revoluteJointDef.enableMotor = false;
	revoluteJointDef.enableLimit = false;
	
	m_cannonWheelJoint = (b2RevoluteJoint*)m_world->CreateJoint( &revoluteJointDef );
	
	moveByVector( startPoint );
}

void Cannon::moveByVector( b2Vec2 position )
{
	m_cannonBarrel->SetTransform( b2Vec2(position.x + m_cannonBarrel->GetPosition().x , position.y + m_cannonBarrel->GetPosition().y) , m_cannonBarrel->GetAngle() );
	m_cannonFrame->SetTransform( b2Vec2(position.x + m_cannonFrame->GetPosition().x , position.y + m_cannonFrame->GetPosition().y) , m_cannonFrame->GetAngle() );
	m_cannonWheel->SetTransform( b2Vec2(position.x + m_cannonWheel->GetPosition().x , position.y + m_cannonWheel->GetPosition().y) , m_cannonWheel->GetAngle() );
}

void Cannon::rotateBarrel( b2Vec2 clickedPoint ){

	float bodyAngle = m_cannonBarrelJoint->GetJointAngle() + 10 * DEGTORAD;
	b2Vec2 toTarget = clickedPoint - m_cannonBarrelJoint->GetAnchorA() ;
	float desiredAngle = atan2f( -toTarget.x, toTarget.y ) + M_PI/2;
	if( desiredAngle > 90 * DEGTORAD ) desiredAngle = 90 * DEGTORAD;
	if( desiredAngle < -20 * DEGTORAD ) desiredAngle = -20 * DEGTORAD;
	float nextAngle = bodyAngle + (float)m_cannonBarrelJoint->GetJointSpeed() / 5.0;

	totalRotation = desiredAngle - nextAngle;

	while ( totalRotation < -180 * DEGTORAD ) totalRotation += 360 * DEGTORAD;
	while ( totalRotation >  180 * DEGTORAD ) totalRotation -= 360 * DEGTORAD;
	
	m_cannonBarrelJoint->EnableMotor(true);
	m_cannonBarrelJoint->SetMaxMotorTorque(2000);
	m_cannonBarrelJoint->SetMotorSpeed( totalRotation < 0 ? -1 : 3 );
}

void Cannon::shoot()
{
	b2Vec2 toTarget = m_cannonBarrel->GetWorldCenter() - m_cannonBarrel->GetWorldPoint( b2Vec2(scale*11,0) ) ;
	m_cannonFrame->ApplyLinearImpulse( b2Vec2(50*toTarget.x,50*toTarget.y), m_cannonFrame->GetWorldCenter(), NULL );
}

b2Vec2 Cannon::GetShootingDirection(){
	b2Vec2 direction = m_cannonBarrel->GetWorldCenter() - m_cannonBarrel->GetWorldPoint( b2Vec2(scale*6.5,0) ) ;
	return direction;
}

b2Vec2 Cannon::GetBarrelExit(){
	b2Vec2 position = m_cannonBarrel->GetWorldPoint( b2Vec2(scale*6,0) );
	return position;
}

float Cannon::GetAngle(){
		return totalRotation;
}

void Cannon::updateSprites(){
				
	if(m_cannonBarrel == NULL || m_cannonFrame == NULL || m_cannonWheel == NULL ) return;
	
	cannonBarrelSprite->setPosition( cocos2d::ccp( m_cannonBarrel->GetPosition().x * PTM_RATIO, m_cannonBarrel->GetPosition().y * PTM_RATIO) );
	cannonBarrelSprite->setRotation( -1 * CC_RADIANS_TO_DEGREES( m_cannonBarrel->GetAngle()) );
	
	cannonFrameSprite->setPosition( cocos2d::ccp( m_cannonFrame->GetPosition().x * PTM_RATIO, m_cannonFrame->GetPosition().y * PTM_RATIO) );
	cannonFrameSprite->setRotation( -1 * CC_RADIANS_TO_DEGREES( m_cannonFrame->GetAngle()) );
	
	cannonWheelSprite->setPosition( cocos2d::ccp( m_cannonWheel->GetPosition().x * PTM_RATIO, m_cannonWheel->GetPosition().y * PTM_RATIO) );
	cannonWheelSprite->setRotation( -1 * CC_RADIANS_TO_DEGREES( m_cannonWheel->GetAngle()) );
	
};

Sprite * Cannon::getMainSprite(){
	return cannonFrameSprite;
}

Cannon::~Cannon(){
	if(m_cannonBarrel != NULL){
			layer->removeChild(cannonBarrelSprite);
			m_world->DestroyBody(m_cannonBarrel);
	}
	if(m_cannonFrame != NULL){
			layer->removeChild(cannonFrameSprite);
			m_world->DestroyBody(m_cannonFrame);
	}
	if(m_cannonWheel != NULL){
			layer->removeChild(cannonWheelSprite);
			m_world->DestroyBody(m_cannonWheel);
	}
}
