#include "MagicScene.h"
#include "MagicEmitter.h"

#include "mp_cocos.h"
#include "2d/CCScene.h"
#include "2d/CCMenuItem.h"
#include "2d/CCMenu.h"
#include "base/CCDirector.h"

USING_NS_CC;

Scene* MagicScene::createScene()
{
	// 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = MagicScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MagicScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

	cur_node=NULL;

	scheduleUpdate();
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	/////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(MagicScene::menuCloseCallback, this));
    
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

	// create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);



	/*
	/////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
    
    // position the label on the center of the screen
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1);

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("HelloWorld.png");

    // position the sprite on the center of the screen
    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(sprite, 2);
	*/


	MP_Manager& MP=MP_Manager::GetInstance();

	#ifdef MAGIC_3D
	bool perspective=true;
	MP.device->SetScene3d(perspective);
	Camera* camera=((MP_Device_Cocos*)MP.device)->GetCamera();
	addChild(camera);

	/*
	if (!perspective)
	{
		// сдвигаем эмиттеры в центр экрана из нижнего левого угла, так как Cocos думает, что при ортогональной проекции координата 0, 0, 0 находится в углу
		cocos2d::Size winSize=Director::getInstance()->getWinSizeInPixels();
		float sx=((float)(winSize.width))/2.f;
		float sy=((float)(winSize.height))/2.f;

		HM_EMITTER hmEmitter=MP.GetFirstEmitter();
		while (hmEmitter)
		{
			MAGIC_POSITION pos;
			Magic_GetEmitterPosition(hmEmitter, &pos);
			pos.x+=sx;
			pos.y+=sy;
			Magic_SetEmitterPositionWithAttachedPhysicObjects(hmEmitter, &pos);
			hmEmitter=MP.GetNextEmitter(hmEmitter);
		}
	}
	*/

	#endif
	
	k_emitter=0;
	HM_EMITTER hmEmitter=MP.GetFirstEmitter();
	while (hmEmitter)
	{
		m_emitter[k_emitter]=hmEmitter;
		k_emitter++;
		hmEmitter=MP.GetNextEmitter(hmEmitter);
	}

	t_emitter=0;
	SelectEmitter(t_emitter);

	return true;
}

void MagicScene::update(float delta) 
{
	cocos2d::Layer::update(delta);

	MP_Emitter_Cocos* emitter=cur_node->GetEmitter();
	if (emitter->GetState()==MAGIC_STATE_STOP)
	{
		t_emitter++;
		if (t_emitter==k_emitter)
			t_emitter=0;

		SelectEmitter(t_emitter);
	}
}

void MagicScene::SelectEmitter(int emitter)
{
	if (cur_node)
		cur_node->removeFromParent();

	HM_EMITTER hmEmitter=m_emitter[emitter];
	cur_node=MagicEmitter::create(hmEmitter);

	#ifdef MAGIC_3D
	cur_node->setCameraMask((unsigned short) CameraFlag::USER1);
	#endif

	addChild(cur_node);
}


void MagicScene::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
