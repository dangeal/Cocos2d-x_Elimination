#ifndef Card_H
#define Card_H

#include "cocos2d.h"

USING_NS_CC;

class Card : public Sprite
{
public:
	static Card* createByType(int type, int x, int y); //���ݿ�Ƭ�����Լ����괴����Ƭ
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
	
	void eliminate(); //����

private:
	//����
	int m_type;

	//����
	int m_x;
	int m_y;

	bool m_isSwaping; //�Ƿ����ڽ���
	bool m_isEliminating; //�Ƿ���������
};

#endif
