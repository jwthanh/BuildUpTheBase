#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "_core/magic.h"

class MagicEmitter;

class MagicScene : public cocos2d::Layer
{
protected:
	MagicEmitter* cur_node;
	int t_emitter;
	int k_emitter;
	HM_EMITTER m_emitter[1000];

public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(MagicScene);

	void update(float delta);

	void SelectEmitter(int emitter);
};

#endif // __HELLOWORLD_SCENE_H__
