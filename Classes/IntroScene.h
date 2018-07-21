#ifndef __INTRO_SCENE_H__
#define __INTRO_SCENE_H__

#include "cocos2d.h"

class IntroScene : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	// a selector callback
	void menuStartCallback(cocos2d::Ref* pSender);

	// implement the "static create()" method manually
	CREATE_FUNC(IntroScene);
};

#endif // __INTRO_SCENE_H__
