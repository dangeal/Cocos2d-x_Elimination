#ifndef Card_H
#define Card_H

#include "cocos2d.h"

USING_NS_CC;

class Card : public Sprite
{
public:
	static Card* createByType(int type, int x, int y); //根据卡片类型以及坐标创建卡片
	bool init(int type, int x, int y);

public:
	int getType(); 
	int getX();
	int getY();

	void setX(int x);
	void setY(int y) ;

	bool isSwaping();
	bool isEliminating();
	
	void setSwapingState(bool state);
	
	void eliminate(); //消除

private:
	//类型
	int m_type;

	//坐标
	int m_x;
	int m_y;

	bool m_isSwaping; //是否正在交换
	bool m_isEliminating; //是否正在消除
};

#endif
