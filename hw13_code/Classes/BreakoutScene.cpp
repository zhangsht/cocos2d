#include "BreakoutScene.h"
#include "SimpleAudioEngine.h"
#include <string>

using namespace std;
using namespace CocosDenshion;

void Breakout::setPhysicsWorld(PhysicsWorld* world) { m_world = world; }

Scene* Breakout::createScene() {
    auto scene = Scene::createWithPhysics();
    //scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    scene->getPhysicsWorld()->setGravity(Point(0, 0));

    auto layer = Breakout::create(scene->getPhysicsWorld());
    scene->addChild(layer);
    return scene;
}

bool Breakout::init(PhysicsWorld* world) {
    if (!Layer::init()) {
        return false;
    }
	
	this->setPhysicsWorld(world);
    visibleSize = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();
	//初始化状态变量
	time = 30;
	clr = 0, pro = 0, life = 0;
	flag = false;
	savetime = -1;

	//增加血条
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));
	pT = ProgressTimer::create(sp);
	pT->setScaleX(90);
	pT->setAnchorPoint(Vec2(0, 0));
	pT->setType(ProgressTimerType::BAR);
	pT->setBarChangeRate(Point(1, 0));
	pT->setMidpoint(Point(0, 1));
	HPbarPercentage = 100;
	pT->setPercentage(HPbarPercentage);
	pT->setPosition(Vec2(origin.x + 14 * pT->getContentSize().width, origin.y + visibleSize.height - 2 * pT->getContentSize().height));
	addChild(pT, 1);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0, 0);

    preloadMusic();
    playBgm();

    addBackground();
    addEdge();
    addPlayer();

	addContactListener();
    addTouchListener();
    addKeyboardListener();

    this->schedule(schedule_selector(Breakout::update), 1);
    return true;
}

Breakout * Breakout::create(PhysicsWorld * world) {
	Breakout* pRet = new(std::nothrow) Breakout();
	if (pRet && pRet->init(world)) {
		pRet->autorelease();
		return pRet;
	}
	delete pRet;
	pRet = NULL;
	return NULL;
}

void Breakout::preloadMusic() {
    SimpleAudioEngine::getInstance()->preloadBackgroundMusic("music/bgm.mp3");
	SimpleAudioEngine::getInstance()->preloadEffect("music/meet_stone.wav");
	SimpleAudioEngine::getInstance()->preloadEffect("music/meet_reward.wav");
}

void Breakout::playBgm() {
    SimpleAudioEngine::getInstance()->playBackgroundMusic("music/bgm.mp3", true);
}

void Breakout::playMeet() {
	SimpleAudioEngine::getInstance()->playEffect("music/meet_stone.wav", false, 1.0f, 1.0f, 1.0f);
}

void Breakout::addBackground() {
    auto bgsprite = Sprite::create("black_hole_bg0.jpg");
    bgsprite->setPosition(visibleSize / 2);
    bgsprite->setScale(Director::getInstance()->getContentScaleFactor());
    this->addChild(bgsprite, 0);

    auto ps = ParticleSystemQuad::create("black_hole.plist");
    ps->setPosition(visibleSize / 2);
    this->addChild(ps);
}

void Breakout::addEdge() {
    auto edgeSp = Sprite::create();
    auto boundBody = PhysicsBody::createEdgeBox(visibleSize);
	boundBody->setDynamic(false);
    boundBody->setTag(0);
    edgeSp->setPosition(Point(visibleSize.width / 2, visibleSize.height / 2));
	edgeSp->setScale(Director::getInstance()->getContentScaleFactor());
    edgeSp->setPhysicsBody(boundBody);
    this->addChild(edgeSp);
}

//添加玩家和左下角的特效
void Breakout::addPlayer() {
    player = Sprite::create("player.png");
    player->setAnchorPoint(Vec2(0.5, 0.5));
    player->setPhysicsBody(PhysicsBody::createCircle(player->getContentSize().height / 2));
	player->getPhysicsBody()->setCategoryBitmask(0x01);
	player->getPhysicsBody()->setCollisionBitmask(0x03);
	player->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
	player->getPhysicsBody()->setTag(1);
    player->setPosition(visibleSize / 2);
    player->getPhysicsBody()->setAngularVelocityLimit(0);
    addChild(player);

	clear = Sprite::create("clear.PNG");
	clear->setPosition(clear->getContentSize().width + origin.x, clear->getContentSize().height / 2 + origin.y);
	clear->setScale(0.8, 0.8);

	protect = Sprite::create("protect.PNG");
	protect->setPosition(clear->getPositionX() + protect->getContentSize().width,clear->getPositionY());
	protect->setScale(0.8, 0.8);

	newLife = Sprite::create("newLife.PNG");
	newLife->setPosition(protect->getPositionX() + newLife->getContentSize().width - 10, protect->getPositionY());
	newLife->setScale(0.6, 0.6);
	addChild(clear, 3);
	addChild(protect, 3);
	addChild(newLife, 3);

	label1 = Label::createWithTTF("0", "fonts/SIMYOU.TTF", 16);
	label1->setPosition(clear->getPosition());
	label1->setColor(ccc3(256, 0, 256));
	addChild(label1, 4);
	label2 = Label::createWithTTF("0", "fonts/SIMYOU.TTF", 16);
	label2->setPosition(protect->getPosition());
	label2->setColor(ccc3(256, 0, 256));
	addChild(label2, 4);
	label3 = Label::createWithTTF("0", "fonts/SIMYOU.TTF", 16);
	label3->setPosition(newLife->getPosition());
	label3->setColor(ccc3(256, 0, 256));
	addChild(label3, 4);
}

void Breakout::addContactListener() {
	auto touchListener = EventListenerPhysicsContact::create();
	touchListener->onContactBegin = CC_CALLBACK_1(Breakout::onConcactBegan, this);
	_eventDispatcher->addEventListenerWithFixedPriority(touchListener, 1);
}

void Breakout::addTouchListener(){
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(Breakout::onTouchBegan,this);
    touchListener->onTouchMoved = CC_CALLBACK_2(Breakout::onTouchMoved,this);
    touchListener->onTouchEnded = CC_CALLBACK_2(Breakout::onTouchEnded,this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

//1、2、3依次对应不同的特效
void Breakout::addKeyboardListener() {
    auto keboardListener = EventListenerKeyboard::create();
    keboardListener->onKeyPressed = CC_CALLBACK_2(Breakout::onKeyPressed, this);
    keboardListener->onKeyReleased = CC_CALLBACK_2(Breakout::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keboardListener, this);
}

bool Breakout::onTouchBegan(Touch *touch, Event *unused_event) {
    Vec2 postion = touch->getLocation();
	auto vec = postion - player->getPosition();
	vec = vec.getNormalized();
	player->getPhysicsBody()->setVelocity((vec) * (60));
    return true;
}

void Breakout::onTouchMoved(Touch *touch, Event *unused_event) {
}

void Breakout::onTouchEnded(Touch *touch, Event *unused_event) {
}

//移除stone并播放效果
void Breakout::removeFromEnemys(int tag, bool flag2) {
	playMeet();
	if (flag2) {
		HPbarPercentage -= 10;
		if (HPbarPercentage <= 0) {
			stopAc(1);
		}
		pT->setPercentage(HPbarPercentage);
	}
	
	Vec2 pos;
	
	for (auto it = enemys.begin(); it != enemys.end(); it++) {
		if ((*it) != NULL && (*it)->getTag() == tag) {
			if(flag2) pos = (*it)->getPosition();
			enemys.erase(it);
			break;
		}
	}
	pos = player->getPosition();
	auto explosion = ParticleSystemQuad::create("explode.plist");
	explosion->setPosition(pos);
	addChild(explosion);
}

//特效标签 得到奖励加1 使用一次减1
void Breakout::addRewardLabel(int tag) {
	SimpleAudioEngine::getInstance()->playEffect("music/meet_reward.wav", false, 1.0f, 1.0f, 1.0f);

	std::stringstream s;
	std::string number;
	Vec2 pos;
	switch (tag)
	{
	case 40:
		clr++;
		s << clr;
		s >> number;
		label1->setString(number);
		break;
	case 50:
		pro++;
		s << pro;
		s >> number;
		label2->setString(number);
		break;
	case 60:
		life++;
		s << life;
		s >> number;
		label3->setString(number);
		break;
	default:
		break;
	}
	
	Vector<PhysicsBody*>::iterator it = rewards.begin();
	for (; it != rewards.end(); it++) {
		if ((*it)->getTag() == tag) {
			rewards.erase(it);
			return;
		}
	}
}

bool Breakout::onConcactBegan(PhysicsContact& contact) {
	auto nodeA = contact.getShapeA()->getBody();
	auto nodeB = contact.getShapeB()->getBody();
	if (nodeA && nodeB) {
		if (nodeA->getTag() == 1) {
			if (nodeB->getTag() > 30) {
				addRewardLabel(nodeB->getTag());
			} else {
				if (!flag) removeFromEnemys(nodeB->getTag(), true);
			}
			nodeB->getNode()->removeFromParentAndCleanup(true);
		}
		else if (nodeB->getTag() == 1) {
			if (nodeA->getTag() > 30) {
				addRewardLabel(nodeA->getTag());
			} else {
				if (!flag) removeFromEnemys(nodeA->getTag(), true);
			}
			nodeA->getNode()->removeFromParentAndCleanup(true);
		}
		return true;
	}
	return false;
}

//停止增加stone并根据玩家情况提示输赢
void Breakout::stopAc(int a) {
	unschedule(schedule_selector(Breakout::update));
	SimpleAudioEngine::getInstance()->pauseBackgroundMusic();

	cocos2d::Label* label;
	if (a == 0) {
		label = Label::createWithTTF("Great! You win!", "fonts/SIMYOU.TTF", 28);
	}
	else {
		label = Label::createWithTTF("GG, You lose!", "fonts/SIMYOU.TTF", 28);
	}
	label->setPosition(visibleSize.width / 2, visibleSize.height - 80);
	label->setColor(ccc3(256, 256, 0));
	addChild(label, 4);
}

void Breakout::update(float f) {
	time--;
	if (time <= 0) {
		stopAc(0);
	}
	if (savetime - time > 3) {
		savetime = -1;
		flag = false;
	}
    newEnemys();
	newReward();
}

//奖励随机出现
void Breakout::newReward() {
	if (time % 5 == 0) {
		if (rewards.size() > 5) return;

		int type = 3;
		if (CCRANDOM_0_1() > 0.85) { type = 4; }
		else if (CCRANDOM_0_1() > 0.6) { type = 5; }

		Point location = Vec2(0, 0);
		switch (rand() % 4)
		{
		case 0:
			location.y = visibleSize.height;
			location.x = rand() % (int)(visibleSize.width);
			break;
		case 1:
			location.x = visibleSize.width;
			location.y = rand() % (int)(visibleSize.height);
			break;
		case 2:
			location.y = 0;
			location.x = rand() % (int)(visibleSize.width);
			break;
		case 3:
			location.x = 0;
			location.y = rand() % (int)(visibleSize.height);
			break;
		default:
			break;
		}
		addReward(type, location);
	}
}

void Breakout::addReward(int type, Point p) {
	char path[100];
	int tag;
	switch (type)
	{
	case 3:
		sprintf(path, "clear.PNG");
		tag = 40;
		break;
	case 4:
		sprintf(path, "protect.PNG");
		tag = 50;
		break;
	case 5:
		sprintf(path, "newLife.PNG");
		tag = 60;
		break;
	default:
		break;
	}
	auto re = Sprite::create(path);
	re->setPhysicsBody(PhysicsBody::createCircle(re->getContentSize().height / 2));
	re->setAnchorPoint(Vec2(0.5, 0.5));
	re->setScale(0.7, 0.7);
	re->setPosition(p);
	re->getPhysicsBody()->setCategoryBitmask(0x02);
	re->getPhysicsBody()->setCollisionBitmask(0x01);
	re->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
	re->getPhysicsBody()->setTag(tag);
	if (rand() % 100 < 50) {
		re->getPhysicsBody()->setVelocity((player->getPosition() - p) * (0.25));
	}
	else {
		re->getPhysicsBody()->setVelocity((Point(rand() % (int)(visibleSize.width - 100) + 50, rand() % (int)(visibleSize.height - 100) + 50) - p) * (0.25));
	}
	re->getPhysicsBody()->setAngularVelocity(CCRANDOM_0_1() * 2);
	rewards.pushBack(re->getPhysicsBody());
	addChild(re);
}

void Breakout::newEnemys() {
	if (enemys.size() > 50) return;
    int newNum = 2;
    while (newNum--) {
        int type = 0;
        if (CCRANDOM_0_1() > 0.85) { type = 2; }
        else if (CCRANDOM_0_1() > 0.6) { type = 1; }

        Point location = Vec2(0, 0);
        switch (rand() % 4)
        {
        case 0:
            location.y = visibleSize.height;
            location.x = rand() % (int)(visibleSize.width);
            break;
        case 1:
            location.x = visibleSize.width;
            location.y = rand() % (int)(visibleSize.height);
            break;
        case 2:
            location.y = 0;
            location.x = rand() % (int)(visibleSize.width);
            break;
        case 3:
            location.x = 0;
            location.y = rand() % (int)(visibleSize.height);
            break;
        default:
            break;
        }
        addEnemy(type, location);
    }
}

void Breakout::addEnemy(int type, Point p) {
    char path[100];
    int tag;
    switch (type)
    {
    case 0:
        sprintf(path, "stone1.png");
        tag = 10;
        break;
    case 1:
        sprintf(path, "stone2.png");
        tag = 20;
        break;
    case 2:
        sprintf(path, "stone3.png");
        tag = 30;
        break;
    default:
        sprintf(path, "stone1.png");
        tag = 10;
        break;
    }
    auto re = Sprite::create(path);
    re->setPhysicsBody(PhysicsBody::createCircle(re->getContentSize().height / 2));
    re->setAnchorPoint(Vec2(0.5, 0.5));
    re->setScale(0.5, 0.5);
    re->setPosition(p);
	re->getPhysicsBody()->setCategoryBitmask(0x02);
	re->getPhysicsBody()->setCollisionBitmask(0x01);
	re->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
	// TODO set bitmask
    re->getPhysicsBody()->setTag(tag);
    if (rand() % 100 < 50) {
        re->getPhysicsBody()->setVelocity((player->getPosition() - p) * (0.25));
    }
    else {
        re->getPhysicsBody()->setVelocity((Point(rand() % (int)(visibleSize.width - 100) + 50, rand() % (int)(visibleSize.height - 100) + 50) - p) * (0.25));
    }
    re->getPhysicsBody()->setAngularVelocity(CCRANDOM_0_1() * 10);
    enemys.pushBack(re->getPhysicsBody());
    addChild(re);

	if (time % 6 == 0) {
		auto re2 = Sprite::create(path);
		re2->setPhysicsBody(PhysicsBody::createCircle(re2->getContentSize().height / 2));
		re2->setAnchorPoint(Vec2(0.5, 0.5));
		re2->setScale(0.5, 0.5);
		re2->setPosition(p);
		re2->getPhysicsBody()->setCategoryBitmask(0x02);
		re2->getPhysicsBody()->setCollisionBitmask(0x01);
		re2->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
		// TODO set bitmask
		re2->getPhysicsBody()->setTag(tag);
		if (rand() % 100 < 50) {
			re2->getPhysicsBody()->setVelocity((player->getPosition() - p) * (0.25));
		}
		else {
			re2->getPhysicsBody()->setVelocity((Point(rand() % (int)(visibleSize.width - 100) + 50, rand() % (int)(visibleSize.height - 100) + 50) - p) * (0.25));
		}
		re2->getPhysicsBody()->setAngularVelocity(CCRANDOM_0_1() * 10);
		enemys.pushBack(re2->getPhysicsBody());
		addChild(re2);

		PhysicsJointSpring* springJoint = PhysicsJointSpring::construct(re->getPhysicsBody(), re2->getPhysicsBody(),
			re->getAnchorPoint(), re2->getAnchorPoint(), 200, 0);
		m_world->addJoint(springJoint);
	}
}

void Breakout::Duang(int tag) {
	switch (tag)
	{
	case 40:
		if (clr > 0) {
			for (auto it = enemys.begin(); it != enemys.end(); ) {
				if ((*it) != NULL && (*it)->getNode() != NULL) {
					(*it)->getNode()->removeFromParentAndCleanup(true);
				}
				it = enemys.erase(it);
			}
			clr -= 2;
			addRewardLabel(40);
		}
		break;
	case 50:
		savetime = time;
		flag = true;
		pro -= 2;
		addRewardLabel(50);
		break;
	case 60:
		HPbarPercentage = 100;
		pT->setPercentage(HPbarPercentage);
		life -= 2;
		addRewardLabel(60);
		break;
	default:
		break;
	}
}

void Breakout::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
    switch (code)
    {
    case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_A:
        player->getPhysicsBody()->setVelocity(Point(-200, player->getPhysicsBody()->getVelocity().y));
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_D:
        player->getPhysicsBody()->setVelocity(Point(200, player->getPhysicsBody()->getVelocity().y));
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_W:
        player->getPhysicsBody()->setVelocity(Point(player->getPhysicsBody()->getVelocity().x, 200));
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_S:
        player->getPhysicsBody()->setVelocity(Point(player->getPhysicsBody()->getVelocity().x, -200));
        break;
	case cocos2d::EventKeyboard::KeyCode::KEY_1:
		if(clr > 0) Duang(40);
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_2:
		if(pro > 0) Duang(50);
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_3:
		if(life > 0) Duang(60);
		break;
    default:
        break;
    }
}

void Breakout::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
    switch (code)
    {
    case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_A:
        player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(-200, 0));
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_D:
        player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(200, 0));
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_W:
        player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(0, 200));
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_S:
        player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(0, -200));
        break;
    default:
        break;
    }
}
