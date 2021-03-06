#pragma execution_character_set("utf-8")
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
	layer = Layer::create();
	layer->setAnchorPoint(Vec2(0, 0));
	layer->setPosition(Vec2(0, 0));
	this->addChild(layer);
	// background
	map0 = Sprite::create("background_0.jpg");
	map1 = Sprite::create("background_1.jpg");
	map2 = Sprite::create("background_2.jpg");
	map3 = Sprite::create("background_3.jpg");

	map0->setScale(2);
	map1->setScale(2);
	map2->setScale(2);
	map3->setScale(2);

	map0->setPosition(origin.x + map0->getContentSize().width, origin.y + map0->getContentSize().height);
	map1->setPosition(map0->getPositionX() + map1->getContentSize().width * 2, map0->getPositionY());
	map2->setPosition(map1->getPositionX() + map2->getContentSize().width * 2, map0->getPositionY());
	map3->setPosition(map2->getPositionX() + map3->getContentSize().width * 2, map0->getPositionY());

	layer->addChild(map0, 0);
	layer->addChild(map1, 0);
	layer->addChild(map2, 0);
	layer->addChild(map3, 0);

	// player
	player_head = Sprite::create("player_head.png");
	player_leg = Sprite::create("player_leg.png");

	player_head->setScale(2);
	player_leg->setScale(2);

	player_leg->setPosition(Vec2(origin.x + 200, origin.y + player_leg->getContentSize().height / 2 + 100));
	player_head->setPosition(Vec2(player_leg->getPositionX() + 10,
		player_leg->getPositionY() + player_head->getContentSize().height));

	player_head->setFlipX(true);
	player_leg->setFlipX(true);

	layer->addChild(player_leg, 3);
	layer->addChild(player_head, 3);

	// 初始化变量
	lastCid = 'D';
	isJump = false;

	initSpriteFrame();
	preloadMusic();

	generateObstacle();
	generateHostage();
	generateEnemy();

	addKeyboardListener();

	schedule(schedule_selector(GameScene::update), 0.05f, kRepeatForever, 0);

	schedule(schedule_selector(GameScene::enemyAction), 1.6f, kRepeatForever, 0);

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
	for (int i = 3; i < 11; i++) {
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

	// 手榴弹爆炸
	auto grenade_boom = Animation::create();
	for (int i = 1; i < 7; i++) {
		char szName[100] = { 0 };
		sprintf(szName, "grenade_boom_%d.png", i);
		grenade_boom->addSpriteFrameWithFile(szName);
	}
	grenade_boom->setDelayPerUnit(0.05f);
	AnimationCache::getInstance()->addAnimation(grenade_boom, "grenade_boom");
}

void GameScene::preloadMusic() {

}

void GameScene::generateObstacle() {
	
}

void GameScene::generateHostage() {
	for (int i = 0; i < 10; i++) {
		auto oldman = Sprite::create("oldman_0.png");
		oldman->setScale(2);
		auto x = cocos2d::random(50.0, (double)(4 * map0->getContentSize().width * 2- 50));
		oldman->setPosition(Vec2(x, origin.y + 100 + oldman->getContentSize().height / 2));
		layer->addChild(oldman, 1);
		hostages.pushBack(oldman);
		auto action = Animate::create(AnimationCache::getInstance()->getAnimation("oldman"));
		oldman->runAction(RepeatForever::create(action));
	}
}

void GameScene::generateEnemy() {
	auto enemy = Sprite::create("enemy_0.png");
	enemy->setScale(2);
	auto action = Animate::create(AnimationCache::getInstance()->getAnimation("enemy"));
	enemy->runAction(RepeatForever::create(action));
	//敌人随机屏幕左边或右边或上面出现
	int dir = cocos2d::random(0, 2);
	if (dir == 0) {
		//从上面出现
		auto x = cocos2d::random(50.0, (double)visibleSize.width - 50);
		auto enemyWorldPosition = Vec2(x, origin.y + visibleSize.height);
		auto enemyPosition = layer->convertToNodeSpace(enemyWorldPosition);
		enemy->setPosition(enemyPosition);
		enemy->setFlippedX(cocos2d::random(0, 1));
		enemy->setName("downing");
		auto moveDown = MoveBy::create(1, Vec2(0, 100 + enemy->getContentSize().height / 2 - visibleSize.height));
		enemy->runAction(Sequence::create(
			moveDown,
			CallFunc::create([=] {enemy->setName("downed"); }),
			nullptr
		));
	}
	else if (dir == 1) {
		auto enemyWorldPosition = Vec2(origin.x, origin.y + 100 + enemy->getContentSize().height / 2);
		auto enemyPosition = layer->convertToNodeSpace(enemyWorldPosition);
		enemy->setPosition(enemyPosition);
		enemy->setFlippedX(true);
	}
	else {
		auto enemyWorldPosition = Vec2(origin.x + visibleSize.width, origin.y + 100 + enemy->getContentSize().height / 2);
		auto enemyPosition = layer->convertToNodeSpace(enemyWorldPosition);
		enemy->setPosition(enemyPosition);
		enemy->setFlippedX(false);
	}
	layer->addChild(enemy, 1);
	enemies.pushBack(enemy);
}

void GameScene::update(float f) {

	if (isMove) move(moveDir);

	if (isCrouch) crouch();

	//移除屏幕外一定距离的敌人
	removeEnemy();
	// 检测玩家是否中弹
	testGetShot();
}

void GameScene::move(char dir) {
	auto r = AnimationCache::getInstance()->getAnimation("run");
	auto player_run = Animate::create(r);
	player_run->setTag(1);
	if (player_leg->getNumberOfRunningActionsByTag(1) == 0) {
		player_leg->runAction(player_run);
	}

	auto player_headPosition = player_head->getPosition();
	auto player_headWorldPosition = layer->convertToWorldSpace(player_headPosition);

	auto map3Position = map3->getPosition();
	auto map3WorldPosition = layer->convertToWorldSpace(map3Position);

	if (dir == 'A') {
		if (lastCid != 'A') {
			player_head->setFlipX(false);
			player_leg->setFlipX(false);
			player_leg->setPositionX(player_leg->getPositionX() + 20);
			shootDir = 'A';
		}
		lastCid = 'A';

		if (player_headWorldPosition.x - player_head->getContentSize().width >= 0) {
			auto moveBy1 = MoveBy::create(0.05f, Vec2(-10, 0));		
			auto moveBy2 = MoveBy::create(0.05f, Vec2(-10, 0));
			moveBy1->setFlags(3);
			moveBy2->setFlags(3);
			player_head->runAction(moveBy1);					
			player_leg->runAction(moveBy2);	
		}
	}
	else {
		if (lastCid != 'D') {
			player_head->setFlipX(true);
			player_leg->setFlipX(true);
			player_leg->setPositionX(player_leg->getPositionX() - 20);
			shootDir = 'D';
		}
		lastCid = 'D';

		auto moveBy1 = MoveBy::create(0.05f, Vec2(10, 0));
		auto moveBy2 = MoveBy::create(0.05f, Vec2(10, 0));
		moveBy1->setFlags(3);
		moveBy2->setFlags(3);
		player_head->runAction(moveBy1);
		player_leg->runAction(moveBy2);

		if (player_headWorldPosition.x >= visibleSize.width / 3 &&
			map3WorldPosition.x + map3->getContentSize().width > visibleSize.width) {
			auto moveBy1 = MoveBy::create(0.05f, Vec2(-10, 0));
			moveBy1->setFlags(3);
			layer->runAction(moveBy1);
		}
	}

	//若玩家与敌人相遇，停止移动
	stopRun();
}

void GameScene::attack() {
	// 判断是用平底锅还是用枪(平底锅打人质是松绑)
	bool useGun = true;
	auto playerPos = player_head->getPosition();
	Sprite* mostNearSprite;
	float mostNearDistance = 2000;
	for each (auto enemy in enemies) {
		auto distance = enemy->getPosition().getDistance(playerPos);
		if (distance < mostNearDistance) {
			mostNearSprite = enemy;
			mostNearDistance = distance;
		}
	}
	bool isHostage = false;
	for each (auto hostage in hostages) {
		auto distance = hostage->getPosition().getDistance(playerPos);
		if (distance < mostNearDistance && distance < 100) {
			mostNearSprite = hostage;
			mostNearDistance = distance;
			isHostage = true;
		}
	}
	if (mostNearDistance < 100) useGun = false;

	if (isHostage) {
		// 解救人质
		auto animation = AnimationCache::getInstance()->getAnimation("pan");
		auto animate = Animate::create(animation);
		player_head->runAction(animate);

		mostNearSprite->stopAllActions();
		auto moveBy_ = MoveBy::create(0.25f, Vec2(-60, 0));
		auto a = AnimationCache::getInstance()->getAnimation("saved");
		auto saved = Animate::create(a);
		auto r_ = AnimationCache::getInstance()->getAnimation("oldman_run");
		auto oldman_run = Animate::create(r_);
		mostNearSprite->runAction(
			Sequence::create(
				CallFunc::create([=]() {
					hostages.erase(hostages.getIndex(mostNearSprite));
				}),
				saved,
				Repeat::create(
					Sequence::create(
						Spawn::createWithTwoActions(oldman_run, moveBy_),
						Spawn::createWithTwoActions(oldman_run->reverse(), moveBy_),
						NULL
					),
					10
				),
			CallFunc::create([=]() {
				mostNearSprite->removeFromParentAndCleanup(true);
			}),
			NULL
			)
		);
	}
	else if (useGun) {
		// 枪攻击
		auto animation = AnimationCache::getInstance()->getAnimation("shoot");
		auto animate = Animate::create(animation);
		player_head->runAction(animate);
		// 根据子弹类型发射不同的子弹

		Sprite* bullet;
		if (bulletLevel == 'M') {
			bullet = Sprite::create("missile.png");
		}
		else {
			bullet = Sprite::create("bullet.png");
		}
		bullet->setScale(2);
		float posOffset = shootDir == 'A' ? -1 : 1;
		bullet->setPosition(Vec2(player_head->getPosition().x + posOffset * 10, player_head->getPosition().y));
		layer->addChild(bullet, 1);
		bullets.pushBack(bullet);
		bullet->runAction(Sequence::create(
			MoveBy::create(5.0f, Vec2(posOffset * 2000, 0)),
			CallFunc::create([=] {
				bullet->removeFromParentAndCleanup(true);
				bullets.erase(bullets.getIndex(bullet));
			}),
			nullptr
			)
		);
	}
	else {
		// 平底锅攻击
		auto animation = AnimationCache::getInstance()->getAnimation("pan");
		auto animate = Animate::create(animation);
		player_head->runAction(animate);

		mostNearSprite->stopAllActions();
		enemies.erase(enemies.getIndex(mostNearSprite));
		auto dead = Animate::create(AnimationCache::getInstance()->getAnimation("enemy_dead"));
		mostNearSprite->runAction(
			Sequence::create(
				dead,
				CallFunc::create([=] {
					mostNearSprite->removeFromParentAndCleanup(true);
				}),
				nullptr
			)
		);
	}
	// 碰撞检测并判断是攻击到的是人质还是敌人
}

void GameScene::jump() {
	// 跳的过程中可以左右移动和攻击
	if (isJump) return;
	isJump = true;
	auto animation = AnimationCache::getInstance()->getAnimation("jump1");
	auto jump1 = Animate::create(animation);
	auto jumpBy = JumpBy::create(0.8f, Vec2(0, 0), 200, 1);
	player_head->runAction(Sequence::create(jumpBy,
		CallFunc::create([this]() {this->isJump = false; }),
		nullptr));
	auto spawn = Spawn::createWithTwoActions(jumpBy->clone(), jump1);
	player_leg->runAction(spawn);
}

void GameScene::crouch() {
	// 蹲着的时候可以左右移动和攻击
}

void GameScene::fireInTheHole() {
	// 扔雷
	auto animation = AnimationCache::getInstance()->getAnimation("bomb");
	auto animate = Animate::create(animation);
	player_head->runAction(animate);

	auto grenade = Sprite::create("grenade.png");
	grenade->setScale(2);
	grenade->setPosition(player_head->getPosition());
	grenades.pushBack(grenade);
	layer->addChild(grenade);

	int param_shoot = shootDir == 'A' ? -1 : 1;
	int y = isJump ? -(player_leg->getPositionY() - 100) : 0;
	grenade->runAction(Sequence::create(
		JumpBy::create(1.0f, Vec2(param_shoot * 400, y), 300, 1),
		CallFunc::create([=]() {
			grenade->setName("boom");
		}),
		Animate::create(AnimationCache::getInstance()->getAnimation("grenade_boom")),
		CallFunc::create([=]() {
			grenades.erase(grenades.getIndex(grenade));
			grenade->removeFromParentAndCleanup(true);
		}),
		nullptr
	));
}

void GameScene::enemyAction(float f) {
	//产生敌人
	generateEnemy();

	for (auto enemy : enemies) {
		if (enemy->getName() == "dead" || enemy->getName() == "downing") continue;
		//1/2概率攻击，1/2概率移动
		bool attack = (cocos2d::random(0, 1) == 0);
		//敌人攻击
		if (attack) {
			if (enemy->getPositionX() - player_head->getPositionX() < 60 &&
				enemy->getPositionX() - player_head->getPositionX() > -60) {
				auto enemy_stab = Animate::create(AnimationCache::getInstance()->getAnimation("enemy_stab"));
				enemy->runAction(Sequence::create(
					//敌人拔刀动画
					enemy_stab,
					CallFunc::create([=] {
					//敌人拔完刀，玩家还在攻击范围内则游戏结束
					if (enemy->getPositionX() - player_head->getPositionX() < 60 &&
						enemy->getPositionX() - player_head->getPositionX() > -60) {
						GameOver();
					}
				}),
					nullptr
					));

			}
			else {
				auto enemy_shoot = Animate::create(AnimationCache::getInstance()->getAnimation("enemy_shoot"));
				enemy->runAction(Sequence::create(
					enemy_shoot,
					CallFunc::create([=] {
					auto bullet = Sprite::create("bullet.png");
					bullet->setFlippedX(enemy->isFlippedX());
					bullet->setScale(1.5);
					int dir = bullet->isFlippedX() ? 1 : -1;
					bullet->setPosition(enemy->getPosition());
					layer->addChild(bullet, 2);
					enemyBullets.pushBack(bullet);
					bullet->runAction(Sequence::create(MoveBy::create(2, Vec2(visibleSize.width * dir, 0)),
						CallFunc::create([=] {
						bullet->removeFromParentAndCleanup(true);
						enemyBullets.erase(enemyBullets.getIndex(bullet));
					}),
						nullptr));
				}),
					nullptr)
				);
			}
			continue;
		}

		//敌人朝着玩家或远离玩家移动，4/5概率朝着玩家移动
		auto ifAway = cocos2d::random(0, 4);
		//移动距离系数
		int dis = cocos2d::random(1, 8);
		bool ifLeft;
		if (player_head->getPositionX() > enemy->getPositionX()) {
			ifLeft = (ifAway == 0);  //玩家在敌人右边时，敌人1/5概率向左移动
		}
		else {
			ifLeft = (ifAway != 0);  //玩家在敌人左边时，敌人1/5概率向左移动
		}
		if (ifLeft) {
			enemy->setFlippedX(false);
			if (enemy->getPositionX() > origin.x) {
				auto move = MoveBy::create(1.6f, Vec2(-10 * dis, 0));
				enemy->runAction(move);
			}
		}
		else {
			enemy->setFlippedX(true);
			if (enemy->getPositionX() < origin.x + visibleSize.width) {
				auto move = MoveBy::create(1.6f, Vec2(10 * dis, 0));
				enemy->runAction(move);
			}
		}
		auto r = AnimationCache::getInstance()->getAnimation("enemy_run");
		auto enemy_run = Animate::create(r);
		enemy->runAction(Sequence::create(enemy_run, enemy_run->clone(), nullptr));

	}

}

void GameScene::testGetShot() {
	//判断玩家中弹
	for (auto bullet : enemyBullets) {
		if (player_head->getBoundingBox().containsPoint(bullet->getPosition())) {
			bullet->removeFromParentAndCleanup(true);
			enemyBullets.erase(enemyBullets.getIndex(bullet));
			GameOver();
			return;
		}
	}
	//判断敌人中弹
	for (auto bullet : bullets) {
		for (auto enemy : enemies) {
			if (enemy->getBoundingBox().containsPoint(bullet->getPosition())) {
				bullet->removeFromParentAndCleanup(true);
				bullets.erase(bullets.getIndex(bullet));
				//敌人中弹只播一次死亡动画，但可以继续挡子弹
				if (enemy->getName() != "dead") {
					enemyDead(enemy);
				}
				return;
			}
		}
	}
	//判断敌人中手榴弹
	for (auto grenade : grenades) {
		if (grenade->getName() != "boom") continue;
		for (auto enemy : enemies) {
			Rect grenadeRect = grenade->getBoundingBox();
			Rect boomRect = Rect(grenadeRect.getMinX() - 100, grenadeRect.getMinY(),
				grenadeRect.getMaxX() - grenadeRect.getMinX() + 200,
				grenadeRect.getMaxY() - grenadeRect.getMinY());
			if (boomRect.containsPoint(enemy->getPosition())) {
				if (enemy->getName() != "dead") {
					enemyDead(enemy);
					return;
				}
			}
		}
	}
}

void GameScene::enemyDead(Sprite* enemy) {
	enemy->setName("dead");
	enemy->stopAllActions();
	auto enemyDead = Animate::create(AnimationCache::getInstance()->getAnimation("enemy_dead"));
	enemy->runAction(Sequence::create(
		enemyDead,
		CallFunc::create([=] {
		enemy->removeFromParentAndCleanup(true);
		enemies.erase(enemies.getIndex(enemy));
	}),
		nullptr));
}

void GameScene::removeEnemy() {
	//删除屏幕外的敌人
	for (auto enemy : enemies) {
		auto enemyPosition = enemy->getPosition();
		auto enemyWorldPosition = layer->convertToWorldSpace(enemyPosition);
		if (enemyWorldPosition.x < origin.x - 100 ||
			enemyWorldPosition.x > origin.x + visibleSize.width + 100) {
			enemy->removeFromParentAndCleanup(true);
			enemies.erase(enemies.getIndex(enemy));
			break;
		}
	}
}

void GameScene::stopRun() {
	if (!isMove) return;
	for (auto enemy : enemies) {
		if (player_head->getPositionY() - enemy->getPositionY() > 50 ||
			player_head->getPositionY() - enemy->getPositionY() < -50) 
			return;
		if (lastCid == 'A') {
			if (player_head->getPositionX() > enemy->getPositionX() && 
				player_head->getPositionX() < enemy->getPositionX() + 2 * enemy->getContentSize().width) {
				player_head->stopActionsByFlags(3);
				player_leg->stopActionsByFlags(3);
				layer->stopActionsByFlags(3);
				return;
			}
		}
		else {
			if (player_head->getPositionX() < enemy->getPositionX() &&
				player_head->getPositionX() > enemy->getPositionX() - 2 * enemy->getContentSize().width) {
				player_head->stopActionsByFlags(3);
				player_leg->stopActionsByFlags(3);
				layer->stopActionsByFlags(3);
				return;
			}
		}
	}
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

void GameScene::showMenu() {
	auto label1 = Label::createWithTTF("Game Over~", "fonts/STXINWEI.TTF", 120);
	label1->setColor(Color3B(255, 0, 0));
	label1->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(label1, 3);

	auto label2 = Label::createWithTTF("重玩", "fonts/STXINWEI.TTF", 80);
	label2->setColor(Color3B(255, 0, 0));
	auto replayBtn = MenuItemLabel::create(label2, CC_CALLBACK_1(GameScene::replayCallback, this));
	Menu* replay = Menu::create(replayBtn, NULL);
	replay->setPosition(visibleSize.width / 2 - 160, visibleSize.height / 2 - 200);
	this->addChild(replay, 3);

	auto label3 = Label::createWithTTF("退出", "fonts/STXINWEI.TTF", 80);
	label3->setColor(Color3B(255, 0, 0));
	auto exitBtn = MenuItemLabel::create(label3, CC_CALLBACK_1(GameScene::exitCallback, this));
	Menu* exit = Menu::create(exitBtn, NULL);
	exit->setPosition(visibleSize.width / 2 + 180, visibleSize.height / 2 - 200);
	this->addChild(exit, 3);
}

void GameScene::GameOver() {
	_eventDispatcher->removeAllEventListeners();
	unschedule(schedule_selector(GameScene::enemyAction));
	unschedule(schedule_selector(GameScene::update));

	player_head->stopAllActions();
	player_leg->stopAllActions();
	player_leg->setVisible(false);
	player_head->setPosition(player_head->getPosition() - Vec2(0,50));
	auto dead = Animate::create(AnimationCache::getInstance()->getAnimation("dead"));
	player_head->runAction(Sequence::create(dead, CallFunc::create([=] {showMenu(); }), nullptr));
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
