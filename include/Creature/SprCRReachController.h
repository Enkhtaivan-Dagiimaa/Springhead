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
/// �c���R���g���[��
struct CRGrabControllerIf : CREngineIf{
	SPR_IFDEF(CRGrabController);

	/** @brief ���̂̏��֎��L�΂��Ă��ށD
	*** @return true: Reach�J�n�����D false: ���̕��̂ւ͎肪�͂��Ȃ��D
	*/
	bool Reach(PHSolidIf* solid, float radius);

	/** @brief �Ώ�Solid��Reach���\���ǂ�����Ԃ��i�����Ƃ��j
	*/
	bool IsReachable(PHSolidIf* solid);

	/** @brief �Ώ�Solid��Reach���\���ǂ�����Ԃ��i�����Ƃ��j
	*** @param safety ���S�W���F1�ȉ��̌W���C������safety�{���Čv�Z�D1.0�̂Ƃ����肬�蓞�B�\
	*/
	bool IsReachable(PHSolidIf* solid, float safety);

	/** @brief Reach�������������ǂ�����Ԃ�
	*/
	bool IsReachComplete();

	/** @brief ���ݕ��̂�����ł���΁C���̕��̂��茳�Ɉ����񂹕ێ�����D
	*** @return true: Uphold�J�n�����D false: ���̂�����ł��Ȃ�(Reach�������܂ށD)
	*/
	bool Uphold();

	/** @brief Uphold���\���ǂ�����Ԃ�
	*/
	bool IsUpholdable();

	/** @brief Uphold�������������ǂ�����Ԃ�
	*/
	bool IsUpholdComplete();

	/** @brief ���ݕ��̂�����ł���΁C���̕��̂����̏ꏊ�ɒu���D
	*** @return true: Place�J�n�����D false: ���̏ꏊ�ւ͎肪�͂��Ȃ��C�܂��͕��̂������ĂȂ��D
	*/
	bool Place(Vec3d pos);

	/** @brief Place���\���ǂ�����Ԃ�
	*/
	bool IsPlaceable(Vec3d pos);

	/** @brief Place���\���ǂ�����Ԃ�
	*** @param safety ���S�W���F1�ȉ��̌W���C������safety�{���Čv�Z�D1.0�̂Ƃ����肬�蓞�B�\
	*/
	bool IsPlaceable(Vec3d pos, float safety);

	/** @brief Place�������������ǂ�����Ԃ�
	*/
	bool IsPlaceComplete();

	/** @brief ���݂̓���𒆒f����
	*/
	void Abort();

	/** @brief ���ׂĂ̔c������𒆒f����
	*/
	void AbortAll();

	/** @brief ���݂̓����Ԃ�Ԃ�
	*/
	enum CRGCControlState {
		CRGC_STANDBY=0,
		CRGC_REACH,  CRGC_REACH_COMPLETE,
		CRGC_UPHOLD, CRGC_UPHOLD_COMPLETE,
		CRGC_PLACE,  CRGC_PLACE_COMPLETE,
	};
	CRGrabControllerIf::CRGCControlState GetControlState();
};

/// �c���R���g���[���̃f�X�N���v�^
struct CRGrabControllerDesc : public CREngineDesc {
	SPR_DESCDEF(CRGrabController);

	/// �̂��_�炩�����邽�߂̃o�l�_���p�ւ̌W��
	double rateSpringSoften, rateDamperSoften;

	/// �̂����������邽�߂̃o�l�_���p�ւ̌W��
	double rateSpringHarden, rateDamperHarden;

	CRGrabControllerDesc(){
		rateSpringSoften = 0.0;  rateDamperSoften = 0.5;
		rateSpringHarden = 1.5;  rateDamperHarden = 1.5;
	}
};

}

#endif // SPR_CR_REACHCONTROLLER_IF_H
