/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRREACHCONTROLLER_H
#define CRREACHCONTROLLER_H

#include <Creature/CREngine.h>
#include <Creature/SprCRController.h>

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief ���B�^���R���g���[��
*/
class CRReachController : public CRController, public CRReachControllerDesc {
private:

public:
	SPR_OBJECTDEF(CRReachController);
	ACCESS_DESC(CRReachController);

	CRReachController(){}
	CRReachController(const CRReachControllerDesc& desc) 
		: CRReachControllerDesc(desc) 
	{
	}

	/// �����������s����
	virtual void Init();

	/// ���䏈�������s����
	virtual void Step();

	///  ��Ԃ����Z�b�g����
	virtual void Reset();

	///  �����Ԃ�
	virtual int  GetStatus();

	///  ���B�Ώۈʒu���Z�b�g����
	virtual void SetTargetPosition(Vec3d pos);
};
}
//@}

#endif//CRREACHCONTROLLER_H
