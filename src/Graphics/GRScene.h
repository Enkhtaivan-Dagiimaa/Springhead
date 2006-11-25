/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
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
/**	�V�[���O���t�̃g�b�v�m�[�h�D�����E���_�����D
	�����_���ƃV�[���O���t�̊֌W���[�����߁C
	�����_�����Ⴄ�ƃV�[���O���t�͂��̂܂܎g�p�ł��Ȃ��D
	�V�[���O���t�́C���Ƃ��΃����_����OpenGL�Ȃ�CdisplayList
	�������Ă��邵�CD3D�Ȃ�� ID3DXMesh�������Ă���D
*/
class SPR_DLL GRScene:public Scene, GRSceneIfInit, public GRSceneDesc{
	OBJECT_DEF(GRScene);
protected:
	///	���[�g�m�[�h
	UTRef<GRFrame> world;
	///	�J����
	UTRef<GRCamera> camera;
public:
	///	�R���X�g���N�^
	GRScene(const GRSceneDesc& desc=GRSceneDesc());
	void Init();
	///	�f�X�g���N�^
	~GRScene(){}

	GRSdkIf* GetSdk();

	///	�V�[������ɂ���D
	void Clear();

	virtual GRFrameIf* CreateFrame(const GRFrameDesc& desc);
	virtual GRFrameIf* GetWorld(){return world->GetIf();}
	virtual GRCameraIf* GetCamera(){return camera->GetIf();}
	virtual bool AddChildObject(ObjectIf* o);
	virtual size_t NChildObject() const;
	virtual ObjectIf* GetChildObject(size_t pos);
	virtual void Render(GRRenderIf* r);

	void* GetDescAddress(){ return (GRSceneDesc*)this; }
	friend class GRFrame;
};

}
#endif
