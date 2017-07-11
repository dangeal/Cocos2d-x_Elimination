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
	//���� OpenGL context ����,Ŀǰֻ������6������
    //red,green,blue,alpha,depth,stencil

    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

//��Ӧ�ó�������ʱִ��,��Ϸ�����������
//���������������˵�һ��scene��������
//�ھ�����Ϸ��ͨ������������loading����
//�����Ϸ�����￪ʼ��
bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
		glview = GLViewImpl::createWithRect("Cute-Elimination", Rect(0, 0, 320, 480)); //����PC�˵��豸�ֱ��ʣ������������ƶ������д�����Ч��
        director->setOpenGLView(glview);
    }

	glview->setDesignResolutionSize(320, 480, ResolutionPolicy::SHOW_ALL); //������Ϸ�ֱ��ʣ���δ�����ƶ��豸��Ļ��
	
	// ����Ļ����ʾFPS��
    // �����׶ν��鿪��������ã�����ͨ��������Լ���Ϸ�����и������˽�
    // ����Ϸ��ʽ����ʱ�ر��������
    director->setDisplayStats(true);

	// ���� FPS�� Ĭ��ֵΪ 1.0/60 
    director->setAnimationInterval(1.0 / 60);

	  // ����һ�� LoadingScene��scene.
    auto scene = LoadingScene::createScene();

	 // ����director���� LoadingScene��scene
    director->runWithScene(scene);

    return true;
}


// ����Ϸ�����̨ʱ������������
// ��������Ϸʱ����android�ֻ���home����
// ���統��Ϸʱ�е绰����ֱ����ʾ�������
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // ��������Ϸʹ����SimpleAudioEngine,���������������ͣ
    // ��ͣ�������£�
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// ����Ϸ�ָ���ǰ̨����ʱ������������
// ����ӵ绰��������Ϸ�����ָֻ���ǰ̨ʱ
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // ��������Ϸʹ����SimpleAudioEngine, ������������лָ�
    // �ָ��������£�
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
