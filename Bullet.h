#ifndef __BULLET_H__
#define __BULLET_H__

#include "GameObject.h"
#include "Shrapnel.h"

using namespace cocos2d;

class Bullet : public GameObject {
	
	private:

		b2Body* bulletBody;
		b2Body* explosionBody;

		Sprite * bulletSprite;
		Sprite * explosionSprite;
	
		float m_radius, m_density ;
				
	public:

		int startX, startY, explosionFramesCounter;
		std::vector <Shrapnel*> shrapnels;
		bool exploded, markedToExplode;
			
		Bullet( Layer * layer, b2World * world, float radius, float density ) 
		: GameObject( layer, world , new std::string("BULLET") ) {

			bulletBody = NULL;
			bulletSprite = NULL;
			explosionSprite = NULL;

			m_radius = radius;
			m_density = density;

			markedToExplode = false;
			exploded = false;
			explosionFramesCounter=0;

			startX = 0.0;
			startY = 0.0;

		}

		void createBullet( b2Vec2 startPoint );

		~Bullet();

		void shoot(b2Vec2 direction, float power);

		void explode();

		void createExplosionSprite();

		bool updateSprites();

		Sprite * getMainSprite();
};

#endif // __BULLET_H__
