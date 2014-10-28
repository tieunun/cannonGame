#include "GameLayer.h"

USING_NS_CC;

Scene * GameLayer::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();
	
	// 'layer' is an autorelease object
	auto layer = GameLayer::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool GameLayer::init()
{
	ground = NULL;
	noiseSprite = NULL;

	if ( !Layer::init() )
	{
		return false;
	}

	winSize = Director::getInstance()->getVisibleSize();

	initBox2D();

	//Generate Ground
	ground = new Ground(this, m_world);
	ground->generateHills();
	genBackground();
			
	this->setTouchEnabled(true);
	
	this->schedule( schedule_selector(GameLayer::tick) );
	
	return true;
}

void GameLayer::initBox2D(){

	b2Vec2 gravity;
	gravity.Set( 0.0f , -10.0f );
	m_world = new b2World( gravity );		

}

Texture2D * GameLayer::createTexture( Color4F bgColor, float textureWidth, float textureHeight, int nStripes ){
	auto *rt = RenderTexture::create(textureWidth, textureHeight);
	rt->beginWithClear(bgColor.r, bgColor.g, bgColor.b, bgColor.a);

		//set texture parameters
		this->textureWidth = textureWidth;
		this->textureHeight = textureHeight;
		this->nStripes = nStripes;
		
		//make custom openGL render
		drawStripesCommand.init( rt->getGlobalZOrder() );
		drawStripesCommand.func = CC_CALLBACK_0( GameLayer::drawStripes , this );
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


void GameLayer::drawStripes()
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

Color4F GameLayer::randomBrightColor()
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

void GameLayer::genBackground() {

	if( ground == NULL ) return;

	Color4F bgColor = randomBrightColor();
	Texture2D * texture = createTexture(bgColor, 512.0 , 512.0 , 6);
	stripes = Sprite::createWithTexture( texture );
	stripes->setPosition( 512, 512 );
	cocos2d::Texture2D::TexParams params = {GL_LINEAR, GL_LINEAR, GL_REPEAT,  GL_REPEAT};
	stripes->getTexture()->setTexParameters(params);
	ground->stripes = stripes;
	stripes->retain();
}

void GameLayer::onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{
	for( auto touch : touches )
	{
		Point location = touch->getLocationInView();
		location = Director::sharedDirector()->convertToGL(location);

	}		
}


void GameLayer::onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{

	Point location = touches[0]->getLocationInView();
	location = Director::sharedDirector()->convertToGL(location);
	
}

void GameLayer::tick(float dt){
	
	int velocityIterations = 8;
	int positionIterations = 1;
	
	m_world->Step(dt, velocityIterations, positionIterations);
			
			
	if( ground != NULL ){
		ground->update();
	}
	
}

