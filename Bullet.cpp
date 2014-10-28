#include "Bullet.h"

using namespace cocos2d;

void Bullet::createBullet( b2Vec2 startPoint ){

	this->startX = startPoint.x;
	this->startY = startPoint.y;

	b2BodyDef bulletDef;
	bulletDef.position.Set( startX, startY );
	bulletDef.type = b2_dynamicBody;

	bulletBody = m_world->CreateBody(&bulletDef);

	b2CircleShape bulletShape;
	bulletShape.m_radius = m_radius; 

	b2FixtureDef bulletFixtureDef;
	bulletFixtureDef.density = m_density;

	//set Colliding
	bulletFixtureDef.filter.categoryBits = BULLET;
	bulletFixtureDef.filter.maskBits = GROUND | ENEMY ;

	bulletFixtureDef.shape = &bulletShape;
	bulletBody->CreateFixture(&bulletFixtureDef);

	bulletSprite = cocos2d::Sprite::create("bullet.png",  cocos2d::CCRectMake(0, 0, 55, 56));
	bulletSprite->setPosition( cocos2d::Vec2( (startX+0.0)*PTM_RATIO , (startY+0.0)*PTM_RATIO ) );
	layer->addChild( bulletSprite , 1 );
	
	bulletBody->SetUserData(this);
	
 }
 
Bullet::~Bullet() {
	
		for(int i = 0 ; i < shrapnels.size(); i++)
			delete shrapnels[i];
		
		shrapnels.clear();
}

void Bullet::shoot(b2Vec2 direction, float power){
	if( bulletBody != NULL )
		bulletBody->ApplyLinearImpulse( b2Vec2(-power*direction.x, -power*direction.y), bulletBody->GetWorldCenter(), NULL );
}

void Bullet::explode(){

	markedToExplode = false;
	exploded = true;
	
	if( bulletBody == NULL ) return;

	b2Vec2 center = bulletBody->GetWorldCenter();
	float blastPower = 90000000.0;
		
	int numRays = 30;
	for (int i = 0; i < numRays; i++) {
		float angle = (i / (float)numRays) * 360 * DEGTORAD;
		b2Vec2 rayDir( sinf(angle), cosf(angle) );

		Shrapnel * s = new Shrapnel(layer, m_world, blastPower, 100 );
		s->createShrapnel( center, rayDir );
		shrapnels.push_back(s);
	}
	
	createExplosionSprite();
	
	m_world->DestroyBody( bulletBody );
	
	layer->removeChild( bulletSprite );
	
	explosionFramesCounter=0;

}

void Bullet::createExplosionSprite()
{
	b2BodyDef explosionDef;
	explosionDef.position.Set( bulletBody->GetPosition().x, bulletBody->GetPosition().y );
	explosionDef.type = b2_staticBody;

	explosionBody = m_world->CreateBody(&explosionDef);

	b2CircleShape explosionShape;
	explosionShape.m_radius = 5; 

	b2FixtureDef explosionFixtureDef;
	
	explosionFixtureDef.shape = &explosionShape;

	//set Colliding
	explosionFixtureDef.filter.categoryBits = BULLET;
	explosionFixtureDef.filter.maskBits = GROUND ;
				
	explosionBody->CreateFixture(&explosionFixtureDef);

	explosionSprite = cocos2d::Sprite::create("explosion.png",  cocos2d::CCRectMake(0, 0, 100, 100));
	explosionSprite->setPosition( cocos2d::Vec2( (bulletBody->GetPosition().x+0.0)*PTM_RATIO , (bulletBody->GetPosition().y+0.0)*PTM_RATIO ) );
	layer->addChild( explosionSprite , 1 );
	
	explosionBody->SetUserData(this);
	
	layer->runAction(cocos2d::CCFollow::create(explosionSprite,  cocos2d::CCRectMake(0, 0, 20000, 1300) ));
}

bool Bullet::updateSprites(){
	
	if(exploded ){
		
		explosionFramesCounter++;	

		if( explosionFramesCounter > 100 ){
			layer->removeChild( explosionSprite );
			m_world->DestroyBody( explosionBody );
			return false;
		}
		else{
			
			for( int i = 0 ; i < shrapnels.size() ; i++ )
				shrapnels[i]->updateSprites();
			
		}
	}	
	else {
		if( markedToExplode ){
			explode();
		}
		else{
	
			if(bulletBody->GetPosition().x * PTM_RATIO > 1200 )
				layer->runAction(cocos2d::CCFollow::create(bulletSprite,  cocos2d::CCRectMake(0, 0, layer->getScale()*20000, layer->getScale()*1300) ));
	
	
			bulletSprite->setPosition( cocos2d::ccp( bulletBody->GetPosition().x * PTM_RATIO , bulletBody->GetPosition().y * PTM_RATIO) );
			bulletSprite->setRotation( -1 * CC_RADIANS_TO_DEGREES( bulletBody->GetAngle()) );
		}
	}

	return true;
};

Sprite * Bullet::getMainSprite(){
	return bulletSprite;
}
