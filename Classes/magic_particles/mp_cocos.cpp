//-----------------------------------------------------------------------------
// File: mp_cocos.cpp
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#include "mp_cocos.h"
#include "MagicEmitter.h"

// -------------------------------------------------------------------------------------------

MP_Emitter_Cocos::MP_Emitter_Cocos(HM_EMITTER emitter, MP_Manager* owner) : MP_Emitter(emitter, owner)
{
	node=NULL;
}

MP_Emitter_Cocos::~MP_Emitter_Cocos()
{
	if (node)
	{
		node->ResetEmitter();
	}
}

void MP_Emitter_Cocos::SetNode(MagicEmitter* node)
{
	this->node=node;
}

MagicEmitter* MP_Emitter_Cocos::GetNode()
{
	return node;
}

void MP_Emitter_Cocos::ResetNode()
{
	node=NULL;
}

// eng: Position of emitter
// rus: Позиция эмиттера
void MP_Emitter_Cocos::SetPosition(MP_POSITION& position)
{
	if (node)
	{
		node->ResetEmitter();
		#ifdef MAGIC_3D
		node->cocos2d::Node::setPosition3D(cocos2d::Vec3(position.x, position.y, position.z));
		#else
		node->cocos2d::Node::setPosition(position.x, position.y);
		#endif
		node->SetEmitter(this);
	}
	MP_Emitter::SetPosition(position);
}

// -------------------------------------------------------------------------------------------

MP_Device_Cocos::MP_Device_Cocos(int width, int height) : MP_Device_WRAP(width, height)
{
    #ifdef MAGIC_3D
	camera=NULL;
    #endif

}

MP_Device_Cocos::~MP_Device_Cocos()
{
}

MP_Emitter* MP_Device_Cocos::NewEmitter(HM_EMITTER emitter, MP_Manager* owner)
{
	return new MP_Emitter_Cocos(emitter, owner);
}

#ifdef MAGIC_3D
// eng: Setting of scene
// rus: Настройка сцены
void MP_Device_Cocos::SetScene3d(bool camera_mode)
{
	MP_Device::SetScene3d(camera_mode);
	cocos2d::Size winSize=cocos2d::Director::getInstance()->getWinSizeInPixels();

	if (camera_mode)
	{
		camera=cocos2d::Camera::createPerspective(30, winSize.width/winSize.height, 1, 10000);
	}
	else
		camera=cocos2d::Camera::createOrthographic(winSize.width, winSize.height, 1, 10000);

	camera->setCameraFlag(cocos2d::CameraFlag::USER1);

	MP_POSITION eye(0.f, 300.f, 840.f);
	MP_POSITION at(0.f, 0.f, 0.f);
	MP_POSITION up(0.f, 1.f, 0.f);

	SetCamera(eye, at, up);
}

// eng: Camera position
// rus: Положение камеры
void MP_Device_Cocos::SetCamera(MP_POSITION& eye, MP_POSITION& at, MP_POSITION& up)
{
	MP_Device::SetCamera(eye, at, up);

	camera->setPosition3D(cocos2d::Vec3(eye.x, eye.y, eye.z));
	camera->lookAt(cocos2d::Vec3(at.x, at.y, at.z), cocos2d::Vec3(up.x, up.y, up.z));

	cocos2d::Mat4 mat;
	#ifdef MAGIC_3D
	if (camera_mode)
		mat=camera->getViewProjectionMatrix();
	else
	{
		mat=camera->getProjectionMatrix();
		cocos2d::Mat4 view=camera->getViewMatrix();

		cocos2d::Size winSize=cocos2d::Director::getInstance()->getWinSizeInPixels();
		cocos2d::Mat4 offset;
		cocos2d::Mat4::createTranslation(((float)(winSize.width))/2.f, ((float)(winSize.height))/2.f, 0, &offset);

		mat.multiply(offset);
		mat.multiply(view);
	}
	#else
	mat=camera->getViewProjectionMatrix();
	#endif

	SetProjectionMatrix((MP_MATRIX*)mat.m);
}

#endif