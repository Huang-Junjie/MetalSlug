#include "GameScene.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* GameScene::createScene() {
	return GameScene::create();
}

// on "init" you need to initialize your instance
bool GameScene::init() {
	if (!Scene::init()) {
		return false;
	}

	visibleSize = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();

	// background
	map0 = Sprite::create("background_0.jpg");
	map1 = Sprite::create("background_1.jpg");
	map2 = Sprite::create("background_2.jpg");
	map3 = Sprite::create("background_3.jpg");

	map0->setPosition(origin.x + map0->getContentSize().width / 2, origin.y + map0->getContentSize().height / 2);
	map1->setPosition(map0->getPositionX() + map1->getContentSize().width, map0->getPositionY());
	map2->setPosition(map1->getPositionX() + map2->getContentSize().width, map0->getPositionY());
	map3->setPosition(map2->getPositionX() + map3->getContentSize().width, map0->getPositionY());

	this->addChild(map0, 0);
	this->addChild(map1, 0);
	this->addChild(map2, 0);
	this->addChild(map3, 0);

	// player
	player_head = Sprite::create("player_head.png");
	player_leg = Sprite::create("player_leg.png");

	player_leg->setPosition(Vec2(origin.x + 100, origin.y + player_leg->getContentSize().height / 2));
	player_head->setPosition(Vec2(player_leg->getPositionX() + 5,
		player_leg->getPositionY() + player_head->getContentSize().height / 2));

	player_head->setFlipX(true);
	player_leg->setFlipX(true);

	this->addChild(player_leg, 1);
	this->addChild(player_head, 1);	

	// 初始化变量
	lastCid = 'D';

	initSpriteFrame();
	preloadMusic();

	generateObstacle();
	generateHostage();
	generateEnemy();

	addKeyboardListener();

	schedule(schedule_selector(GameScene::update), 0.05f, kRepeatForever, 0);

	return true;
}

void GameScene::initSpriteFrame() {
	/******************************************************************************
	*                                 Player                                     *
	******************************************************************************/

	// 玩家抖动
	// player
	auto player = Animation::create();
	for (int i = 0; i < 3; i++) {
		char szName[100] = { 0 };
		sprintf(szName, "player_head_%d.png", i);
		player->addSpriteFrameWithFile(szName);
	}
	player->setDelayPerUnit(0.2f);
	auto action = Animate::create(player);
	player_head->runAction(RepeatForever::create(action));

	// 玩家跑动
	// run
	auto run = Animation::create();
	for (int i = 0; i < 6; i++) {
		char szName[100] = { 0 };
		sprintf(szName, "run_%d.png", i);
		run->addSpriteFrameWithFile(szName);
	}
	run->setDelayPerUnit(0.3f / 6.0f);
	AnimationCache::getInstance()->addAnimation(run, "run");

	// 玩家射击
	// shoot
	auto shoot = Animation::create();
	for (int i = 0; i < 5; i++) {
		char szName[100] = { 0 };
		sprintf(szName, "shoot_%d.png", i);
		shoot->addSpriteFrameWithFile(szName);
	}
	shoot->addSpriteFrameWithFile("player_head.png");
	shoot->setDelayPerUnit(0.1f / 6.0f);
	AnimationCache::getInstance()->addAnimation(shoot, "shoot");

	// 玩家平底锅
	// pan
	auto pan = Animation::create();
	for (int i = 0; i < 5; i++) {
		char szName[100] = { 0 };
		sprintf(szName, "pan_%d.png", i);
		pan->addSpriteFrameWithFile(szName);
	}
	pan->addSpriteFrameWithFile("player_head.png");
	pan->setDelayPerUnit(0.3f / 6.0f);
	AnimationCache::getInstance()->addAnimation(pan, "pan");

	// 玩家扔手榴弹
	// bomb
	auto bomb = Animation::create();
	for (int i = 0; i < 5; i++) {
		char szName[100] = { 0 };
		sprintf(szName, "bomb_%d.png", i);
		bomb->addSpriteFrameWithFile(szName);
	}
	bomb->addSpriteFrameWithFile("player_head.png");
	bomb->setDelayPerUnit(0.15f / 5.0f);
	AnimationCache::getInstance()->addAnimation(bomb, "bomb");

	// 玩家跳: jump1只是腿部的部分，故完整跳还需要加入JumpBy，具体看下面
	// jump
	auto jump1 = Animation::create(); // leg
	for (int i = 0; i < 5; i++) {
		char szName[100] = { 0 };
		sprintf(szName, "jump_leg_%d.png", i);
		jump1->addSpriteFrameWithFile(szName);
	}
	jump1->addSpriteFrameWithFile("player_leg.png");
	jump1->setDelayPerUnit(1.0f / 5.0f);
	AnimationCache::getInstance()->addAnimation(jump1, "jump1");
	/*******************************************************************************
	jump 完整用法：
	auto animation = AnimationCache::getInstance()->getAnimation("jump1");
	auto jump1 = Animate::create(animation);
	auto jumpBy = JumpBy::create(0.8f, Vec2(0, 0), 100, 1);
	player_head->runAction(jumpBy);
	auto spawn = Spawn::createWithTwoActions(jumpBy->clone(), jump1);
	player_leg->runAction(spawn);
	*******************************************************************************/

	// 玩家死亡
	// dead
	auto dead = Animation::create();
	for (int i = 0; i < 11; i++) {
		char szName[100] = { 0 };
		sprintf(szName, "player_dead_%d.png", i);
		dead->addSpriteFrameWithFile(szName);
	}
	dead->setDelayPerUnit(0.1f);
	AnimationCache::getInstance()->addAnimation(dead, "dead");


	/******************************************************************************
	*                                 Oldman                                     *
	******************************************************************************/

	/*******************************************************************************
	oldman 被解救后动画全过程：
	oldman->stopAllActions();
	auto moveBy_ = MoveBy::create(0.25f, Vec2(-30, 0));
	auto a = AnimationCache::getInstance()->getAnimation("saved");
	auto saved = Animate::create(a);
	auto r_ = AnimationCache::getInstance()->getAnimation("oldman_run");
	auto oldman_run = Animate::create(r_);
	oldman->runAction(
	Sequence::create(
	saved,
	Repeat::create(
	Sequence::create(
	Spawn::createWithTwoActions(oldman_run, moveBy_),
	Spawn::createWithTwoActions(oldman_run->reverse(), moveBy_),
	NULL
	),
	10
	),
	NULL
	)
	);
	*******************************************************************************/


	// oldman 挣扎
	// oldman
	auto oldman = Animation::create();
	for (int i = 0; i < 3; i++) {
		char szName[100] = { 0 };
		sprintf(szName, "oldman_%d.png", i);
		oldman->addSpriteFrameWithFile(szName);
	}
	oldman->setDelayPerUnit(0.2f);
	AnimationCache::getInstance()->addAnimation(oldman, "oldman");

	// oldman 获救
	// oldman saved
	auto saved = Animation::create();
	for (int i = 0; i < 3; i++) {
		char szName[100] = { 0 };
		sprintf(szName, "oldman_saved_%d.png", i);
		saved->addSpriteFrameWithFile(szName);
	}
	saved->setDelayPerUnit(0.5f / 3.0f);
	AnimationCache::getInstance()->addAnimation(saved, "saved");

	// oldman 跑步
	// oldman run
	auto oldman_run = Animation::create();
	for (int i = 0; i < 5; i++) {
		char szName[100] = { 0 };
		sprintf(szName, "oldman_run_%d.png", i);
		oldman_run->addSpriteFrameWithFile(szName);
	}
	oldman_run->setDelayPerUnit(0.05f);
	AnimationCache::getInstance()->addAnimation(oldman_run, "oldman_run");


	/******************************************************************************
	*                                 Enemy                                      *
	******************************************************************************/

	/*******************************************************************************
	敌人完整动画展示：3次行走->射击->三次行走->出刀->三次行走->死亡
	enemy->stopAllActions();
	auto moveBy = MoveBy::create(0.8f, Vec2(-10, 0));
	auto s = AnimationCache::getInstance()->getAnimation("enemy_shoot");
	auto shoot = Animate::create(s);
	auto r = AnimationCache::getInstance()->getAnimation("enemy_run");
	auto enemy_run = Animate::create(r);
	auto enemy_stab = Animate::create(AnimationCache::getInstance()->getAnimation("enemy_stab"));
	auto enemy_dead = Animate::create(AnimationCache::getInstance()->getAnimation("enemy_dead"));
	enemy->runAction(
	Sequence::create(
	Repeat::create(
	Spawn::createWithTwoActions(enemy_run, moveBy),
	3
	),
	shoot,
	Repeat::create(
	Spawn::createWithTwoActions(enemy_run, moveBy),
	3
	),
	enemy_stab,
	Repeat::create(
	Spawn::createWithTwoActions(enemy_run, moveBy),
	3
	),
	enemy_dead,
	NULL
	)
	);
	*******************************************************************************/

	// 敌人抖动
	// enemy
	auto enemy = Animation::create();
	for (int i = 0; i < 2; i++) {
		char szName[100] = { 0 };
		sprintf(szName, "enemy_%d.png", i);
		enemy->addSpriteFrameWithFile(szName);
	}
	enemy->setDelayPerUnit(0.4f);
	AnimationCache::getInstance()->addAnimation(enemy, "enemy");

	// 敌人跑动，需加MoveBy
	// enemy run
	auto enemy_run = Animation::create();
	for (int i = 0; i < 8; i++) {
		char szName[100] = { 0 };
		sprintf(szName, "enemy_walk_%d.png", i);
		enemy_run->addSpriteFrameWithFile(szName);
	}
	enemy_run->setDelayPerUnit(0.1f);
	AnimationCache::getInstance()->addAnimation(enemy_run, "enemy_run");
	/*******************************************************************************
	敌人跑动完整用法：
	enemy->stopAllActions();
	auto moveBy = MoveBy::create(0.8f, Vec2(-10, 0));
	auto r = AnimationCache::getInstance()->getAnimation("enemy_run");
	auto enemy_run = Animate::create(r);
	enmey->runAction(Spawn::createWithTwoActions(moveBy, enemy_run));
	*******************************************************************************/

	// 敌人射击
	// enemy shoot
	auto enemy_shoot = Animation::create();
	for (int i = 0; i < 11; i++) {
		char szName[100] = { 0 };
		sprintf(szName, "enemy_shoot_%d.png", i);
		enemy_shoot->addSpriteFrameWithFile(szName);
	}
	enemy_shoot->setDelayPerUnit(0.15f);
	AnimationCache::getInstance()->addAnimation(enemy_shoot, "enemy_shoot");

	// 敌人出刀
	// enemy stab
	auto enemy_stab = Animation::create();
	for (int i = 0; i < 7; i++) {
		char szName[100] = { 0 };
		sprintf(szName, "enemy_stab_%d.png", i);
		enemy_stab->addSpriteFrameWithFile(szName);
	}
	enemy_stab->setDelayPerUnit(0.15f);
	AnimationCache::getInstance()->addAnimation(enemy_stab, "enemy_stab");

	// 敌人死亡
	// enemy dead
	auto enemy_dead = Animation::create();
	for (int i = 0; i < 7; i++) {
		char szName[100] = { 0 };
		sprintf(szName, "enemy_dead_%d.png", i);
		enemy_dead->addSpriteFrameWithFile(szName);
	}
	enemy_dead->setDelayPerUnit(0.1f);
	AnimationCache::getInstance()->addAnimation(enemy_dead, "enemy_dead");
}

void GameScene::preloadMusic() {

}

void GameScene::generateObstacle() {
	
}

void GameScene::generateHostage() {
	// one oldman for test
	auto oldman = Sprite::create("oldman_0.png");
	oldman->setPosition(Vec2(origin.x + 300, origin.y + 80 + oldman->getContentSize().height / 2));
	this->addChild(oldman, 1);
	hostages.pushBack(oldman);
	
	auto action = Animate::create(AnimationCache::getInstance()->getAnimation("oldman"));
	oldman->runAction(RepeatForever::create(action));
}

void GameScene::generateEnemy() {
	// one enemy for test
	auto enemy = Sprite::create("enemy_0.png");
	enemy->setPosition(Vec2(origin.x + 500, origin.y + 80 + enemy->getContentSize().height / 2));
	this->addChild(enemy, 1);
	enemies.pushBack(enemy);
	
	auto action2 = Animate::create(AnimationCache::getInstance()->getAnimation("enemy"));
	enemy->runAction(RepeatForever::create(action2));
}

void GameScene::update(float f) {

	if (isMove) move(moveDir);

	if (isCrouch) crouch();

	enemyAutoAttack();

	// 自动移除飞出屏幕外的子弹
	
}

void GameScene::move(char dir) {
	auto r = AnimationCache::getInstance()->getAnimation("run");
	auto player_run = Animate::create(r);
	player_run->setTag(1);
	if (player_leg->getNumberOfRunningActionsByTag(1) == 0) {
		player_leg->runAction(player_run);
	}

	if (dir == 'A') {
		if (lastCid != 'A') {
			player_head->setFlipX(false);
			player_leg->setFlipX(false);
			player_leg->setPositionX(player_leg->getPositionX() + 10);
		}
		lastCid = 'A';
		
		if (player_head->getPositionX() - player_head->getContentSize().width / 2 >= 0) {
			auto moveBy1 = MoveBy::create(0.05f, Vec2(-5, 0));		
			auto moveBy2 = MoveBy::create(0.05f, Vec2(-5, 0));
			player_head->runAction(moveBy1);					
			player_leg->runAction(moveBy2);	
		}
	}
	else {
		if (lastCid != 'D') {
			player_head->setFlipX(true);
			player_leg->setFlipX(true);
			player_leg->setPositionX(player_leg->getPositionX() - 10);
		}
		lastCid = 'D';

		if (player_head->getPositionX() >= visibleSize.width / 3) {
			if (map3->getPositionX() + map3->getContentSize().width / 2 <= visibleSize.width) {
				auto moveBy1 = MoveBy::create(0.05f, Vec2(5, 0));
				auto moveBy2 = MoveBy::create(0.05f, Vec2(5, 0));
				player_head->runAction(moveBy1);
				player_leg->runAction(moveBy2);
			}
			else {
				map0->setPositionX(map0->getPositionX() - 5);
				map1->setPositionX(map1->getPositionX() - 5);
				map2->setPositionX(map2->getPositionX() - 5);
				map3->setPositionX(map3->getPositionX() - 5);
				auto it1 = enemies.begin();
				for (; it1 != enemies.end(); it1++) {
					(*it1)->setPositionX((*it1)->getPositionX() - 5);
				}
				auto it2 = hostages.begin();
				for (; it2 != hostages.end(); it2++) {
					(*it2)->setPositionX((*it2)->getPositionX() - 5);
				}
			}
		}
		else {
			auto moveBy1 = MoveBy::create(0.05f, Vec2(5, 0));
			auto moveBy2 = MoveBy::create(0.05f, Vec2(5, 0));
			player_head->runAction(moveBy1);
			player_leg->runAction(moveBy2);
		}
	}
}

void GameScene::attack() {
	// 判断是用平底锅还是用枪(平底锅打人质是松绑)
	// 根据子弹类型发射不同的子弹
	// 碰撞检测并判断是攻击到的是人质还是敌人
}

void GameScene::jump() {
	// 跳的过程中可以左右移动和攻击
}

void GameScene::crouch() {
	// 蹲着的时候可以左右移动和攻击
}

void GameScene::fireInTheHole() {
	// 扔雷
}

void GameScene::enemyAutoAttack() {
	// 根据判断是用刺刀还是枪
	// 每隔一段时间会扔手雷
	
}

// 添加键盘事件监听器
void GameScene::addKeyboardListener() {
	auto keyboardlistener = EventListenerKeyboard::create();
	keyboardlistener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
	keyboardlistener->onKeyReleased = CC_CALLBACK_2(GameScene::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardlistener, this);
}

void GameScene::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
		case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		case EventKeyboard::KeyCode::KEY_CAPITAL_A:
		case EventKeyboard::KeyCode::KEY_A:
			moveDir = 'A';
			isMove = true;
			break;
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		case EventKeyboard::KeyCode::KEY_CAPITAL_D:
		case EventKeyboard::KeyCode::KEY_D:
			moveDir = 'D';
			isMove = true;
			break;
		case EventKeyboard::KeyCode::KEY_CAPITAL_W:
		case EventKeyboard::KeyCode::KEY_W:
			shootDir = 'W';
			break;
		case EventKeyboard::KeyCode::KEY_CAPITAL_S:
		case EventKeyboard::KeyCode::KEY_S:
			// shootDir = 'S';	// 暂时先不做跳起来往下射击的操作
			isCrouch = true;
			break;
		case EventKeyboard::KeyCode::KEY_CAPITAL_J:
		case EventKeyboard::KeyCode::KEY_J:
			attack();
			break;
		case EventKeyboard::KeyCode::KEY_CAPITAL_K:
		case EventKeyboard::KeyCode::KEY_K:
			jump();
			break;
		case EventKeyboard::KeyCode::KEY_CAPITAL_L:
		case EventKeyboard::KeyCode::KEY_L:
			fireInTheHole();
			break;
	}
}

void GameScene::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
		case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		case EventKeyboard::KeyCode::KEY_A:
		case EventKeyboard::KeyCode::KEY_CAPITAL_A:
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		case EventKeyboard::KeyCode::KEY_D:
		case EventKeyboard::KeyCode::KEY_CAPITAL_D:
			isMove = false;
			break;
		case EventKeyboard::KeyCode::KEY_CAPITAL_W:
		case EventKeyboard::KeyCode::KEY_W:
			shootDir = lastCid;
			break;
		case EventKeyboard::KeyCode::KEY_CAPITAL_S:
		case EventKeyboard::KeyCode::KEY_S:
			isCrouch = false;
			break;
	}
}

void GameScene::GameOver() {
	_eventDispatcher->removeAllEventListeners();
	unschedule(schedule_selector(GameScene::update));

	auto label1 = Label::createWithTTF("Game Over~", "fonts/STXINWEI.TTF", 60);
	label1->setColor(Color3B(0, 0, 0));
	label1->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(label1);

	auto label2 = Label::createWithTTF("重玩", "fonts/STXINWEI.TTF", 40);
	label2->setColor(Color3B(0, 0, 0));
	auto replayBtn = MenuItemLabel::create(label2, CC_CALLBACK_1(GameScene::replayCallback, this));
	Menu* replay = Menu::create(replayBtn, NULL);
	replay->setPosition(visibleSize.width / 2 - 80, visibleSize.height / 2 - 100);
	this->addChild(replay);

	auto label3 = Label::createWithTTF("退出", "fonts/STXINWEI.TTF", 40);
	label3->setColor(Color3B(0, 0, 0));
	auto exitBtn = MenuItemLabel::create(label3, CC_CALLBACK_1(GameScene::exitCallback, this));
	Menu* exit = Menu::create(exitBtn, NULL);
	exit->setPosition(visibleSize.width / 2 + 90, visibleSize.height / 2 - 100);
	this->addChild(exit);
}

// 继续或重玩按钮响应函数
void GameScene::replayCallback(Ref * pSender) {
	Director::getInstance()->replaceScene(GameScene::createScene());
}

// 退出
void GameScene::exitCallback(Ref * pSender) {
	Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}
