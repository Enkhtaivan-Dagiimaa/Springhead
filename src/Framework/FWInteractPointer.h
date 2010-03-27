/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#ifndef FWINTERACT_POINTER_H
#define FWINTERACT_POINTER_H

#include <Foundation/Object.h>
#include <Framework/SprFWInteractPointer.h>
#include <Physics/PHSolid.h>
#include <Framework/FWInteractInfo.h>

namespace Spr{;

class FWInteractPointer : public SceneObject, public FWInteractPointerDesc{
public:
	SPR_OBJECTDEF(FWInteractPointer);
	PHSolid			hiSolid;
	AllInteractInfo interactInfo;
	bool			bForce;
	bool			bVibration;
	std::vector<bool>	bContact;
	int			grabFlag;	//�ڐG���Ă��鍄�̂ƃo�[�`�����J�b�v�����O�Œ͂ޏꍇ��Flag
							// 0:�ڐG���Ă��鍄�̂ƃo�[�`�����J�b�v�����O�͂��Ȃ�
							// 1:�ڐG���Ă��鍄�̂ƃo�[�`�����J�b�v�����O�����邽�߂ɃW���C���g(K,D��傫���ݒ肵��)���쐬����
							// 2:K,D��傫���ݒ肵���W���C���g�ɂ��o�[�`�����J�b�v�����O��
							// 3:�ݒ肵��K,D���g�p�����W���C���g�ɂ��o�[�`�����J�b�v�����O��
							// 4:�ڐG���Ă��鍄�̂ɐڑ������W���C���g���N���A����

	Posed  cameraOrigin;	//�J�����ɑ΂��Ċ�ƂȂ�p���C�J�����̈ړ��ɂ��ψʂ��~�ς���
	Posed  cameraPose;		//1�X�e�b�v�O�̃J�����p����ۑ�

	double correctionSpringK;
	double correctionDamperD;

	FWInteractPointer();
	FWInteractPointer(const FWInteractPointerDesc& desc);

	void		Init();
	void		Sync();
	void		SetPointerSolid(PHSolidIf* solid);
	PHSolidIf*	GetPointerSolid();
	void		SetHI(HIBaseIf* hi);
	HIBaseIf*	GetHI();
	void		SetPosScale(double s);
	double		GetPosScale();
	void		SetWorldScale(double s);
	double		GetWorldScale();
	void		SetForceScale(double s);
	double		GetForceScale();
	void		SetTorqueScale(double s);
	double		GetTorqueScale();
	void		SetLocalRange(double r);
	double		GetLocalRange();
	void		SetDefaultPosition(Posed p);
	Posed		GetDefaultPosition();
	void		SetPointersCalibPosition(Posed p);
	Posed		GetPointersCalibPosition();
	void		CalcCorrectionSpringDamper();
	void		EnableForce(bool b = true);
	void		EnableVibration(bool b = true);
	void		Calibration();
	void		SetGrabFlag(int f);
	int			GetGrabFlag();
	void		 SetCameraOri(Quaterniond q);
	Quaterniond  GetCameraOri();
	void		 SetCameraOrigin(Posed p);
	Posed		 GetCameraOrigin();
	void		 SetSpringK(double k);
	double		 GetSpringK();
	void		 SetDamperD(double d);
	double		 GetDamperD();
};

}
#endif