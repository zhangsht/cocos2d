#ifndef __MENU_SEBCE_H__
#define __MENU_SEBCE_H__

#include "cocos2d.h"
USING_NS_CC;

class MenuSence : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();
	void MenuSence::menuCallback(cocos2d::Ref* pSender);
	virtual bool onTouchBegan(Touch *touch, Event *unused_event);

	// implement the "static create()" method manually
	CREATE_FUNC(MenuSence);
};

#endif // __MENU_SEBCE_H__
