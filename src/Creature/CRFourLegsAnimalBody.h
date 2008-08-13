/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRFOURLEGSANIMALBODY_H
#define CRFOURLEGSANIMALBODY_H

#include <Springhead.h>

#include <Foundation/Object.h>

#include <vector>

#include "CRBody.h"

//@{
namespace Spr{;

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// CRFourLegsAnimalBody
// �{�[����q���W�̃W���C���g��p�����M���ރ��f���E�N���X�̎���
class CRFourLegsAnimalBody : public CRBody, public CRFourLegsAnimalBodyDesc {
private:
	
	std::vector<PHSolidIf*> upperBody;		//< ��̂��\�����鍄�̂̓o�^��̔z��
	
	void InitBody();						//< �̊����̍쐬�v��𗧂Ă�
	void CreateWaist();						//< �����쐬����
	void CreateChest();						//< �����쐬����
	void CreateTail();						//< �����쐬����

	void InitHead();						//< �����̍쐬�v��𗧂Ă�
	void CreateNeck();						//< ����쐬����
	void CreateHead();						//< ���̍쐬������

	void InitFrontLegs();					//< �O�r���̍쐬�v��𗧂Ă�
	void CreateBreastBone(LREnum lr);		//< ��r�����쐬����
	void CreateRadius(LREnum lr);			//< �O�r�����쐬����
	void CreateFrontCannonBone(LREnum lr);	//< ���荜���쐬����
	void CreateFrontToeBones(LREnum lr);	//< �w�����쐬����

	void InitRearLegs();					//< ��r���̍쐬�v��𗧂Ă�
	void CreateFemur(LREnum lr);			//< ��ڍ����쐬����
	void CreateTibia(LREnum lr);			//< �������쐬����
	void CreateRearCannonBone(LREnum lr);	//< ���������쐬����
	void CreateRearToeBones(LREnum lr);		//< �䍜���쐬����
	
	void InitEyes();						//< ���o��̍쐬�v��𗧂Ă�i�������j
	void CreateEye(LREnum lr);				//< �ڂ��쐬����i�������j

	void InitContact();						//< �{�f�B�S�Ă̍��̓��m�̐ڐG��؂�

	void SetUpperBody();					//< �{�f�B�̏�̂��\�����鍄�̂�o�^����
	
public:
	SPR_OBJECTDEF(CRFourLegsAnimalBody);
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
		Init();
	}

	// �C���^�t�F�[�X�̎���
	virtual void	Init();						//< �����������s����
	virtual Vec3d	GetUpperCenterOfMass();		//< ��̂̍��̂̏d�S�𓾂�
	virtual int		NSolids();					//< �{�f�B�Ɋ܂܂�Ă��鍄�̂̐���Ԃ�
	virtual int		NJoints();					//< �{�f�B�Ɋ܂܂�Ă���֐߂̐���Ԃ�
	virtual int		NBallJoints();				//< �{�f�B�Ɋ܂܂�Ă���{�[���W���C���g�̐���Ԃ�
	virtual int		NHingeJoints();				//< �{�f�B�Ɋ܂܂�Ă���q���W�W���C���g�̐���Ԃ�
	virtual double	VSolid(int i);				//< i�Ԗڂ̍��̂̑̐ς�Ԃ�
	virtual double	VSolids();					//< ���̂̑̐ϑ��a��Ԃ�
	virtual double	GetTotalMass();				//< �����ʂ�Ԃ�
	virtual void	SetTotalMass(double value);	//< �����ʂ�ݒ肷��
	virtual double  GetLegLength(int i);		//< �r�̒�����Ԃ�[0]:�E�O�C[1]:���O, [2]:�E��, [3]:����
};

}
//@}

#endif//CRFOURLEGSANIMALBODY_H
