#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#include "Box2D/Box2D.h"
#include "cocos2d.h"

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
	
		float scale;
	
	public:
		
		std::string * name;
		
		GameObject( cocos2d::Layer * layer, b2World * world, std::string * name )
		{ 
			this->m_world = world;
			this->name = name;
			this->layer = layer;
			
			scale = 1;
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
		
		
};

#endif // __GAME_OBJECT_H__
