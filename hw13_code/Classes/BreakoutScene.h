#pragma once

#include "cocos2d.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

class Breakout :public Layer {
public:
	void setPhysicsWorld(PhysicsWorld * world);
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init(PhysicsWorld* world);

    // implement the "static create()" method manually
	static Breakout* create(PhysicsWorld* world);

private:
    //“Ù¿÷º”‘ÿ”Î≤•∑≈
    Sprite* player;
	Sprite *clear, *protect, *newLife;
	PhysicsWorld* m_world;
	Label* label1, *label2, *label3;
	Size visibleSize;
	cocos2d::Vec2 origin;
	Vector<PhysicsBody*> enemys;
	Vector<PhysicsBody*> rewards;
	int time;
	int clr, pro, life;
	int HPbarPercentage;
	cocos2d::ProgressTimer* pT;
	bool flag;
	int savetime;

	void stopAc(int a);

    void preloadMusic();
    void playBgm();
	void playMeet();

    void addBackground();
    void addEdge();
	void addPlayer();

	void addContactListener();
	void addTouchListener();
	void addKeyboardListener();
	void removeFromEnemys(int tag, bool flag2);
	void addRewardLabel(int tag);
	void Duang(int tag);

	void update(float f);

	bool onConcactBegan(PhysicsContact& contact);

    bool onTouchBegan(Touch *touch, Event *unused_event);
    void onTouchMoved(Touch *touch, Event *unused_event);
    void onTouchEnded(Touch *touch, Event *unused_event);
    void onKeyPressed(EventKeyboard::KeyCode code, Event* event);
    void onKeyReleased(EventKeyboard::KeyCode code, Event* event);

	void newEnemys();
	void newReward();
	void addEnemy(int type, Point p);
	void addReward(int type, Point p);
};