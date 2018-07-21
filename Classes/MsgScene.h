#ifndef __MSG_SCENE_H__
#define __MSG_SCENE_H__

#include "cocos2d.h"

class MsgScene : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	// a selector callback
	void menuStartCallback(cocos2d::Ref* pSender);

	// implement the "static create()" method manually
	CREATE_FUNC(MsgScene);
};

#endif // __MSG_SCENE_H__