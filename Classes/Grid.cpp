#include "Grid.h"
#include "Card.h"
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;

Grid* Grid::create(int row, int col)
{
	auto Cardsgrid = new Grid();
	if (Cardsgrid && Cardsgrid->init(row, col))
	{
		Cardsgrid->autorelease();
		return Cardsgrid;
	}
	else
	{
		CC_SAFE_DELETE(Cardsgrid);
		return nullptr;
	}
}

bool Grid::init(int row, int col)
{
	Node::init();

	m_row = row;
	m_col = col;

	m_CardSelected = nullptr;
	m_CardSwapped = nullptr;

	//�������г�ʼ��һ���յĿ�Ƭ������С
	m_CardsBox.resize(m_row);
	for (auto &vec : m_CardsBox)
		vec.resize(m_col);         //std::vector resize(x)����ָ�����еĳ��ȡ�

	//1.���ݲ��ִ�С��������Ƭ����
	//2.�������������½�Ϊԭ�㣬x��y��Ϊ������
	for (int x = 0; x < m_col; x++)
	{
		for (int y = 0; y < m_row; y++)
		{
			m_CardsBox[x][y] = createACard(x, y); 
		}
	}

	while (isDeadMap())
	{
		log("dead map! need to update");
		updateMap();
	}
	
	//���봥������
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(Grid::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(Grid::onTouchMoved, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	log("Grid init!");
	return true;
}

void Grid::updateMap()
{
	for (int x = 0; x < m_col; x++)
	{
		for (int y = 0; y < m_row; y++)
		{
			m_CardsBox[x][y]->removeFromParent();
			m_CardsBox[x][y] = createACard(x, y); 
		}
	}

	log("update a new map!");
}

bool Grid::isDeadMap()
{
	//ģ�⽻�����жϽ������Ƿ����������粻�ܣ���ô���Ǹ���ͼ
	auto swap = [](Card** a, Card** b)
	{
		auto temp = *a;
		*a = *b;
		*b = temp;
	};

	bool isDeadMap = true;

	//����ÿһ����Ƭ
	for (int x = 0; x < m_col; x++)
	{
		for (int y = 0; y < m_row; y++)
		{
			//����ߵĽ���
			if (x > 0)
			{
				swap(&m_CardsBox[x][y], &m_CardsBox[x-1][y]);
				if (canEliminate())
					isDeadMap = false;
				swap(&m_CardsBox[x][y], &m_CardsBox[x-1][y]);
			}

			//���ұߵĽ���
			if (x < m_col - 1)
			{
				swap(&m_CardsBox[x][y], &m_CardsBox[x+1][y]);
				if (canEliminate())
					isDeadMap = false;
				swap(&m_CardsBox[x][y], &m_CardsBox[x+1][y]);
			}

			//������Ľ���
			if (y < m_row - 1)
			{
				swap(&m_CardsBox[x][y], &m_CardsBox[x][y+1]);
				if (canEliminate())
					isDeadMap = false;
				swap(&m_CardsBox[x][y], &m_CardsBox[x][y+1]);
			}

			//������Ľ���
			if (y > 0)
			{
				swap(&m_CardsBox[x][y], &m_CardsBox[x][y-1]);
				if (canEliminate())
					isDeadMap = false;
				swap(&m_CardsBox[x][y], &m_CardsBox[x][y-1]);
			}
		}
	}

	//canEliminate��洢�������Ŀ�Ƭ��ȥ��������ģ�⽻�������Ի�Ҫ���
	m_eliminateCardBox.clear();

	return isDeadMap;
}

Card* Grid::createACard(int x, int y)
{
	//1.���ݲ������괴��һ�ſ�Ƭ���������
	//2.�жϸÿ�Ƭ�Ƿ�Ϸ�������������
	//3.���øÿ�Ƭ����������
	//4.���ÿ�Ƭ������Ⱦ�ڵ�
	Card* card = nullptr;

	while(1)
	{
		card = Card::createByType(random(FIRST_SPRITE_ID, LAST_SPRITE_ID), x, y);
		
		if (isCardLegal(card, x, y))
		{
			break;
		}
	}	

	setCardPixPos(card, x, y);
	addChild(card);

	//log("add a Card!---type:%d---x:%d---y:%d", Card->getType(), x, y);

	return card;
}

bool Grid::isCardLegal(Card* card, int x, int y)
{
	//1.�ֱ��ж��¼���Ŀ�Ƭ��x��y�᷽���Ƿ������
	//2.�����Ǵ���������뿨Ƭ�����ֻ�����������ж�
	//3.x��y����С�ڵ���1�����ж�
	//4.����ͬʱ�Ϸ����Ϸ�
	bool isXLegal = true;
	bool isYLegal = true;

	if (x > 1)
	{
		//��x�Ḻ����ֱ�Ƚ���λ�������Ƭ���Ͷ�һ������ô��ʾ�������ÿ�Ƭ���Ϸ�
		if (card->getType() == m_CardsBox[x-1][y]->getType() && 
			card->getType() == m_CardsBox[x-2][y]->getType()
			)
		{
			isXLegal = false;
		}
		else
			isXLegal = true;
	}
	
	if (y > 1)
	{
		//��y�Ḻ����ֱ�Ƚ���λ�������Ƭ���Ͷ�һ������ô��ʾ�������ÿ�Ƭ���Ϸ�
		if (card->getType() == m_CardsBox[x][y-1]->getType() && 
			card->getType() == m_CardsBox[x][y-2]->getType())
		{
			isYLegal = false;
		}
		else
			isYLegal = true;
	}

	return isXLegal && isYLegal;
}

void Grid::setCardPixPos(Card* card, float x, float y)
{
	card->setPosition(x * GRID_WIDTH, y * GRID_WIDTH); 
}

bool Grid::onTouchBegan(Touch* pTouch, Event* pEvent)
{
	//�������������ת��Ϊģ������??????????
	auto pos = this->convertToNodeSpace(pTouch->getLocation());
	
	//�Ƿ��а��ڿ�Ƭ������
	if (Rect(0, 0, m_col*GRID_WIDTH, m_row*GRID_WIDTH).containsPoint(pos))
	{
		//�õ���������
		int x = pos.x / GRID_WIDTH;
		int y = pos.y / GRID_WIDTH;

		//�õ���ǰѡ�еĿ�Ƭ
		m_CardSelected = m_CardsBox[x][y];

		//log("touch coordinate: x=%d,y=%d Card's type:%d", x, y, m_CardSelected->getType());

		return true;
	}
	else
	{
		return false;
	}
}

void Grid::onTouchMoved(Touch* pTouch, Event* pEvent)
{
	//���û��ѡ��Ƭ����ô����
	if (!m_CardSelected)
	{
		return;
	}

	//��ѡ��Ƭ�Ĳ�������
	int startX = m_CardSelected->getX();
	int startY = m_CardSelected->getY();

	//������Ĳ�������
	auto pos = this->convertToNodeSpace(pTouch->getLocation());
	int touchX = pos.x / GRID_WIDTH;
	int touchY = pos.y / GRID_WIDTH;

	//��������㲻�ڲ����ڣ����ߴ����㲼���������ѡ��Ƭ��������һ������ô����
	if (!Rect(0, 0, m_col*GRID_WIDTH, m_row*GRID_WIDTH).containsPoint(pos) || Vec2(touchX, touchY) == Vec2(startX, startY)) 
	{
		return;
	}

	//�ж���ѡ��Ƭ�Ĳ��������봥����Ĳ��������Ƿ�ֱ�����һ����λ
	if (abs(startX - touchX) + abs(startY - touchY) != 1)
	{
		//log("touch pos not on border");
		return;
	}

	//���µ����������������Ŀ�Ƭ���������н����Ŀ�Ƭ
	//ͨ��������������ȡ�������Ŀ�Ƭ
	m_CardSwapped = m_CardsBox[touchX][touchY];

	//������Ƭ����������״̬��׽�������ڽ�����ɺ��ж��Ƿ����������
	swapCards(m_CardSelected, m_CardSwapped);
	schedule(schedule_selector(Grid::onCardsSwaping));
}

void Grid::swapCards(Card *cardA, Card *cardB)
{
	_eventDispatcher->pauseEventListenersForTarget(this); //������ʼ���رմ�������

	//1.������Ƭ�����ڵĿ�Ƭָ��
	//2.������Ƭ����
	//3.��Ƭ�ƶ����µ�λ��
	auto temp = m_CardsBox[cardA->getX()][cardA->getY()];
	m_CardsBox[cardA->getX()][cardA->getY()] = m_CardsBox[cardB->getX()][cardB->getY()];
	m_CardsBox[cardB->getX()][cardB->getY()] = temp;

	auto tempX = cardA->getX();
	cardA->setX(cardB->getX());
	cardB->setX(tempX);

	auto tempY = cardA->getY();
	cardA->setY(cardB->getY());
	cardB->setY(tempY);

	swapCardToNewPos(cardA);
	swapCardToNewPos(cardB);
}

void Grid::swapCardToNewPos(Card* card)
{
	//�ƶ���ʼ���ÿ�Ƭ����״̬Ϊ�棬�ƶ�����������Ϊ��
	card->setSwapingState(true);
	auto move = MoveTo::create(MOVE_SPEED, Vec2(card->getX() * GRID_WIDTH, card->getY() * GRID_WIDTH));
	auto call = CallFunc::create([card](){
		card->setSwapingState(false);
	});
	card->runAction(Sequence::create(move, call, nullptr));
}

bool Grid::canEliminate()
{
	//�ú����жϵ�ǰ״̬�Ŀ�Ƭ�����Ƿ�������
	//���������Ŀ�Ƭ����������Ƭ���ӣ��ȴ�����

	int count = 0; //������
	Card *cardBegin = nullptr; //��ʼ�����Ŀ�Ƭ
	Card *cardNext = nullptr; //����ʼ��Ƭ��ʼ��ǰ�����Ŀ�Ƭ

	//����ÿһ��
	for (int x = 0; x < m_col; x++) 
	{
		for (int y = 0; y < m_row - 1;)
		{
			count = 1;
			cardBegin = m_CardsBox[x][y];
			cardNext = m_CardsBox[x][y + 1];

			 //�����������ͬ����
			while (cardBegin->getType() == cardNext->getType())
			{
				count++;
				int nextIndex = y + count;
				if (nextIndex > m_row - 1)
					break;
				cardNext = m_CardsBox[x][nextIndex];
			}
			//������������ڵ���3����ô��������Щ��ƬӦ�������������Ǵ���������Ƭ����
			if (count >= 3)
			{
				for (int n = 0; n < count; n++)
				{
					auto card = m_CardsBox[x][y+n];
					m_eliminateCardBox.pushBack(card);
				}
			}
			y += count;
		}
	}

	//����ÿһ�У��߼��ͱ���ÿһ����һ����
	for (int y = 0; y < m_row; y++) 
	{
		for (int x = 0; x < m_col - 1;)
		{
			count = 1;
			cardBegin = m_CardsBox[x][y];
			cardNext = m_CardsBox[x+1][y];

			while (cardBegin->getType() == cardNext->getType())
			{
				count++;
				int nextIndex = x + count;
				if (nextIndex > m_col - 1)
					break;
				cardNext = m_CardsBox[nextIndex][y];
			}
			if (count >= 3)
			{
				for (int n = 0; n < count; n++)
				{
					auto card = m_CardsBox[x+n][y];
					//�п����п�Ƭͬʱ���п���������ô�����ظ��洢��������Ƭ���ӣ��������һ���ж�
					if (m_eliminateCardBox.find(card) != m_eliminateCardBox.end())
					{
						continue;
					}
					m_eliminateCardBox.pushBack(card);
				}
			}
			x += count;
		}
	}	

	//���������Ƭ���Ӳ�Ϊ�գ���ô˵�������п�������������
	if (!m_eliminateCardBox.empty())
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Grid::goEliminate()
{
	//����������Ƭ���ӣ������еĿ�Ƭ������������
	for (auto card : m_eliminateCardBox)
	{
		//�����µĿ�Ƭ�������������ʼλ����������һ�е��ϱ�һ�У�������һ��
		auto newCard = Card::createByType(random(FIRST_SPRITE_ID, LAST_SPRITE_ID), card->getX(), m_row); 
		setCardPixPos(newCard, newCard->getX(), m_row); 
		addChild(newCard);

		//���¿�Ƭ�ŵ��¿�Ƭ�����ڣ��ȴ����벼��
		m_newCardBox.pushBack(newCard);

		//��Ƭ������Ӧ��ˢ�µĿ�Ƭ��ʱ��Ϊ��
		m_CardsBox[card->getX()][card->getY()] = nullptr;

		//ԭ�п�Ƭ��������
		card->eliminate();
	}
}

void Grid::refreshGrid()
{
	//�����У���������п�λ����ôӦ��ˢ��
	for (int x = 0; x < m_col; x++)
	{
		int empty_count = 0; //һ���ܵĿո�����

		for (int y = 0; y < m_row; y++)
		{
			//��������������Ƭ�����ڵĿ�Ƭָ�룬���Ϊ�գ���ô˵��������λ��Ϊ��
			auto card = m_CardsBox[x][y];
			if (!card)
				empty_count++;
		}
		if (empty_count)
		{
			//log("the %d col has %d empty", x, empty_count);
			//�ҵ��п�λ���У�ˢ�¸��еĿ�Ƭ
			refreshCardsToNewPos(x);
		}
	}
}

void Grid::refreshCardsToNewPos(int col)
{
	//ˢ�¸�������Ŀ�Ƭ
	int n = 0; //��ǰ�������Ŀ�λ��
	auto pCardsbox = &m_CardsBox; //����һ����Ƭ���ӵ�ָ�룬����Ϊ�������ܴ���lambda

	//�������еĿ�Ƭ����
	for (int y = 0; y < m_row; y++)
	{
		auto card = m_CardsBox[col][y];

		if (!card)
		{
			n++;
			continue;
		}
		else if (n != 0)
		{
			card->setY(card->getY() - n);
			auto move = MoveBy::create(0.2, Vec2(0, -n*GRID_WIDTH));
			auto call = CallFunc::create([pCardsbox, card](){
				//���¿�Ƭ�����ڵ�����
				(*pCardsbox)[card->getX()][card->getY()] = card;
			});

			card->runAction(Sequence::create(move, call, nullptr));
		}
	}
	
	//�����¿�Ƭ����
	int i = n;
	int delta = 1;

	for (auto card : m_newCardBox)
	{
		if (card->getX() == col)
		{
			card->setY(m_row - i);

			auto delay = DelayTime::create(0.2);
			//��������ٶ�������һЩ
			auto move = MoveBy::create(0.2*delta++, Vec2(0, -i--*GRID_WIDTH));
			auto call = CallFunc::create([card, pCardsbox, this](){
				(*pCardsbox)[card->getX()][card->getY()] = card;
				//���¿�Ƭ�������Ƴ��ÿ�Ƭ
				m_newCardBox.eraseObject(card);
			});

			card->runAction(Sequence::create(delay, move, call, nullptr));
		}
	}
}

void Grid::onCardsSwaping(float dt)
{
	//��׽�������ڽ����Ŀ�Ƭ�Ľ��������Ƿ��Ѿ�ֹͣ�����ûֹͣ�����ؼ�����׽
	if (m_CardSelected->isSwaping() || m_CardSwapped->isSwaping())
	{
		return;
	}
	//�����Ƭ��������ִ�����
	else
	{
		unschedule(schedule_selector(Grid::onCardsSwaping));  //ֹͣ��׽

		log("swap over!");

		log("is it can eliminate?");
		
		//�жϵ�ǰ״̬�Ƿ��������
		if (canEliminate())
		{
			log("yes,eliminate!");

			m_CardSelected = nullptr;

			//��ʼ��������������״̬��׽����(��׽��������Ϻ�ˢ�²���)����һ��������ʽ��ʼ
			SimpleAudioEngine::getInstance()->playEffect("eliminate.ogg");
			goEliminate();
			schedule(schedule_selector(Grid::onCardsEliminating));
		}
		else
		{
			log("no, cant eliminate!");

			//����������������ȥ��������������ʱ�Ĳ�׽����(��׽��������Ϻ󣬿�����������)
			SimpleAudioEngine::getInstance()->playEffect("swapback.ogg");
			swapCards(m_CardSelected, m_CardSwapped);
			schedule(schedule_selector(Grid::onCardsSwapingBack));
		}
	}
}

void Grid::onCardsSwapingBack(float dt)
{
	//��׽�������ڽ����Ŀ�Ƭ�Ľ��������Ƿ��Ѿ�ֹͣ�����ûֹͣ�����ؼ�����׽
	if (m_CardSelected->isSwaping() || m_CardSwapped->isSwaping())
	{
		return;
	}
	else
	{
		unschedule(schedule_selector(Grid::onCardsSwapingBack)); //ֹͣ��׽

		log("swap back!");

		m_CardSelected = nullptr;

		_eventDispatcher->resumeEventListenersForTarget(this); //���¿�ʼ��������
	}
}

void Grid::onCardsEliminating(float dt)
{
	//��׽��Ƭ����״̬������п�Ƭ������������ô������׽
	for (auto card : m_eliminateCardBox)
	{
		if (card->isEliminating())
		{
			//log("Eliminating");
			return;
		}
	}
	
	//���ȫ����Ƭ�Ѿ�������ϣ�ֹͣ��׽����
	unschedule(schedule_selector(Grid::onCardsEliminating));

	m_eliminateCardBox.clear(); //���������Ƭ����
	
	log("eliminate over!");
	log("begin to refresh!");

	//ˢ�¿�Ƭ���У�������ˢ��״̬��׽������ˢ��һ������������ж��������Ƿ��������
	refreshGrid();
	schedule(schedule_selector(Grid::onCardsRefreshing));
}

void Grid::onCardsRefreshing(float dt)
{
	//��׽��Ƭˢ��״̬������¿�Ƭ���ӻ��п�Ƭ�����¿�Ƭ����ˢ�µ��У�����ô������׽
	if (m_newCardBox.size() != 0)
	{
		//log("refreshing!");
		return;
	}
	else
	{
		unschedule(schedule_selector(Grid::onCardsRefreshing));

		log("refresh over!");
		log("and now, is it can eliminate?");

		if (canEliminate())
		{
			log("yes, eliminate again!");

			//�������������ô��������
			SimpleAudioEngine::getInstance()->playEffect("eliminate.ogg");
			goEliminate();
			schedule(schedule_selector(Grid::onCardsEliminating));
		}
		else
		{
			log("no, cannot eliminate! over!");

			//�ж��Ƿ�Ϊ��ͼ������ǣ���ִ��һ�����ֶ�������ʾ�������µ�ͼ
			if (isDeadMap())
			{
				log("cant eliminate any more, updating a new map!");

				auto winSize = Director::getInstance()->getWinSize();
				auto label = Label::createWithTTF("Cant Eliminate Any More, Change!", "fonts/Marker Felt.ttf", 24);
				label->setTextColor(Color4B::BLACK);
				label->setPosition(winSize.width / 2, winSize.height / 2);
				label->setOpacity(0);
				this->getParent()->addChild(label);

				//��ʾ���ֵ��뵭���󣬸��µ�ͼ���ٿ�����������
				auto fadein = FadeIn::create(0.5);
				auto fadeout = FadeOut::create(0.5);

				auto call = CallFunc::create([this, label](){
					do
					{
						updateMap();
					} while (isDeadMap());

					label->removeFromParent();

					_eventDispatcher->resumeEventListenersForTarget(this);
				});

				label->runAction(Sequence::create(fadein, DelayTime::create(2), fadeout, call, nullptr));
			}
			else
			{
				//���������ͼ����ô��ֱ�ӿ��������������ȴ���һ�ֵĽ�������
				_eventDispatcher->resumeEventListenersForTarget(this);
			}
		}
	}
}