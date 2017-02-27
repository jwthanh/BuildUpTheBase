//  CricketAndroidJni.h
#include <jni.h>

#ifndef __ANDROID_JNI__
#define __ANDROID_JNI__
#ifdef __cplusplus
extern "C" {
#endif

    JNIEXPORT void JNICALL Java_com_tankorsmash_buildupthebase_AppActivity_initCricket(JNIEnv *, jobject obj, jobject thiz);

#ifdef __cplusplus
}
#endif
#endif __ANDROID_JNI__
