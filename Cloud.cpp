#include "Cloud.h"

using namespace cocos2d;

void Cloud::generateRandomCloud( Point startPosition ){
	
	this->startPosition = startPosition;
	
	spriteCount = 3 + rand() % (MAX_CLOUD_SPRITES-4);
	
	cloudWidth = 100 + rand() % 300;
	cloudHeight = 50 + rand() % 150;
	
	int zOrder = -5+rand()%5;
	
	for( int i = 0 ; i < spriteCount ; i++ ){
		
		float spriteWidth = 0.6*cloudWidth + ( rand() % (int)(0.5*cloudWidth) ) ; 
		
		float bLX = startPosition.x + rand() % (int)(cloudWidth);
		float bLY = startPosition.y + rand() % (int)(0.3*cloudHeight);

		cloudSprites[i] = Sprite::create("cloud.png", Rect( 0 , 0 , 400 , 400 ) ); 
		cloudSprites[i]->setScale( spriteWidth/400 );

		cloudSprites[i]->setPosition( bLX , bLY );
	}
	
	for( int i = 0 ; i < spriteCount ; i++ )
		layer->addChild( cloudSprites[i] , zOrder );
}


void Cloud::createCloud( Point startPosition ){
	
	generateRandomCloud( startPosition );
	
};
	
void Cloud::updateSprites(){

	for( int i = 0 ; i < spriteCount ; i++ )
	{
		float perspectiveFactor = getCloudPerspectiveFactor( cloudSprites[i]->getPosition().x );

		Point currentPosition = cloudSprites[i]->getPosition();
		
		//KILL this cloud and GENERATE NEW ONE
		if( currentPosition.x > worldEndX + 100 )
		{
			disposeCloud();
			generateRandomCloud( Point( worldStartX-300 , startPosition.y  ) );
			return;
		}
		if( currentPosition.y < 100 + perspectiveFactor*200 )
			currentPosition.y += 2;
		
		cloudSprites[i]->setPosition( currentPosition.x+perspectiveFactor*baseMoveSpeed, currentPosition.y + (1-perspectiveFactor)*(-0.25+(rand()%10)/30) );		
		
		if( cloudSprites[i]->getScale() > perspectiveFactor ) cloudSprites[i]->setScale( perspectiveFactor );
		//if( cloudSprites[i]->getScaleY() > perspectiveFactor ) cloudSprites[i]->setScaleY( perspectiveFactor );
	}

};

// Implementation of perspective
float Cloud::getCloudPerspectiveFactor( float positionX ){
	
	//Before worldStart point
	if( positionX <= worldStartX ) return 1.0;
	
	float a = 0.9 / (worldEndX - worldStartX);
	float scale = 1 - a*(positionX - worldStartX); 
	
	return scale;
}

Cloud::~Cloud(){
	disposeCloud();
};

void Cloud::disposeCloud(){
	for( int i = 0 ; i < spriteCount ; i++ )
		if( cloudSprites[i] ) layer->removeChild( cloudSprites[i] );	
	spriteCount = 0;
}
