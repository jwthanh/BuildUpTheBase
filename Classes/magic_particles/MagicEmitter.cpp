
#include "MagicEmitter.h"
#include "mp_cocos.h"

using namespace cocos2d;

MagicEmitter::MagicEmitter() : cocos2d::Node()
{
	emitter=NULL;
}

MagicEmitter::~MagicEmitter()
{
	if (emitter)
	{
		emitter->ResetNode();
		MP_Manager& MP=MP_Manager::GetInstance();
		HM_EMITTER hmEmitter=emitter->GetEmitter();
		MP.DeleteEmitter(hmEmitter);
	}
}

MagicEmitter* MagicEmitter::create(const char* emitterName)
{
	MP_Manager& MP=MP_Manager::GetInstance();
	MP_Emitter_Cocos* e=(MP_Emitter_Cocos*)MP.GetEmitterByName(emitterName);
	if (e)
		return create(e);
	return NULL;
}

MagicEmitter* MagicEmitter::create(HM_EMITTER hmEmitter)
{
	MP_Manager& MP=MP_Manager::GetInstance();
	MP_Emitter_Cocos* e=(MP_Emitter_Cocos*)MP.GetEmitter(hmEmitter);
	if (e)
		return create(e);
	return NULL;
}

MagicEmitter* MagicEmitter::create(MP_Emitter_Cocos* emitter)
{
	MagicEmitter* node=new MagicEmitter();

	MP_Manager& MP=MP_Manager::GetInstance();
	HM_EMITTER hmEmitter=emitter->GetEmitter();
	HM_EMITTER copy=MP.DuplicateEmitter(hmEmitter);
	emitter=(MP_Emitter_Cocos*)MP.GetEmitter(copy);

	node->emitter=emitter;
	emitter->SetNode(node);
	emitter->SetState(MAGIC_STATE_UPDATE);

	MP_POSITION pos;
	emitter->GetPosition(pos);
	node->cocos2d::Node::setPosition(pos.x, pos.y);
	#ifdef MAGIC_3D
	node->cocos2d::Node::setPosition3D(cocos2d::Vec3(pos.x, pos.y, pos.z));
	#endif

	node->scheduleUpdate();

	return node;
}

void MagicEmitter::ResetEmitter()
{
	emitter=NULL;
}

void MagicEmitter::update(float delta) 
{
	Node::update(delta);

	if (emitter)
	{
		double rate=delta*1000;
		if (rate>500)
			rate=0.01;
		emitter->Update(rate);
	}
}

void MagicEmitter::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
	Node::draw(renderer, transform, flags);

	if (emitter)
	{
		_customCommand.init(_globalZOrder);
		_customCommand.func = [this]()
		{
			emitter->Render();
		};
		renderer->addCommand(&_customCommand);
	}
}

void MagicEmitter::setPosition(float x, float y)
{
	if (emitter)
	{
		MP_POSITION pos;
		pos.x=x;
		pos.y=y;
		#ifdef MAGIC_3D
		pos.z=0.f;
		#endif
		emitter->ResetNode();
		emitter->SetPosition(pos);
		emitter->SetNode(this);
	}
	else
		Node::setPosition(x, y);
}

#ifdef MAGIC_3D
void MagicEmitter::setPosition3D(float x, float y, float z)
{
	if (emitter)
	{
		MP_POSITION pos;
		pos.x=x;
		pos.y=y;
		pos.z=z;
		emitter->ResetNode();
		emitter->SetPosition(pos);
		emitter->SetNode(this);
	}
	else
		Node::setPosition3D(cocos2d::Vec3(x, y, z));
}
#endif