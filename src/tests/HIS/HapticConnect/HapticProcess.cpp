#include "HapticProcess.h"
#include "BoxStack.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <ctime>
#include <string>
#include <GL/glut.h>
#include <vector>
#include <windows.h>
#include <fstream>

bool vhaptic = false;

HapticProcess hprocess;	

HapticProcess::HapticProcess(){
	dt = 0.001f;
	K = 20;
	D = 0.1;
	bDisplayforce = false;
	bInter = true;
	hpointer.SetDynamical(false);
	hpointer.SetFrozen(true);
	stepcount = 1;
	countmax = 100;
};

void HapticProcess::Init(){
	InitDevice();
}

void HapticProcess::InitDevice(){
	// �f�o�C�X�}�l�[�W���̏�����
	devMan.RPool().Register(new DRUsb20Sh4(0));	    // USB2.0�ŃR���g���[�� 8���[�^
	devMan.RPool().Register(new DRUsb20Sh4(1));	    // USB2.0�ŃR���g���[�� 8���[�^
	devMan.Init();														// �f�o�C�X�̏�����
	DSTR << devMan;												// �������̌��ʂ�\��

	// SPIDAR�̏�����
	spidarG6.Init(devMan, false);
	for(int i = 0; i < 8; ++i){
		spidarG6.motor[i].maxForce = 5.0f;
	}
	spidarG6.Calib();
}

void HapticProcess::Step(){
	UpdateSpidar();
//	FindNearestPoint();
	HapticRendering();
	LocalDynamics();
}

void HapticProcess::UpdateSpidar(){
	const float posScale = 200;
	spidarG6.Update(dt);
	hpointer.SetFramePosition(spidarG6.GetPos() * posScale);
//	hpointer.SetOrientation(spidarG6.GetOri());
	hpointer.SetVelocity(spidarG6.GetVel() * posScale);
//	hpointer.SetAngularVelocity(spidarG6.GetAngVel());
}

//void HapticProcess::FindNearestPoint(){
//	for(unsigned i = 0; i < neighborObjects.size(); i++){
//		if(!neighborObjects[i].blocal) continue;																									// Local�ɍ��̂������ĂȂ��������߂�
//
//		CDConvex* a = DCAST(CDConvex, neighborObjects[i].phSolidIf->GetShape(0));											// ���̂����ʌ`��
//		CDConvex* b = DCAST(CDConvex, hpointer.GetShape(0));																		// �͊o�|�C���^�̓ʌ`��
//		Posed a2w, b2w;																																	// ���̂̃��[���h���W
///*		if(neighborObjects[i].blocal) 	a2w = neighborObjects[i].phSolid.GetPose();												// blocal��true�Ȃ�ŐV�̏��ł��
//		else */								a2w = neighborObjects[i].phSolidIf->GetPose();
//		b2w = hpointer.GetPose();																													// �͊o�|�C���^�̃��[���h���W
//		Vec3d pa ,pb;																																		// pa:���̂̋ߖT�_�Cpb:�͊o�|�C���^�̋ߖT�_�i���[�J�����W�j
//		pa = pb = Vec3d(0.0, 0.0, 0.0);
//		FindClosestPoints(a, b, a2w, b2w, pa, pb);																								// GJK�ŋߖT�_�̎Z�o
//		Vec3d wa = a2w * pa;																															// ���̋ߖT�_�̃��[���h���W
//		Vec3d wb = b2w * pb;																															// �͊o�|�C���^�ߖT�_�̃��[���h���W
//		Vec3d a2b = wb - wa;																															// ���̂���͊o�|�C���^�ւ̃x�N�g��
//		Vec3d normal = a2b.unit();
//		// �͊o�|�C���^�ƍ��̂����łɐڐG���Ă�����CCDGJK�Ŗ@�������߂�
//		if(a2b.norm() < 0.01){																																
//			pa = pb = Vec3d(0.0, 0.0, 0.0);
//			Vec3d dir = -neighborObjects[i].face_normal;
//			if(dir == Vec3f(0.0, 0.0, 0.0) ) dir = -(hpointer.GetCenterPosition() - wa);
//			double dist = 0.0;
//			int cp = ContFindCommonPoint(a, b, a2w, b2w, dir, -DBL_MAX, 1, normal, pa, pb, dist);
//			if(cp != 1){
//				ContFindCommonPointSaveParam(a, b, a2w, b2w, dir, -DBL_MAX, 1, normal, pa, pb, dist);
//				DSTR << "contfindcommonpoint don not find contact point" << endl;
//			}
//		}
//		neighborObjects[i].closestPoint = pa;			// ���̋ߖT�_�̃��[�J�����W
//		neighborObjects[i].pointerPoint = pb;			// �͊o�|�C���^�ߖT�_�̃��[�J�����W
//		neighborObjects[i].last_face_normal = neighborObjects[i].face_normal;
//		neighborObjects[i].face_normal = normal;		// ���̂���͊o�|�C���^�ւ̖@��
//	}
//}

void HapticProcess::HapticRendering(){
	double vibA = -200;
	double vibB = 120;
	double vibW = 300;
	static double vibT = 0;
	static bool vibFlag = false;
	Vec3d vibV = spidarG6.GetVel();
	static Vec3d vibVo = vibV;
	double vibforce = 0;

	displayforce = Vec3d(0.0, 0.0, 0.0);								
	bool noContact = true;
	for(unsigned i = 0; i < neighborObjects.size(); i++){
		if(!neighborObjects[i].blocal) continue;
		Vec3d cPoint = neighborObjects[i].phSolid.GetPose() * neighborObjects[i].closestPoint;			// ���̂̋ߖT�_�̃��[���h���W�n
		Vec3d pPoint = hpointer.GetPose() * neighborObjects[i].pointerPoint;									// �͊o�|�C���^�̋ߖT�_�̃��[���h���W�n
		Vec3d force_dir = pPoint - cPoint;
		Vec3d interpolation_normal;																								// �񎦗͌v�Z�ɂ��悤����@���i�O��̖@���Ƃ̊Ԃ��Ԃ���j
		if(bInter){
			// ���̖̂ʂ̖@�����
			// �O��̖@���ƌ��݂̖@���̊Ԃ��Ԃ��Ȃ���X�V
			interpolation_normal = (stepcount * neighborObjects[i].face_normal + (50 - stepcount) * neighborObjects[i].last_face_normal) / 50;															
			if(stepcount > 50)	interpolation_normal = neighborObjects[i].face_normal;				
		}else{
			// ���݂̖@�����g��
			interpolation_normal = neighborObjects[i].face_normal;
		}

		float	f = force_dir * interpolation_normal;								// ���̖̂ʂ̖@���Ɠ��ς��Ƃ�
		if(f < 0.0){																			// ���ς����Ȃ�͂��v�Z
			Vec3d ortho = f * interpolation_normal;								// �ߖT�_����͊o�|�C���^�ւ̃x�N�g���̖ʂ̖@���ւ̐��ˉe
			Vec3d dv = neighborObjects[i].phSolid.GetPointVelocity(cPoint) - hpointer.GetPointVelocity(pPoint);
			Vec3d dvortho = dv.norm() * interpolation_normal;
			Vec3d addforce = -K * ortho + D * dvortho;// * ortho.norm();						// �񎦗͌v�Z (*�_���p�̍���ortho�̃m�����������Ă݂�)

			if(!vibFlag){
				vibT = 0;
				vibVo = vibV;
			}
			vibFlag = true;
			if(vhaptic){
				vibforce = vibA * (vibVo * addforce.unit()) * exp(-vibB * vibT) * sin(2 * M_PI * vibW * vibT);		//�U���v�Z
			}
			displayforce += addforce + (vibforce * addforce.unit());																			 
			neighborObjects[i].phSolid.AddForce(-addforce, cPoint);			// �v�Z�����͂����̂ɉ�����
			neighborObjects[i].test_force_norm = addforce.norm();
			noContact = false;
		}
	}
	if (noContact) vibFlag = false;

	vibT += dt;
	if(bDisplayforce) spidarG6.SetForce(displayforce);													// �͊o��
//	DSTR  << displayforce.norm() << "::" << displayforce << endl;
}

void HapticProcess::LocalDynamics(){
	for(unsigned i = 0; i < neighborObjects.size(); i++){
		if(!neighborObjects[i].blocal) continue;
		SpatialVector vel;																				// ���̂̑��x�i���[���h���W�n�j
		vel.v() = neighborObjects[i].phSolid.GetVelocity();
		vel.w() = neighborObjects[i].phSolid.GetAngularVelocity();
		if(stepcount == 1) vel += (neighborObjects[i].curb - neighborObjects[i].lastb) *  bstack.dt;		// �Փ˂̉e���𔽉f
		vel += (neighborObjects[i].A * neighborObjects[i].phSolid.nextForce + neighborObjects[i].b) * dt;	// �͊o�|�C���^����̗͂ɂ�鑬�x�ω�
		neighborObjects[i].phSolid.SetVelocity(vel.v());																		
		neighborObjects[i].phSolid.SetAngularVelocity(vel.w());
		neighborObjects[i].phSolid.SetCenterPosition(neighborObjects[i].phSolid.GetCenterPosition() + vel.v() * dt);
		neighborObjects[i].phSolid.SetOrientation(( Quaterniond::Rot(vel.w() * dt) * neighborObjects[i].phSolid.GetOrientation()).unit());
 		neighborObjects[i].phSolid.SetUpdated(true);
		neighborObjects[i].phSolid.Step();
	}
}

void HapticProcess::Keyboard(unsigned char key){
	switch(key){
		case 'f':
			if(bDisplayforce){
				bDisplayforce = false;
				spidarG6.SetForce(Vec3d(0, 0, 0));
				DSTR << "Force OFF" << endl;
			}else{
				bDisplayforce = true;
				spidarG6.SetForce(Vec3d(0, 0, 0));
				DSTR << "Force ON" << endl;
			}
			break;
		case 'o':
			if(vhaptic){
				vhaptic = false;
				DSTR << "Vibration Disconnect" << endl;
			}else{
				vhaptic = true;
				DSTR << "Vibration Connect" << endl;
			}
			break;
		case 'c':
			spidarG6.SetForce(Vec3d(0, 0, 0));
			spidarG6.Calib();
			DSTR << "Calibration" << endl;
			break;
		case 'i':
			if(bInter){
				bInter = false;
				DSTR << "Use Current Face_Normal" << endl;
			}else{
				bInter = true;
				DSTR << "Use Interpolate Face_Normal" << endl;
			}
			break;
		default:
			break;
	}
}