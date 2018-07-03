#ifndef __THUNDER_H__
#define __THUNDER_H__

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

	void initSpriteFrame();		// ��ʼ������֡����
	void preloadMusic();        // Ԥ��������
	void GameOver();            // ��Ϸ����
								
	void replayCallback(Ref* pSender);	// ���水ť��Ӧ����
	void exitCallback(Ref* pSender);	// �˳���ť��Ӧ����

	void move(char dir);	// �ƶ�
	void attack();			// ����
	void crouch();			// ����
	void jump();			// ��
	void fireInTheHole();	// ����

	void generateObstacle();	// �����ϰ���
	void generateHostage();		// ��������
	void generateEnemy();		// ���ɵ���
	void enemyAutoAttack();	// �����Զ�����

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

	Vector<Sprite*> enemies;				// ����
	Vector<Sprite*> hostages;				// ����
	Vector<Sprite*>	sandBags;				// ɳ��
	Vector<Sprite*> woodenBarrels;			// ľͰ
	Vector<Sprite*> ironDrums;				// ��Ͱ
	Vector<Sprite*> bullets;				// �ӵ�

	bool isMove;
	bool isAttack;
	bool isCrouch;
	char moveDir;
	char shootDir;
	char lastCid;
	char bulletLevel;
};

#endif // __THUNDER_H__