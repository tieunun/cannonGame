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
	
	void generateHills() {
		
		Size winSize = Director::getInstance()->getVisibleSize();
		
		float x = 0;
		float y = winSize.height / 4;
    
		hillPoints[0] = Point( -3*winSize.width/4 , winSize.height );
    
		for(int i = 1; i < maxHillPoints; ++i ) {
			hillPoints[i] = Point(x, y);
			x += winSize.width/4;
			y = (rand() % (int) winSize.height/2 );
			hillPointsCount++;
		}
		
		setBounds();
		setBox2DBody();
		
	}
	 
	void update(){
		drawHillsCommand.init( layer->getGlobalZOrder() );
		drawHillsCommand.func = CC_CALLBACK_0( Ground::drawHills , this );
		auto renderer = Director::getInstance()->getRenderer();
		renderer->addCommand(&drawHillsCommand);
	}
	 
	void setBounds(){
		
		while( hillPoints[startPoint].x < offsetX-winSize.width*4/8/layer->getScale() )
			startPoint++;

		endPoint = startPoint;
			
		while( hillPoints[endPoint].x < offsetX+winSize.width*12/8/layer->getScale() )
			endPoint++;
	
		setVertex();
		setBox2DBody();
	
		prevStartPoint = startPoint;
		prevEndPoint = endPoint; 
					
	}
	
	void setVertex()
	{
		float minY = 0;
		
		// vertices for visible area
		_nHillVertices = 0;
		_nBorderVertices = 0;
		Point p0, p1, pt0, pt1;
		p0 = Point( (hillPoints[startPoint].x - offsetX) * layer->getScale() , hillPoints[startPoint].y  * layer->getScale() ) ;
		for (int i=startPoint+1; i<endPoint+1; i++) {
			p1 =Point( ( hillPoints[i].x - offsetX )  * layer->getScale() , hillPoints[i].y  * layer->getScale() );
	 
			// triangle strip between p0 and p1
			int hSegments = floorf((p1.x-p0.x)/hillSegmentWidth);
			float dx = (p1.x - p0.x) / hSegments;
			float da = M_PI / hSegments;
			float ymid = (p0.y + p1.y) / 2;
			float ampl = (p0.y - p1.y) / 2;
			pt0 = p0;
			borderPoints[_nBorderVertices++] = pt0;
			for (int j=1; j<hSegments+1; j++) {
				pt1.x = p0.x + j*dx;
				pt1.y = ymid + ampl * cosf(da*j);
				borderPoints[_nBorderVertices++] = pt1;
	 
				vertexPoints[_nHillVertices] = Point(pt0.x, 0 );
				textureCoords[_nHillVertices++] = Point((pt0.x-offsetXTexture)/512, 1.0f );
				vertexPoints[_nHillVertices] = Point(pt1.x, 0 );
				textureCoords[_nHillVertices++] = Point((pt1.x-offsetXTexture)/512, 1.0f );
	 
				vertexPoints[_nHillVertices] = Point(pt0.x, pt0.y);
				textureCoords[_nHillVertices++] = Point((pt0.x-offsetXTexture)/512, 0);
				vertexPoints[_nHillVertices] = Point(pt1.x, pt1.y);
				textureCoords[_nHillVertices++] = Point((pt1.x-offsetXTexture)/512, 0);
	 
				pt0 = pt1;
			}
	 
			p0 = p1;
		}
		
	}
	 
	void drawHills(){
		
		Size winSize = Director::getInstance()->getVisibleSize();

		setBounds();

		// INIT OPENGL
		this->setGLProgram(ShaderCache::getInstance()->getGLProgram(GLProgram::SHADER_NAME_POSITION_TEXTURE));
		this->getGLProgram()->use();
		this->getGLProgram()->setUniformsForBuiltins();
		
		// DRAW VERTEX
 		CC_NODE_DRAW_SETUP();
		
		if( stripes != NULL ) ccGLBindTexture2D(stripes->getTexture()->getName());

		GL::enableVertexAttribs( GL::VERTEX_ATTRIB_FLAG_POSITION | GL::VERTEX_ATTRIB_FLAG_TEX_COORD );
		
		ccDrawColor4F(1.0f, 1.0f, 1.0f, 1.0f);
		
		glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, vertexPoints);
		glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, textureCoords);
		
		glDrawArrays( GL_TRIANGLE_STRIP , 0, (GLsizei)_nHillVertices );
		
		drawBox2DGround();

	}
	
	void drawBox2DGround(){
		
		glLineWidth(10.0f);
		ccDrawColor4F( 0.5, 0.5, 0.5, 0.5 );

		for(int i = startPoint ; i < endPoint ; ++i) {    
			
			ccDrawLine( Point( (hillPoints[i-1].x-offsetX) * layer->getScale() , hillPoints[i-1].y * layer->getScale() ) , Point( (hillPoints[i].x-offsetX) * layer->getScale() , hillPoints[i].y * layer->getScale() ) );    
				
		}
		
		glLineWidth(10.0f);
		ccDrawColor4F( 1, 0.5, 0.5, 0.5 );

		int prevIndex = 0;
		for(int i = 1 ; i < _nBorderVertices-1 ; ++i) {    
			
			if( borderPoints[i].x - borderPoints[prevIndex].x > 50 )
			{
				ccDrawLine( Point(borderPoints[prevIndex].x , borderPoints[prevIndex].y) , Point(borderPoints[i].x , borderPoints[i].y ) );    
				prevIndex=i;
			}
			
		}

	}
	
	void setOffsetX( int offset )
	{
		if(offset > 0 )	this->offsetX = offset;
		else this->offsetX = 0;
	}

	void setOffsetXTexture( int offset )
	{
		if(offset > 0 )	this->offsetXTexture = offset;
		else this->offsetXTexture = 0;
	}

	void setBox2DBody() {
	 
		if(groundBody != NULL) {
			m_world->DestroyBody(groundBody);
		}
	 
		b2BodyDef bd;
		bd.type = b2_staticBody;
		bd.position.Set(0, 0);
	 
		groundBody = m_world->CreateBody(&bd);
	
		b2ChainShape shape;
	 
		b2FixtureDef groundFixtureDef;
		groundFixtureDef.density = 100;
		
		//set Colliding
		groundFixtureDef.filter.categoryBits = GROUND;
		groundFixtureDef.filter.maskBits = CANNON | BULLET;

		b2Vec2 border[ maxHillPoints ];
		
		int prevIndex = 0;
		int count = 0;
		for(int i = 1 ; i < _nBorderVertices-1 ; ++i) {    
			if( borderPoints[i].x - borderPoints[prevIndex].x > 50 )
			{
				border[count].Set( (borderPoints[i].x)/PTM_RATIO , (borderPoints[i].y)/PTM_RATIO ) ;    
				prevIndex=i;
				count++;
			}
		}
		
		shape.CreateChain( border , count );

		groundFixtureDef.shape = &shape;
		groundBody->CreateFixture(&groundFixtureDef);

	}



};

#endif // __GROUND_H__
