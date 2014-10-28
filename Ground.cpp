#include "Ground.h"

using namespace cocos2d;

// UPDATING

void Ground::update(){
	drawHillsCommand.init( layer->getGlobalZOrder() );
	drawHillsCommand.func = CC_CALLBACK_0( Ground::drawHills , this );
	Director::getInstance()->getRenderer()->addCommand(&drawHillsCommand);	
}

// RANDOM GROUND GENERATING

void Ground::initGround(){
	setTexture();
	generateHills();
}

void Ground::generateHills() {
	
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

// TRIANGLE DRAWING 

void Ground::setBounds(){
	
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

void Ground::setVertex()
{
	float minY = 0;
	
	// vertices for visible area
	hillVerticesCount = 0;
	borderVerticesCount = 0;
	Point p0, p1, pt0, pt1;
	p0 = Point( (hillPoints[startPoint].x - offsetX) * layer->getScale() , hillPoints[startPoint].y  * layer->getScale() ) ;
	for (int i = startPoint+1; i < endPoint+1; i++) {
		p1 =Point( ( hillPoints[i].x - offsetX )  * layer->getScale() , hillPoints[i].y  * layer->getScale() );
 
		// triangle strip between p0 and p1
		int hSegments = floorf((p1.x-p0.x)/hillSegmentWidth);
		float dx = (p1.x - p0.x) / hSegments;
		float da = M_PI / hSegments;
		float ymid = (p0.y + p1.y) / 2;
		float ampl = (p0.y - p1.y) / 2;
		pt0 = p0;
		borderPoints[ borderVerticesCount++ ] = pt0;
		for (int j=1; j<hSegments+1; j++) {
			pt1.x = p0.x + j*dx;
			pt1.y = ymid + ampl * cosf(da*j);
			borderPoints[ borderVerticesCount++ ] = pt1;
 
			vertexPoints[ hillVerticesCount ] = Point(pt0.x, 0 );
			textureCoords[ hillVerticesCount++ ] = Point((pt0.x-offsetXTexture)/512, 1.0f );
			vertexPoints[ hillVerticesCount ] = Point(pt1.x, 0 );
			textureCoords[ hillVerticesCount++ ] = Point((pt1.x-offsetXTexture)/512, 1.0f );
 
			vertexPoints[ hillVerticesCount ] = Point(pt0.x, pt0.y);
			textureCoords[ hillVerticesCount++ ] = Point((pt0.x-offsetXTexture)/512, 0);
			vertexPoints[ hillVerticesCount ] = Point(pt1.x, pt1.y);
			textureCoords[ hillVerticesCount++ ] = Point((pt1.x-offsetXTexture)/512, 0);
 
			pt0 = pt1;
		}
 
		p0 = p1;
	}
	
}
 
void Ground::drawHills(){
	
	Size winSize = Director::getInstance()->getVisibleSize();

	setBounds();

	// INIT OPENGL
	this->setGLProgram(ShaderCache::getInstance()->getGLProgram(GLProgram::SHADER_NAME_POSITION_TEXTURE));
	this->getGLProgram()->use();
	this->getGLProgram()->setUniformsForBuiltins();
	
	// ATTACH TEXTURE
	if( textureSprite != NULL ) ccGLBindTexture2D( textureSprite->getTexture()->getName() );

	// DRAW TRAINGLES
	CC_NODE_DRAW_SETUP();
	
	GL::enableVertexAttribs( GL::VERTEX_ATTRIB_FLAG_POSITION | GL::VERTEX_ATTRIB_FLAG_TEX_COORD );
	
	ccDrawColor4F(1.0f, 1.0f, 1.0f, 1.0f);
	
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, vertexPoints);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, textureCoords);
	
	glDrawArrays( GL_TRIANGLE_STRIP , 0, (GLsizei) hillVerticesCount );
	
	drawBox2DGround();

}

// BOX2D

void Ground::drawBox2DGround(){

	//DRAW BASIC LINE
	
	glLineWidth(10.0f);
	ccDrawColor4F( 0.5, 0.5, 0.5, 0.5 );

	for(int i = startPoint ; i < endPoint ; ++i) {    
		
		ccDrawLine( Point( (hillPoints[i-1].x-offsetX) * layer->getScale() , hillPoints[i-1].y * layer->getScale() ) , Point( (hillPoints[i].x-offsetX) * layer->getScale() , hillPoints[i].y * layer->getScale() ) );    
			
	}
	
	//DRAW FINAL LINE
	
	glLineWidth(10.0f);
	ccDrawColor4F( 1, 0.5, 0.5, 0.5 );

	int prevIndex = 0;
	for(int i = 1 ; i < borderVerticesCount-1 ; ++i) {    
		
		if( borderPoints[i].x - borderPoints[prevIndex].x > 50 )
		{
			ccDrawLine( Point(borderPoints[prevIndex].x , borderPoints[prevIndex].y) , Point(borderPoints[i].x , borderPoints[i].y ) );    
			prevIndex=i;
		}
		
	}

}

void Ground::setBox2DBody() {
 
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
	for(int i = 1 ; i < borderVerticesCount-1 ; ++i) {    
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

// TEXTURE

void Ground::setTexture() {

	Color4F bgColor = randomBrightColor();
	cocos2d::Texture2D::TexParams params = {GL_LINEAR, GL_LINEAR, GL_REPEAT,  GL_REPEAT};
	
	Texture2D * texture = createTexture(bgColor, 512.0 , 512.0 , 6);
	
	textureSprite = Sprite::createWithTexture( texture );
	textureSprite->getTexture()->setTexParameters(params);
	textureSprite->retain();
	
}

Texture2D * Ground::createTexture( Color4F bgColor, float textureWidth, float textureHeight, int nStripes ){

	auto *rt = RenderTexture::create(textureWidth, textureHeight);

	rt->beginWithClear(bgColor.r, bgColor.g, bgColor.b, bgColor.a);

		//set texture parameters
		this->textureWidth = textureWidth;
		this->textureHeight = textureHeight;
		this->nStripes = nStripes;
		
		//make custom openGL render
		drawStripesCommand.init( rt->getGlobalZOrder() );
		drawStripesCommand.func = CC_CALLBACK_0( Ground::drawTexture , this );
		auto renderer = Director::getInstance()->getRenderer();
		renderer->addCommand(&drawStripesCommand);

		//blend some noise
		noiseSprite = Sprite::create("Noise.png");
		noiseSprite->setBlendFunc({GL_DST_COLOR, GL_ZERO});
		noiseSprite->setPosition( Point(textureWidth/2, textureHeight/2) );
		noiseSprite->visit();

	rt->end();

	return rt->getSprite()->getTexture();

}


void Ground::drawTexture()
{

	//INIT DRAW VARIABLES
	Color4F c2 = randomBrightColor();

	Vec2 * vertices = new Vec2[nStripes*6];
	Color4F * colors = new Color4F[nStripes*6];

	// INIT OPENGL
	
	this->setGLProgram(ShaderCache::getInstance()->getGLProgram(GLProgram::SHADER_NAME_POSITION_COLOR));
	this->getGLProgram()->use();
	this->getGLProgram()->setUniformsForBuiltins();

	GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_COLOR | GL::VERTEX_ATTRIB_FLAG_POSITION);

	//DRAW STRIPES

	int nVertices = 0;
	float x1 = -textureWidth;
	float x2;
	float y1 = textureHeight;
	float y2 = 0;
	float dx = textureWidth / nStripes * 2;
	float stripeWidth = dx/2;
	
	for (int i=0; i<nStripes; i++) {
		x2 = x1 + 512.0;

		vertices[nVertices] = Vec2(x1/this->getScale(), y1/this->getScale());
		colors[nVertices++] = (Color4F){c2.r, c2.g, c2.b, c2.a};

		vertices[nVertices] = Vec2((x1+stripeWidth)/this->getScale(), y1/this->getScale());
		colors[nVertices++] = (Color4F){c2.r, c2.g, c2.b, c2.a};

		vertices[nVertices] = Vec2(x2/this->getScale(), y2/this->getScale());
		colors[nVertices++] = (Color4F){c2.r, c2.g, c2.b, c2.a};

		vertices[nVertices] = vertices[nVertices-2];
		colors[nVertices++] = (Color4F){c2.r, c2.g, c2.b, c2.a};

		vertices[nVertices] = vertices[nVertices-2];
		colors[nVertices++] = (Color4F){c2.r, c2.g, c2.b, c2.a};

		vertices[nVertices] = Vec2((x2+stripeWidth)/this->getScale(), y2/this->getScale());
		colors[nVertices++] = (Color4F){c2.r, c2.g, c2.b, c2.a};
		
		x1 += dx;
	}

	CC_NODE_DRAW_SETUP();
	
	GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_COLOR | GL::VERTEX_ATTRIB_FLAG_POSITION);
	
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, vertices);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_FLOAT, GL_TRUE, 0, colors);
	glBlendFunc( GL_DST_ALPHA, GL_ONE );
	glDrawArrays( GL_TRIANGLES , 0, (GLsizei)nVertices);

	// DRAW DARK GRADIENT
	
	float gradientAlpha =  0.9;
	nVertices = 0;

	vertices[nVertices] = Vec2(0, 0);
	colors[nVertices++] = (Color4F){0, 0, 0, 0 };
	vertices[nVertices] = Vec2(textureWidth/this->getScale(), 0);
	colors[nVertices++] = (Color4F){0, 0, 0, 0};
	vertices[nVertices] = Vec2(0, textureHeight/this->getScale());
	colors[nVertices++] = (Color4F){0, 0, 0, gradientAlpha};
	vertices[nVertices] = Vec2(textureWidth/this->getScale(), textureHeight/this->getScale());
	colors[nVertices++] = (Color4F){0, 0, 0, gradientAlpha};

	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, vertices);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 0, colors);
	glBlendFunc(CC_BLEND_SRC, CC_BLEND_DST);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)nVertices);

	// DRAW HIGHLIGHT BORDER
	
	float borderHeight = textureHeight/16;
	float borderAlpha = 0.5;
	
	nVertices = 0;
	
	vertices[nVertices] = Vec2(0, 0);
	colors[nVertices++] = (Color4F){0.8, 0.8, 0.8, borderAlpha};
	 
	vertices[nVertices] = Vec2(textureWidth/this->getScale(), 0);
	colors[nVertices++] = (Color4F){0.8, 0.8, 0.8, borderAlpha};
	 
	vertices[nVertices] = Vec2(0, borderHeight/this->getScale());
	colors[nVertices++] = (Color4F){0, 0, 0, 0};
	 
	vertices[nVertices] = Vec2(textureWidth/this->getScale(), borderHeight/this->getScale());
	colors[nVertices++] = (Color4F){0, 0, 0, 0};
	 
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, vertices);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 0, colors);
	glBlendFunc(CC_BLEND_SRC, CC_BLEND_DST);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)nVertices);
	
	//FREE ALLOCATED VARIABLES
	
	if( vertices != NULL )delete vertices;
	if( colors != NULL ) delete colors;

}

Color4F Ground::randomBrightColor()
{
	while (true) 
	{
		float requiredBrightness = 192;
		Color4B randomColor = 
		ccc4(arc4random() % 255,
			 arc4random() % 255, 
			 arc4random() % 255, 
			 255);
		if (randomColor.r > requiredBrightness || 
			randomColor.g > requiredBrightness ||
			randomColor.b > requiredBrightness) {
			return ccc4FFromccc4B(randomColor);
		}        
	}
}

// GETTERS SETTERS

void Ground::setOffsetX( int offset )
{
	if(offset > 0 )	this->offsetX = offset;
	else this->offsetX = 0;
}

void Ground::setOffsetXTexture( int offset )
{
	if(offset > 0 )	this->offsetXTexture = offset;
	else this->offsetXTexture = 0;
}
