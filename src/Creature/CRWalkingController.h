/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRWALKINGCONTROLLER_H
#define CRWALKINGCONTROLLER_H

#include <Foundation/Object.h>
#include "IfStubCreature.h"

#include <Springhead.h>		//	Springhead�̃C���^�t�F�[�X

#include "CRController.h"
#include ".\crwctimeleft.h"
#include ".\crwcchangearoundcenter.h"
#include ".\crwclandingsite.h"
#include ".\crwcfootforce.h"
#include ".\crwcgeneforce.h"
/////////////////////////////

namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief ���ӃR���g���[��
*/
class CRWalkingController : public CRController, public CRWalkingControllerIfInit, public CRWalkingControllerDesc {
private:
	/// �ˑ����鑼�̃I�u�W�F�N�g�̗�D�K�v�Ȃ��̂�I��ŋL�q�D������CRController�Ɋ��ɂ�����͕̂s�v�D

	std::vector<PHSolidIf*> UpperBody;
	PHSolidIf* tfHead;
	PHSolidIf* tfChest;
	PHSolidIf* tfHip;
	PHSolidIf* tfFootLeft;
	PHSolidIf* tfFootRight;

	PHSolidIf* hiHead;
	PHSolidIf* hiChest;
	PHSolidIf* hiHip;
	PHSolidIf* hiFootLeft;
	PHSolidIf* hiFootRight;

	PHSolidIf* syncFootLeft;
	PHSolidIf* syncFootRight;
	PHSolidIf* syncHip;

	CRHingeHumanBodyIf* hibody;
	CRTrunkFootHumanBodyIf* tfbody;

	double paramVelocityX, paramVelocityZ;              //X, Z�����̖ڕW���s���x 
    double paramHalfStrideX, paramHalfStrideZ;          //X, Z�����̖ڕW���� 
	double footHeight;                                  //�V�r�̍ō��_ 
    double amplitude;                                   //�d�S�̏㉺���̐U�� 

	/////////////���ϐ�//////
	double T0;            //��{���s����
	double Tc;            //��{���s���������߂�̂Ɏg�p����萔
	double TimeStep;
	double et;      //elapsed time �P�r�x�����Ԃ̌o�ߎ���
	double timeleft;      //�P�r�x�����Ԃ̎c�莞��
	double timeDSleft;    //���r�x�����Ԃ̎c�莞��
	double timehalfcycle; //���s�̗��r�x�����Ԃ����˂��P�T�C�N���̂Q���̂P
	double footsize;      //���̃T�C�Y
	double SingleSupportEndX;   //�P�r�x�����Ԃ̏I�����̏d�S�ʒuX
	double SingleSupportEndZ;   //�P�r�x�����Ԃ̏I�����̏d�S�ʒuZ
	double previousPX;          //1�X�e�b�v�O�̏d�S�ʒuX
	double previousPZ;          //1�X�e�b�v�O�̏d�S�ʒuZ
	double DSDX;                //���r�x�����Ԃɏd�S�̐i�ދ���
	double CurrentDirection;    //x,z���ʏ�̌��݂̑̂̕���
	double TargetDirection;   //x,z���ʏ�̌��݂̖̑̂ڕW����
	bool LF;       //���r���V�r�̎� true
	bool RF;      //�E�r���V�r�̎� true
	bool DSterm;  //���r�x�����Ԃ̎� true
	bool EarthConnection; //���݂̎x���r�����n�����𖞂����Ă��鎞 true
	bool LandAble;        //���̒��n�_�֒��n���邱�Ƃ��\�ȏꍇ true
	bool completefall; //���s�̌p�����s�\�ȏꍇ true
	Vec3d currentlandingsite;   //���݂̒��n�_
	Vec3d nextlandingsite;      //���̖ڕW���n�_
	Vec3d CenterVelocity;       //�d�S�ʒu�̑��x
	Vec3d PreviousV;            //1�X�e�b�v�O�̏d�S�ʒu�̑��x
	Vec3d PositionOfWholeBody;  //�d�S�ʒu
	Vec3d PositionOfUpperBody;  //�㔼�g�̈ʒu
	Vec3d change;   //�o�����X�Ɋւ��鐧��� PD���� 
	long totalStep;   //�J�n����̃X�e�b�v��

	int katoki;       //�X�e�b�v��gaitbegin�Ɏ���܂ŋr�𓮂����Ȃ�����̂��߂Ɏg�p.������Ȃ���
	int gaitbegin;   //�X�e�b�v��gaitbegin�Ɏ���܂ŋr�𓮂����Ȃ�����̂��߂Ɏg�p.������Ȃ���
	int forcestep1;   //�f���Ɏg�p���Ă���ϐ�
	int flag;         //�f���p�Ɏg�p
	int forcestep2;   //�f���Ɏg�p���Ă���ϐ�
	int flag2;        //�f���p�Ɏg�p
	int flag3;        //�f���p�Ɏg�p

	double Xs;
	double Zs;

	double nextSpeed;
	bool stop;
	bool reverse;

	CRWCTimeLeft* tl; 
	CRWCChangeAroundCenter* cac;
	CRWCLandingSite* ls;
	CRWCFootForce* ff;
	CRWCGeneForce* gf;

	Vec3d GetCenterOfBlocks(std::vector<PHSolidIf*> objects);
	Vec3d CalcCenterVelocity(std::vector<PHSolidIf*> objs);
	double TotalMass(std::vector<PHSolidIf*> objects);
	double CalcBasicCycle(void);
	double CalcZParamV(double hz, double restT);
	double CalcZParamH(double v, double restT);
	double CalcLocalX(double xb, double zb, double xt, double zt, double theta);
	double CalcLocalZ(double xb, double zb, double xt, double zt, double theta);
	double CalcLocalVX(double vx, double vz, double theta);
	double CalcLocalVZ(double vx, double vz, double theta);
	double square(double a);
	void VelocityXChange(double vx);
	void VelocityZChange(double vz);
	void HalfStrideXChange(double sx);
	void AmplitudeChange(double amp);
	void FootHeightChange(double fh);
	void WCSetSpeed(double v);
	void WCStop(void);
	void WCReverse(void);
	void CalcCurrentDirection(void);
	void CalcTargetAngle(void);
	void ConstraintForce(void);
	void completeFall(void);
	void fallForce(void);
	bool CalcSustainable(void);
	Vec3d CalcChange(void);
	double CalcTimeLeft(void);
	Vec3d CalcNextLandingSite(void);
	double CalcTimeHalfCycle(void);
	void GenerateCenterForce(void);
	void GenerateFootForce(void);
	void func(void);
	void CreateUpperBody(void);
	void AssignFoot(void);
	void AssignCenterObject(void);
	void AssignHip(void);
	void AssignHead(void);
	void SetTimeParams(void);
	void AssignInitialLandingSite(void);
	void CreateCRWCTimeLeft(void);
	void CreateCRWCChangeAroundCenter(void);
	void CreateCRWCLandingSite(void);
	void CreateCRWCFootForce(void);
	void CreateCRWCGeneForce(void);
	void stand(void);
	void gait(void);

	void SuperimposeHingeBody(void);


public:
	OBJECTDEF(CRWalkingController, CRController);
	ACCESS_DESC(CRWalkingController);

	CRWalkingController(){
	}
	CRWalkingController(const CRWalkingControllerDesc& desc, CRCreatureIf* c=NULL) 
		: CRWalkingControllerDesc(desc) 
		, CRController((const CRControllerDesc&)desc, c)
	{
	}

	/** @brief ���������s��
	*/
	virtual void Init();

	/** @brief �ď��������s��
	*/
	virtual void ReInit();

	/** @brief ��������s����
	*/
	virtual void Step();

	/** @brief ���s�̑��x��ݒ肷��
	*/
	virtual void SetSpeed(double speed);

	/** @brief �]��p�x��ݒ肷��
	*/
	virtual void SetRotationAngle(double rot);

	/** @brief ���[���h���W�n�̐i�s�p�x��ݒ肷��
	*/
	virtual void SetRotationWorldCoordinate(double r);

	/** @brief �Î~����
	*/
	virtual void Stop();

	/** @brief �������ɕ���
	*/
	virtual void Reverse();

	/** @brief �ʒu��ݒ肷��i������Ԍ���̂��߁j
	*/
	virtual void SetPos(Vec3f pos);

	/** @brief ��{���s�������擾
	*/
	virtual double GetBasicCycle() { return et/(et+timeleft); }

	/** @brief ���s�̌p�����s�\���H
	*/
	virtual bool IsCompleteFall() { return completefall; }

	/** @brief �������V�����H
	*/
	virtual bool isLeftSwingFoot() { return LF; }

	/** @brief ����
	*/
	virtual double GetHalfStrideX() { return abs(CalcLocalX(currentlandingsite.x, currentlandingsite.z, nextlandingsite.x, currentlandingsite.z, CurrentDirection)/2.0); }
};
}
//@}

#endif//CRWALKINGCONTROLLER_H
