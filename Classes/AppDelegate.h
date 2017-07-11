#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_

#include "cocos2d.h"

/**
@brief    The cocos2d Application.

The reason for implement as private inheritance is to hide some interface call by Director.
*/
class  AppDelegate : private cocos2d::Application
{
public:
    AppDelegate();
    virtual ~AppDelegate();


	//���� OpenGL context(������)OpenGL�ܻ��ܴ�ܸ��Ӻ�Ѥ������ά���磬
	//���϶�Ҫ��һЩ������¼һЩ״̬����ţ�����Щ״̬����ż���������
	//��Ϊһ�����壬����OpenGL�����ġ�OpenGL������Щ״̬����ţ�
	//��֪���Լ�Ӧ�û�ʲô�����Խ��������ġ�

    //������ö�����ƽ̨����Ч
    virtual void initGLContextAttrs();

    /**
    E
    */

	//�߼���ʼ��
    virtual bool applicationDidFinishLaunching();

    /**
    @brief  The function be called when the application enter background
    @param  the pointer of the application
    */

	//�л�����̨
    virtual void applicationDidEnterBackground();

    /**
    @brief  The function be called when the application enter foreground
    @param  the pointer of the application
    */

	//�л���ǰ̨
    virtual void applicationWillEnterForeground();
};

#endif // _APP_DELEGATE_H_

