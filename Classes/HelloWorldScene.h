#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include <vector>
#include "cocos2d.h"
USING_NS_CC;

class HelloWorld : public cocos2d::CCLayer
{
public:
	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();

	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::CCScene* scene();

	// a selector callback
	void menuCloseCallback(CCObject* pSender);

	// implement the "static node()" method manually
	CREATE_FUNC(HelloWorld);

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
	virtual void onEnter();

	bool warriorRun(float Rx, float Ry);
	void addTarget();
	void refreshCDTime();
	void moveEnemy(float dt);
	void spriteMoveFinished(CCNode* pSender);
	void effectPlayFinished(CCNode* pSender);
	void enemyMoveFinished(CCNode* pSender);
	void update(float t);
	void scheduleAStart(float dt);
	bool aStart(CCPoint pMap);
	void gameLogic(float dt)
	{
		addTarget();
		refreshCDTime();
	}
	CCAnimate* effect(int type);
	CCAnimate* enemyFightEffect();
	CCAnimate* warriorEffect();
	void gameLayer(CCObject* pSender);
	void gameBack(CCObject* pSender);
	void HelloWorld::gameOver();
	CCMenuItemImage* pCloseItem, *pStartItem, *pBackItem, *pAgainItem;
	CCSprite* map;
	CCSprite* warrior;
	CCSpriteFrameCache *frameCache;
	CCLabelTTF* hp, *cd, *kill;
	CCFiniteTimeAction* actionMoveMap;
	CCFiniteTimeAction* actionMoveWarrior;
	CCFiniteTimeAction* actionPlay;

	bool move(float warriorX, float warriorY, float mapX, float mapY, int direct);
	CCAnimate* moveFrame(int direction);
	CCAnimate* enemyMoveFrame(int direct);

};

#endif // __HELLOWORLD_SCENE_H__
