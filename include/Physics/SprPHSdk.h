/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file SprPHSdk.h
 *	@brief �����V�~�����[�V����SDK
*/
#ifndef SPR_PHSDKIF_H
#define SPR_PHSDKIF_H
#include <Collision/SprCDShape.h>

namespace Spr{;

/** \addtogroup gpPhysics */
//@{

struct PHSceneIf;
struct PHSceneDesc;

//	�������[�h�̂��߂̒��g�Ȃ��̃f�X�N���v�^
struct PHSdkDesc{
};

///	�����V�~�����[�V����SDK
struct PHSdkIf : public NameManagerIf{
	IF_DEF(PHSdk);
	///	Scene�̍쐬
	virtual PHSceneIf* CreateScene()=0;
	///	Scene�̍쐬
	virtual PHSceneIf* CreateScene(const PHSceneDesc& desc)=0;
	///	Scene�̐����擾
	virtual int NScene()=0;
	///	Scene�̎擾
	virtual PHSceneIf* GetScene(size_t i)=0;
	
	///	Shape�쐬
	virtual CDShapeIf* CreateShape(const CDShapeDesc& desc)=0;
	///	Shape�̐�
	virtual int NShape()=0;
	///	Shape�̎擾
	virtual CDShapeIf* GetShape(int i)=0;
	//virtual CDShapeIf** GetShapes()=0;
};
PHSdkIf* SPR_CDECL CreatePHSdk();

void PHRegisterSdk();

//@}

}	//	namespace Spr
#endif
