#include "AppDelegate.h"
#include "LoadingScene.h"

USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

void AppDelegate::initGLContextAttrs()
{
	//设置 OpenGL context 属性,目前只能设置6个属性
    //red,green,blue,alpha,depth,stencil

    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

//当应用程序启动时执行,游戏程序启动入口
//在这里我们启动了第一个scene（场景）
//在具体游戏中通常在这里启动loading界面
//你的游戏从这里开始！
bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
		glview = GLViewImpl::createWithRect("Cute-Elimination", Rect(0, 0, 320, 480)); //设置PC端的设备分辨率，窗口名。在移动端这行代码无效。
        director->setOpenGLView(glview);
    }

	glview->setDesignResolutionSize(320, 480, ResolutionPolicy::SHOW_ALL); //设置游戏分辨率，尚未适配移动设备屏幕。
	
	// 在屏幕上显示FPS数
    // 开发阶段建议开启这个设置，可以通过这个对自己游戏性能有个大体了解
    // 等游戏正式发布时关闭这个设置
    director->setDisplayStats(true);

	// 设置 FPS数 默认值为 1.0/60 
    director->setAnimationInterval(1.0 / 60);

	  // 创建一个 LoadingScene的scene.
    auto scene = LoadingScene::createScene();

	 // 告诉director运行 LoadingScene的scene
    director->runWithScene(scene);

    return true;
}


// 当游戏进入后台时会调用这个方法
// 比如玩游戏时按下android手机的home按键
// 比如当游戏时有电话打入直接显示来电界面
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // 如果你的游戏使用了SimpleAudioEngine,必须在这里进行暂停
    // 暂停代码如下：
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// 当游戏恢复到前台运行时会调用这个方法
// 比如接电话结束是游戏界面又恢复到前台时
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // 如果你的游戏使用了SimpleAudioEngine, 必须在这里进行恢复
    // 恢复代码如下：
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
