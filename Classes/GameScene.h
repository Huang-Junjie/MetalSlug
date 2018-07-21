#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
USING_NS_CC;

class GameScene : public cocos2d::Scene {
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	void update(float f);

	void addKeyboardListener();

	void onKeyPressed(EventKeyboard::KeyCode code, Event* event);
	void onKeyReleased(EventKeyboard::KeyCode code, Event* event);

	void initSpriteFrame();		// 初始化序列帧动画
	void preloadMusic();        // 预加载音乐
	void GameOver();            // 游戏结束
	void showMenu();

	void replayCallback(Ref* pSender);	// 重玩按钮响应函数
	void exitCallback(Ref* pSender);	// 退出按钮响应函数

	void move(char dir);	// 移动
	void attack();			// 攻击
	void crouch();			// 蹲下
	void jump();			// 跳
	void fireInTheHole();	// 扔雷

	void generateObstacle();	// 生成障碍物
	void generateHostage();		// 生成人质
	void generateEnemy();		// 生成敌人
	void testGetShot();			// 检测玩家和敌人是否中弹
	void removeEnemy();			// 移除屏幕外一定距离的敌人
	void enemyAction(float f);	// 敌人的动作
	void enemyDead(Sprite* enemy);  //敌人死亡
	void stopRun();
	// implement the "static create()" method manually
	CREATE_FUNC(GameScene);

private:
	Size visibleSize;
	Vec2 origin;

	Sprite* map0;
	Sprite* map1;
	Sprite* map2;
	Sprite* map3;
	Sprite* player_head;
	Sprite* player_leg;

	Vector<Sprite*> enemies;				// 敌人
	Vector<Sprite*> hostages;				// 人质
	Vector<Sprite*>	sandBags;				// 沙包
	Vector<Sprite*> woodenBarrels;			// 木桶
	Vector<Sprite*> ironDrums;				// 铁桶
	Vector<Sprite*> bullets;				// 玩家子弹
	Vector<Sprite*> enemyBullets;			// 敌人子弹
	Vector<Sprite*> grenades;				// 手雷

	Layer* layer;
	bool isMove;
	bool isAttack;
	bool isCrouch;
	bool isJump;
	char moveDir;
	char shootDir;
	char lastCid;
	char bulletLevel;
};

#endif // __GAME_SCENE_H__