#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "cocos2d.h"
#include "ui\CocosGUI.h"

USING_NS_CC;
using namespace ui;

class Grid;

class GameScene : public Layer
{
public:
	static Scene* createScene();
	CREATE_FUNC(GameScene);
	bool init();

public:
	static void addBonus(int bonus);

private:
	static LoadingBar* m_bonusbar;
	static int m_score;
	static Label* m_scorelabel; //����ֵ

private:
	void onUpdateMenuCallback(Ref*);

	void onReducingBonus(float dt); //��������ʱ

	void storeScore(); //�洢��Ϸ����

	Grid* m_Cardsgrid;
	Sprite* m_bg;
};

#endif