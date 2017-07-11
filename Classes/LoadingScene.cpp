#include "LoadingScene.h"
#include "GameScene.h"
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;


//创建场景
Scene* LoadingScene::createScene()
{
	//创建一个自释放的场景对象
	auto scene = Scene::create();

	//创建一个自释放的画面层对象
	auto layer = LoadingScene::create();
	
	//把创建的画面层添加到场景中
    //一个场景可以添加多个画面层
	scene->addChild(layer);
	
	//返回这个创建的场景
	return scene;
}

// 场景初始化方法
bool LoadingScene::init()
{
	// 首先进行父类初始化
	Layer::init();

	//getVisibleSize：获得视口（可视区域）的大小，
	//若是DesignResolutionSize跟屏幕尺寸一样大，则getVisibleSize便是getWinSize。
	//???????????????
	auto winSize = Director::getInstance()->getWinSize();



	m_texture_num = 0; //当前已加载的卡片
	int *ptexture_num = &m_texture_num;

	auto label = Label::createWithSystemFont("Loading...", "Arial", 36);
	label->setPosition(winSize.width / 2, winSize.height / 2);
	addChild(label);
	//log("%f %f", winSize.width , winSize.height);  //320*480注意要%f而非%d

	//加载完毕回调
	auto addTextureCallback = [&,ptexture_num](Texture2D* texture)
	{
		(*ptexture_num)++;
		log("load a texture async");
		log("m_texture_num=%d",*ptexture_num);
	};

	//异步预加载卡片（异步预加载？？？？？？？？？？？？）
	TextureCache::getInstance()->addImageAsync("card1.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("card2.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("card3.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("card4.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("card5.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("card6.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("card7.png", addTextureCallback);

	//异步预加载背景图
	TextureCache::getInstance()->addImageAsync("bground1.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("bground2.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("bground3.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("bground4.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("gameover.png", addTextureCallback);

	//异步加载一些其它纹理图
	TextureCache::getInstance()->addImageAsync("board.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("bonusbar.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("bonusbar_fill.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("bonus.png", addTextureCallback);
	
	//开启加载进度检测
	schedule(schedule_selector(LoadingScene::onTextureLoading));

	//预加载音效
	SimpleAudioEngine::getInstance()->preloadEffect("eliminate.ogg");
	SimpleAudioEngine::getInstance()->preloadEffect("swapback.ogg");

	return true;
}


//检测纹理加载是否完毕
void LoadingScene::onTextureLoading(float dt)
{
	//一旦卡片加载完毕，那么进入游戏场景GameScene
	if (m_texture_num == 16)  //加载了16张卡片
	{
		unschedule(schedule_selector(LoadingScene::onTextureLoading));
		log("loading down!");

		auto call = CallFunc::create([](){
			auto scene = GameScene::createScene();
			Director::getInstance()->replaceScene(TransitionFade::create(0.5, scene));
		});

		//等待一会儿，进入
		this->runAction(Sequence::create(DelayTime::create(0.51), call, nullptr));
	}
}