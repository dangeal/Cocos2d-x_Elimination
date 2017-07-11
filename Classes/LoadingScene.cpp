#include "LoadingScene.h"
#include "GameScene.h"
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;


//��������
Scene* LoadingScene::createScene()
{
	//����һ�����ͷŵĳ�������
	auto scene = Scene::create();

	//����һ�����ͷŵĻ�������
	auto layer = LoadingScene::create();
	
	//�Ѵ����Ļ������ӵ�������
    //һ������������Ӷ�������
	scene->addChild(layer);
	
	//������������ĳ���
	return scene;
}

// ������ʼ������
bool LoadingScene::init()
{
	// ���Ƚ��и����ʼ��
	Layer::init();

	//getVisibleSize������ӿڣ��������򣩵Ĵ�С��
	//����DesignResolutionSize����Ļ�ߴ�һ������getVisibleSize����getWinSize��
	//???????????????
	auto winSize = Director::getInstance()->getWinSize();



	m_texture_num = 0; //��ǰ�Ѽ��صĿ�Ƭ
	int *ptexture_num = &m_texture_num;

	auto label = Label::createWithSystemFont("Loading...", "Arial", 36);
	label->setPosition(winSize.width / 2, winSize.height / 2);
	addChild(label);
	//log("%f %f", winSize.width , winSize.height);  //320*480ע��Ҫ%f����%d

	//������ϻص�
	auto addTextureCallback = [&,ptexture_num](Texture2D* texture)
	{
		(*ptexture_num)++;
		log("load a texture async");
		log("m_texture_num=%d",*ptexture_num);
	};

	//�첽Ԥ���ؿ�Ƭ���첽Ԥ���أ�������������������������
	TextureCache::getInstance()->addImageAsync("card1.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("card2.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("card3.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("card4.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("card5.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("card6.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("card7.png", addTextureCallback);

	//�첽Ԥ���ر���ͼ
	TextureCache::getInstance()->addImageAsync("bground1.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("bground2.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("bground3.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("bground4.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("gameover.png", addTextureCallback);

	//�첽����һЩ��������ͼ
	TextureCache::getInstance()->addImageAsync("board.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("bonusbar.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("bonusbar_fill.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("bonus.png", addTextureCallback);
	
	//�������ؽ��ȼ��
	schedule(schedule_selector(LoadingScene::onTextureLoading));

	//Ԥ������Ч
	SimpleAudioEngine::getInstance()->preloadEffect("eliminate.ogg");
	SimpleAudioEngine::getInstance()->preloadEffect("swapback.ogg");

	return true;
}


//�����������Ƿ����
void LoadingScene::onTextureLoading(float dt)
{
	//һ����Ƭ������ϣ���ô������Ϸ����GameScene
	if (m_texture_num == 16)  //������16�ſ�Ƭ
	{
		unschedule(schedule_selector(LoadingScene::onTextureLoading));
		log("loading down!");

		auto call = CallFunc::create([](){
			auto scene = GameScene::createScene();
			Director::getInstance()->replaceScene(TransitionFade::create(0.5, scene));
		});

		//�ȴ�һ���������
		this->runAction(Sequence::create(DelayTime::create(0.51), call, nullptr));
	}
}