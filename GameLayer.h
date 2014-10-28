#ifndef __GAME_LAYER_H__
#define __GAME_LAYER_H__

#include "Box2D/Box2D.h"
#include "cocos2d.h"
#include "Ground.h"
#include "Cannon.h"
#include "Bullet.h"
#include "Shrapnel.h"

using namespace cocos2d;

class BulletListener : public b2ContactListener
{
	
	void BeginContact(b2Contact* contact) {
   
		if(!contact->IsTouching()) return;
   
		//check if fixture A was a projectile
		void* bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
		if ( bodyUserData )
		{
			if( strcmp( static_cast<GameObject*>( bodyUserData )->name->c_str() , "BULLET" ) == 0 )
				static_cast<Bullet*>( bodyUserData )->markedToExplode = true;
		}
			
		//check if fixture B was a ball
		bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
		if ( bodyUserData )
		{
			if( strcmp( static_cast<GameObject*>( bodyUserData )->name->c_str() , "BULLET" )  == 0 )
				static_cast<Bullet*>( bodyUserData )->markedToExplode = true;
		}		
    }
};

BulletListener bulletListener;

class GameLayer : public cocos2d::Layer
{
		
private:

	Sprite * bodySprite;
	b2Body * body;

	//texture data
	int nStripes;
	float textureWidth;
	float textureHeight;

	Size winSize;
	Ground * ground;

    b2World * m_world;

	Cannon * cannon;

	b2Vec2 clickedPoint;

	std::vector<Bullet*> bullets;

public:

	CustomCommand drawStripesCommand;
	Sprite * noiseSprite;
	Sprite * stripes;

    static cocos2d::Scene* createScene();

    // implement the "static create()" method manually
    CREATE_FUNC(GameLayer);

	bool init()
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
		
		cannon = NULL;
		body = NULL;
		bodySprite = NULL;
		clickedPoint = b2Vec2 ( 100 , 10 );
		
		this->setTouchEnabled(true);
		
		this->schedule( schedule_selector(GameLayer::tick) );
	  	
	  	this->setScale( 1 );
	  	
		return true;
	}
    
	void initBox2D(){
	
		b2Vec2 gravity;
		gravity.Set( 0.0f , -10.0f );
		m_world = new b2World( gravity );		

		m_world->SetContactListener(&bulletListener);
	}
	
	Texture2D * createTexture( Color4F bgColor, float textureWidth, float textureHeight, int nStripes ){
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


	void drawStripes()
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

	Color4F randomBrightColor()
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
 
	void genBackground() {
 
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
 
 	void createSky(){
		Color4F bgColor = randomBrightColor();
		Texture2D * texture = createTexture(bgColor, 512.0 , 512.0 , 6);
		Sprite * sky = Sprite::createWithTexture( texture );
		sky->setPosition( winSize.width/2, winSize.height/2 );
		cocos2d::Texture2D::TexParams params = {GL_LINEAR, GL_LINEAR, GL_REPEAT,  GL_REPEAT};
		sky->getTexture()->setTexParameters(params);

		this->addChild(sky,20);
	}
	
	void createBodyAtPoint(b2Vec2 point)
	{
		
		if( body != NULL ){
			m_world->DestroyBody( body );
			body = NULL;
		}
		
		if( bodySprite != NULL )
		{
			this->removeChild( bodySprite );
			bodySprite = NULL;
		}
		
		b2BodyDef def;
		def.type = b2_dynamicBody;
		def.position.Set( point.x , point.y );
		
		b2FixtureDef fixture;
		
		b2CircleShape shape;

		shape.m_radius = 2; 
			
		body = m_world->CreateBody( &def );
			
		fixture.density = 1;
		fixture.shape = &shape;
		//fixture.filter.categoryBits = BULLET;
		//fixture.filter.maskBits = GROUND | CANNON;
		
		body->CreateFixture(&fixture);
			
		bodySprite = Sprite::create("kolo.png",  cocos2d::CCRectMake(0, 0, 256, 256));
		bodySprite->setScale(0.5);
		bodySprite->setPosition( Vec2( ( point.x )*PTM_RATIO  , ( point.x )*PTM_RATIO  ) );
		
		this->addChild( bodySprite , 0 );
		 
	}
	
	void createBullet(){

		Bullet * bullet = new Bullet(this, m_world, 0.5 , 1);
		bullet->createBullet( cannon->GetBarrelExit() );
		bullet->shoot( cannon->GetShootingDirection() , 2 );
		
		bullets.push_back(bullet);

	}
	
	void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
	{
		for( auto touch : touches )
		{
			// Choose one of the touches to work with
			Point location = touch->getLocationInView();
			location = Director::sharedDirector()->convertToGL(location);

			if( location.x > 300 && location.x < 800 ){	
				clickedPoint = b2Vec2( location.x/PTM_RATIO , location.y/PTM_RATIO );
			}
		}		
	}


	void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
	{

		Point location = touches[0]->getLocationInView();
		location = Director::sharedDirector()->convertToGL(location);
		
		if( location.x < 150 )
		{	
			
			if( ground != NULL )
			{
				genBackground();
				ground->generateHills();
			}
			
			if( cannon != NULL )
			{
				delete cannon;
			}
			
			//Add cannon
			cannon = new Cannon(this, m_world);
			cannon->createCannon( b2Vec2( location.x/PTM_RATIO , location.y/PTM_RATIO ) );
		}
		
		if( location.x > 800 )
		{
			cannon->shoot();
			createBullet();
		}
		
	}

	void tick(float dt){
		
		int velocityIterations = 8;
		int positionIterations = 1;
		
		static float offsetCannon = 0;
		
		m_world->Step(dt, velocityIterations, positionIterations);
				
				
		if( cannon != NULL )
		{
			cannon->rotateBarrel( clickedPoint );
			cannon->updateSprites();
			offsetCannon = cannon->getMainSprite()->getPosition().x;
		}
		
		//Explode bullets
		for(int i = 0; i < bullets.size(); i++)
		{
			Bullet * bullet = bullets[i];
			if( !bullet->updateSprites() )
			{
				bullets.erase(bullets.begin()+i);
				delete bullet;
			}
			else{
				offsetCannon = bullet->getMainSprite()->getPosition().x;
			}
		}
		
		if( bodySprite != NULL )
		{
			bodySprite->setPosition( Point( body->GetPosition().x * PTM_RATIO, body->GetPosition().y * PTM_RATIO) );
			bodySprite->setRotation( -1 * CC_RADIANS_TO_DEGREES( body->GetAngle()) );
		}
						
		float PIXELS_PER_SECOND = 100;
		static float offset = 0;
		offset += PIXELS_PER_SECOND * dt;
  
		if( ground != NULL )
		{
			ground->setOffsetXTexture( offset );
			ground->setOffsetX( offsetCannon );
			ground->update();
		}
		
		
	}



};

#endif // __GAME_LAYER_H__
