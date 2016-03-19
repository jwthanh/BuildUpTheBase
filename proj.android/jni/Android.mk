LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos/editor-support/cocostudio)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_SRC_FILES := hellocpp/main.cpp \
                   ../../Classes/AppDelegate.cpp \
                   ../../Classes/Beatup.cpp \
                   ../../Classes/BaseMenu.cpp \
                   ../../Classes/NuMenu.cpp \
                   ../../Classes/Resources.cpp \
                   ../../Classes/GameLayer.cpp \
                   ../../Classes/Weapons.cpp \
                   ../../Classes/Fist.cpp \
                   ../../Classes/FileOperation.cpp \
                   ../../Classes/StaticData.cpp \
                   ../../Classes/Face.cpp \
                   ../../Classes/Clock.cpp \
                   ../../Classes/Buyable.cpp \
                   ../../Classes/SoundEngine.cpp \
                   ../../Classes/DataManager.cpp \
                   ../../Classes/Combo.cpp \
                   ../../Classes/ProgressBar.cpp \
                   ../../Classes/ShatterNode.cpp \
                   ../../Classes/Quest.cpp \
                   ../../Classes/Alert.cpp \
                   ../../Classes/Util.cpp \
                   ../../Classes/Level.cpp \
                   ../../Classes/FShake.cpp \
                   ../../Classes/MainMenu.cpp \
                   ../../Classes/HouseBuilding.cpp \
                   ../../Classes/Recipe.cpp \
                   ../../Classes/Combat.cpp \
                   ../../Classes/attribute.cpp \
                   ../../Classes/attr_effect.cpp \
                   ../../Classes/attribute_container.cpp \
                   ../../Classes/Worker.cpp \

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes

LOCAL_STATIC_LIBRARIES := cocos2dx_static

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)

APP_PLATFORM := android-10

LOCAL_CPPFLAGS := -DCOCOS2D_DEBUG=0
