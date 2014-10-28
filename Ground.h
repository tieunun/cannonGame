#ifndef __GROUND_H__
#define __GROUND_H__

#include "GameObject.h"

#define maxHillPoints  1000
#define hillSegmentWidth  10
#define maxVertexPoints 7000

using namespace cocos2d;

class Ground : public GameObject , public Node{
	
private:
	
	Size winSize;
	
	// OFFSETS TODO!
	int offsetX, offsetXTexture, hillPointsCount;
	
	// USED TO DETERMINE HOW MUCH SHOULD BE SHOWN
	int startPoint, endPoint, prevStartPoint, prevEndPoint;
	
	// USED TO DRAW TRIANGES
	Point hillPoints[maxHillPoints];
	Point borderPoints[maxHillPoints];
	Point vertexPoints[maxVertexPoints];
	Point textureCoords[maxVertexPoints];
	int borderVerticesCount;
	int hillVerticesCount;
	
	// CUSTOM COMMANDS FOR OPENGL
	CustomCommand drawHillsCommand;
	CustomCommand drawStripesCommand;
	
	// B2GROUND BODY
	b2Body * groundBody;
	
	// TEXTURE
	Sprite * noiseSprite;
	Sprite * textureSprite;
	int nStripes;
	float textureWidth;
	float textureHeight;
	
public:
	
	Ground( Layer * layer , b2World * world) 
	:GameObject( layer , world, new std::string("GROUND") ) {

		winSize = Director::getInstance()->getVisibleSize();
		
		textureSprite = NULL;
		groundBody = NULL;
		noiseSprite = NULL;
		
		offsetX = 0;
		offsetXTexture = 0;
		hillPointsCount = 0;
		startPoint = 0;
		endPoint = 0;
		prevStartPoint = -1;
		prevEndPoint = -1;
		
	}
	
	//UPDATE
	void update();
	
	//RANDOM GROUND GENERATION
	void initGround();
	void generateHills();
	
	//TRIANGLES
	void setBounds();
	void setVertex();
	void drawHills();

	//B2BODY
	void setBox2DBody();
	void drawBox2DGround();
	
	//TEXTURING
	void setTexture();
	Color4F randomBrightColor();
	Texture2D * createTexture( Color4F bgColor, float textureWidth, float textureHeight, int nStripes );
	void drawTexture();
	
	//GETTERS SETTERS
	void setOffsetX( int offset );
	void setOffsetXTexture( int offset );

};

#endif // __GROUND_H__
