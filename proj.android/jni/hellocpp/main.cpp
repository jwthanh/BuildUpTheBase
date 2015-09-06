#include "AppDelegate.h"
#include "cocos2d.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include <android/log.h>

#define  LOG_TAG    "main"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

using namespace cocos2d;

// The signature of this method changed in v3.7
#if COCOS2D_VERSION <= 0x00030600
void cocos_android_app_init(JNIEnv* env, jobject thiz)
#else
void cocos_android_app_init(JNIEnv* env)
#endif
{
    LOGD("cocos_android_app_init");
    AppDelegate *pAppDelegate = new AppDelegate();
}

void vibrate(int milliseconds)
{
    JniMethodInfo minfo;
    if (JniHelper::getStaticMethodInfo(minfo, "com/tankorsmash/beatupface/AppActivity", "vibrate", "(I)V"), "Function doesn't exist")
    {
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, (jint)milliseconds);
    }
    else
    {
        LOGD("VIBRATION FAILED");
    };
    minfo.env->DeleteLocalRef(minfo.classID);
}
