#include "HelpScene.h"
#include "WelcomeScene.h"
#pragma execution_character_set("utf-8")

USING_NS_CC;

cocos2d::Scene* HelpScene::createScene() {
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = HelpScene::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool HelpScene::init() {
	if (!Layer::init())
	{
		return false;
	}
	visibleSize = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();

	// background
	background = Sprite::create("helpBackground.jpg");
	background->setScale(Director::getInstance()->getContentScaleFactor());
	background->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	this->addChild(background, 0);

	// information label
	TTFConfig ttfConfig;
	ttfConfig.fontFilePath = "fonts/�����е��߼���.ttf";
	ttfConfig.fontSize = 25;
	TextHAlignment alignment = TextHAlignment::LEFT;
	auto label = Label::createWithTTF(ttfConfig,
		"A S D J K L: �ֱ��Ӧ�������ֹ��\n\nP: �п���Ļ�м�ɳ�����������������\n\n�����������񣬻��ͷ���������\n\n�����ȼ���Perfect, Good, Miss\n\n���������󲻼��������Ĺ������ֽ�����Ϸ�ɣ�", 
		alignment);

	// position the label on the center of the screen
	label->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - label->getContentSize().height));

	// add the label as a child to this layer
	this->addChild(label, 1);

	// exit button
	auto exit_button = MenuItemImage::create(
		"exit.png",
		"exit.png",
		CC_CALLBACK_1(HelpScene::exitScene, this));

	exit_button->setPosition(Vec2(100, exit_button->getContentSize().height));

	auto menu = Menu::create(exit_button, NULL);
	menu->setPosition(Vec2::ZERO);

	addChild(menu, 1);

	return true;
}

void HelpScene::exitScene(cocos2d::Ref* pSender) {
	Director::getInstance()->replaceScene(TransitionSlideInL::create(1.0, WelcomeScene::createScene()));
}
