//  CricketAndroidJni.cpp
#include "CricketAndroidJni.h"

#include "ck/ck.h"
#include "ck/config.h"

JNIEXPORT void JNICALL Java_com_tankorsmash_buildupthebase_AppActivity_initCricket(JNIEnv *env, jobject obj, jobject thiz) {
    CkConfig config(env, thiz);
    CkInit(&config);
}
