/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef GRSCENE_H
#define GRSCENE_H
#include <Graphics/SprGRScene.h>
#include <Foundation/Scene.h>
#include <Graphics/GRFrame.h>
#include <Graphics/GRRender.h>

namespace Spr {;

class GRCamera;
class GRAnimationController;
/**	�V�[���O���t�̃g�b�v�m�[�h�D�����E���_�����D
	�����_���ƃV�[���O���t�̊֌W���[�����߁C
	�����_�����Ⴄ�ƃV�[���O���t�͂��̂܂܎g�p�ł��Ȃ��D
	�V�[���O���t�́C���Ƃ��΃����_����OpenGL�Ȃ�CdisplayList
	�������Ă��邵�CD3D�Ȃ�� ID3DXMesh�������Ă���D
*/
class SPR_DLL GRScene:public Scene, public GRSceneIfInit, public GRSceneDesc{
	OBJECTDEF(GRScene, Scene);
protected:
	///	���[�g�m�[�h
	UTRef<GRFrame> world;
	///	�J����
	UTRef<GRCamera> camera;
	///	�A�j���[�V�����R���g���[��
	UTRef<GRAnimationController> animationController;
public:
	///	�R���X�g���N�^
	GRScene(const GRSceneDesc& desc=GRSceneDesc());
	void Init();
	///	�f�X�g���N�^
	~GRScene(){}

	GRSdkIf* GetSdk();

	///	�V�[������ɂ���D
	void Clear();

	virtual GRVisualIf* CreateVisual(const IfInfo* info, 
		const GRVisualDesc& desc, GRFrameIf* parent = NULL);
	virtual GRFrameIf* GetWorld(){return world->Cast();}
	virtual void		SetCamera(const GRCameraDesc& desc);
	virtual GRCameraIf* GetCamera(){return camera->Cast();}
	virtual GRAnimationControllerIf* GetAnimationController(){return animationController->Cast();}
	virtual bool AddChildObject(ObjectIf* o);
	virtual size_t NChildObject() const;
	virtual ObjectIf* GetChildObject(size_t pos);
	virtual void Render(GRRenderIf* r);

	void* GetDescAddress(){ return (GRSceneDesc*)this; }
	friend class GRFrame;
};

}
#endif
