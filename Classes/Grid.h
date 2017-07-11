#ifndef GRID_H
#define GRID_H

#include "cocos2d.h"
#include <vector>

USING_NS_CC;
using namespace std;

#define GRID_WIDTH 40 //һ����������Ϊ40
#define MOVE_SPEED 0.2 //��Ƭ�ƶ��ٶ�
#define FIRST_SPRITE_ID 1 //��һ����Ƭ��ID����ƬIDĬ�����ε�����
#define LAST_SPRITE_ID 7 //���һ����Ƭ��ID

class Card;

//�����࣬�����˿�Ƭ�ڲ����ڵ��߼�
//�����񲼾����������½�Ϊԭ�㣬x��y��Ϊ������
class Grid : public Node
{
public:
	static Grid* create(int row, int col); //�����и��д�������
	bool init(int row, int col);

	void updateMap(); //���¿�Ƭ����
	bool isDeadMap(); //�жϵ�ǰ�����Ƿ���ͼ�������������ˣ�

private:
	//������Ƭ�ķ���
	Card* createACard(int x, int y); //���ݲ������괴��һ����Ƭ
	bool isCardLegal(Card* Card, int x, int y); //�жϴ����Ŀ�Ƭ�Ƿ񲻻����������Ϸ���
	void setCardPixPos(Card* Card, float x, float y); //��������Ϸʵ������λ�á����������������½�Ϊԭ�㣬x��y��Ϊ����

	//������Ƭ�ķ���
	void swapCards(Card *CardA, Card *CardB); //����������Ƭ
	void swapCardToNewPos(Card* Card); //�ƶ�����λ��

	void refreshGrid(); //ˢ��������Ŀ�Ƭ����
	void refreshCardsToNewPos(int col); //ˢ��һ�п�Ƭ

private:
	//�¼���Ӧ����
	bool onTouchBegan(Touch*, Event*);
	void onTouchMoved(Touch*, Event*);

private:
	//��Ϸ�߼�����
	bool canEliminate(); //�жϵ�ǰ״̬�Ŀ�Ƭ�����Ƿ�������
	void goEliminate(); //��ʼ����

	//��׽��������׽���������Ƿ���ɣ�Ȼ�������������
	void onCardsSwaping(float dt);
	void onCardsSwapingBack(float dt);
	void onCardsEliminating(float dt);
	void onCardsRefreshing(float dt);

public:
	int getRow() { return m_row; }
	int getCol() { return m_col; }

private:
	int m_row; //����
	int m_col; //����

	Card* m_CardSelected; //��ǰѡ��Ŀ�Ƭ
	Card* m_CardSwapped; //�������Ŀ�Ƭ

	vector<vector<Card*>> m_CardsBox; //��ſ�Ƭ�����������ͨ������������Ƭ
	Vector<Card*> m_eliminateCardBox; //׼�������Ŀ�Ƭ����
	Vector<Card*> m_newCardBox; //׼�����벼�ֵĿ�Ƭ������
};

#endif

