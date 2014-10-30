#include "Cloud.h"

using namespace cocos2d;

void Cloud::generateRandomCloud(){
	
	spriteCount = 3 + rand() % (MAX_CLOUD_SPRITES-4);
	
	cloudWidth = 200 + rand() % 300;
	cloudHeight = 100 + rand() % 200;
	
	for( int i = 0 ; i < spriteCount ; i++ ){
		
		float spriteWidth = 100 + rand() % (cloudWidth/spriteCount) ; 
		float spriteHeight = 20 + rand() % (cloudHeight/spriteCount) ; 
		
		float bLX = startPosition.x + rand() % cloudWidth;
		float bLY = startPosition.y + rand() % cloudHeight;

		cloudSprites[i] = Sprite::create("bg.png", Rect( 0 , 0 , spriteWidth , spriteHeight ) ); 
		cloudSprites[i]->setPosition( bLX , bLY );
	}
	
}

void Cloud::createCloud( Point startPosition ){
	
	this->startPosition = startPosition;
	generateRandomCloud( );
	
	for( int i = 0 ; i < spriteCount ; i++ )
		layer->addChild( cloudSprites[i] , 0 );
	
};
	
void Cloud::updateSprites(){

	for( int i = 0 ; i < spriteCount ; i++ )
	{
		float perspectiveFactor = getPerspectiveFactor( cloudSprites[i]->getPosition().x );

		Point currentPosition = cloudSprites[i]->getPosition();
		if( currentPosition.x > worldEndX - 50 )
		{
			currentPosition.x = worldStartX-100;
			currentPosition.y = startPosition.y + rand() % cloudHeight;
		}
		if( currentPosition.y < 100 + perspectiveFactor*200 )
			currentPosition.y += 2;
		
		cloudSprites[i]->setPosition( currentPosition.x+perspectiveFactor*baseMoveSpeed, currentPosition.y + (1-perspectiveFactor)*(-0.5+(rand()%10)/30) );		
		
		cloudSprites[i]->setScale( perspectiveFactor );
	}

};

void Cloud::reduceCloudSpeed( float ){
	
};

Cloud::~Cloud(){
	for( int i = 0 ; i < spriteCount ; i++ )
		if( cloudSprites[i] ) layer->removeChild( cloudSprites[i] );	
};
