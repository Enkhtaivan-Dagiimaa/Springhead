/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRFOURLEGSANIMALBODY_H
#define CRFOURLEGSANIMALBODY_H

#include <Springhead.h>

#include <Foundation/Object.h>
#include "IfStubCreature.h"

#include <vector>

#include "CRBody.h"

//@{
namespace Spr{;

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// CRFourLegsAnimalBody
// �{�[����q���W�̃W���C���g��p�����M���ރ��f���E�N���X�̎���
class CRFourLegsAnimalBody : public CRBody, public CRFourLegsAnimalBodyIfInit, public CRFourLegsAnimalBodyDesc {
private:

	std::vector<PHSolidIf*> upperBody;	//< ��̂��\�����鍄�̂̓o�^��̔z��
	
	/// �̊����̍쐬
	void InitBody();
	void CreateWaist();
	void CreateChest();
	void CreateTail();

	/// �����̍쐬
	void InitHead();
	void CreateNeck();
	void CreateHead();

	/// �O�r���̍쐬
	void InitFrontLegs();
	void CreateBreastBone(LREnum lr);
	void CreateRadius(LREnum lr);
	void CreateFrontCannonBone(LREnum lr);
	void CreateFrontToeBones(LREnum lr);

	/// ��r���̍쐬
	void InitRearLegs();
	void CreateFemur(LREnum lr);
	void CreateTibia(LREnum lr);
	void CreateRearCannonBone(LREnum lr);
	void CreateRearToeBones(LREnum lr);
	
	/// ���o��̍쐬�i�������j
	void InitEyes();
	void CreateEye(LREnum lr);

	/// �{�f�B�S�Ă̍��̓��m�̐ڐG��؂�
	void InitContact();

	/// �{�f�B�̏�̂��\�����鍄�̂�o�^����
	void SetUpperBody();
	
public:
	OBJECTDEF(CRFourLegsAnimalBody, CRBody);
	ACCESS_DESC(CRFourLegsAnimalBody);

	CRFourLegsAnimalBody(){}
	CRFourLegsAnimalBody(const CRFourLegsAnimalBodyDesc& desc, CRCreatureIf* c=NULL) 
		: CRFourLegsAnimalBodyDesc(desc) 
		, CRBody((const CRBodyDesc&)desc, c)
	{
		solids.resize(CRFourLegsAnimalBodyDesc::SO_NSOLIDS);
		joints.resize(CRFourLegsAnimalBodyDesc::JO_NJOINTS);

		InitBody();
		InitHead();
		InitFrontLegs();
		InitRearLegs();
		InitEyes();
		

		InitContact();
	}

	

	/** @brief �����������s����
	*/
	virtual void Init();
	
	/** @brief ��̂̍��̂̏d�S�𓾂�
	*/
	Vec3d GetUpperCenterOfMass();

};

}
//@}

#endif//CRFOURLEGSANIMALBODY_H
