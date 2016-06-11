#ifndef __THUNDER_H__
#define __THUNDER_H__

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include <vector>

USING_NS_CC;

class Thunder : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

    // implement the "static create()" method manually
    CREATE_FUNC(Thunder);
private:
    void preloadMusic();
    void playBgm();
	void playFire();
	void playExplore();

    
    void addKeyboardListener();
	void addCustomListener();

    void addEnemy(int n);
    void update(float f);
    void fire();
    void meet(EventCustom* event);
	void alwaysMove(float f);
	void getClose(float f);
	void stopAc();

    void onKeyPressed(EventKeyboard::KeyCode code, Event* event);
    void onKeyReleased(EventKeyboard::KeyCode code, Event* event);

	int move;
    Size visibleSize;
	Sprite* player;
	Sprite* bullet;

    std::vector<Sprite*> enemys;
	CocosDenshion::SimpleAudioEngine* audio;
	static double count;
	static int dir;
	bool isPressed;
	EventKeyboard::KeyCode pressedCode;
};

#endif // __THUNDER_H__
