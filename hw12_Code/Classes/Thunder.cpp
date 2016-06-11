#include "Thunder.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include <algorithm>

USING_NS_CC;

using namespace CocosDenshion;

double Thunder::count = 0.0;
int Thunder::dir = 10;
Scene* Thunder::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = Thunder::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

bool Thunder::init() {
    if ( !Layer::init() ) {
        return false;
    }
	isPressed = false;
	schedule(schedule_selector(Thunder::update), 0.1f, kRepeatForever, 0);
	schedule(schedule_selector(Thunder::alwaysMove), 0.1f, kRepeatForever, 0);
	schedule(schedule_selector(Thunder::getClose), 10.0f, kRepeatForever, 1);


    visibleSize = Director::getInstance()->getVisibleSize();

    auto bgsprite = Sprite::create("background.jpg");
    bgsprite->setPosition(visibleSize / 2);
     bgsprite->setScale(Director::getInstance()->getContentScaleFactor());
    this->addChild(bgsprite, 0);

    player = Sprite::create("player.png");
    player->setPosition(visibleSize.width / 2, player->getContentSize().height + 5);
    player->setName("player");
    this->addChild(player, 1);

    addEnemy(5);

	audio = SimpleAudioEngine::getInstance();
    preloadMusic();
    playBgm();

    addKeyboardListener();
    addCustomListener();

    // TODO
    // add schedule
    
    return true;
}

void Thunder::preloadMusic() {
	audio->preloadBackgroundMusic("music/bgm.mp3");
	audio->preloadEffect("music/fire.wav");
	audio->preloadEffect("music/explore.wav");
}

void Thunder::playBgm() {
	audio->playBackgroundMusic("music/bgm.mp3", true);
}

void Thunder::playFire() {
	audio->playEffect("music/fire.wav", false, 1.0f, 1.0f, 1.0f);
}

void Thunder::playExplore() {
	audio->playEffect("music/explore.wav", false, 1.0f, 1.0f, 1.0f);
}

void Thunder::addEnemy(int n) {
    enemys.resize(n * 3);
    for(unsigned i = 0; i < 3; ++i) {
        char enemyPath[20];
        sprintf(enemyPath, "stone%d.png", 3 - i);
        double width  = (visibleSize.width - 20) / (n + 1.0),
               height = visibleSize.height - (50 * i);
        for(int j = 0; j < n; ++j) {
            auto enemy = Sprite::create(enemyPath);
            enemy->setAnchorPoint(Vec2(0.5, 0.5));
            enemy->setScale(0.5, 0.5);
            enemy->setPosition(width * (j + 1), height);
            enemys[i * n + j] = enemy;
			addChild(enemy);
        }
    }
}

void Thunder::stopAc() {
	unschedule(schedule_selector(Thunder::update));
	unschedule(schedule_selector(Thunder::alwaysMove));
	unschedule(schedule_selector(Thunder::getClose));

	audio->pauseBackgroundMusic();
	auto label = Label::createWithTTF("Game Over", "fonts/SIMYOU.TTF", 28);
	label->setColor(ccc3(0,0,0));
	label->setPosition(visibleSize / 2);
	this->addChild(label, 3);
}

void Thunder::getClose(float f) {
	if (enemys.size() > 1) {
		std::vector<Sprite*>::iterator it = enemys.end();
		do {
			it--;
			(*it)->setPosition((*it)->getPosition() + Vec2(0, -10));
			if ((*it)->getPosition().getDistance(player->getPosition()) < 40) {
				stopAc();
			}
		} while (it != enemys.begin());
		if ((*it)->getPosition().getDistance(player->getPosition()) < 40) {
			stopAc();
		}
	}
}

void Thunder::addKeyboardListener() {
	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(Thunder::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(Thunder::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, player);
}

void Thunder::alwaysMove(float f) {
	if (isPressed) {
		switch (pressedCode) {
		case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		case cocos2d::EventKeyboard::KeyCode::KEY_A:
			if (player->getPositionX() > 10)
				player->setPosition(player->getPositionX() - 10, player->getPositionY());
			break;
		case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		case cocos2d::EventKeyboard::KeyCode::KEY_D:
			if (player->getPositionX() < visibleSize.width - 10)
				player->setPosition(player->getPositionX() + 10, player->getPositionY());
			break;
		default:
			break;
		}
	}
}

void Thunder::update(float f) {
    // TODO
	count += f;
	if (count > 1) { count = 0.0; dir = -dir; }

	if (bullet != NULL) {
		bullet->setPosition(bullet->getPositionX(), bullet->getPositionY() + 20);
		if (bullet->getPositionY() > visibleSize.height - 5) {
			bullet->removeFromParentAndCleanup(true);
			bullet = NULL;
		}
	}

	for (unsigned i = 0; i < enemys.size(); i++) {
		if (enemys[i] != NULL) {
			enemys[i]->setPosition(enemys[i]->getPosition() + Vec2(dir, 0));
			if (bullet != NULL && bullet->getPosition().getDistance(enemys[i]->getPosition()) < 20) {
				EventCustom e("meet");
				e.setUserData(&i);
				_eventDispatcher->dispatchEvent(&e);
			}
		}
    }
}

void Thunder::fire() {
    if (bullet != NULL) return;
    bullet = Sprite::create("bullet.png");
    bullet->setPosition(player->getPosition());
	addChild(bullet);
	playFire();
}

void Thunder::addCustomListener() {
	auto meetListener = EventListenerCustom::create("meet", CC_CALLBACK_1(Thunder::meet, this));
	_eventDispatcher->addEventListenerWithFixedPriority(meetListener, 1);
}
void Thunder::meet(EventCustom* event) {
	unsigned* pos = (unsigned*)event->getUserData();
	bullet->removeFromParentAndCleanup(true);
	enemys[*pos]->removeFromParentAndCleanup(true);
	
	playExplore();
	bullet = NULL;
	enemys.erase(enemys.begin() + *pos, enemys.begin() + *pos + 1);
	if (enemys.size() <= 0) {
		stopAc();
	}
}


void Thunder::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
	pressedCode = code;
    switch (code) {
        case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_A:
			if (player->getPositionX() > 5)
				player->setPosition(player->getPositionX() - 5, player->getPositionY());
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_D:
			if (player->getPositionX() < visibleSize.width - 5)
				player->setPosition(player->getPositionX() + 5, player->getPositionY());
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
            fire();
            break;
        default:
            break;
    }
	isPressed = true;
}
void Thunder::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	isPressed = false;
}
