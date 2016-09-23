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
                   ../../Classes/NuMenu.cpp \
                   ../../Classes/HarvestScene.cpp \
                   ../../Classes/BuildingsSideBar.cpp \
                   ../../Classes/Harvestable.cpp \
                   ../../Classes/Visitors.cpp \
                   ../../Classes/Technology.cpp \
                   ../../Classes/Resources.cpp \
                   ../../Classes/GameLogic.cpp \
                   ../../Classes/GameLayer.cpp \
                   ../../Classes/MiscUI.cpp \
                   ../../Classes/Network.cpp \
                   ../../Classes/FileOperation.cpp \
                   ../../Classes/StaticData.cpp \
                   ../../Classes/Clock.cpp \
                   ../../Classes/Buyable.cpp \
                   ../../Classes/SoundEngine.cpp \
                   ../../Classes/DataManager.cpp \
                   ../../Classes/ProgressBar.cpp \
                   ../../Classes/ShatterNode.cpp \
                   ../../Classes/Alert.cpp \
                   ../../Classes/Util.cpp \
                   ../../Classes/FShake.cpp \
                   ../../Classes/HouseBuilding.cpp \
                   ../../Classes/Recipe.cpp \
                   ../../Classes/Combat.cpp \
                   ../../Classes/attribute.cpp \
                   ../../Classes/attr_effect.cpp \
                   ../../Classes/attribute_container.cpp \
                   ../../Classes/Worker.cpp \
                   ../../Classes/Updateable.cpp \
                   ../../Classes/Fighter.cpp \
                   ../../Classes/Animal.cpp \
                   ../../Classes/Serializer.cpp \
                   ../../Classes/Modal.cpp \
                   ../../Classes/Item.cpp \
                   ../../Classes/Logging.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes

LOCAL_STATIC_LIBRARIES := cocos2dx_static

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)

APP_PLATFORM := android-10

LOCAL_CPPFLAGS := -DCOCOS2D_DEBUG=0
