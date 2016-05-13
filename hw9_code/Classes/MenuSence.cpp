#include "MenuSence.h"
#include "GameSence.h"
USING_NS_CC;

Scene* MenuSence::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();
	// 'layer' is an autorelease object
	auto layer = MenuSence::create();
	// add layer as a child to scene
	scene->addChild(layer);
	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool MenuSence::init()
{

	if (!Layer::init())
	{
		return false;
	}
	//add touch listener
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(MenuSence::onTouchBegan, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bg_sky = Sprite::create("menu-background-sky.jpg");
	bg_sky->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y + 150));
	this->addChild(bg_sky, 0);

	auto bg = Sprite::create("menu-background.png");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y - 60));
	this->addChild(bg, 0);

	//caveAnimation of Spangle
	auto cave = Sprite::createWithSpriteFrameName("cave-0.png");
	Animate* caveAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("caveAnimation"));
	cave->runAction(RepeatForever::create(caveAnimate));
	cave->setPosition(visibleSize.width / 2 + origin.x + 160, visibleSize.height / 2 + origin.y - 60);
	this->addChild(cave, 1);

	auto miner = Sprite::create("menu-miner.png");
	miner->setPosition(Vec2(150 + origin.x, visibleSize.height / 2 + origin.y - 60));
	this->addChild(miner, 1);

	auto leg = Sprite::createWithSpriteFrameName("miner-leg-0.png");
	Animate* legAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("legAnimation"));
	leg->runAction(RepeatForever::create(legAnimate));
	leg->setPosition(110 + origin.x, origin.y + 102);
	this->addChild(leg, 1);

	
	auto gold = Sprite::create("menu-start-gold.png");
	gold->setPosition(Vec2(visibleSize.width / 1.3 + origin.x, visibleSize.height / 2 + origin.y - 160));
	this->addChild(gold, 0);

	//Click the Start button to switch scenes and change
	auto startItem = MenuItemImage::create(
		"start-0.png",
		"start-1.png",
		CC_CALLBACK_1(MenuSence::menuCallback, this));

	startItem->setPosition(Vec2(visibleSize.width / 1.3 + origin.x, visibleSize.height / 2 + origin.y - 120));

	// create menu, it's an autorelease object
	auto menu = Menu::create(startItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	auto text = Sprite::create("gold-miner-text.png");
	text->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 1.2 + origin.y));
	this->addChild(text, 1);

	return true;
}


void MenuSence::menuCallback(cocos2d::Ref* pSender) {
	auto transition = TransitionCrossFade::create(0.2f, GameSence::createScene());
	Director::getInstance()->replaceScene(transition);
}

bool MenuSence::onTouchBegan(Touch *touch, Event *unused_event) {
	auto location = touch->getLocation();

	return true;
}


