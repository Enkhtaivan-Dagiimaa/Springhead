/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_CR_REACHCONTROLLER_IF_H
#define SPR_CR_REACHCONTROLLER_IF_H

#include <Creature/SprCREngine.h>

namespace Spr{;

// ------------------------------------------------------------------------------

struct PHSolidIf;
struct CRIKSolidIf;

/// ���B�^���R���g���[���̃C���^�[�t�F�[�X
struct CRReachingControllerIf : CREngineIf{
	SPR_IFDEF(CRReachingController);

	/** @brief �ڕW�ʒu�̍Đݒ�
		@param pos �ڕW�ʒu
	*/
	void SetPos(Vec3f pos);

	/** @brief �ʒu�𓞒B������
		@param pos �ڕW�ʒu
		@param v �ڕW���x
		@param t �ڕW���B����
	*/
	void Start(Vec3d pose, Vec3d v, float t);

	/** @brief ���B��Ԃ���������
	*/
	void Stop();

	/** @brief ����Ώۍ��̂̐ݒ�
	*/
	void SetCRSolid(CRIKSolidIf* cso);

	/** @brief IK����_�̎擾
	*/
	CRIKSolidIf* GetCRSolid();
};

/// ���B�^���R���g���[���̃f�X�N���v�^
struct CRReachingControllerDesc : public CREngineDesc{
	SPR_DESCDEF(CRReachingController);
};

/// ���B�^���R���g���[���̏W���̂̃C���^�[�t�F�[�X
struct CRReachingControllersIf : CREngineIf{
	SPR_IFDEF(CRReachingControllers);

	/** @brief ���B�^���R���g���[�����擾����
		@param solid ���B�Ώۂ̍���
	*/
	CRReachingControllerIf* GetReachingController(PHSolidIf* solid);
};

/// ���B�^���R���g���[���̏W���̂̃f�X�N���v�^
struct CRReachingControllersDesc : public CREngineDesc{
	SPR_DESCDEF(CRReachingControllers);
};

// ------------------------------------------------------------------------------
/// �c���R���g���[���i����ŁC���������j
struct CRGrabControllerIf : CREngineIf{
	SPR_IFDEF(CRGrabController);

	/** @brief �w�肵�����̂����ށD
	*/
	void Grab(PHSolidIf* targetSolid);

	/** @brief ����ł��镨�̂�Ԃ��D����ł��Ȃ����NULL
	*/
	PHSolidIf* GetGrabbingSolid();

	/** @brief ����ł��镨�̂�����D
	*/
	void Release();
};

/// �c���R���g���[���̃f�X�N���v�^
struct CRGrabControllerDesc : public CREngineDesc {
	SPR_DESCDEF(CRGrabController);

	CRGrabControllerDesc(){
	}
};

}

#endif // SPR_CR_REACHCONTROLLER_IF_H
