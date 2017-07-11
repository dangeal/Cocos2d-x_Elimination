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


	//设置 OpenGL context(上下文)OpenGL能画很大很复杂很绚丽的三维世界，
	//它肯定要用一些变量记录一些状态、编号，把这些状态、编号集合起来，
	//作为一个整体，就是OpenGL上下文。OpenGL看到这些状态、编号，
	//才知道自己应该画什么，所以叫做上下文。

    //这个设置对所有平台都有效
    virtual void initGLContextAttrs();

    /**
    E
    */

	//逻辑初始化
    virtual bool applicationDidFinishLaunching();

    /**
    @brief  The function be called when the application enter background
    @param  the pointer of the application
    */

	//切换到后台
    virtual void applicationDidEnterBackground();

    /**
    @brief  The function be called when the application enter foreground
    @param  the pointer of the application
    */

	//切换到前台
    virtual void applicationWillEnterForeground();
};

#endif // _APP_DELEGATE_H_

