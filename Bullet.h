#ifndef __BULLET_H__
#define __BULLET_H__

#include "GameObject.h"
#include "Shrapnel.h"

using namespace cocos2d;

class Bullet : public GameObject {
	
	private:

		b2Body* ghostBody;
		b2Body* bulletBody;
		b2Body* explosionBody;

		Sprite * bulletSprite;
		
		SpriteBatchNode * explosionBath;
		Sprite * explosionSprite;
	
		bool follow;
		float m_radius, m_density ;
		float bulletScale;
				
	public:

		int startX, startY, explosionFramesCounter;
		std::vector <Shrapnel*> shrapnels;
		bool exploded, markedToExplode;
			
		Bullet( Layer * layer, b2World * world, float radius, float density, float worldStartX, float worldEndX, float perspectiveX ) 
		: GameObject( layer, world , worldStartX, worldEndX, perspectiveX, new std::string("BULLET") ) {

			ghostBody = NULL;
			bulletBody = NULL;
			bulletSprite = NULL;
			explosionSprite = NULL;
			explosionBath = NULL;

			m_radius = radius;
			m_density = density;

			markedToExplode = false;
			exploded = false;
			explosionFramesCounter=0;

			bulletScale = 1;
			startX = 0.0;
			startY = 0.0;
			
			follow = false;

		}

		void createBullet( b2Vec2 startPoint, float scale );

		~Bullet();

		void shoot(b2Vec2 direction, float power);

		void explode();

		void createExplosionSprite();

		bool updateSprites();
		
		void reduceBulletSpeed( float );

		Sprite * getMainSprite();
};

#endif // __BULLET_H__
