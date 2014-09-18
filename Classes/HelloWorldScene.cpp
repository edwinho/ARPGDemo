#include "HelloWorldScene.h"
#include "AppMacros.h"
USING_NS_CC;
#include <math.h>

CCScene* HelloWorld::scene()
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::create();

	// 'layer' is an autorelease object
	HelloWorld *layer = HelloWorld::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}
struct Node
{
	float x, y, f, g;
	int direct;
};

const float obstructionX[4][2] = { { 115, 124 }, { 188, 204 }, { 246, 311 }, { 335, 400 } };
const float obstructionY[4][2] = { { 113, 188 }, { 155, 228 }, {  72, 130 }, { 155, 180} };

Node usePoint[5000];
Node pathPoint[1000];
Node usePointTemp[8];
int pathCount = 0,count = 0,target_count = 0;
CCSprite* target[5];
CCSprite* enemy[4];
CCSprite* effectPlay;
CCSprite* enemyEffectPlay;
bool play = false, boob = false, left = true, touch = false;
bool enemyFight[4], chooseEnemy[4], targetFight[5], enterVisible[4];
int hpNum, cdNum, killNum=0, enemyCDTime;
const CCPoint pEnemy[4][2] = { ccp(150, 220), ccp(150, 140), ccp(120, 80), ccp(200, 80), ccp(380, 220), ccp(280, 220), ccp(380, 90), ccp(380, 150) };
CCRect enemyRect[4], enemyVisible[4];
CCPoint pTarget;
float g;


void HelloWorld::spriteMoveFinished(CCNode* pSender)
{
	CCSprite *sprite = (CCSprite *)pSender;
	sprite->setVisible(false);
}
void HelloWorld::effectPlayFinished(CCNode* pSender)
{
	CCSprite *sprite = (CCSprite *)pSender;
	sprite->setVisible(false);
}
void HelloWorld::enemyMoveFinished(CCNode* pSender)
{
	for (int i = 0; i < 4; i++)
	{
		enterVisible[i] = false;
	}
}

void HelloWorld::refreshCDTime()
{
	enemyCDTime = enemyCDTime == 0 ? 0 : enemyCDTime - 1;

	cdNum = cdNum == 0 ? 0 : cdNum - 1;
	char str[10];
	sprintf(str, "CD:%d", cdNum);
	cd->setString(str);
	cdNum == 0 ? cd->setColor(ccc3(0, 255, 0)) : cd->setColor(ccc3(255, 255, 255));
}

void HelloWorld::addTarget()
{
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	target[target_count] = CCSprite::create("Target.png", CCRect(0, 0, 16, 16));

	// Determine where to spawn the target along the Y axis

	target[target_count]->setVisible(true);
	targetFight[target_count] = true;
	int minDuration = 2.0;
	int maxDuration = 4.0;
	int rangeDuration = maxDuration - minDuration + 2;
	int actualDuration = (CCRANDOM_0_1() * rangeDuration) + minDuration;
	map->addChild(target[target_count], 0);
	CCMoveTo* actionMove;
	if (boob)
	{
		int minY = target[target_count]->getContentSize().height / 2;
		int maxY = visibleSize.height - target[target_count]->getContentSize().height / 2;
		int rangeY = maxY - minY;
		int actualY = (CCRANDOM_0_1() * rangeY) + minY;
		//target[target_count]->setPosition(CCPoint(visibleSize.width + target[target_count]->getContentSize().width / 2, actualY));
		target[target_count]->setPosition(ccp(280, 97));
		actionMove = CCMoveTo::create(actualDuration,
			CCPoint(-target[target_count]->getContentSize().width, actualY));
	}
	else
	{
		int minX = target[target_count]->getContentSize().width / 2;
		int maxX = visibleSize.width - target[target_count]->getContentSize().width / 2;
		int rangeX = maxX - minX;
		int actualX = (CCRANDOM_0_1() * rangeX) + minX;
		target[target_count]->setPosition(ccp(375, 163));
		actionMove = CCMoveTo::create(actualDuration,
			CCPoint(actualX, -target[target_count]->getContentSize().height));
	}
	boob = !boob;
	// Create the target slightly off-screen along the right edge,
	// and along a random position along the Y axis as calculated above

	// Determine speed of the target

	auto actionMoveDone = CCCallFuncN::create(this, callfuncN_selector(HelloWorld::spriteMoveFinished));

	target[target_count]->runAction(CCSequence::create(actionMove, actionMoveDone, NULL));
	target_count++;
	target_count = target_count == 5 ? 0 : target_count;

}

void HelloWorld::update(float t)
{
	
	if (hpNum == 0)
	{
		gameOver();
		return;
	}
	
	auto warriorRect = CCRect(
		warrior->getPositionX() - warrior->getTextureRect().size.width * 0.5f,
		warrior->getPositionY() - warrior->getTextureRect().size.height*2,
		warrior->getTextureRect().size.width,
		warrior->getTextureRect().size.height);
	auto warriorAttackRect = CCRect(
		warrior->getPositionX() - warrior->getTextureRect().size.width * 1.5f,
		warrior->getPositionY() - warrior->getTextureRect().size.height * 2.5f,
		warrior->getTextureRect().size.width * 3,
		warrior->getTextureRect().size.height * 3);
	for (int j = 0; j < 5; j++)
	{

		/*if (sqrt(
			(target[j]->getPositionX() - warrior->getPositionX()+10)*(target[j]->getPositionX() - warrior->getPositionX()+10) +
			(target[j]->getPositionY() - warrior->getPositionY()+5)*(target[j]->getPositionY() - warrior->getPositionY()+5)) < 10)*/
		if (warriorRect.containsPoint(ccp(target[j]->getPositionX(), target[j]->getPositionY())))
		{
			effectPlay->setPosition(ccp(warrior->getPositionX()-15, warrior->getPositionY()-5 ));
			effectPlay->setVisible(true);
			auto actionPlayDone = CCCallFuncN::create(this, callfuncN_selector(HelloWorld::effectPlayFinished));
			effectPlay->runAction(CCSequence::create(CCRepeat::create(effect(1), 1.0f), actionPlayDone, NULL));
			target[j]->setVisible(false);
			if (targetFight[j])
			{
				targetFight[j] = false;
				hpNum = hpNum == 0 ? 0 : hpNum - 1;
				char str[10];
				sprintf(str, "HP:%d", hpNum);
				hp->setString(str);
				hpNum > 2 ? hp->setColor(ccc3(255, 255, 255)) : hp->setColor(ccc3(255, 0, 0));
			}
		}

	}
	
	for (int i = 0; i < 4; i++)
	{
		if (chooseEnemy[i])
		{
			pTarget = enemy[i]->getPosition();
		}

		//是否进入Enemy视野
		if (!enterVisible[i] && enemyVisible[i].containsPoint(ccp(warrior->getPosition().x, warrior->getPositionY()-15)))
		{
			enterVisible[i] = true;
			enemy[i]->stopAllActions();
			if (i == 1 || i == 2)
			{
				if (warrior->getPositionX() > enemy[i]->getPositionX())
				{
					enemy[i]->setRotationY(0);
					enemy[i]->runAction(CCRepeatForever::create(enemyMoveFrame(2)));
				}
				else
				{
					enemy[i]->setRotationY(180);
					enemy[i]->runAction(CCRepeatForever::create(enemyMoveFrame(8)));
				}
			}
			else
			{
				if (warrior->getPositionY() > enemy[i]->getPositionY())
				{
					enemy[i]->runAction(CCRepeatForever::create(enemyMoveFrame(0)));
				}
				else
				{
					enemy[i]->runAction(CCRepeatForever::create(enemyMoveFrame(4)));
				}
			}
			auto actionPlayDone = CCCallFuncN::create(this, callfuncN_selector(HelloWorld::enemyMoveFinished));
			enemy[i]->runAction(CCSequence::create(CCMoveTo::create(1.0f, warrior->getPosition()),/* actionPlayDone,*/ NULL));
		}

		enemyRect[i] = CCRect(
			enemy[i]->getPositionX() - enemy[i]->getTextureRect().size.width *0.75,
			enemy[i]->getPositionY() - enemy[i]->getTextureRect().size.height,
			enemy[i]->getTextureRect().size.width * 2*0.75,
			enemy[i]->getTextureRect().size.height * 2*0.8);
		/*CCLOG(" %d  x:%0.2f  y:%0.2f  Ox:%0.2f  Oy:%0.2f  width:%0.2f  height:%0.2f ", i, enemy[i]->getPositionX(), enemy[i]->getPositionY(),
			enemy[i]->getPositionX() - enemy[i]->getTextureRect().size.width,
			enemy[i]->getPositionY() - enemy[i]->getTextureRect().size.height,
			enemy[i]->getTextureRect().size.width * 2,
			enemy[i]->getTextureRect().size.height * 2);*/

		if (cdNum == 0)
		{
			enemyFight[i] = false;
			enemy[i]->setVisible(true);
			if (warriorAttackRect.containsPoint(ccp(enemy[i]->getPositionX(), enemy[i]->getPositionY()-10)))
			{
				enemy[i]->stopAllActions();
				auto actionPlayDone = CCCallFuncN::create(this, callfuncN_selector(HelloWorld::effectPlayFinished));
				enemy[i]->runAction(CCSequence::create(CCRepeat::create(warriorEffect(), 1.2f), actionPlayDone, NULL));
				enemyFight[i] = true;
				cdNum = 10;
				char str[10];
				enemyCDTime = 5;
				killNum++;
				sprintf(str, "SCORE:%d", killNum);
				kill->setString(str);
				touch = false;
				chooseEnemy[i] = false;
				//enterVisible[i] = false;
			}
		}

		if (enemyCDTime == 0)
		{
			enemyFight[i] = false;
			enemy[i]->setVisible(true);
			if (enemyRect[i].containsPoint(ccp(warrior->getPosition().x, warrior->getPositionY() - 15)))
			{
				char str[10];
				auto actionPlayDone = CCCallFuncN::create(this, callfuncN_selector(HelloWorld::effectPlayFinished));
				effectPlay->setPosition(ccp(warrior->getPositionX()-15, warrior->getPositionY() ));
				effectPlay->setVisible(true);
				effectPlay->runAction(CCSequence::create(CCRepeat::create(enemyFightEffect(), 1.2f), actionPlayDone, NULL));
				if (cdNum == 0)
				{
					enemyFight[i] = true;
					enemy[i]->runAction(CCSequence::create(CCRepeat::create(warriorEffect(), 1.2f), actionPlayDone, NULL));
					cdNum = 10;
					killNum++;
					sprintf(str, "SCORE:%d", killNum);
					kill->setString(str);
				}
				hpNum = hpNum == 0 ? 0 : hpNum - 1;
				sprintf(str, "HP:%d", hpNum);
				hp->setString(str);
				hpNum > 2 ? hp->setColor(ccc3(255, 255, 255)) : hp->setColor(ccc3(255, 0, 0));
				enemyCDTime = 5;
				touch = false;
				chooseEnemy[i] = false;
				//enterVisible[i] = false;
			}
		}
	}
}

CCAnimate* HelloWorld::enemyFightEffect()
{
	CCSpriteFrame *frame0, *frame1, *frame2, *frame3;
	CCSpriteFrameCache* effectFrameCache = CCSpriteFrameCache::sharedSpriteFrameCache();
	effectFrameCache->addSpriteFramesWithFile("pugong_zhang.plist");
	CCArray *animFrames = CCArray::createWithCapacity(4);

	frame0 = effectFrameCache->spriteFrameByName("1404890476337_0_0_0.png");
	frame1 = effectFrameCache->spriteFrameByName("1404890476337_0_0_1.png");
	frame2 = effectFrameCache->spriteFrameByName("1404890476337_0_0_2.png");
	frame3 = effectFrameCache->spriteFrameByName("1404890476337_0_0_3.png");

	animFrames->addObject(frame0);
	animFrames->addObject(frame1);
	animFrames->addObject(frame2);
	animFrames->addObject(frame3);

	//定义每一帧和动画的间隔时间
	CCAnimation *animation = CCAnimation::createWithSpriteFrames(animFrames, 0.2f);
	//创建动画的Action
	CCAnimate *animate = CCAnimate::create(animation);

	return animate;
}

CCAnimate* HelloWorld::warriorEffect()
{
	CCSpriteFrame *frame0, *frame1, *frame2, *frame3, *frame4, *frame5;
	CCSpriteFrameCache* effectFrameCache = CCSpriteFrameCache::sharedSpriteFrameCache();
	effectFrameCache->addSpriteFramesWithFile("caiji.plist");
	CCArray *animFrames = CCArray::createWithCapacity(6);

	frame0 = effectFrameCache->spriteFrameByName("1387767650755390_0_0_0.png");
	frame1 = effectFrameCache->spriteFrameByName("1387767650755390_0_0_1.png");
	frame2 = effectFrameCache->spriteFrameByName("1387767650755390_0_0_2.png");
	frame3 = effectFrameCache->spriteFrameByName("1387767650755390_0_0_3.png");
	frame4 = effectFrameCache->spriteFrameByName("1387767650755390_0_0_4.png");
	frame5 = effectFrameCache->spriteFrameByName("1387767650755390_0_0_5.png");

	animFrames->addObject(frame0);
	animFrames->addObject(frame1);
	animFrames->addObject(frame2);
	animFrames->addObject(frame3);
	animFrames->addObject(frame4);
	animFrames->addObject(frame5);

	//定义每一帧和动画的间隔时间
	CCAnimation *animation = CCAnimation::createWithSpriteFrames(animFrames, 0.2f);
	//创建动画的Action
	CCAnimate *animate = CCAnimate::create(animation);

	return animate;
}

//type:特效的类型  1，玩家被攻击 2，怪物被选中
CCAnimate* HelloWorld::effect(int type)
{
	CCSpriteFrame *frame0, *frame1, *frame2;
	CCSpriteFrameCache* effectFrameCache = CCSpriteFrameCache::sharedSpriteFrameCache();
	CCArray *animFrames = CCArray::createWithCapacity(3);
	if (type == 1)
	{
		effectFrameCache->addSpriteFramesWithFile("zhu_xian_jian.plist");
		frame0 = effectFrameCache->spriteFrameByName("138776636226525_0_0_0.png");
		frame1 = effectFrameCache->spriteFrameByName("138776636226525_0_0_1.png");
		frame2 = effectFrameCache->spriteFrameByName("138776636226525_0_0_2.png");
	}
	if (type == 2)
	{
		effectFrameCache->addSpriteFramesWithFile("lingliqiu_lan.plist");
		frame0 = effectFrameCache->spriteFrameByName("138776659263692_0_0_0.png");
		frame1 = effectFrameCache->spriteFrameByName("138776659263692_0_0_1.png");
		frame2 = effectFrameCache->spriteFrameByName("138776659263692_0_0_2.png");
	}


	animFrames->addObject(frame0);
	animFrames->addObject(frame1);
	animFrames->addObject(frame2);

	//定义每一帧和动画的间隔时间
	CCAnimation *animation = CCAnimation::createWithSpriteFrames(animFrames, 0.2f);
	//创建动画的Action
	CCAnimate *animate = CCAnimate::create(animation);

	return animate;
}

CCAnimate* HelloWorld::enemyMoveFrame(int direct)
{
	CCSpriteFrameCache* enemyFrameCache = CCSpriteFrameCache::sharedSpriteFrameCache();
	enemyFrameCache->addSpriteFramesWithFile("100.plist");
	CCSpriteFrame *frame0, *frame1, *frame2, *frame3, *frame4, *frame5;
	CCArray *animFrames = CCArray::createWithCapacity(6);
	if (direct == 0)
	{
		frame0 = enemyFrameCache->spriteFrameByName("138751363692014_2_0_0.png");
		frame1 = enemyFrameCache->spriteFrameByName("138751363692014_2_0_1.png");
		frame2 = enemyFrameCache->spriteFrameByName("138751363692014_2_0_2.png");
		frame3 = enemyFrameCache->spriteFrameByName("138751363692014_2_0_3.png");
		frame4 = enemyFrameCache->spriteFrameByName("138751363692014_2_0_4.png");
		frame5 = enemyFrameCache->spriteFrameByName("138751363692014_2_0_5.png");
	}
	if (direct == 2 || direct == 8)
	{
		frame0 = enemyFrameCache->spriteFrameByName("138751363692014_2_2_0.png");
		frame1 = enemyFrameCache->spriteFrameByName("138751363692014_2_2_1.png");
		frame2 = enemyFrameCache->spriteFrameByName("138751363692014_2_2_2.png");
		frame3 = enemyFrameCache->spriteFrameByName("138751363692014_2_2_3.png");
		frame4 = enemyFrameCache->spriteFrameByName("138751363692014_2_2_4.png");
		frame5 = enemyFrameCache->spriteFrameByName("138751363692014_2_2_5.png");
	}
	if (direct == 4)
	{
		frame0 = enemyFrameCache->spriteFrameByName("138751363692014_2_4_0.png");
		frame1 = enemyFrameCache->spriteFrameByName("138751363692014_2_4_1.png");
		frame2 = enemyFrameCache->spriteFrameByName("138751363692014_2_4_2.png");
		frame3 = enemyFrameCache->spriteFrameByName("138751363692014_2_4_3.png");
		frame4 = enemyFrameCache->spriteFrameByName("138751363692014_2_4_4.png");
		frame5 = enemyFrameCache->spriteFrameByName("138751363692014_2_4_5.png");
	}

	animFrames->addObject(frame0);
	animFrames->addObject(frame1);
	animFrames->addObject(frame2);
	animFrames->addObject(frame3);
	animFrames->addObject(frame4);
	animFrames->addObject(frame5);

	//定义每一帧和动画的间隔时间
	CCAnimation *animation = CCAnimation::createWithSpriteFrames(animFrames, 0.2f);
	//创建动画的Action
	CCAnimate *animate = CCAnimate::create(animation);
	return animate;
}

void HelloWorld::scheduleAStart(float dt)
{
	if (touch && sqrt((pTarget.x - warrior->getPosition().x) * (pTarget.x - warrior->getPosition().x)
		+ (pTarget.y - warrior->getPosition().y+30) * (pTarget.y - warrior->getPosition().y+30)) > 10)
	{
		aStart(pTarget);
	}

}

void HelloWorld::moveEnemy(float dt)
{
	for (int i = 0; i < 4; i++)
	{
		if (enemyFight[i])
		{
			enemy[i]->stopAllActions();
			enemy[i]->setVisible(false);
		}

		//刷新Enemy
		enterVisible[i] = false;
	}

	if (left)
	{
		if (!enterVisible[0])
		{
			enemy[0]->stopAllActions();
			enemy[0]->runAction(CCMoveTo::create(2.0f, pEnemy[0][1]));
			enemy[0]->runAction(CCRepeatForever::create(enemyMoveFrame(4)));
		}
		if (!enterVisible[1])
		{
			enemy[1]->stopAllActions();
			enemy[1]->setRotationY(0);
			enemy[1]->runAction(CCMoveTo::create(2.0f, pEnemy[1][1]));
			enemy[1]->runAction(CCRepeatForever::create(enemyMoveFrame(2)));
		}
		if (!enterVisible[2])
		{
			enemy[2]->stopAllActions();
			enemy[2]->setRotationY(180);
			enemy[2]->runAction(CCMoveTo::create(2.0f, pEnemy[2][1]));
			enemy[2]->runAction(CCRepeatForever::create(enemyMoveFrame(8)));
		}
		if (!enterVisible[3])
		{
			enemy[3]->stopAllActions();
			enemy[3]->runAction(CCMoveTo::create(2.0f, pEnemy[3][1]));
			enemy[3]->runAction(CCRepeatForever::create(enemyMoveFrame(0)));
		}
	}
	else
	{
		if (!enterVisible[0])
		{
			enemy[0]->stopAllActions();
			enemy[0]->runAction(CCMoveTo::create(2.0f, pEnemy[0][0]));
			enemy[0]->runAction(CCRepeatForever::create(enemyMoveFrame(0)));
		}
		if (!enterVisible[1])
		{
			enemy[1]->stopAllActions();
			enemy[1]->setRotationY(180);
			enemy[1]->runAction(CCMoveTo::create(2.0f, pEnemy[1][0]));
			enemy[1]->runAction(CCRepeatForever::create(enemyMoveFrame(8)));
		}
		if (!enterVisible[2])
		{
			enemy[2]->stopAllActions();
			enemy[2]->setRotationY(0);
			enemy[2]->runAction(CCMoveTo::create(2.0f, pEnemy[2][0]));
			enemy[2]->runAction(CCRepeatForever::create(enemyMoveFrame(2)));
		}
		if (!enterVisible[3])
		{
			enemy[3]->stopAllActions();
			enemy[3]->runAction(CCMoveTo::create(2.0f, pEnemy[3][0]));
			enemy[3]->runAction(CCRepeatForever::create(enemyMoveFrame(4)));
		}
	}
	left = !left;

}

CCAnimate* HelloWorld::moveFrame(int direct)
{
	CCSpriteFrame *frame0, *frame1, *frame2, *frame3, *frame4, *frame5;
	CCArray *animFrames = CCArray::createWithCapacity(6);
	bool condition1 = direct == 1 || direct == 2 || direct == -4;
	bool condition2 = direct == 4 || direct == -1 || direct == -2;
	if (condition1)
	{
		frame0 = frameCache->spriteFrameByName("14085040176750_2_0_0.png");
		frame1 = frameCache->spriteFrameByName("14085040176750_2_0_1.png");
		frame2 = frameCache->spriteFrameByName("14085040176750_2_0_2.png");
		frame3 = frameCache->spriteFrameByName("14085040176750_2_0_3.png");
		frame4 = frameCache->spriteFrameByName("14085040176750_2_0_4.png");
		frame5 = frameCache->spriteFrameByName("14085040176750_2_0_5.png");
	}
	if (direct == 3 || direct == -3)
	{
		frame0 = frameCache->spriteFrameByName("14085040176750_2_2_0.png");
		frame1 = frameCache->spriteFrameByName("14085040176750_2_2_1.png");
		frame2 = frameCache->spriteFrameByName("14085040176750_2_2_2.png");
		frame3 = frameCache->spriteFrameByName("14085040176750_2_2_3.png");
		frame4 = frameCache->spriteFrameByName("14085040176750_2_2_4.png");
		frame5 = frameCache->spriteFrameByName("14085040176750_2_2_5.png");
	}
	if (condition2)
	{
		frame0 = frameCache->spriteFrameByName("14085040176750_2_4_0.png");
		frame1 = frameCache->spriteFrameByName("14085040176750_2_4_1.png");
		frame2 = frameCache->spriteFrameByName("14085040176750_2_4_2.png");
		frame3 = frameCache->spriteFrameByName("14085040176750_2_4_3.png");
		frame4 = frameCache->spriteFrameByName("14085040176750_2_4_4.png");
		frame5 = frameCache->spriteFrameByName("14085040176750_2_4_5.png");
	}

	animFrames->addObject(frame0);
	animFrames->addObject(frame1);
	animFrames->addObject(frame2);
	animFrames->addObject(frame3);
	animFrames->addObject(frame4);
	animFrames->addObject(frame5);

	//定义每一帧和动画的间隔时间
	CCAnimation *animation = CCAnimation::createWithSpriteFrames(animFrames, 0.1f);
	//创建动画的Action
	CCAnimate *animate = CCAnimate::create(animation);
	return animate;
}

bool isInObstruction(float x, float y)
{
	for (int i = 0; i < 4; i++)
	{
		if (x >= obstructionX[i][0] && x <= obstructionX[i][1])
		{
			if (y >= obstructionY[i][0] && y <= obstructionY[i][1])
				return true;
		}
	}
	return false;
}

bool isInMap(float x, float y)
{
	if (x < 58 || x > 442 || y < 39 || y > 238)
	{
		return true;
	}
	return false;
}

bool HelloWorld::move(float warriorX, float warriorY, float mapX, float mapY, int direct)
{

	actionPlay = CCRepeat::create(moveFrame(direct), 1.0f);
	//CCDelayTime *delayAction = CCDelayTime::create(k * 0.5f);
	if (direct == -3)
	{
		//warrior->setFlipX(true);
		warrior->setRotationY(180);
	}
	if (direct == 3)
	{
		warrior->setRotationY(0);
	}
	actionMoveMap = CCMoveTo::create(0.5f, ccp(mapX, mapY));
	actionMoveWarrior = CCMoveTo::create(0.5f, ccp(warriorX, warriorY+30));
	warrior->runAction(actionMoveWarrior);
	map->runAction(actionMoveMap);
	warrior->runAction(actionPlay);
	return true;
}

bool hadUsed(Node p)
{
	for (int i = 0; i<count; i++)
	{
		if (usePoint[i].x == p.x && usePoint[i].y == p.y)
		//if (fabs(usePoint[i].x - p.x) < 1 && fabs(usePoint[i].y - p.y) < 1)
		{
			return true;
		}
	}
	for (int i = 0; i<pathCount; i++)
	{
		if (pathPoint[i].x == p.x && pathPoint[i].y == p.y)
		//if (fabs(pathPoint[i].x - p.x) < 1 && fabs(pathPoint[i].y - p.y) < 1)
		{
			return true;
		}
	}
	return false;
}

bool HelloWorld::aStart(CCPoint pMap)
{

	warrior->stopAllActions();
	map->stopAllActions();

	float targetX = pMap.x;
	float targetY = pMap.y;
	float curX = warrior->getPosition().x;
	float curY = warrior->getPosition().y;

	/*CCLOG("Warrior Position %.2f %.2f", warrior->getPosition().x, warrior->getPosition().y);
	CCLOG("map Position %.2f %.2f", map->getPosition().x, map->getPosition().y);*/
	curY -= 30;

	int j = 0;
	//第一步
	Node next, point, mapPoint;
	mapPoint.x = map->getPositionX();
	mapPoint.y = map->getPositionY();
	bool used;

	used = true;
	point.x = curX;
	point.y = curY + 8;
	point.direct = 1;
	if (!hadUsed(point) && !isInObstruction(point.x, point.y) && !isInMap(point.x, point.y))
	{
		usePointTemp[j++] = point;
		point.g = g + 8;
		point.f = point.g + sqrt((targetX - point.x)*(targetX - point.x) + (targetY - point.y)*(targetY - point.y));
		next = point;
		used = false;
	}

	point.x = curX + 8;
	point.y = curY + 8;
	point.direct = 2;
	if (!hadUsed(point) && !isInObstruction(point.x, point.y) && !isInMap(point.x, point.y))
	{
		usePointTemp[j++] = point;

		point.g = g + 1.414 * 8;
		point.f = point.g + sqrt((targetX - point.x)*(targetX - point.x) + (targetY - point.y)*(targetY - point.y));
		if (point.f < next.f || used)
		{
			next = point;
			used = false;
		}
	}
	point.x = curX + 8;
	point.y = curY;
	point.direct = 3;
	if (!hadUsed(point) && !isInObstruction(point.x, point.y) && !isInMap(point.x, point.y))
	{
		usePointTemp[j++] = point;

		point.g = g + 8;
		point.f = point.g + sqrt((targetX - point.x)*(targetX - point.x) + (targetY - point.y)*(targetY - point.y));
		if (point.f < next.f || used)
		{
			next = point;
			used = false;
		}
	}
	point.x = curX + 8;
	point.y = curY - 8;
	point.direct = 4;
	if (!hadUsed(point) && !isInObstruction(point.x, point.y) && !isInMap(point.x, point.y))
	{
		usePointTemp[j++] = point;

		point.g = g + 1.414 * 8;
		point.f = point.g + sqrt((targetX - point.x)*(targetX - point.x) + (targetY - point.y)*(targetY - point.y));
		if (point.f < next.f || used)
		{
			next = point;
			used = false;
		}
	}
	point.x = curX;
	point.y = curY - 8;
	point.direct = -1;
	if (!hadUsed(point) && !isInObstruction(point.x, point.y) && !isInMap(point.x, point.y))
	{
		usePointTemp[j++] = point;

		point.g = g + 8;
		point.f = point.g + sqrt((targetX - point.x)*(targetX - point.x) + (targetY - point.y)*(targetY - point.y));
		if (point.f < next.f || used)
		{
			next = point;
			used = false;
		}
	}
	point.x = curX - 8;
	point.y = curY - 8;
	point.direct = -2;
	if (!hadUsed(point) && !isInObstruction(point.x, point.y) && !isInMap(point.x, point.y))
	{
		usePointTemp[j++] = point;

		point.g = g + 1.414 * 8;
		point.f = point.g + sqrt((targetX - point.x)*(targetX - point.x) + (targetY - point.y)*(targetY - point.y));
		if (point.f < next.f || used)
		{
			next = point;
			used = false;
		}
	}
	point.x = curX - 8;
	point.y = curY;
	point.direct = -3;
	if (!hadUsed(point) && !isInObstruction(point.x, point.y) && !isInMap(point.x, point.y))
	{
		usePointTemp[j++] = point;

		point.g = g + 8;
		point.f = point.g + sqrt((targetX - point.x)*(targetX - point.x) + (targetY - point.y)*(targetY - point.y));
		if (point.f < next.f || used)
		{
			next = point;
			used = false;
		}
	}
	point.x = curX - 8;
	point.y = curY + 8;
	point.direct = -4;
	if (!hadUsed(point) && !isInObstruction(point.x, point.y) && !isInMap(point.x, point.y))
	{
		usePointTemp[j++] = point;

		point.g = g + 1.414 * 8;
		point.f = point.g + sqrt((targetX - point.x)*(targetX - point.x) + (targetY - point.y)*(targetY - point.y));
		if (point.f < next.f || used)
		{
			next = point;
			used = false;
		}
	}

	if (!used)
	{
		mapPoint.x += curX - next.x;
		mapPoint.y += curY - next.y;

		pathPoint[pathCount++] = next;
		//direct = next.direct;
		/*CCLOG("  X:%.2f  Y:%.2f warrior to", next.x, next.y);
		CCLOG("  X:%.2f  Y:%.2f map to", mapPoint.x, mapPoint.y);*/
		move(next.x, next.y, mapPoint.x, mapPoint.y, next.direct);
		for (int i = 0; i < j; i++)
		{
			usePoint[count++] = usePointTemp[i];
		}
		g = next.g;
	}
	else
	{
		//usePoint[count++] = next;
		count = 0;
		//CCLOG("no useable point");
	}

	return true;
}

bool HelloWorld::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	if (play)
		return false;

	// 按下   
	//CCLOG("ccTouchBegan %.2f %.2f", pTouch->getLocation().x, pTouch->getLocation().y);
	CCPoint pMap = map->convertTouchToNodeSpace(pTouch);
	CCLOG("ccTouchConvert %.2f %.2f", pMap.x, pMap.y);

	if (isInMap(pMap.x, pMap.y))
	{
		//不在地图内
		return false;
	}
	if (isInObstruction(pMap.x, pMap.y))
	{
		//目的地在障碍物内
		return false;
	}
	pTarget = pMap;
	for (int i = 0; i < 4; i++)
	{
		enemyRect[i] = CCRect(
			enemy[i]->getPositionX() - 15,
			enemy[i]->getPositionY() - 15,
			30,
			30);
		if (enemyRect[i].containsPoint(pMap))
		{
			enemy[i]->runAction(CCRepeatForever::create(effect(2)));
			chooseEnemy[i] = true;
		}
	}

	g = 0;
	pathCount = 0;
	touch = true;
	count = 0;

	return true;
}
void HelloWorld::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
	// 滑动   
	//CCLOG("ccTouchMoved %.2f %.2f", pTouch->getLocation().x, pTouch->getLocation().y);
}
void HelloWorld::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	// 弹起   
	//CCLOG("ccTouchEnded %.2f %.2f", pTouch->getLocation().x, pTouch->getLocation().y);
}
void HelloWorld::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{
	// 中断   
}
void HelloWorld::onEnter()
{
	CCLayer::onEnter();
	// 打开接收事件   
	this->setTouchEnabled(true);
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
}

void HelloWorld::gameLayer(CCObject* pSender)
{
	hpNum = 10;
	cdNum = 5;
	killNum = 0;
	enemyCDTime = 0;
	play = false;
	count = 0;
	target_count = 0;
	touch = false;
	pBackItem->setVisible(true);
	pCloseItem->setVisible(false);
	pStartItem->setVisible(false);
	for (int i = 0; i < 5; i++)
	{
		target[i] = CCSprite::create("Target.png", CCRect(0, 0, 16, 16));
		target[i]->setVisible(false);
		target[i]->setPosition(CCPoint(0, 0));
		this->addChild(target[i], 0);
	}
	effectPlay = CCSprite::create("Target.png", CCRect(0, 0, 16, 16));
	effectPlay->setVisible(false);
	effectPlay->setPosition(CCPoint(0, 0));
	enemyEffectPlay = CCSprite::create("Target.png", CCRect(0, 0, 16, 16));
	enemyEffectPlay->setVisible(false);
	enemyEffectPlay->setPosition(CCPoint(0, 0));

	this->schedule(schedule_selector(HelloWorld::gameLogic), 1.0f);
	this->schedule(schedule_selector(HelloWorld::moveEnemy), 2.5f);
	this->schedule(schedule_selector(HelloWorld::scheduleAStart), 0.51f);
	this->scheduleUpdate();

	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

	map = CCSprite::create("bg.png");

	CCSprite* gun1 = CCSprite::create("MachineGunTurret.png");
	gun1->setPosition(ccp(280, 100));
	gun1->setRotationY(180);
	CCSprite* gun2 = CCSprite::create("MachineGunTurret.png");
	gun2->setPosition(ccp(372, 166));
	gun2->setRotation(90);
	map->addChild(gun1);
	map->addChild(gun2);

	enemyVisible[0] = CCRect(120, 100, 60, 130);
	enemyVisible[1] = CCRect(90, 40, 140, 50);
	enemyVisible[2] = CCRect(250, 180, 140, 50);
	enemyVisible[3] = CCRect(350, 50, 60, 110);

	for (int i = 0; i < 4; i++)
	{
		chooseEnemy[i] = false;
		enemyFight[i] = false;
		enterVisible[i] = false;
		enemy[i] = CCSprite::create();
		enemy[i]->setScale(0.75);
		map->addChild(enemy[i]);
	}
	enemy[0]->setPosition(ccp(150, 220));
	enemy[1]->setPosition(ccp(120, 80));
	enemy[2]->setPosition(ccp(380, 220));
	enemy[3]->setPosition(ccp(380, 90));
	enemy[2]->setRotationY(180);
	enemy[0]->runAction(CCRepeatForever::create(enemyMoveFrame(4)));
	enemy[1]->runAction(CCRepeatForever::create(enemyMoveFrame(2)));
	enemy[2]->runAction(CCRepeatForever::create(enemyMoveFrame(8)));
	enemy[3]->runAction(CCRepeatForever::create(enemyMoveFrame(0)));
	//CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("20006.plist");
	frameCache = CCSpriteFrameCache::sharedSpriteFrameCache();
	frameCache->addSpriteFramesWithFile("20006.plist");

	// position the sprite on the center of the screen
	map->setPosition(ccp(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

	char str[10];
	sprintf(str, "HP:%d", hpNum);

	hp = CCLabelTTF::create(str, "Arial", TITLE_FONT_SIZE);
	hp->setPosition(ccp(origin.x + hp->getContentSize().width, origin.y + visibleSize.height - hp->getContentSize().height));
	this->addChild(hp, 1);

	cd = CCLabelTTF::create("CD:5", "Arial", TITLE_FONT_SIZE);
	cd->setPosition(ccp(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - cd->getContentSize().height));
	this->addChild(cd, 1);

	kill = CCLabelTTF::create("SCORE:0", "Arial", TITLE_FONT_SIZE);
	kill->setPosition(ccp(origin.x + visibleSize.width - kill->getContentSize().width, origin.y + visibleSize.height - kill->getContentSize().height));
	this->addChild(kill, 1);

	warrior = CCSprite::create();
	warrior->setPosition(ccp(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	//warrior->setAnchorPoint(ccp(0.5, 0.5));

	CCSpriteFrame *framedie = frameCache->spriteFrameByName("14085040176750_0_4_0.png");//below
	warrior->setDisplayFrame(framedie);

	map->setScale(2.0);

	// add the sprite as a child to this layer
	this->addChild(map, 0);
	map->addChild(warrior);

	this->addChild(effectPlay, 0);
	this->addChild(enemyEffectPlay, 0);

}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	// 1. super init first
	if (!CCLayer::init())
	{
		return false;
	}
	play = true;

	// add a "close" icon to exit the progress. it's an autorelease object
	pCloseItem = CCMenuItemImage::create(
		"CloseNormal.png",
		"CloseSelected.png",
		this,
		menu_selector(HelloWorld::menuCloseCallback));

	pStartItem = CCMenuItemImage::create(
		"start.jpg",
		"start.jpg",
		this,
		menu_selector(HelloWorld::gameLayer));
	pBackItem = CCMenuItemImage::create(
		"back.png",
		"back.png",
		this,
		menu_selector(HelloWorld::gameBack));
	pBackItem->setScale(0.3f);
	pBackItem->setVisible(false);
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

	pCloseItem->setPosition(ccp(visibleSize.width / 2 + pCloseItem->getContentSize().width / 2,
		visibleSize.height / 2 - pCloseItem->getContentSize().height));

	pStartItem->setPosition(ccp(visibleSize.width / 2 + pStartItem->getContentSize().width / 2 + 5,
		visibleSize.height / 2 + pStartItem->getContentSize().height));

	pBackItem->setPosition(ccp(visibleSize.width + pBackItem->getContentSize().width / 3,
		visibleSize.height - pBackItem->getContentSize().height / 6));

	// create menu, it's an autorelease object
	CCMenu* pMenu = CCMenu::create(pCloseItem, pStartItem, pBackItem, NULL);
	pMenu->setPosition(CCPointZero);
	this->addChild(pMenu, 1);

	return true;
}

void HelloWorld::gameBack(CCObject* pSender)
{
	//this->unscheduleUpdate();
	this->unscheduleAllSelectors();
	this->removeAllChildren();
	init();
}

void HelloWorld::gameOver()
{
	play = true;
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
	this->unscheduleAllSelectors();
	this->removeAllChildren();
	char str[10];
	sprintf(str, "SCORE:%d", killNum);
	cd = CCLabelTTF::create(str, "Arial", TITLE_FONT_SIZE);
	cd->setPosition(ccp(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - cd->getContentSize().height));
	this->addChild(cd, 1);
	pCloseItem = CCMenuItemImage::create(
		"CloseNormal.png",
		"CloseSelected.png",
		this,
		menu_selector(HelloWorld::menuCloseCallback));
	pCloseItem->setPosition(ccp(visibleSize.width / 2 + pCloseItem->getContentSize().width / 2,
		visibleSize.height / 2 - pCloseItem->getContentSize().height));
	pAgainItem = CCMenuItemImage::create(
		"again.jpg",
		"again.jpg",
		this,
		menu_selector(HelloWorld::gameBack));
	pAgainItem->setPosition(ccp(visibleSize.width / 2 + pAgainItem->getContentSize().width / 2 + 5,
		visibleSize.height / 2 + pAgainItem->getContentSize().height));
	CCMenu* pMenu = CCMenu::create(pCloseItem, pAgainItem, NULL);
	pMenu->setPosition(CCPointZero);
	this->addChild(pMenu, 1);
	
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
	CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}
