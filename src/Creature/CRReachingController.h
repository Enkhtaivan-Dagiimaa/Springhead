/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRREACHINGCONTROLLER_H
#define CRREACHINGCONTROLLER_H

#include <Springhead.h>

#include <Foundation/Object.h>

#include "CRController.h"

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief ���B�^���R���g���[��
*/
class CRReachingController : public CRController, public CRReachingControllerDesc {
private:
	/// �o�ߎ���
	float time;

	/// ���B�ڕW����
	float period;

	/// �ŏI�ڕW�ʒu�E���x�E�p���E�p���x
	Vec3f fP, fV;

	/// �L�����ǂ���
	bool bActive;

	/// ����Ώۂ�ikcp
	PHIKPosCtlIf* ikcp;

public:
	SPR_OBJECTDEF(CRReachingController);
	ACCESS_DESC(CRReachingController);

	CRReachingController(){}
	CRReachingController(const CRReachingControllerDesc& desc, CRCreatureIf* c=NULL) 
		: CRController((const CRControllerDesc&)desc, c)
		, CRReachingControllerDesc(desc)
	{
		bActive = false;
	}

	/** @ brief �����������s����
	*/
	virtual void Init();

	/** @ brief ����̃X�e�b�v�����s����
	*/
	virtual void Step();

	/** @brief �ʒu�𓞒B������
	*/
	virtual void Reach(PHIKPosCtlIf* ikcp, Vec3d pos, Vec3d v, float t);

	/** @brief �ڕW�ʒu�̍Đݒ�
	*/
	void SetPos(Vec3f pos){ fP = pos; }
};
}
//@}

#endif//CRREACHINGCONTROLLER_H
