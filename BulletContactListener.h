#ifndef __BULLET_CONTACT_LISTENER_H__
#define __BULLET_CONTACT_LISTENER_H__

#include "Bullet.h"
#include "cocos2d.h"
#include "Box2D/Box2D.h"

using namespace cocos2d;

class BulletContactListener : public b2ContactListener
{
	
	void BeginContact(b2Contact* contact) {
   
		if(!contact->IsTouching()) return;
   
		//check if fixture A was a projectile
		void* bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
		if ( bodyUserData )
		{
			if( strcmp( static_cast<GameObject*>( bodyUserData )->name->c_str() , "BULLET" ) == 0 )
				static_cast<Bullet*>( bodyUserData )->markedToExplode = true;
		}
			
		//check if fixture B was a ball
		bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
		if ( bodyUserData )
		{
			if( strcmp( static_cast<GameObject*>( bodyUserData )->name->c_str() , "BULLET" )  == 0 )
				static_cast<Bullet*>( bodyUserData )->markedToExplode = true;
		}		
    }
};

#endif // __BULLET_CONTACT_LISTENER_H__
