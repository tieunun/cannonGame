#include "Bullet.h"

using namespace cocos2d;

void Bullet::createBullet( b2Vec2 startPoint , float scale ){

	setScale(scale);

	this->startX = startPoint.x;
	this->startY = startPoint.y;

	b2BodyDef bulletDef;
	bulletDef.position.Set( startX, startY );
	bulletDef.type = b2_dynamicBody;

	bulletBody = m_world->CreateBody(&bulletDef);
	ghostBody = m_world->CreateBody(&bulletDef);

	b2CircleShape bulletShape;
	bulletShape.m_radius = scale*m_radius; 

	b2FixtureDef bulletFixtureDef;
	bulletFixtureDef.density = m_density;

	//set Colliding
	bulletFixtureDef.filter.categoryBits = BULLET;
	bulletFixtureDef.filter.maskBits = GROUND | ENEMY ;

	bulletFixtureDef.shape = &bulletShape;
	bulletBody->CreateFixture(&bulletFixtureDef);

	//ghost doesn`t collide
	bulletFixtureDef.filter.maskBits = 0;
	ghostBody->CreateFixture(&bulletFixtureDef);

	bulletSprite = cocos2d::Sprite::create("bullet.png",  cocos2d::CCRectMake(0, 0, 55, 56));
	bulletSprite->setPosition( cocos2d::Vec2( (startX+0.0)*PTM_RATIO , (startY+0.0)*PTM_RATIO ) );
	bulletSprite->setScale(scale);
	layer->addChild( bulletSprite , 2 );
	
	bulletBody->SetUserData(this);
	
 }
 
Bullet::~Bullet() {
	
	if( bulletBody != NULL ) m_world->DestroyBody( bulletBody );
	if( ghostBody != NULL ) m_world->DestroyBody( ghostBody );
	if( bulletSprite != NULL ) layer->removeChild( bulletSprite );
	if( explosionBath != NULL ) layer->removeChild( explosionSprite );
	
	for(int i = 0 ; i < shrapnels.size(); i++)
		delete shrapnels[i];
		
	shrapnels.clear();
}

void Bullet::shoot(b2Vec2 direction, float power){
	if( bulletBody != NULL )
	{
		
		// MAX POWER IS WHEN MAX HEIGHT IS IN PERSPECTIVEX POINT
		// SO MAX DISTANCE IS 2*PERSPECTIVEX
		
		float angle = atan2f(direction.y,direction.x)+M_PI;
		float desiredV0 = sqrtf( (2*perspectiveX/PTM_RATIO)*10.0/sinf(2*M_PI/4) );
		float desiredForce = bulletBody->GetMass()*desiredV0;
		
		direction.Normalize();
		
		bulletBody->ApplyLinearImpulse( b2Vec2(-desiredForce*direction.x, -desiredForce*direction.y), bulletBody->GetWorldCenter(), NULL );

		//same thing for ghost
		ghostBody->ApplyLinearImpulse( b2Vec2(-desiredForce*direction.x, -desiredForce*direction.y), bulletBody->GetWorldCenter(), NULL );
		
		float v0 = bulletBody->GetLinearVelocity().Length();
		float distance = ( v0*v0*sinf( 2*angle) )/10.0;

		
		g_screenLog->log( LL_INFO , "v0 = %f% ", v0  );
		g_screenLog->log( LL_INFO , "Angle = %f%", angle*RADTODEG );
		g_screenLog->log( LL_INFO , "Distance = %f%", distance );
		
	}
}

void Bullet::explode(){

	markedToExplode = false;
	exploded = true;
	
	if( bulletBody == NULL ) return;

	b2Vec2 center = bulletBody->GetWorldCenter();
	float blastPower = 90000000.0;
		
	int numRays = 10;
	for (int i = 0; i < numRays; i++) {
		float angle = ((i / (float)numRays)+ rand()%30 ) * 360 * DEGTORAD;
		b2Vec2 rayDir( sinf(angle), cosf(angle) );

		Shrapnel * s = new Shrapnel(layer, m_world, blastPower, 100, worldStartX, worldEndX, perspectiveX );
		s->setScale(scale);
		s->createShrapnel( center, rayDir );
		shrapnels.push_back(s);
	}
	
	createExplosionSprite();
	
	if( bulletBody != NULL ) m_world->DestroyBody( bulletBody );
	if( ghostBody != NULL ) m_world->DestroyBody( ghostBody );
	if( bulletSprite != NULL ) layer->removeChild( bulletSprite );
	
	bulletBody = NULL;
	ghostBody = NULL;
	bulletSprite = NULL;
	
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

	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("explotion.plist");
	explosionBath=SpriteBatchNode::create("explotion.png");
	layer->addChild(explosionBath);

	explosionSprite=Sprite::createWithSpriteFrameName("explotion_01.png");
	auto animation=Animation::create();

	for(int i=1;i<=3;i++)
	{
		char szName[100]={0};
		sprintf(szName,"explotion_0%d.png",i);
		animation->addSpriteFrame(SpriteFrameCache::getInstance()->spriteFrameByName(szName));
	}

	animation->setDelayPerUnit(0.1f); 
	explosionSprite->setPosition( cocos2d::Vec2( (bulletBody->GetPosition().x+0.0)*PTM_RATIO , (bulletBody->GetPosition().y+0.0)*PTM_RATIO ) );
	
	explosionSprite->runAction(  CCAnimate::create(animation) ); 
	explosionBath->addChild(explosionSprite);
	
	explosionBody->SetUserData(this);
	
	layer->runAction(cocos2d::CCFollow::create(explosionSprite,  Rect(worldStartX*layer->getScale(), worldStartX*layer->getScale(), worldEndX*layer->getScale() , worldEndX*layer->getScale() ) ));

}

bool Bullet::updateSprites(){
	
	if(exploded ){
		
		explosionFramesCounter++;	

		if( explosionFramesCounter > 100 ){
			layer->removeChild( explosionBath );
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
		else if( bulletBody->GetPosition().x * PTM_RATIO > worldEndX )
		{
			explode();
		}
		else{
	
			if(!follow)
			{
				//SET WIN SIZE
				Size winSize = Director::getInstance()->getVisibleSize();
				if( bulletBody->GetPosition().x * PTM_RATIO > winSize.width/2 || bulletBody->GetPosition().y * PTM_RATIO > winSize.height/2 )
				{
					follow = true;
					layer->runAction(cocos2d::CCFollow::create(bulletSprite,  Rect(worldStartX*layer->getScale(), worldStartX*layer->getScale(), worldEndX*layer->getScale() , worldEndX*layer->getScale() ) ));
				}
			}
			
			bulletSprite->setPosition( cocos2d::ccp( bulletBody->GetPosition().x * PTM_RATIO , bulletBody->GetPosition().y * PTM_RATIO) );
			bulletSprite->setRotation( -1 * CC_RADIANS_TO_DEGREES( bulletBody->GetAngle()) );
			
			//setPerspective
			float perspectiveFactor = getPerspectiveFactor( bulletBody->GetPosition().x * PTM_RATIO );
			bulletSprite->setScale( perspectiveFactor * scale );
			
			//realistic perspective body movement
			reduceBulletSpeed( perspectiveFactor );

		}
	}

	return true;
};

void Bullet::reduceBulletSpeed( float perspectiveFactor )
{
	float minVelocity = 5.0;
	if( bulletBody->GetLinearVelocity().Length() <= minVelocity ){
			
		g_screenLog->log( LL_INFO , " !Minimum bullet speed reached " );
		return;
	}
	
	//get velocity direction
	b2Vec2 normalizedVelocityVector = ghostBody->GetLinearVelocity();
	normalizedVelocityVector.Normalize();
	
	//get velocity values
	float currentVelocity = ghostBody->GetLinearVelocity().Length();
	float desiredVelocity = perspectiveFactor*currentVelocity;
	
	//calculate force needed to set desired speed
	//float neededForce = ghostBody->GetMass()*(desiredVelocity-currentVelocity);
	
	//apply counter impulse
	//bulletBody->ApplyForce( neededForce*normalizedVelocityVector, bulletBody->GetWorldCenter(), NULL );	
	bulletBody->SetLinearVelocity(desiredVelocity*normalizedVelocityVector);

}

Sprite * Bullet::getMainSprite(){
	return bulletSprite;
}
