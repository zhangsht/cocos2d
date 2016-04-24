#include "HelloWorldScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    auto label = Label::createWithTTF("你好双涛", "fonts/SIMYOU.TTF", 20);
	auto label2 = Label::createWithTTF("14331370", "fonts/SIMYOU.TTF", 20);

    
    // position the label on the center of the screen
    label->setPosition(Vec2(origin.x + visibleSize.width/4,
                            origin.y + visibleSize.height - label->getContentSize().height));
	label2->setPosition(Vec2(origin.x + visibleSize.width/4,
		origin.y + visibleSize.height - label2->getContentSize().height - label->getContentSize().height));
	label->setColor( ccc3(255, 0, 156) );
	label2->setColor( ccc3(256, 0, 256) );

    // add the label as a child to this layer
    this->addChild(label, 1);
	this->addChild(label2, 1);


    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("huiqiao.jpg");

    // position the sprite on the center of the screen
    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/3 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(sprite, 0);

	// add "HelloWorld" splash screen"
	auto sprite2 = Sprite::create("meng2.jpg");
	float scalX = visibleSize.width / sprite2->getContentSize().width;
	float scalY = visibleSize.height / sprite2->getContentSize().height;
	sprite2->setScaleX(scalX * 0.3);
	sprite2->setScaleY(scalY * 0.3);


	// position the sprite on the center of the screen
	sprite2->setPosition(Vec2(visibleSize.width / 3 + origin.x, visibleSize.height/8 + origin.y));

	// add the sprite as a child to this layer
	this->addChild(sprite2, 0);
    
    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
