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
	
	b2Body * groundBody;
	
public:

	Sprite * stripes;
	
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
		stripes = NULL;
		groundBody = NULL;
	}
	
	void generateHills();
	void update();
	void setBounds();
	void setVertex();
	void drawHills();
	void drawBox2DGround();
	void setOffsetX( int offset );
	void setOffsetXTexture( int offset );
	void setBox2DBody();

};

#endif // __GROUND_H__
