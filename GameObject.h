#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#include "Box2D/Box2D.h"
#include "cocos2d.h"
#include "ScreenLog.h"

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define PTM_RATIO 32.0

enum _entityCategory {
	CANNON 		= 0x0001,
	BULLET 		= 0x0002,
	SHRAPNEL 	= 0x0004,
	ENEMY 		= 0x0008,
	GROUND 		= 0x0010,
};

class GameObject
{
	protected:
	
		float worldStartX, worldEndX, perspectiveX;
		float scale;
	
	public:
		
		std::string * name;
		
		GameObject( cocos2d::Layer * layer, b2World * world, float worldStartX, float worldEndX, float perspectiveX, std::string * name )
		{ 
			this->m_world = world;
			this->name = name;
			this->layer = layer;
			
			
			this->worldStartX = worldStartX;
			this->worldEndX = worldEndX;
			this->perspectiveX = perspectiveX;
			this->scale = 1;
		};
		
		~GameObject(){
			if( name != NULL ) delete name;
		};
		
		void setScale(float scale)
		{
			this->scale = scale;
		}

	protected:

		cocos2d::Layer * layer; 
		
		b2World*  m_world;
		
		// Implementation of perspective
		float getPerspectiveFactor( float positionX ){
			
			//Before perspective point
			if( positionX <= perspectiveX ) return 1.0;
			
			// Simple linear function y = 1 - (poistionX-perspectiveX)/(worldEndX - perspectiveX);
			float a = 0.8 / (worldEndX - perspectiveX);
			float scale = 1 - a*(positionX - perspectiveX); 
			
			return scale;
		}

		
};

#endif // __GAME_OBJECT_H__
