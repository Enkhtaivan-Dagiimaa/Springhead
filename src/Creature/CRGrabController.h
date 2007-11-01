/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRGRABCONTROLLER_H
#define CRGRABCONTROLLER_H

#include <Springhead.h>

#include <Foundation/Object.h>
#include "IfStubCreature.h"

#include "CRController.h"

#include <map>

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �c���R���g���[��
*/
class CRGrabController : public CRController, public CRGrabControllerIfInit, public CRGrabControllerDesc {
private:
	/// ���ʂ̃R���g���[���FReachingController
	CRReachingControllerIf *reachLeft, *reachRight, *reachChest;

	/// �{�f�B
	CRBodyIf* body;

	/// ���̍��́i���낢���ɂȂ�j
	PHSolidIf* soWaist;

	/// ���̍��́i��͂肢�낢���ɂȂ�j
	PHSolidIf* soChest;

	/// ���𒆐S�ɓ��B�\�ȋ���
	float reachableDistance;

	/// ������݂����̍���
	float shoulderHeightFromWaist;

	/// �c���Ώۂ�Solid
	PHSolidIf* targetSolid;

	/// �c���Ώۂ̑傫���i���a�j
	float targetRadius;

	/// �c���������̂�u��(Place)���̖ڕW�ʒu
	Vec3f placePos;

	/// ���݂̐�����
	CRGrabControllerIf::CRGCControlState controlState;

	/// ���݂̕��̂�c�����邽�߂�Spring�̃y�A
	typedef std::pair<PHSpringIf*, PHSpringIf*> SpringPair;
	SpringPair grabSpring;

	/// �c�����s�����߂̍쐬��Spring�̃}�b�v
	//// �c���Ώۂ̍��́��i����A���΂ˁE�E��A���΂ˁj
	typedef std::map< PHSolidIf*, std::pair<PHSpringIf*, PHSpringIf*> > GrabSpringMap;
	GrabSpringMap grabSpringMap;

	///
	int waittime;

public:
	OBJECTDEF(CRGrabController, CRController);
	ACCESS_DESC(CRGrabController);

	CRGrabController(){}
	CRGrabController(const CRGrabControllerDesc& desc, CRCreatureIf* c=NULL) 
		: CRGrabControllerDesc(desc) 
		, CRController((const CRControllerDesc&)desc, c)
	{
	}

	/** @brief ������
	*/
	virtual void Init();

	/** @brief �P�X�e�b�v
	*/
	virtual void Step();

	/** @brief ���̂̏��֎��L�΂��Ă��ށD
	*** @return true: Reach�J�n�����D false: ���̕��̂ւ͎肪�͂��Ȃ��D
	*/
	virtual bool Reach(PHSolidIf* solid, float radius);

	/** @brief �Ώ�Solid��Reach���\���ǂ�����Ԃ��i�����Ƃ��j
	*/
	virtual bool IsReachable(PHSolidIf* solid);

	/** @brief �Ώ�Solid��Reach���\���ǂ�����Ԃ��i�����Ƃ��j
	*** @param safety ���S�W���F1�ȉ��̌W���C������safety�{���Čv�Z�D1.0�̂Ƃ����肬�蓞�B�\
	*/
	virtual bool IsReachable(PHSolidIf* solid, float safety);

	/** @brief Reach�������������ǂ�����Ԃ�
	*/
	virtual bool IsReachComplete();

	/** @brief ���ݕ��̂�����ł���΁C���̕��̂��茳�Ɉ����񂹕ێ�����D
	*** @return true: Uphold�J�n�����D false: ���̂�����ł��Ȃ�(Reach�������܂ށD)
	*/
	virtual bool Uphold();

	/** @brief Uphold���\���ǂ�����Ԃ�
	*/
	virtual bool IsUpholdable();

	/** @brief Uphold�������������ǂ�����Ԃ�
	*/
	virtual bool IsUpholdComplete();

	/** @brief ���ݕ��̂�����ł���΁C���̕��̂����̏ꏊ�ɒu���D
	*** @return true: Place�J�n�����D false: ���̏ꏊ�ւ͎肪�͂��Ȃ��C�܂��͕��̂������ĂȂ��D
	*/
	virtual bool Place(Vec3d pos);

	/** @brief Place���\���ǂ�����Ԃ�
	*/
	virtual bool IsPlaceable(Vec3d pos);

	/** @brief Place���\���ǂ�����Ԃ�
	*** @param safety ���S�W���F1�ȉ��̌W���C������safety�{���Čv�Z�D1.0�̂Ƃ����肬�蓞�B�\
	*/
	virtual bool IsPlaceable(Vec3d pos, float safety);

	/** @brief Place�������������ǂ�����Ԃ��D
	*/
	virtual bool IsPlaceComplete();

	/** @brief ����𒆒f����
	*/
	virtual void Abort();

	/** @brief ���ׂĂ̔c������𒆒f����
	*/
	virtual void AbortAll();

	/** @brief ���݂̓����Ԃ�Ԃ�
	*/
	virtual CRGrabControllerIf::CRGCControlState GetControlState();
};
}
//@}

#endif//CRGRABCONTROLLER_H
