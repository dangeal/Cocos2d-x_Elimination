#ifndef GRID_H
#define GRID_H

#include "cocos2d.h"
#include <vector>

USING_NS_CC;
using namespace std;

#define GRID_WIDTH 40 //一个格子像素为40
#define MOVE_SPEED 0.2 //卡片移动速度
#define FIRST_SPRITE_ID 1 //第一个卡片的ID，卡片ID默认依次递增的
#define LAST_SPRITE_ID 7 //最后一个卡片的ID

class Card;

//布局类，包含了卡片在布局内的逻辑
//该网格布局坐标以左下角为原点，x右y上为正方向
class Grid : public Node
{
public:
	static Grid* create(int row, int col); //根据行跟列创建布局
	bool init(int row, int col);

	void updateMap(); //更新卡片阵列
	bool isDeadMap(); //判断当前布局是否死图（不能再消除了）

private:
	//创建卡片的方法
	Card* createACard(int x, int y); //根据布局坐标创建一个卡片
	bool isCardLegal(Card* Card, int x, int y); //判断创建的卡片是否不会三消（即合法）
	void setCardPixPos(Card* Card, float x, float y); //设置其游戏实际像素位置。网格坐标轴以左下角为原点，x右y上为正轴

	//操作卡片的方法
	void swapCards(Card *CardA, Card *CardB); //交换两个卡片
	void swapCardToNewPos(Card* Card); //移动到新位置

	void refreshGrid(); //刷新消除后的卡片阵列
	void refreshCardsToNewPos(int col); //刷新一列卡片

private:
	//事件响应部分
	bool onTouchBegan(Touch*, Event*);
	void onTouchMoved(Touch*, Event*);

private:
	//游戏逻辑部分
	bool canEliminate(); //判断当前状态的卡片阵列是否能消除
	void goEliminate(); //开始消除

	//捕捉函数，捕捉消除步骤是否完成，然后控制消除流程
	void onCardsSwaping(float dt);
	void onCardsSwapingBack(float dt);
	void onCardsEliminating(float dt);
	void onCardsRefreshing(float dt);

public:
	int getRow() { return m_row; }
	int getCol() { return m_col; }

private:
	int m_row; //行数
	int m_col; //列数

	Card* m_CardSelected; //当前选择的卡片
	Card* m_CardSwapped; //欲交换的卡片

	vector<vector<Card*>> m_CardsBox; //存放卡片对象的容器，通过坐标索引卡片
	Vector<Card*> m_eliminateCardBox; //准备消除的卡片容器
	Vector<Card*> m_newCardBox; //准备加入布局的卡片的容器
};

#endif

