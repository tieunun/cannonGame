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
	int offsetX, offsetXTexture, hillPointsCount, startPoint, endPoint, prevStartPoint, prevEndPoint;
	Point hillPoints[maxHillPoints];
	Point borderPoints[maxHillPoints];
	Point vertexPoints[maxVertexPoints];
	Point textureCoords[maxVertexPoints];
	int _nBorderVertices;
	int _nHillVertices;
	CustomCommand drawHillsCommand;
	CustomCommand drawStripesCommand;
	
	b2Body * groundBody;
	
	//texture data
	Sprite * noiseSprite;
	Sprite * textureSprite;
	int nStripes;
	float textureWidth;
	float textureHeight;
	
public:
	
	Ground( Layer * layer , b2World * world) 
	:GameObject( layer , world, new std::string("GROUND") ) {
		offsetX = 0;
		offsetXTexture = 0;
		hillPointsCount = 0;
		winSize = Director::getInstance()->getVisibleSize();
		startPoint = 0;
		endPoint = 0;
		prevStartPoint = -1;
		prevEndPoint = -1;
		textureSprite = NULL;
		groundBody = NULL;
		noiseSprite = NULL;
	}
	
	void initGround();
	void generateHills();
	void update();
	void setBounds();
	void setVertex();
	void drawHills();
	void drawBox2DGround();
	void setOffsetX( int offset );
	void setOffsetXTexture( int offset );
	void setBox2DBody();
	
	Texture2D * createTexture( Color4F bgColor, float textureWidth, float textureHeight, int nStripes );

	void drawStripes();

	Color4F randomBrightColor();
	
	void setTexture();
	

};

#endif // __GROUND_H__
