/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_CRCONTROLLERIF_H
#define SPR_CRCONTROLLERIF_H

#include <Foundation/SprObject.h>
#include <Creature/SprCREngine.h>

namespace Spr{;

struct PHSolidIf;

//@{

///	�N���[�`���E�R���g���[���̊�{�N���X
struct CRControllerIf : public CREngineIf{
	SPR_IFDEF(CRController);

	///	��Ԃ����Z�b�g����
	void Reset();

	/// �����Ԃ�
	int GetStatus();
};

struct CRControllerDesc{
	SPR_DESCDEF(CRController);

	/// �R���g���[���̌����\��Enum
	enum CRControllerStatus {
		CS_WAITING = 0,
		CS_WORKING,
		CS_DONE,
		CS_FAILED,
	};

	CRControllerDesc(){
	}
};

///	�����R���g���[��
struct CRGazeControllerIf : public CRControllerIf{
	SPR_IFDEF(CRGazeController);

	///  ���Ώۈʒu���Z�b�g����
	void SetTargetPosition(Vec3d pos);
};

struct CRGazeControllerDesc{
	SPR_DESCDEF(CRGazeController);

	/// ���x�� <!!>

	CRGazeControllerDesc(){
	}
};

///	���B�^���R���g���[��
struct CRReachControllerIf : public CRControllerIf{
	SPR_IFDEF(CRReachController);

	///  ���B�Ώۈʒu���Z�b�g����
	void SetTargetPosition(Vec3d pos);
};

struct CRReachControllerDesc{
	SPR_DESCDEF(CRReachController);

	/// ���x�� <!!>

	CRReachControllerDesc(){
	}
};

///	�c���R���g���[��
struct CRGrabControllerIf : public CRControllerIf{
	SPR_IFDEF(CRGrabController);

	///  �c���Ώۍ��̂��Z�b�g����
	void SetTargetSolid(PHSolidIf* targetSolid);

	/// ����ł��镨�̂�Ԃ��D����ł��Ȃ����NULL
	PHSolidIf* GetGrabbingSolid();

	/// �c���Ɏg�p���鍄�̂�Ԃ�
	PHSolidIf* GetSolid();
};

struct CRGrabControllerDesc{
	SPR_DESCDEF(CRGrabController);

	/// ���x�� <!!>

	CRGrabControllerDesc(){
	}
};

//@}

}

#endif//SPR_CRENGINEIF_H
