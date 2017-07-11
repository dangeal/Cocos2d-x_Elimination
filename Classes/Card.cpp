#include "Card.h"
#include "GameScene.h"

Card* Card::createByType(int type, int x, int y)
{
	auto card = new Card();

	if (card && card->init(type, x, y))
	{
		card->autorelease();
		return card;
	}
	else
	{
		CC_SAFE_DELETE(card);
		return nullptr;
	}
}

bool Card::init(int type, int x, int y)
{
	Sprite::init();

	m_type = type;
	m_x = x;
	m_y = y;
	m_isSwaping = false;
	m_isEliminating = false;
	
	//������Դ����ʼ����������ԴӦ������Ϸ�������������ϵ�
	char name[100] = {0};
	sprintf(name, "card%d.png", m_type);
	this->initWithTexture(TextureCache::getInstance()->getTextureForKey(name));

	this->setAnchorPoint(Vec2(0, 0)); //���½�Ϊê��

	return true;
}

void Card::eliminate()
{
	//�ӷ�
	GameScene::addBonus(1);

	//��ʼ����������״̬Ϊ�棬ֱ��������������������Ƭ�Ƴ���Ⱦ�ڵ㣬��������״̬Ϊ��
	m_isEliminating = true;
	auto action = FadeOut::create(0.2);
	auto call = CallFunc::create([this](){
		this->removeFromParent();
		m_isEliminating = false;
	});
	this->runAction(Sequence::create(action, call, nullptr));
}


    int Card::getType() 
	{
		return m_type; 
	}
	
	int Card::getX() 
	{ 
		return m_x; 
	}
	
	int Card::getY() 
	{ 
		return m_y; 
	}

	void Card::setX(int x) 
	{ 
		m_x = x; 
	}
	
	void Card::setY(int y) 
	{ 
		m_y = y; 
	}

	bool Card::isSwaping() 
	{ 
		return m_isSwaping; 
	}
	
	bool Card::isEliminating() 
	{ 
		return m_isEliminating; 
	}
	
	void Card::setSwapingState(bool state) 
	{ 
		m_isSwaping = state; 
	}