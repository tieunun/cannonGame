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

