LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := magic
LOCAL_SRC_FILES := libamagic.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := ck
LOCAL_SRC_FILES := libck.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos/editor-support/cocostudio)

LOCAL_MODULE := cocos2dcpp_shared


LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_SRC_FILES := hellocpp/main.cpp \
                   hellocpp/CricketAndroidJni.cpp\
                   ../../Classes/AppDelegate.cpp \
                   ../../Classes/Beatup.cpp \
                   ../../Classes/NuMenu.cpp \
                   ../../Classes/HarvestScene.cpp \
                   ../../Classes/BuildingsSideBar.cpp \
                   ../../Classes/Harvestable.cpp \
                   ../../Classes/HarvestableManager.cpp \
                   ../../Classes/Visitors.cpp \
                   ../../Classes/Technology.cpp \
                   ../../Classes/Ingredients.cpp \
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
                   ../../Classes/Serializer.cpp \
                   ../../Classes/Modal.cpp \
                   ../../Classes/Item.cpp \
                   ../../Classes/Logging.cpp \
                   ../../Classes/Miner.cpp \
                   ../../Classes/NodeBuilder.cpp \
                   ../../Classes/ItemScene.cpp \
                   ../../Classes/Equipment.cpp \
                   ../../Classes/Tutorial.cpp \
                   ../../Classes/debugging/ActionPanel.cpp \
                   ../../Classes/utilities/vibration.cpp \
                   ../../Classes/goals/Achievement.cpp \
\
\
\
                   ../../Classes/magic_particles/MagicEmitter.cpp \
                   ../../Classes/magic_particles/mp_cocos.cpp \
                   ../../Classes/magic_particles/platform_cocos.cpp \
                   ../../Classes/magic_particles/_core/mp.cpp \
                   ../../Classes/magic_particles/_core/platform_win_posix.cpp \
                   ../../Classes/magic_particles/opengl/mp_wrap.cpp \
                   ../../Classes/magic_particles/opengl/image_loader.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes $(LOCAL_PATH)/../../../_core $(LOCAL_PATH)/../../../opengl $(LOCAL_PATH)/../../Classes/cricket_audio/inc

LOCAL_STATIC_LIBRARIES := cocos2dx_static
LOCAL_STATIC_LIBRARIES += magic
LOCAL_STATIC_LIBRARIES += ck

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)

APP_PLATFORM := android-10

LOCAL_CPPFLAGS := -DCOCOS2D_DEBUG=0
