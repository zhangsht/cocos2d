#include "GameSence.h"

USING_NS_CC;

Scene* GameSence::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();
	// 'layer' is an autorelease object
	auto layer = GameSence::create();
	// add layer as a child to scene
	scene->addChild(layer);
	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool GameSence::init()
{
	if (!Layer::init())
	{
		return false;
	}

	//add touch listener
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(GameSence::onTouchBegan, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	stoneLayer = Layer::create();
	stoneLayer->setAnchorPoint(Vec2(0, 0));
	stoneLayer->setPosition(Vec2(0, 0));

	auto background = Sprite::create("level-background-0.jpg");
	auto bSize = background->getContentSize();
	background->setScaleX(visibleSize.width / bSize.width);
	background->setScaleY(visibleSize.height / bSize.height);
	background->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	this->addChild(background, 0);

	// add the sprite as a child to this layer
	stone = Sprite::create("stone.png");
	stone->setPosition(Vec2(560, 480));
	stoneLayer->addChild(stone, 1);
	this->addChild(stoneLayer, 1);

	auto shootLabel = MenuItemFont::create("Shoot",
		CC_CALLBACK_1(GameSence::shoot, this));
	shootLabel->setPosition(Point(700, 480));
	auto item = Menu::create(shootLabel, NULL);
	item->setPosition(Vec2::ZERO);
	stoneLayer->addChild(item, 2);

	mouseLayer = Layer::create();
	mouseLayer->setAnchorPoint(Vec2(0, 0));
	mouseLayer->setPosition(Vec2(0, visibleSize.height / 2));

	auto spriteSheet = SpriteFrameCache::getInstance();
	spriteSheet->addSpriteFramesWithFile("level-sheet.plist");
	mouse = Sprite::createWithSpriteFrameName("mouse-0.png");
	mouse->setPosition(Vec2(visibleSize.width / 2, 0));
	mouseLayer->addChild(mouse, 0);
	this->addChild(mouseLayer, 2);
	return true;
}

void GameSence::shoot(Ref* pSender) {
	//Shot Animation
	auto shootStone = Sprite::create("stone.png");
	shootStone->setPosition(Vec2(560,480));
	stoneLayer->addChild(shootStone, 2);

	auto stoneFadeOut = FadeOut::create(1);
	auto shootLocation = mouseLayer->convertToWorldSpace(mouse->getPosition());
	auto shootTo = MoveTo::create(1, shootLocation);
	auto shootAnimation = Spawn::createWithTwoActions(stoneFadeOut, shootTo);
	shootStone->runAction(shootAnimation);

	//Diamond Rewards
	auto diamond = Sprite::create("diamond.png");
	diamond->setPosition(shootLocation);
	stoneLayer->addChild(diamond, 3);

	auto mouseTo = MoveBy::create(1, Vec2(rand() % 200, rand() % 200));
	mouse->runAction(mouseTo);
}

bool GameSence::onTouchBegan(Touch *touch, Event *unused_event) {
	//Click to appear cheese
	auto location = touch->getLocation();
	auto currentCheese = mouseLayer->convertToNodeSpace(location);
	
	auto newCheese = Sprite::create("cheese.png");
	newCheese->setPosition(location);
	stoneLayer->addChild(newCheese, 1);
	//But over time, would disappear
	newCheese->runAction(FadeOut::create(3));

	auto moveToCheese = MoveTo::create(1, currentCheese);
	auto seq = Sequence::create(moveToCheese, nullptr);
	mouse->runAction(seq);
	return true;
}
