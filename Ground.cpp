#include "Ground.h"

using namespace cocos2d;

// UPDATING

void Ground::update(){
	if( groundBody == NULL || groundSprite == NULL ) return;
	
	groundSprite->setPosition( Point( groundBody->GetPosition().x * PTM_RATIO, groundBody->GetPosition().y * PTM_RATIO) );
	groundSprite->setRotation( -1 * CC_RADIANS_TO_DEGREES( groundBody->GetAngle()) );

}

// GROUND GENERATE

void Ground::initGround( Point bottomLeft , Point topRight ){
		
	//Color4F bgColor = randomBrightColor();
	Texture2D::TexParams params = {GL_LINEAR, GL_LINEAR, GL_REPEAT,  GL_REPEAT};
	
	//Texture2D * texture = createTexture(bgColor, 512.0 , 512.0 , 6);
	
	groundSprite = Sprite::create( "ground.png" , Rect(bottomLeft.x , bottomLeft.y , topRight.x , topRight.y ) );
	groundSprite->getTexture()->setTexParameters(params);
	
	layer->addChild(groundSprite, 5);
	
	this->setBox2DBody( bottomLeft , topRight );
	
}

// BOX2D

void Ground::setBox2DBody( Point bottomLeft , Point topRight ) {
 
	if(groundBody != NULL) {
		m_world->DestroyBody(groundBody);
	}
	
	b2BodyDef groundDef;
	groundDef.type = b2_staticBody;
	groundDef.position.Set( (worldEndX - worldStartX)/(2*PTM_RATIO) , 0 );
 
	b2PolygonShape groundShape;
	groundShape.SetAsBox( (topRight.x - bottomLeft.x)/(2*PTM_RATIO) , (topRight.y - bottomLeft.y)/(2*PTM_RATIO) );
	
	b2FixtureDef groundFixtureDef;
	groundFixtureDef.density = 1.0;
	groundFixtureDef.friction = 0.3;
	groundFixtureDef.filter.categoryBits = GROUND;
	groundFixtureDef.filter.maskBits = CANNON | BULLET | SHRAPNEL | ENEMY;

	groundBody = m_world->CreateBody( &groundDef );
	
	groundFixtureDef.shape = &groundShape;
	groundBody->CreateFixture(&groundFixtureDef);

}

// TEXTURE

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
