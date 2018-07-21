#include "IntroScene.h"
#include "MsgScene.h"

USING_NS_CC;

Scene* IntroScene::createScene()
{
	return IntroScene::create();
}

bool IntroScene::init()
{
	if (!Scene::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto background = Sprite::create("IntroBG.png");
	background->setScale(2);
	background->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
	this->addChild(background, 0);

	auto startItem = MenuItemImage::create("IntroStart.png", "IntroStart.png", CC_CALLBACK_1(IntroScene::menuStartCallback, this));
	startItem->setScale(2);
	startItem->setPosition(Vec2(origin.x + visibleSize.width / 2 + 40, 210));

	auto menu = Menu::create(startItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	return true;
}

void IntroScene::menuStartCallback(Ref* pSender)
{
	auto myScene = MsgScene::create();
	//Close the cocos2d-x game scene and quit the application
	Director::getInstance()->replaceScene(TransitionFade::create(0.5, myScene, Color3B(0, 0, 0)));

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif

	/*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

	//EventCustom customEndEvent("game_scene_close_event");
	//_eventDispatcher->dispatchEvent(&customEndEvent);


}
