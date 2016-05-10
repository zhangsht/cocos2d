#include "HelloWorldScene.h"
#pragma execution_character_set("utf-8")
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

    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
	killNum = 0;
	lastCid = 'A';
	dtime = 120;

	pdb = NULL;
	path = FileUtils::getInstance()->getWritablePath() + "data.db";
	int result = sqlite3_open(path.c_str(), &pdb);
	std::string sql = "create table Grade(Id int primary key not null, Number int);";
	result = sqlite3_exec(pdb, sql.c_str(), NULL, NULL, NULL);
	sql = "insert into Grade values(0,0);";
	result = sqlite3_exec(pdb, sql.c_str(), NULL, NULL, NULL);

	TMXTiledMap* tmx = TMXTiledMap::create("bg.tmx");
	tmx->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	tmx->setAnchorPoint(Vec2(0.5, 0.5));
	tmx->setScale(Director::getInstance()->getContentScaleFactor());
	this->addChild(tmx, 0);
	//创建一张贴图
	auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
	//从贴图中以像素单位切割，创建关键帧
	auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
	//使用第一帧创建精灵
	player = Sprite::createWithSpriteFrame(frame0);
	player->setPosition(Vec2(origin.x + visibleSize.width / 2,
							origin.y + visibleSize.height/2));
	addChild(player, 3);


	TTFConfig ttfConfig;
	ttfConfig.fontFilePath = "fonts/arial.ttf";
	ttfConfig.fontSize = 36;
	
	//倒计时
	time = Label::createWithTTF(ttfConfig, "120");
	//倒计时周期性调用调度器
	schedule(schedule_selector(HelloWorld::updateTime), 1.0f, kRepeatForever, 0);
	schedule(schedule_selector(HelloWorld::createMonster), 3.0f, kRepeatForever, 0);

	//倒计时的数字
	
	time->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - time->getContentSize().height));
	addChild(time);

	kill = Label::createWithTTF(ttfConfig, "0");
	kill->setPosition(Vec2(origin.x + visibleSize.width / 2 + kill->getContentSize().width * 4,
		origin.y + visibleSize.height - kill->getContentSize().height));
	addChild(kill);

	//hp条
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	//使用hp条设置progressBar
	pT = ProgressTimer::create(sp);
	pT->setScaleX(90);
	pT->setAnchorPoint(Vec2(0, 0));
	pT->setType(ProgressTimerType::BAR);
	pT->setBarChangeRate(Point(1, 0));
	pT->setMidpoint(Point(0, 1));
	HPbarPercentage = 100;
	pT->setPercentage(HPbarPercentage);
	pT->setPosition(Vec2(origin.x+14*pT->getContentSize().width,origin.y + visibleSize.height - 2*pT->getContentSize().height));
	addChild(pT,1);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0,0);

	//静态动画
	idle.reserve(1);
	idle.pushBack(frame0);

	//攻击动画
	attack.reserve(17);
	for (int i = 0; i < 17; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(113*i,0,113,113)));
		attack.pushBack(frame);
	}
	
	//死亡动画
	auto texture2 = Director::getInstance()->getTextureCache()->addImage("$lucia_dead.png");
	dead.reserve(22);
	for (int i = 0; i < 22; i++) {
		auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(79 * i, 0, 79, 90)));
		dead.pushBack(frame);
	}
	
	//运动动画
	auto texture3 = Director::getInstance()->getTextureCache()->addImage("$lucia_forward.png");
	for (int i = 0; i < 8; i++) {
		auto frame = SpriteFrame::createWithTexture(texture3, CC_RECT_PIXELS_TO_POINTS(Rect(68 * i, 0, 68, 101)));
		run.pushBack(frame);
	}

	//Label
	auto menuLabel1 = Label::createWithTTF(ttfConfig, "W");
	auto menuLabel2 = Label::createWithTTF(ttfConfig, "S");
	auto menuLabel3 = Label::createWithTTF(ttfConfig, "A");
	auto menuLabel4 = Label::createWithTTF(ttfConfig, "D");
	auto menuLabel6 = Label::createWithTTF(ttfConfig, "Y");
	//menuItem
	auto item1 = MenuItemLabel::create(menuLabel1, CC_CALLBACK_1(HelloWorld::moveEvent, this,'W'));
	auto item2 = MenuItemLabel::create(menuLabel2, CC_CALLBACK_1(HelloWorld::moveEvent, this,'S'));
	auto item3 = MenuItemLabel::create(menuLabel3, CC_CALLBACK_1(HelloWorld::moveEvent, this,'A'));
	auto item4 = MenuItemLabel::create(menuLabel4, CC_CALLBACK_1(HelloWorld::moveEvent, this,'D'));
	auto item6 = MenuItemLabel::create(menuLabel6, CC_CALLBACK_1(HelloWorld::actionEvent, this, 'Y'));

	item3->setPosition(Vec2(origin.x+item3->getContentSize().width,origin.y+item3->getContentSize().height));
	item4->setPosition(Vec2(item3->getPosition().x + 3 * item4->getContentSize().width, item3->getPosition().y));
	item2->setPosition(Vec2(item3->getPosition().x + 1.5*item2->getContentSize().width, item3->getPosition().y));
	item1->setPosition(Vec2(item2->getPosition().x, item2->getPosition().y + item1->getContentSize().height));
	item6->setPosition(Vec2(origin.x+visibleSize.width-item6->getContentSize().width,item1->getPosition().y));
	
	auto menu = Menu::create(item1, item2, item3, item4, item6, NULL);
	menu->setPosition(Vec2(0, 0));
	addChild(menu, 1);

	factory = Factory::getInstance();
	return true;
}


void HelloWorld::moveEvent(Ref*,char cid) {
	Point move;
	float step = 20;

	switch (cid) {
	case 'W':
		if (lastCid != 'W') {
			player->setFlipX(true);
		}
		lastCid = 'W';
		move.setPoint(0, step);
		break;
	case 'A':
		if (lastCid != 'A') {
			player->setFlipX(true);
		}
		lastCid = 'A';
		move.setPoint(-step, 0);
		break;
	case 'S':
		if (lastCid != 'S') {
			player->setFlipX(false);
		}
		lastCid = 'S';
		move.setPoint(0, -step);
		break;
	case 'D':
		if (lastCid != 'D') {
			player->setFlipX(false);
		}
		lastCid = 'D';
		move.setPoint(step, 0);
		break;
	}
	auto position = player->getPosition() + move;
	if (position.x >= origin.x && position.x <= origin.x + visibleSize.width - 20
		&& position.y >= origin.y && position.y <= origin.y + origin.y + visibleSize.height - 60) {
		auto action = MoveBy::create(0.1, move);
		player->runAction(action);
		auto runAnimation = Animation::createWithSpriteFrames(run, 0.1f);
		auto run = Animate::create(runAnimation);
		player->runAction(Repeat::create(run, 1));
	}
}

void HelloWorld::actionEvent(Ref*, char cid) {
	Animation* action = nullptr;
	switch (cid) {
	case 'X':
		action = Animation::createWithSpriteFrames(dead, 0.1f);
		if (HPbarPercentage >= 10) {
			HPbarPercentage -= 10;
			CCProgressFromTo *change = CCProgressFromTo::create(0.2, HPbarPercentage + 10, HPbarPercentage);
			pT->runAction(change);
		}
		break;
	case 'Y':
		action = Animation::createWithSpriteFrames(attack, 0.1f);
		if (attackMonster()) {
			if (HPbarPercentage <= 90) {
				HPbarPercentage += 10;
				CCProgressFromTo *change = CCProgressFromTo::create(0.2, HPbarPercentage - 10, HPbarPercentage);
				pT->runAction(change);
				killNum++;
				std::stringstream s;
				std::string updateKill;
				s << killNum;
				s >> updateKill;
				kill->setString(updateKill);
				std::string sql = "update Grade set Number=Number + 1 where Id=0;";
				int result = sqlite3_exec(pdb, sql.c_str(), NULL, NULL, NULL);

			}
		}
		break;
	}

	auto actionTake = Animate::create(action);
	auto idleAnimation = Animation::createWithSpriteFrames(idle, 0.1f);
	auto idleAnimate = Animate::create(idleAnimation);
	auto sequence = Sequence::createWithTwoActions(actionTake, idleAnimate);
	player->runAction(sequence);
}

void HelloWorld::stopAc() {
	unschedule(schedule_selector(HelloWorld::updateTime));
	unschedule(schedule_selector(HelloWorld::createMonster));
}

void HelloWorld::updateTime(float dt) {
	if (dtime > 1) {
		dtime--;
		std::stringstream s;
		std::string updateTime;
		s << dtime;
		s >> updateTime;
		time->setString(updateTime);
	}
	else {
		time->setString("Times up");
		stopAc();
	}
	
}

void HelloWorld::createMonster(float dt) {
	auto monster = factory->createMonster();
	float x = random(origin.x, visibleSize.width);
	float y = random(origin.y, visibleSize.height);
	monster->setPosition(x, y);
	this->addChild(monster, 3);
	factory->moveMonster(player->getPosition(), 1.0f);
	hitByMonster();
}

void HelloWorld::hitByMonster() {
	Rect playerRect = player->getBoundingBox();
	Rect hitRect = Rect(playerRect.getMinX() - 10, playerRect.getMinY() - 10,
		playerRect.getMaxX() - playerRect.getMinX() + 20,
		playerRect.getMaxY() - playerRect.getMinY() + 20);
	Sprite* collision = factory->collider(hitRect);
	if (collision != NULL) {
		factory->removeMonster(collision);
		actionEvent(this, 'X');
	}
}

bool HelloWorld::attackMonster() {
	Rect playerRect = player->getBoundingBox();
	Rect attackRect = Rect(playerRect.getMinX() - 20, playerRect.getMinY() - 20,
		playerRect.getMaxX() - playerRect.getMinX() + 40,
		playerRect.getMaxY() - playerRect.getMinY() + 40);
	Sprite* collision = factory->collider(attackRect);
	if (collision != NULL) {
		factory->removeMonster(collision);
		return true;
	}
	return false;
}