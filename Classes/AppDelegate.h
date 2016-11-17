#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_
#include <CCApplication.h>


class MP_Device_WRAP;

/**
@brief    The cocos2d Application.

The reason for implement as private inheritance is to hide some interface call by Director.
*/
class  AppDelegate : private cocos2d::Application
{

protected:
	MP_Device_WRAP* device;
public:
    AppDelegate();
    virtual ~AppDelegate();

    virtual void initGLContextAttrs();

    void init_magic_particles();

    /**
    @brief    Implement Director and Scene init code here.
    @return true    Initialize success, app continue.
    @return false   Initialize failed, app terminate.
    */
    virtual bool applicationDidFinishLaunching();

    /**
    @brief  The function be called when the application enter background
    @param  the pointer of the application
    */
    virtual void applicationDidEnterBackground();

    /**
    @brief  The function be called when the application enter foreground
    @param  the pointer of the application
    */
    virtual void applicationWillEnterForeground();

    void preload_all();
    void preload_sprites();
    void preload_sounds();
    void preload_particles();
};

#endif // _APP_DELEGATE_H_

