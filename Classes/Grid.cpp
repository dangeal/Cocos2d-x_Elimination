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

	//根据行列初始化一个空的卡片容器大小
	m_CardsBox.resize(m_row);
	for (auto &vec : m_CardsBox)
		vec.resize(m_col);         //std::vector resize(x)重新指定队列的长度。

	//1.根据布局大小创建出卡片阵列
	//2.布局坐标以左下角为原点，x右y上为正方向
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
	
	//加入触摸监听
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
	//模拟交换，判断交换后是否能消除，如不能，那么就是个死图
	auto swap = [](Card** a, Card** b)
	{
		auto temp = *a;
		*a = *b;
		*b = temp;
	};

	bool isDeadMap = true;

	//遍历每一个卡片
	for (int x = 0; x < m_col; x++)
	{
		for (int y = 0; y < m_row; y++)
		{
			//跟左边的交换
			if (x > 0)
			{
				swap(&m_CardsBox[x][y], &m_CardsBox[x-1][y]);
				if (canEliminate())
					isDeadMap = false;
				swap(&m_CardsBox[x][y], &m_CardsBox[x-1][y]);
			}

			//跟右边的交换
			if (x < m_col - 1)
			{
				swap(&m_CardsBox[x][y], &m_CardsBox[x+1][y]);
				if (canEliminate())
					isDeadMap = false;
				swap(&m_CardsBox[x][y], &m_CardsBox[x+1][y]);
			}

			//跟上面的交换
			if (y < m_row - 1)
			{
				swap(&m_CardsBox[x][y], &m_CardsBox[x][y+1]);
				if (canEliminate())
					isDeadMap = false;
				swap(&m_CardsBox[x][y], &m_CardsBox[x][y+1]);
			}

			//跟下面的交换
			if (y > 0)
			{
				swap(&m_CardsBox[x][y], &m_CardsBox[x][y-1]);
				if (canEliminate())
					isDeadMap = false;
				swap(&m_CardsBox[x][y], &m_CardsBox[x][y-1]);
			}
		}
	}

	//canEliminate会存储能消除的卡片进去，由于是模拟交换，所以还要清空
	m_eliminateCardBox.clear();

	return isDeadMap;
}

Card* Grid::createACard(int x, int y)
{
	//1.根据布局坐标创建一颗卡片，类型随机
	//2.判断该卡片是否合法（不会三消）
	//3.设置该卡片的世界坐标
	//4.将该卡片加入渲染节点
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
	//1.分别判断新加入的卡片在x轴y轴方向是否会三消
	//2.由于是从正方向加入卡片，因此只需往负方向判断
	//3.x，y坐标小于等于1不必判断
	//4.两轴同时合法方合法
	bool isXLegal = true;
	bool isYLegal = true;

	if (x > 1)
	{
		//向x轴负方向分别比较两位，如果卡片类型都一样，那么表示三消，该卡片不合法
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
		//向y轴负方向分别比较两位，如果卡片类型都一样，那么表示三消，该卡片不合法
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
	//将触摸点的坐标转化为模型坐标??????????
	auto pos = this->convertToNodeSpace(pTouch->getLocation());
	
	//是否有按在卡片布局上
	if (Rect(0, 0, m_col*GRID_WIDTH, m_row*GRID_WIDTH).containsPoint(pos))
	{
		//得到布局坐标
		int x = pos.x / GRID_WIDTH;
		int y = pos.y / GRID_WIDTH;

		//得到当前选中的卡片
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
	//如果没有选择卡片，那么返回
	if (!m_CardSelected)
	{
		return;
	}

	//已选择卡片的布局坐标
	int startX = m_CardSelected->getX();
	int startY = m_CardSelected->getY();

	//触摸点的布局坐标
	auto pos = this->convertToNodeSpace(pTouch->getLocation());
	int touchX = pos.x / GRID_WIDTH;
	int touchY = pos.y / GRID_WIDTH;

	//如果触摸点不在布局内，或者触摸点布局坐标和已选卡片布局坐标一样，那么返回
	if (!Rect(0, 0, m_col*GRID_WIDTH, m_row*GRID_WIDTH).containsPoint(pos) || Vec2(touchX, touchY) == Vec2(startX, startY)) 
	{
		return;
	}

	//判断已选卡片的布局坐标与触摸点的布局坐标是否直角相隔一个单位
	if (abs(startX - touchX) + abs(startY - touchY) != 1)
	{
		//log("touch pos not on border");
		return;
	}

	//余下的情况，触摸点上面的卡片就是欲进行交换的卡片
	//通过坐标索引，获取欲交换的卡片
	m_CardSwapped = m_CardsBox[touchX][touchY];

	//交换卡片，开启交换状态捕捉函数（在交换完成后，判断是否可以消除）
	swapCards(m_CardSelected, m_CardSwapped);
	schedule(schedule_selector(Grid::onCardsSwaping));
}

void Grid::swapCards(Card *cardA, Card *cardB)
{
	_eventDispatcher->pauseEventListenersForTarget(this); //交换开始，关闭触摸监听

	//1.交换卡片容器内的卡片指针
	//2.交换卡片坐标
	//3.卡片移动到新的位置
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
	//移动开始设置卡片交换状态为真，移动结束再设置为假
	card->setSwapingState(true);
	auto move = MoveTo::create(MOVE_SPEED, Vec2(card->getX() * GRID_WIDTH, card->getY() * GRID_WIDTH));
	auto call = CallFunc::create([card](){
		card->setSwapingState(false);
	});
	card->runAction(Sequence::create(move, call, nullptr));
}

bool Grid::canEliminate()
{
	//该函数判断当前状态的卡片阵列是否能消除
	//将能消除的卡片加入消除卡片盒子，等待消除

	int count = 0; //连续数
	Card *cardBegin = nullptr; //起始遍历的卡片
	Card *cardNext = nullptr; //从起始卡片开始往前遍历的卡片

	//遍历每一列
	for (int x = 0; x < m_col; x++) 
	{
		for (int y = 0; y < m_row - 1;)
		{
			count = 1;
			cardBegin = m_CardsBox[x][y];
			cardNext = m_CardsBox[x][y + 1];

			 //如果连续出现同类型
			while (cardBegin->getType() == cardNext->getType())
			{
				count++;
				int nextIndex = y + count;
				if (nextIndex > m_row - 1)
					break;
				cardNext = m_CardsBox[x][nextIndex];
			}
			//如果连续数大于等于3，那么遍历的这些卡片应当消除，把它们存入消除卡片盒子
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

	//遍历每一行，逻辑和遍历每一列是一样的
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
					//有可能有卡片同时行列可消除，那么不能重复存储到消除卡片盒子，故需添加一次判断
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

	//如果消除卡片盒子不为空，那么说明该阵列可消除，返回真
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
	//遍历消除卡片盒子，对其中的卡片进行消除操作
	for (auto card : m_eliminateCardBox)
	{
		//生成新的卡片，类型随机，初始位置在最上面一行的上边一行（布局外一格）
		auto newCard = Card::createByType(random(FIRST_SPRITE_ID, LAST_SPRITE_ID), card->getX(), m_row); 
		setCardPixPos(newCard, newCard->getX(), m_row); 
		addChild(newCard);

		//将新卡片放到新卡片盒子内，等待加入布局
		m_newCardBox.pushBack(newCard);

		//卡片盒子内应当刷新的卡片暂时置为空
		m_CardsBox[card->getX()][card->getY()] = nullptr;

		//原有卡片对象消除
		card->eliminate();
	}
}

void Grid::refreshGrid()
{
	//遍历列，如果该列有空位，那么应当刷新
	for (int x = 0; x < m_col; x++)
	{
		int empty_count = 0; //一列总的空格子数

		for (int y = 0; y < m_row; y++)
		{
			//根据坐标索引卡片盒子内的卡片指针，如果为空，那么说明该坐标位置为空
			auto card = m_CardsBox[x][y];
			if (!card)
				empty_count++;
		}
		if (empty_count)
		{
			//log("the %d col has %d empty", x, empty_count);
			//找到有空位的列，刷新该列的卡片
			refreshCardsToNewPos(x);
		}
	}
}

void Grid::refreshCardsToNewPos(int col)
{
	//刷新该列上面的卡片
	int n = 0; //当前遍历到的空位数
	auto pCardsbox = &m_CardsBox; //保存一个卡片盒子的指针，这是为了让其能传入lambda

	//先让现有的卡片下落
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
				//更新卡片盒子内的数据
				(*pCardsbox)[card->getX()][card->getY()] = card;
			});

			card->runAction(Sequence::create(move, call, nullptr));
		}
	}
	
	//再让新卡片下落
	int i = n;
	int delta = 1;

	for (auto card : m_newCardBox)
	{
		if (card->getX() == col)
		{
			card->setY(m_row - i);

			auto delay = DelayTime::create(0.2);
			//后下落的速度设置慢一些
			auto move = MoveBy::create(0.2*delta++, Vec2(0, -i--*GRID_WIDTH));
			auto call = CallFunc::create([card, pCardsbox, this](){
				(*pCardsbox)[card->getX()][card->getY()] = card;
				//从新卡片盒子中移除该卡片
				m_newCardBox.eraseObject(card);
			});

			card->runAction(Sequence::create(delay, move, call, nullptr));
		}
	}
}

void Grid::onCardsSwaping(float dt)
{
	//捕捉两个正在交换的卡片的交换动作是否已经停止，如果没停止，返回继续捕捉
	if (m_CardSelected->isSwaping() || m_CardSwapped->isSwaping())
	{
		return;
	}
	//如果卡片交换动作执行完毕
	else
	{
		unschedule(schedule_selector(Grid::onCardsSwaping));  //停止捕捉

		log("swap over!");

		log("is it can eliminate?");
		
		//判断当前状态是否可以消除
		if (canEliminate())
		{
			log("yes,eliminate!");

			m_CardSelected = nullptr;

			//开始消除，开启消除状态捕捉函数(捕捉到消除完毕后，刷新布局)，这一轮消除正式开始
			SimpleAudioEngine::getInstance()->playEffect("eliminate.ogg");
			goEliminate();
			schedule(schedule_selector(Grid::onCardsEliminating));
		}
		else
		{
			log("no, cant eliminate!");

			//不能消除，交换回去，开启交换返回时的捕捉函数(捕捉到消除完毕后，开启触摸接听)
			SimpleAudioEngine::getInstance()->playEffect("swapback.ogg");
			swapCards(m_CardSelected, m_CardSwapped);
			schedule(schedule_selector(Grid::onCardsSwapingBack));
		}
	}
}

void Grid::onCardsSwapingBack(float dt)
{
	//捕捉两个正在交换的卡片的交换动作是否已经停止，如果没停止，返回继续捕捉
	if (m_CardSelected->isSwaping() || m_CardSwapped->isSwaping())
	{
		return;
	}
	else
	{
		unschedule(schedule_selector(Grid::onCardsSwapingBack)); //停止捕捉

		log("swap back!");

		m_CardSelected = nullptr;

		_eventDispatcher->resumeEventListenersForTarget(this); //重新开始触摸监听
	}
}

void Grid::onCardsEliminating(float dt)
{
	//捕捉卡片消除状态，如果有卡片还在消除，那么继续捕捉
	for (auto card : m_eliminateCardBox)
	{
		if (card->isEliminating())
		{
			//log("Eliminating");
			return;
		}
	}
	
	//如果全部卡片已经消除完毕，停止捕捉函数
	unschedule(schedule_selector(Grid::onCardsEliminating));

	m_eliminateCardBox.clear(); //清空消除卡片盒子
	
	log("eliminate over!");
	log("begin to refresh!");

	//刷新卡片阵列，并开启刷新状态捕捉函数（刷新一遍结束，重新判断新阵列是否可消除）
	refreshGrid();
	schedule(schedule_selector(Grid::onCardsRefreshing));
}

void Grid::onCardsRefreshing(float dt)
{
	//捕捉卡片刷新状态，如果新卡片盒子还有卡片（即新卡片还在刷新当中），那么继续捕捉
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

			//如果能消除，那么继续消除
			SimpleAudioEngine::getInstance()->playEffect("eliminate.ogg");
			goEliminate();
			schedule(schedule_selector(Grid::onCardsEliminating));
		}
		else
		{
			log("no, cannot eliminate! over!");

			//判断是否为死图，如果是，则执行一段文字动画，提示即将更新地图
			if (isDeadMap())
			{
				log("cant eliminate any more, updating a new map!");

				auto winSize = Director::getInstance()->getWinSize();
				auto label = Label::createWithTTF("Cant Eliminate Any More, Change!", "fonts/Marker Felt.ttf", 24);
				label->setTextColor(Color4B::BLACK);
				label->setPosition(winSize.width / 2, winSize.height / 2);
				label->setOpacity(0);
				this->getParent()->addChild(label);

				//提示文字淡入淡出后，更新地图，再开启触摸监听
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
				//如果不是死图，那么就直接开启触摸监听，等待下一轮的交互操作
				_eventDispatcher->resumeEventListenersForTarget(this);
			}
		}
	}
}