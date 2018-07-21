#ifndef __CHOOSE_SCENE_H__
#define __CHOOSE_SCENE_H__

#include "cocos2d.h"

class ChooseScene : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	// a selector callback
	void menuStartCallback(cocos2d::Ref* pSender);

	void mouseMove(cocos2d::Event* event);

	// implement the "static create()" method manually
	CREATE_FUNC(ChooseScene);

private:
	cocos2d::MenuItemImage* startItem;
};

#endif // __CHOOSE_SCENE_H__
