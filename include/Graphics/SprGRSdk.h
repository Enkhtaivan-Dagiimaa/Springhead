/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

/**
 *	@file SprGRSdk.h
 *	@brief �O���t�B�b�N�XSDK�̊�{�N���X
 */
#ifndef SPR_GRSDK_H
#define SPR_GRSDK_H

namespace Spr{;

/**	\defgroup gpGraphics	�O���t�B�b�N�XSDK	*/
//@{
struct GRDeviceGLIf;
struct GRRenderIf;
struct GRSceneIf;
struct GRSceneDesc;

///	@brief GRSdkDesc GRSDK�̐ݒ�DDirectX���g����OpenGL���g�����̑I���Ȃǂ������ɓ���\��D
struct GRSdkDesc{
};

/**	@brief	�O���t�B�b�N�XSDK�̊�{�N���X�@ */
struct GRSdkIf: public SdkIf{
	SPR_IFDEF(GRSdk);
	GRRenderIf* CreateRender();
	GRDeviceGLIf* CreateDeviceGL();
	GRSceneIf* CreateScene(const GRSceneDesc& desc);
	GRSceneIf* GetScene(size_t i);
	size_t NScene();
	void MergeScene(GRSceneIf* scene0, GRSceneIf* scene1);
	static GRSdkIf* SPR_CDECL CreateSdk();
	static void SPR_CDECL RegisterSdk();
};
//@}
}
#endif
