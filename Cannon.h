#ifndef __CANNON_H__
#define __CANNON_H__

#include "GameObject.h"

using namespace cocos2d;

class Cannon : public GameObject {
	
	private:
	
		b2Body* m_cannonWheel;
		b2Body* m_cannonFrame;	
		b2Body* m_cannonHolder;
		b2Body* m_cannonBarrel;
		b2RevoluteJoint * m_cannonBarrelJoint;
		b2RevoluteJoint * m_cannonWheelJoint;
	
		cocos2d::Sprite * cannonBarrelSprite;
		cocos2d::Sprite * cannonFrameSprite;
		cocos2d::Sprite * cannonWheelSprite;
				
		float totalRotation;
		
		
	public:
	
		Cannon(  cocos2d::Layer * layer, b2World * world ) 
		: GameObject( layer, world, new std::string("CANNON") )  {
			m_cannonWheel = NULL;
			m_cannonFrame = NULL;	
			m_cannonHolder = NULL;
			m_cannonBarrel = NULL;
			m_cannonBarrelJoint = NULL;
			m_cannonWheelJoint = NULL;
			
			cannonWheelSprite = NULL;
			cannonFrameSprite = NULL;
			cannonBarrelSprite = NULL;
			
			totalRotation = 0;
		}
	
		void createCannon( b2Vec2 startPoint, float scale );
		
		void moveByVector( b2Vec2 position );
		
		void rotateBarrel( b2Vec2 clickedPoint );
	
		void shoot();
		
		b2Vec2 GetShootingDirection();
		
		b2Vec2 GetBarrelExit();
		
		float GetAngle();
		
		void updateSprites();
		
		cocos2d::Sprite * getMainSprite();
		
		~Cannon();
};

#endif // __CANNON_H__
