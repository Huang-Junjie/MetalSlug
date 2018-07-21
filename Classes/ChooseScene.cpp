#include "ChooseScene.h"
#include "GameScene.h"

USING_NS_CC;

Scene* ChooseScene::createScene()
{
	return ChooseScene::create();
}

bool ChooseScene::init()
{
	if (!Scene::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto background = Sprite::create("ChooseBG.png");
	background->setScale(2);
	background->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
	this->addChild(background, 0);

	startItem = MenuItemImage::create("chosen.png", "chosen.png", CC_CALLBACK_1(ChooseScene::menuStartCallback, this));
	startItem->setScale(2);
	startItem->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
	startItem->setVisible(false);

	auto menu = Menu::create(startItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	auto mouseListener = EventListenerMouse::create();
	mouseListener->onMouseMove = CC_CALLBACK_1(ChooseScene::mouseMove, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

	return true;
}

void ChooseScene::menuStartCallback(Ref* pSender)
{
	auto myScene = GameScene::create();
	//Close the cocos2d-x game scene and quit the application
	Director::getInstance()->replaceScene(TransitionFade::create(0.5, myScene, Color3B(0, 0, 0)));

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif

	/*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

	//EventCustom customEndEvent("game_scene_close_event");
	//_eventDispatcher->dispatchEvent(&customEndEvent);


}

void ChooseScene::mouseMove(Event* event) {
	EventMouse* e = (EventMouse*)event;
	float x = e->getCursorX(), y = e->getCursorY();
	if (x > 70 && x < 540 && y > 40 && y < 630) {
		startItem->setVisible(true);
	}
	else {
		startItem->setVisible(false);
	}
}
