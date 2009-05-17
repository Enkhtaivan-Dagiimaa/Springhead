/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 * 
 *�@SwingDir�̐����͓����Ă��܂���B�K�v�Ȑl���撣���Ď������Ă�������(by toki 2007.12.05)
 *
 */
#include "Physics.h"
#include <fstream>
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

using namespace PTM;
using namespace std;

namespace Spr{;
//----------------------------------------------------------------------------
// PHBallJointDesc

PHBallJointDesc::PHBallJointDesc(){

	spring			= 0.0;
	damper			= 0.0;
	limitSwing[0]	= FLT_MAX;
	limitSwing[1]	= FLT_MAX;
	limitTwist[0]	= FLT_MAX;
	limitTwist[1]	= FLT_MAX;	
	limitDir		= Vec3d(0.0, 0.0, 1.0);
	goal			= Quaterniond(1, 0, 0, 0);
	fMax			= FLT_MAX;
	fMin			= -FLT_MAX;
	PoleTwist		= Vec2d(FLT_MAX,FLT_MAX);
	offset			= Vec3d();
}

//----------------------------------------------------------------------------
// PHBallJoint
PHBallJoint::PHBallJoint(const PHBallJointDesc& desc){
	SetDesc(&desc);
	axisIndex[0] = 3;
	axisIndex[1] = 4;
	axisIndex[2] = 5;
	
	// limit��ɋ��邩�ǂ����̃t���O�̏�����
	for(int i=0; i<3; ++i){
		onLimit[i].onLower = false;
		onLimit[i].onUpper = false;
	}
	anyLimit = false;
	fMaxDt			= FLT_MAX;
	fMinDt			= -FLT_MAX;
	limitCount[0]	= 0;
	limitCount[1]	= 0;

}

void PHBallJoint::AfterSetDesc(){
	if (GetScene()){
		fMinDt = fMin * GetScene()->GetTimeStep();
		fMaxDt = fMax * GetScene()->GetTimeStep();
	}
	PHJointND<3>::AfterSetDesc();
	limitDir /= limitDir.norm();
	if(limitDir != Vec3d(0.0,0.0,1.0))
		limDir.Ex() = cross(Vec3d(0.0,0.0,1.0),limitDir);
	else limDir.Ex() = Vec3d(1.0,0.0,0.0);
	limDir.Ey() = cross(limitDir,limDir.Ex());
	limDir.Ez() = limitDir;
	BefPos   = Vec3d(0.0,0.0,0.0);

}

void PHBallJoint::UpdateJointState(){
	// Jc.Ez() : Socket�ɑ΂���Plug�̌����Ă������(��currentVector)
	Jc.Ez() = Xjrel.q * Vec3f(0.0, 0.0, 1.0);
	if((onLimit[0].onLower || onLimit[0].onUpper) && (Jc.Ez() != limitDir)){
		Jc.Ex() = cross(Jc.Ez(),(Jc.Ez() - limitDir)).unit();
		Jc.Ey() = cross(Jc.Ez(), Jc.Ex());
		Jcinv   = Jc.trans();
	}
}

void PHBallJoint::SetConstrainedIndex(bool* con){

	// -----bool* con�̈Ӗ�----------------------------------
	// �S�����鎲�ɂ��Ẵt���O
	// con[0]-con[2]:���i�^���ix,y,z�j
	// con[3]-con[5]:��]�^���ix���܂��,y���܂��,z���܂��j
	// true:�S������Afalse:�S�����Ȃ�
	//-------------------------------------------------------

	con[0] = con[1] = con[2] = true;				
	con[3] = con[4] = con[5] = false;
	
	// ����̃`�F�b�N
	// ���݂�Socket��Plug�Ƃ̊Ԃ̊p�x���v�Z
	nowTheta[0]	= acos(dot(limitDir, Jc.Ez()));			///< Swing�p�̌v�Z	

	Vec3d PolarCoord;
	PolarCoord = Jc.Ez() * limDir.trans();					///< �|�������̌v�Z�J�n
	if(PolarCoord.x == 0){
		if(PolarCoord.y >= 0) nowTheta[2] = M_PI / 2;
		else nowTheta[2] = 3 * M_PI / 2;
	}
	else {
		nowTheta[2] = atan(PolarCoord.y / PolarCoord.x);
	}
	if (PolarCoord.x < 0) nowTheta[2] += M_PI;								///< 0 <= nowTheta[2] <= 2�΂͈̔͂ɕύX
	else if(PolarCoord.x > 0 && PolarCoord.y < 0) nowTheta[2] += 2 * M_PI;

	Quaterniond qSwing;
	Vec3d half =  0.5 * (Vec3d(0.0, 0.0, 1.0) + Jc.Ez());	
	double l = half.norm();
	if (l>1e-20){
		half /= l;
		qSwing.V() = cross(half, Vec3d(0,0,1));
		qSwing.w = sqrt(1-qSwing.V().square());
	}
	else{
		qSwing.V() = Vec3d(1,0,0);
		qSwing.w = 0;
	}

	Quaterniond twistQ = qSwing * Xjrel.q;
	nowTheta[1] = twistQ.Theta();						///< Twist�p�̌v�Z���s���Ă���	
	if (twistQ.z < 0)
		nowTheta[1]  *= -1;								///< Twist��]�������΂��������Ƃ�����̂ł��̑΍�
	
	if(nowTheta[1] < Rad(-180)) nowTheta[1] += Rad(360);
	if(nowTheta[1] > Rad( 180)) nowTheta[1] -= Rad(360);
	
	// ���搧���ɂ������Ă��邩�̔���

	//swing�p�̉��搧���̊m�F
	if (limitCount[1] != 0){
		FunNum = 1;
		while(LimitLine.SwingUp[FunNum][0] < nowTheta[2]){				//���݂̎p�����S���Ȑ��̂ǂ̋�Ԃɂ���̂����ׂ�B
			FunNum++;
			if(limitCount[1] < FunNum) break;
		}
		double Herx,Swing;
		// �X�C���O�����̍S�����W�̐����l�����߂�
		Herx = (nowTheta[2] - LimitLine.SwingUp[FunNum-1][0]) / (LimitLine.SwingUp[FunNum][0] - LimitLine.SwingUp[FunNum-1][0]);				//�X�v���C���Ȑ�����S�����W�����߂�
		limitSwing[1] = (2 * LimitLine.SwingUp[FunNum-1][1] - 2 * LimitLine.SwingUp[FunNum][1] + LimitLine.SwingUp[FunNum][4] + LimitLine.SwingUp[FunNum-1][4]) * Herx * Herx * Herx
					+ (3 * LimitLine.SwingUp[FunNum][1] - 3 * LimitLine.SwingUp[FunNum-1][1] - 2 * LimitLine.SwingUp[FunNum-1][4] - LimitLine.SwingUp[FunNum][4]) * Herx * Herx
					+ LimitLine.SwingUp[FunNum-1][4] * Herx + LimitLine.SwingUp[FunNum-1][1];
		if(nowTheta[0] >= limitSwing[1]){
			onLimit[0].onUpper = true;					//������O��Ă���ꍇ�ɂ͍S�����s��

			double SerchStep=0.001;		//�ߖT�_�����߂邽�߂̌J��Ԃ��v�Z�̃X�e�b�v��
			int k = 30;					//�ߖT�_�����߂邽�߂̌J��Ԃ��v�Z�̃X�e�b�v��
			double IrruptUp;			//�ߖT�_�܂ł̍ŒZ����
			double minDif;			//�ߖT�_�̃X�C���O���ʊp

			SerchStep = (nowTheta[2] - BefPos[2]) / 25;
			if(SerchStep > 0.005) SerchStep = 0.005;
			else if (SerchStep < -0.005) SerchStep = -0.005;
			int FunStack;				//�S���ɗ��p����֐��̔ԍ����擾���Ă���

			Vec3d Neighbor;				//�ߖT�_
			FunStack = FunNum;
			minDif = nowTheta[2];		//���i�K�ł̋ߖT�_���̃X�C���O���ʊp�̕ێ�
			nowTheta[2] = BefPos[2];
			Neighbor.x = sin(limitSwing[1]) * cos(nowTheta[2]);			//�\�P�b�g���W�n�ł̋ߖT�_�̍��W
			Neighbor.y = sin(limitSwing[1]) * sin(nowTheta[2]);
			Neighbor.z = cos(limitSwing[1]);
			Irrupt = (cross(Neighbor,Jc.Ez())).norm();				//���݂̒l�ƋߖT�_�̊p�x�̍��@sin(Rad)

			// �X�e�b�v���Ƃɕω������Ă���
			for(int i = 0;i < k;i++){
				nowTheta[2] += SerchStep;
				if(nowTheta[2] >= Rad(360)){		//�X�C���O���ʊp�͈̔͂̐��� 0<nowTheta[2]<360
					nowTheta[2] = 0;
					FunNum = 1;
				}
				else if(nowTheta[2] < 0){
					nowTheta[2] = Rad(360);
					FunNum = limitCount[1] - 1;
				}

				while(nowTheta[2] > LimitLine.SwingUp[FunNum][0]){ //�S���̋Ȑ��̊֐��̋�Ԃ𒴂��Ă����玟�̋�Ԃ�
					FunNum++;
				}
				while(nowTheta[2] < LimitLine.SwingUp[FunNum-1][0]){
					FunNum--;
				}
				Herx = (nowTheta[2] - LimitLine.SwingUp[FunNum-1][0]) / (LimitLine.SwingUp[FunNum][0] - LimitLine.SwingUp[FunNum-1][0]);	//�͈͂�0-1�ɂ����Ƃ���x���W
				Swing = (2 * LimitLine.SwingUp[FunNum-1][1] - 2 * LimitLine.SwingUp[FunNum][1] + LimitLine.SwingUp[FunNum][4] + LimitLine.SwingUp[FunNum-1][4]) * Herx * Herx * Herx
							+ (3 * LimitLine.SwingUp[FunNum][1] - 3 * LimitLine.SwingUp[FunNum-1][1] - 2 * LimitLine.SwingUp[FunNum-1][4] - LimitLine.SwingUp[FunNum][4]) * Herx * Herx
							+ LimitLine.SwingUp[FunNum-1][4] * Herx + LimitLine.SwingUp[FunNum-1][1];	//�v�Z�ʒu�ł�Swing�p�̐���
				Neighbor.x = sin(Swing) * cos(nowTheta[2]);		//�ߖT�_�̍��W�i���S����̃x�N�g���j���v�Z
				Neighbor.y = sin(Swing) * sin(nowTheta[2]);
				Neighbor.z = cos(Swing);
				IrruptUp = (cross(Neighbor,Jc.Ez())).norm();		//���݂̒l�Ƃ̋�����cos�����߂�
				if(Irrupt > IrruptUp){								//�O�̈ʒu���߂��ߖT�_�𓾂�ꂽ���������
					Irrupt = IrruptUp;
					minDif = nowTheta[2];
					FunStack = FunNum;
				}
			}
			nowTheta[2] = minDif;			//�ߖT�_�̃X�C���O���ʊp�ł̍S���ʒu�ւ̃x�N�g�����v�Z����
			FunNum = FunStack;
			Herx = (nowTheta[2] - LimitLine.SwingUp[FunNum-1][0]) / (LimitLine.SwingUp[FunNum][0] - LimitLine.SwingUp[FunNum-1][0]);
			if(Herx > 1 || Herx < 0){
				FunNum = 1;
				while(LimitLine.SwingUp[FunNum][0] < nowTheta[2]){
					FunNum++;
					if(limitCount[1] < FunNum) break;
				}
				Herx = (nowTheta[2] - LimitLine.SwingUp[FunNum-1][0]) / (LimitLine.SwingUp[FunNum][0] - LimitLine.SwingUp[FunNum-1][0]);
			}
			limitSwing[1] = (2 * LimitLine.SwingUp[FunNum-1][1] - 2 * LimitLine.SwingUp[FunNum][1] + LimitLine.SwingUp[FunNum][4] + LimitLine.SwingUp[FunNum-1][4]) * Herx * Herx * Herx
						+ (3 * LimitLine.SwingUp[FunNum][1] - 3 * LimitLine.SwingUp[FunNum-1][1] - 2 * LimitLine.SwingUp[FunNum-1][4] - LimitLine.SwingUp[FunNum][4]) * Herx * Herx
						+ LimitLine.SwingUp[FunNum-1][4] * Herx + LimitLine.SwingUp[FunNum-1][1];
			Neighbor.x = sin(limitSwing[1]) * cos(nowTheta[2]);		//�ߖT�_�̍��W�i���S����̃x�N�g���j���v�Z
			Neighbor.y = sin(limitSwing[1]) * sin(nowTheta[2]);
			Neighbor.z = cos(limitSwing[1]);
			Irrupt = (cross(Neighbor,Jc.Ez())).norm();				//�ߖT�_�܂ł̍ŒZ�ƂȂ�p�x�����߂�
			Irrupt = asin(Irrupt);
			//�����܂�
			//�ڐ����������߂�
			tanLine.x = 3 * (2 * LimitLine.SwingUp[FunNum-1][1] - 2 * LimitLine.SwingUp[FunNum][1] + LimitLine.SwingUp[FunNum][4] + LimitLine.SwingUp[FunNum-1][4]) * Herx * Herx
							+ 2 * (3 * LimitLine.SwingUp[FunNum][1] - 3 * LimitLine.SwingUp[FunNum-1][1] - 2 * LimitLine.SwingUp[FunNum-1][4] - LimitLine.SwingUp[FunNum][4]) * Herx
							+ LimitLine.SwingUp[FunNum-1][4]; //��'�̌v�Z
			tanLine.z = -tanLine.x * sin(limitSwing[1]);
			tanLine.x *= cos(limitSwing[1]);
			tanLine.y = tanLine.x * sin(nowTheta[2]) + Neighbor.x;
			tanLine.x = tanLine.x * cos(nowTheta[2]) - Neighbor.y;
			tanLine = tanLine.unit();

			// ���W�ϊ��������ɍs��
			Jc.Ez() = Xjrel.q * Vec3f(0.0, 0.0, 1.0);
			Jc.Ex() = tanLine;
			Jc.Ey() = cross(Jc.Ez(), Jc.Ex());
			Jc.Ex() = cross(Jc.Ey(), Jc.Ez());
			Jcinv   = Jc.trans();
		}
		else{
		onLimit[0].onUpper = false;
		}
	}
	else{ onLimit[0].onUpper = false;
	}
//Swing�p�̍ŏ��l�����߂�
	if (limitCount[0] != 0 && !onLimit[0].onUpper){
		FunNum = 1;
		while(LimitLine.SwingLow[FunNum][0] < nowTheta[2]){
			FunNum++;
			if(limitCount[0] < FunNum) break;
		}
		double Herx,Swing;
		// �X�C���O�����̍S�����W�̐����l�����߂�
		Herx = (nowTheta[2] - LimitLine.SwingLow[FunNum-1][0]) / (LimitLine.SwingLow[FunNum][0] - LimitLine.SwingLow[FunNum-1][0]);
		limitSwing[0] = (2 * LimitLine.SwingLow[FunNum-1][1] - 2 * LimitLine.SwingLow[FunNum][1] + LimitLine.SwingLow[FunNum][4] + LimitLine.SwingLow[FunNum-1][4]) * Herx * Herx * Herx
					+ (3 * LimitLine.SwingLow[FunNum][1] - 3 * LimitLine.SwingLow[FunNum-1][1] - 2 * LimitLine.SwingLow[FunNum-1][4] - LimitLine.SwingLow[FunNum][4]) * Herx * Herx
					+ LimitLine.SwingLow[FunNum-1][4] * Herx + LimitLine.SwingLow[FunNum-1][1];
		if(nowTheta[0] <= limitSwing[0]){

			onLimit[0].onLower = true;
			double SerchStep = 0.001;
			int k = 30;
			SerchStep = (nowTheta[2] - BefPos[2]) / 25;
			if(SerchStep > 0.005) SerchStep = 0.005;
			else if (SerchStep < -0.005) SerchStep = -0.005;
			double IrruptUp;
			double minDif;
			int FunStack;
			Vec3d Neighbor;

			//�ߖT�_�����߂�
			FunNum = 1;
			while(LimitLine.SwingLow[FunNum][0] < nowTheta[2]){
				FunNum++;
				if(limitCount[0] < FunNum + 1) break;
			}
			FunStack = FunNum;
			minDif = nowTheta[2];
			nowTheta[2] = BefPos[2];
			Herx = (nowTheta[2] - LimitLine.SwingLow[FunNum-1][0]) / (LimitLine.SwingLow[FunNum][0] - LimitLine.SwingLow[FunNum-1][0]);
			limitSwing[0] = (2 * LimitLine.SwingLow[FunNum-1][1] - 2 * LimitLine.SwingLow[FunNum][1] + LimitLine.SwingLow[FunNum][4] + LimitLine.SwingLow[FunNum-1][4]) * Herx * Herx * Herx
						+ (3 * LimitLine.SwingLow[FunNum][1] - 3 * LimitLine.SwingLow[FunNum-1][1] - 2 * LimitLine.SwingLow[FunNum-1][4] - LimitLine.SwingLow[FunNum][4]) * Herx * Herx
						+ LimitLine.SwingLow[FunNum-1][4] * Herx + LimitLine.SwingLow[FunNum-1][1];
			Neighbor.x = sin(limitSwing[0]) * cos(nowTheta[2]);			//�\�P�b�g���W�n�ł̋ߖT�_�̍��W
			Neighbor.y = sin(limitSwing[0]) * sin(nowTheta[2]);
			Neighbor.z = cos(limitSwing[0]);
			Irrupt = (cross(Neighbor,Jc.Ez())).norm();				//���݂̒l�ƋߖT�_�̕ψڊp�x�@sin(Rad)
			// �X�e�b�v���Ƃɕω������Ă���
			for(int i = 0;i < k;i++){
				nowTheta[2] += SerchStep;
				if(nowTheta[2] >= Rad(360)){		//x�͈̔͂̐���
					nowTheta[2] = 0;
					FunNum = 1;
				}
				else if(nowTheta[2] < 0){
					nowTheta[2] = Rad(360);
					FunNum = limitCount[0] - 1;
				}
				while(nowTheta[2] > LimitLine.SwingLow[FunNum][0]){
					FunNum++;	//��Ԃ𒴂��Ă����玟�̋�Ԃ�
				}
				while(nowTheta[2] < LimitLine.SwingLow[FunNum-1][0]){
					FunNum--;
				}
				Herx = (nowTheta[2] - LimitLine.SwingLow[FunNum-1][0]) / (LimitLine.SwingLow[FunNum][0] - LimitLine.SwingLow[FunNum-1][0]);	//�͈͂�0-1�ɂ����Ƃ���x���W
				Swing = (2 * LimitLine.SwingLow[FunNum-1][1] - 2 * LimitLine.SwingLow[FunNum][1] + LimitLine.SwingLow[FunNum][4] + LimitLine.SwingLow[FunNum-1][4]) * Herx * Herx * Herx
							+ (3 * LimitLine.SwingLow[FunNum][1] - 3 * LimitLine.SwingLow[FunNum-1][1] - 2 * LimitLine.SwingLow[FunNum-1][4] - LimitLine.SwingLow[FunNum][4]) * Herx * Herx
							+ LimitLine.SwingLow[FunNum-1][4] * Herx + LimitLine.SwingLow[FunNum-1][1];	//�v�Z�ʒu�ł�Swing�p�̐���
				Neighbor.x = sin(Swing) * cos(nowTheta[2]);		//�ߖT�_�̍��W�i���S����̃x�N�g���j���v�Z
				Neighbor.y = sin(Swing) * sin(nowTheta[2]);
				Neighbor.z = cos(Swing);
				IrruptUp = (cross(Neighbor,Jc.Ez())).norm();		//���݂̒l�Ƃ̋�����sin�����߂�
				if(Irrupt > IrruptUp){
					Irrupt = IrruptUp;
					minDif = nowTheta[2];
					FunStack = FunNum;
				}
			}
			nowTheta[2] = minDif;			//�ߖT�_�̃X�C���O���ʊp�ł̍S���ʒu�ւ̃x�N�g�����v�Z����
			FunNum = FunStack;
			Herx = (nowTheta[2] - LimitLine.SwingLow[FunNum-1][0]) / (LimitLine.SwingLow[FunNum][0] - LimitLine.SwingLow[FunNum-1][0]);
			if(Herx > 1 || Herx < 0){
				FunNum = 1;
				while(LimitLine.SwingLow[FunNum][0] < nowTheta[2]){
					FunNum++;
					if(limitCount[1] < FunNum) break;
				}
				Herx = (nowTheta[2] - LimitLine.SwingLow[FunNum-1][0]) / (LimitLine.SwingLow[FunNum][0] - LimitLine.SwingLow[FunNum-1][0]);
			}
			limitSwing[1] = (2 * LimitLine.SwingLow[FunNum-1][1] - 2 * LimitLine.SwingLow[FunNum][1] + LimitLine.SwingLow[FunNum][4] + LimitLine.SwingLow[FunNum-1][4]) * Herx * Herx * Herx
						+ (3 * LimitLine.SwingLow[FunNum][1] - 3 * LimitLine.SwingLow[FunNum-1][1] - 2 * LimitLine.SwingLow[FunNum-1][4] - LimitLine.SwingLow[FunNum][4]) * Herx * Herx
						+ LimitLine.SwingLow[FunNum-1][4] * Herx + LimitLine.SwingLow[FunNum-1][1];
			Neighbor.x = sin(limitSwing[1]) * cos(nowTheta[2]);		//�ߖT�_�̍��W�i���S����̃x�N�g���j���v�Z
			Neighbor.y = sin(limitSwing[1]) * sin(nowTheta[2]);
			Neighbor.z = cos(limitSwing[1]);
			Irrupt = (cross(Neighbor,Jc.Ez())).norm();				//�ߖT�_�܂ł̍ŒZ�ƂȂ�p�x�����߂�
			Irrupt = asin(Irrupt);
			//�����܂�
			//�ڐ����������߂�
			tanLine.x = 3 * (2 * LimitLine.SwingLow[FunNum-1][1] - 2 * LimitLine.SwingLow[FunNum][1] + LimitLine.SwingLow[FunNum][4] + LimitLine.SwingLow[FunNum-1][4]) * Herx * Herx
							+ 2 * (3 * LimitLine.SwingLow[FunNum][1] - 3 * LimitLine.SwingLow[FunNum-1][1] - 2 * LimitLine.SwingLow[FunNum-1][4] - LimitLine.SwingLow[FunNum][4]) * Herx
							+ LimitLine.SwingLow[FunNum-1][4]; //��'�̌v�Z
			tanLine.z = -tanLine.x * sin(limitSwing[1]);
			tanLine.x *= cos(limitSwing[1]);
			tanLine.y = tanLine.x * sin(nowTheta[2]) + Neighbor.x;
			tanLine.x = tanLine.x * cos(nowTheta[2]) - Neighbor.y;
			tanLine = tanLine.unit();

			// ���W�ϊ��������ɍs��
			Jc.Ez() = Xjrel.q * Vec3f(0.0, 0.0, 1.0);
			Jc.Ex() = -tanLine;
			Jc.Ey() = cross(Jc.Ez(), Jc.Ex());
			Jc.Ex() = cross(Jc.Ey(), Jc.Ez());
			Jcinv   = Jc.trans();
		}
		else{
		onLimit[0].onLower = false;
		}
	}
	else{onLimit[0].onLower = false;}
	BefPos = nowTheta;

	// swing�p�̉��搧���̊m�F
	if (limitCount[0] == 0 && (!onLimit[0].onUpper)){
		if (limitSwing[0]!=FLT_MAX && nowTheta[0] < limitSwing[0]){
			onLimit[0].onLower = true;
			Irrupt = nowTheta[0] - limitSwing[0];
		}
		else{
		onLimit[0].onLower = false;
		}
	}	
	if(limitCount[1] == 0 && !onLimit[0].onLower){
		if(limitSwing[1]!=FLT_MAX && nowTheta[0] > limitSwing[1]){
			onLimit[0].onUpper = true;
			Irrupt = nowTheta[0] - limitSwing[1];
		}
		else onLimit[0].onUpper = false;
	}
	//�c�C�X�g�p�̉��搧���̊m�F
	if(PoleTwist.upper != FLT_MAX || PoleTwist.lower != FLT_MAX){
		double Herx,Theta;
		Vec2d LSwing,LimTwistL,LimTwistU;
		if(limitCount[1] != 0){		//�X�C���O�p�̍ő�l������Ƃ��ő�l�ƍő�c�C�X�g�p�����߂�
			FunNum = 1;
			while(LimitLine.SwingUp[FunNum][0] < nowTheta[2]){
				FunNum++;
				if(limitCount[1] < FunNum) break;
			}
			Herx = (nowTheta[2] - LimitLine.SwingUp[FunNum-1][0]) / (LimitLine.SwingUp[FunNum][0] - LimitLine.SwingUp[FunNum-1][0]);
			LimTwistU.upper = (2 * LimitLine.SwingUp[FunNum-1][3] - 2 * LimitLine.SwingUp[FunNum][3] + LimitLine.SwingUp[FunNum][4] + LimitLine.SwingUp[FunNum-1][4]) * Herx * Herx * Herx
						+ (3 * LimitLine.SwingUp[FunNum][3] - 3 * LimitLine.SwingUp[FunNum-1][3] - 2 * LimitLine.SwingUp[FunNum-1][4] - LimitLine.SwingUp[FunNum][4]) * Herx * Herx
						+ LimitLine.SwingUp[FunNum-1][4] * Herx + LimitLine.SwingUp[FunNum-1][3];
			LSwing[1] = (2 * LimitLine.SwingUp[FunNum-1][1] - 2 * LimitLine.SwingUp[FunNum][1] + LimitLine.SwingUp[FunNum][4] + LimitLine.SwingUp[FunNum-1][4]) * Herx * Herx * Herx
						+ (3 * LimitLine.SwingUp[FunNum][1] - 3 * LimitLine.SwingUp[FunNum-1][1] - 2 * LimitLine.SwingUp[FunNum-1][4] - LimitLine.SwingUp[FunNum][4]) * Herx * Herx
						+ LimitLine.SwingUp[FunNum-1][4] * Herx + LimitLine.SwingUp[FunNum-1][1];
			LimTwistU.lower = (2 * LimitLine.SwingUp[FunNum-1][2] - 2 * LimitLine.SwingUp[FunNum][2] + LimitLine.SwingUp[FunNum][4] + LimitLine.SwingUp[FunNum-1][4]) * Herx * Herx * Herx
						+ (3 * LimitLine.SwingUp[FunNum][2] - 3 * LimitLine.SwingUp[FunNum-1][2] - 2 * LimitLine.SwingUp[FunNum-1][4] - LimitLine.SwingUp[FunNum][4]) * Herx * Herx
						+ LimitLine.SwingUp[FunNum-1][4] * Herx + LimitLine.SwingUp[FunNum-1][2];
		}
		else{
			LimTwistU.upper = PoleTwist.upper;
			LSwing[1] = M_PI;
			LimTwistU.lower = PoleTwist.lower;
		}		
		//�X�C���O�p�̍ŏ��l������Ƃ�
		if(limitCount[0] != 0){
			FunNum = 1;
			while(LimitLine.SwingUp[FunNum][0] < nowTheta[2]){
				FunNum++;
				if(limitCount[1] < FunNum) break;
			}
			Herx = (nowTheta[2] - LimitLine.SwingLow[FunNum-1][0]) / (LimitLine.SwingLow[FunNum][0] - LimitLine.SwingLow[FunNum-1][0]);
			LimTwistL.lower = (2 * LimitLine.SwingLow[FunNum-1][2] - 2 * LimitLine.SwingLow[FunNum][2] + LimitLine.SwingLow[FunNum][4] + LimitLine.SwingLow[FunNum-1][4]) * Herx * Herx * Herx
						+ (3 * LimitLine.SwingLow[FunNum][2] - 3 * LimitLine.SwingLow[FunNum-1][2] - 2 * LimitLine.SwingLow[FunNum-1][4] - LimitLine.SwingLow[FunNum][4]) * Herx * Herx
						+ LimitLine.SwingLow[FunNum-1][4] * Herx + LimitLine.SwingLow[FunNum-1][2];
			LSwing[0] = (2 * LimitLine.SwingLow[FunNum-1][1] - 2 * LimitLine.SwingLow[FunNum][1] + LimitLine.SwingLow[FunNum][4] + LimitLine.SwingLow[FunNum-1][4]) * Herx * Herx * Herx
						+ (3 * LimitLine.SwingLow[FunNum][1] - 3 * LimitLine.SwingLow[FunNum-1][1] - 2 * LimitLine.SwingLow[FunNum-1][4] - LimitLine.SwingLow[FunNum][4]) * Herx * Herx
						+ LimitLine.SwingLow[FunNum-1][4] * Herx + LimitLine.SwingLow[FunNum-1][1];
			LimTwistL.upper = (2 * LimitLine.SwingLow[FunNum-1][3] - 2 * LimitLine.SwingLow[FunNum][3] + LimitLine.SwingLow[FunNum][4] + LimitLine.SwingLow[FunNum-1][4]) * Herx * Herx * Herx
						+ (3 * LimitLine.SwingLow[FunNum][3] - 3 * LimitLine.SwingLow[FunNum-1][3] - 2 * LimitLine.SwingLow[FunNum-1][4] - LimitLine.SwingLow[FunNum][4]) * Herx * Herx
						+ LimitLine.SwingLow[FunNum-1][4] * Herx + LimitLine.SwingLow[FunNum-1][3];
		}
		else{
			LimTwistL.upper = PoleTwist.upper;
			LSwing[0] = 0;
			LimTwistL.lower = PoleTwist.lower;
		}
		if(nowTheta[0] > LSwing[1]) Theta = LSwing[1];
		else if(nowTheta[0] < LSwing[0]) Theta = LSwing[0];
		else Theta = nowTheta[0];
		for(int i = 0;i<2;i++){
			if(PoleTwist[i] != FLT_MAX){
				if(LSwing.lower - LSwing.upper){
					limitTwist[i] = ((LimTwistL[i] - LimTwistU[i]) * Theta + (LSwing.lower * LimTwistU[i] - LSwing.upper * LimTwistL[i])) / (LSwing.lower - LSwing.upper);
				}
				else if(i==0) limitTwist[i] = min(LimTwistL[i] , LimTwistU[i]);
				else limitTwist[i] = max(LimTwistL[i] , LimTwistU[i]);
			}
		}
	}

	// twist�p�̉��搧���̊m�F
	if(limitTwist[0]!=FLT_MAX && nowTheta[1] < limitTwist[0])
		onLimit[1].onLower = true;
	else if(limitTwist[1]!=FLT_MAX && nowTheta[1] > limitTwist[1])
		onLimit[1].onUpper = true;
	else{
		onLimit[1].onLower = false;
		onLimit[1].onUpper = false;
	}

	// �ǂ��������搧���ɂ������Ă����true
	if((onLimit[0].onUpper || onLimit[0].onLower) ||
	   (onLimit[1].onUpper || onLimit[1].onLower))  
	   anyLimit = true;
	else anyLimit = false;


	// ��̌v�Z�𓥂܂��Ė���A��]���̍S�������̍X�V������
	if(mode == PHJointDesc::MODE_POSITION){
		con[3] = onLimit[0].onUpper || onLimit[0].onLower || spring != 0.0 || damper != 0.0;
		con[4] = spring != 0.0	    || damper != 0.0;
		con[5] = onLimit[1].onUpper || onLimit[1].onLower || spring != 0.0 || damper != 0.0;
	} else if (mode == PHJointDesc::MODE_VELOCITY || mode == PHJointDesc::MODE_TRAJ){
		con[3] = true;
		con[4] = true;
		con[5] = true;
	} else {
		con[3] = onLimit[0].onUpper || onLimit[0].onLower;
		con[4] = false;
		con[5] = onLimit[1].onUpper || onLimit[1].onLower;
	}
}

// ���R�r�A���̊p���x���������W�ϊ�����SwingTwist�p�̎��ԕω����ւ̃��R�r�A���ɂ���
void PHBallJoint::ModifyJacobian(){
	//J[0].wv()����0�̑��x���瑊�Ίp���x�֕ϊ����郄�R�r�A��
	//J[0].ww()����0�̑��x���瑊�Ίp���x�֕ϊ����郄�R�r�A��
	//J[1].wv()����1�̑��x���瑊�Ίp���x�֕ϊ����郄�R�r�A��
	//J[1].ww()����1�̑��x���瑊�Ίp���x�֕ϊ����郄�R�r�A��
	if(anyLimit){
		J[0].wv() = Jcinv * J[0].wv();
		J[0].ww() = Jcinv * J[0].ww();
		J[1].wv() = Jcinv * J[1].wv();
		J[1].ww() = Jcinv * J[1].ww();
	}
}

void PHBallJoint::ControlCheck(double dtinv){
	// �ʒu����̌v�Z
	/*******************************************************************************************************
	����Ȃ��p�x�̍�����]���x�N�g���ɕϊ��DpropV(�c�肳��̘_���ł���q[t])�ɑ΂���db.w()���v�Z���Ă���.
	���R����0[rad]�ŁCpropV[rad]�L�т����ɑ΂��Ă̊p�x�o�l���\�����Ă���ƍl����΂����D
	********************************************************************************************************/
	Quaterniond propQ = goal * Xjrel.q.Inv();	// Xjrel.q�̖ڕWgoal��Xjrel.q�̎��ۂ̊p�x�̍���Quaternion�Ŏ擾
	Vec3d propV = propQ.RotationHalf();
	// ���搧�����������Ă���ꍇ��prop�̍��W��ϊ����čl���Ȃ��Ƃ����Ȃ��B
	if (anyLimit)
		propV = Jcinv * propV;

	if(mode == PHJointDesc::MODE_VELOCITY){
		if(anyLimit)
			db.w() = -Jcinv * desiredVelocity;
		else
			db.w() = - desiredVelocity;
	}else if(mode == PHJointDesc::MODE_TRAJ){
		if(anyLimit)
			db.w() = -Jcinv * (desiredVelocity + spring * propV);
		else
			db.w() = - (desiredVelocity + spring * propV);
	}else if (mode == PHJointDesc::MODE_POSITION){
		// �o�l�_���p�������Ă�����\�z����
		if (spring != 0.0 || damper != 0.0){
			double tmp = 1.0 / (damper + spring * GetScene()->GetTimeStep());
			/*****************************************************************************
			w[t+1] = (A+dA) * ��[t+1] + (b+db)
			��A�̓��C�o�l�_���p���\������̂́CBallJoint�̏ꍇ�͉�]�̎O���ɂ��ĂȂ̂ŁC
			SpatialVector dA.w()��tmp = (D + K��t)^{-1}�����Ă���D
			dtinv������ɂ�����̂�,ppt�ȂǕ\�L�Ƃ͈Ⴂ�CA=JM^{-1}J^T ��t�̂����C
			�v���O�������Ōv�Z���Ă���̂́CA = JM^{-1}J^T�����D
			���Ƃ���(A + dA)��t�����Ă��邽��
			******************************************************************************/
			dA.w() = tmp * dtinv * Vec3d(1.0, 1.0, 1.0);

			/****
			�c�肳��̘_���̎�25��db�ɑ�������D
			�ʒu����݂̂ł���΁C�ȉ��̎���1�s�ڂ̂݁D
			�O���Ǐ]����ł͎c���2�s���ӂ��ށDoffset�ɂ͊O�Ōv�Z���Ă������������e���\����������
			****/
			db.w() = tmp * (- spring * propV
						 -    damper * desiredVelocity
						 -    offset);
		}
	}
}

void PHBallJoint::MovableCheck(double dtinv){
	// vJc : Jc�ɂ���Ďʑ������S�����W�n���猩��Plug�̊p���x
	Vec3d vJc = Jc * vjrel.w();

	// ����t���O�̎w��onLimit[0]: swing, onLimit[1]: twist
	// nowTheta[0]: swing, nowTheta[1]: twist
	// ���搧�����z���Ă�����AdA:�֐߂��_�炩�����鐬����0�ɂ���Adb:�N�����Ă������������ɖ߂�	
	// x�������ɍS����������ꍇ	
	if(onLimit[0].onLower && limitCount[0] == 0){
		double fCheck;
		double dbLCP = (Irrupt) * dtinv * engine->GetVelCorrectionRate();	//LCP�ɂ���ĉ�����
		double bSame = b[3] + J[0].row(3) * solid[0]->dv + J[1].row(3) * solid[1]->dv;
		fCheck = (A.w().x + dA.w().x) * A.w()[0] *													//�S�����Ȃ����̑��x�ƁA�S������Ƃ��̑��x�̍����v�Z 
					(A.w()[0] * (dA.w()[0] * f.w()[0] + db.w()[0]) - dA.w()[0] * (dbLCP + bSame));	//dA,db�ȊO�͉ߋ��̒l��p���Ă���
		if(fCheck < 0 ){																			//�S���ᔽ�����Ȃ��Ȃ�S�����Ȃ�
			db.w()[0] += dbLCP;
		}
		else{
			dA.w()[0] = 0;
			db.w()[0] = dbLCP;
		}
	}
	
	else if(onLimit[0].onUpper || onLimit[0].onLower){
		double fCheck;
		double dbLCP = (Irrupt) * dtinv * engine->GetVelCorrectionRate();
		double bSame = b[3] + J[0].row(3) * solid[0]->dv + J[1].row(3) * solid[1]->dv;
		fCheck = (A.w().x + dA.w().x) * A.w()[0] *
					(A.w()[0] * (dA.w()[0] * f.w()[0] + db.w()[0]) - dA.w()[0] * (dbLCP + bSame));
		if(fCheck > 0 ){
			db.w()[0] += dbLCP;
		}
		else{
			dA.w()[0] = 0;
			db.w()[0] = dbLCP;
		}
	}

	//z�������ɍS����������ꍇ
	if(onLimit[1].onLower && (vJc.z < 0)){
		double fCheck;
		double dbLCP = (nowTheta[1] - limitTwist[0]) * dtinv * engine->GetVelCorrectionRate();
		double bSame = b[5] + J[0].row(5) * solid[0]->dv + J[1].row(5) * solid[1]->dv;
		fCheck = (A.w().z + dA.w().z) * A.w()[2] *
					(A.w()[2] * (dA.w()[2] * f.w()[2] + db.w()[2]) - dA.w()[2] * (dbLCP + bSame));
		if(fCheck > 0 ){
			db.w()[2] += dbLCP;
		}
		else{
			dA.w()[2] = 0;
			db.w()[2] = dbLCP;
		}
	}

	else if(onLimit[1].onUpper && (vJc.z > 0)){
		double fCheck;
		double dbLCP = (nowTheta[1] - limitTwist[1]) * dtinv * engine->GetVelCorrectionRate();
		double bSame = b[5] + J[0].row(5) * solid[0]->dv + J[1].row(5) * solid[1]->dv;
		fCheck = (A.w().z + dA.w().z) * A.w()[2] *
					(A.w()[2] * (dA.w()[2] * f.w()[2] + db.w()[2]) - dA.w()[2] * (dbLCP + bSame));	
		if(fCheck > 0 ){																			
			db.w()[2] += dbLCP;
		}
		else{
			dA.w()[2] = 0;
			db.w()[2] = dbLCP;
		}
	}
}
void PHBallJoint::CompBias(){
	double dtinv = 1.0 / GetScene()->GetTimeStep();
	
	db.v() = Xjrel.r * dtinv;		//	���i�덷�̉����̂��߁A���x�Ɍ덷/dt�����Z, Xjrel.r: �\�P�b�g�ɑ΂���v���O�̈ʒu�̃Y��
	db.v() *= engine->velCorrectionRate;

	// �֐߂̐�����s���ꍇ�C�s��A,�x�N�g��b��␳
	ControlCheck(dtinv);
	// �֐߂̉���͐�������D�悳���D
	// �����ɉ����čs��A,�x�N�g��b��␳
	MovableCheck(dtinv);
}

void PHBallJoint::CompError(){
	B.v() = Xjrel.r;
	
	B.w()[0] = (onLimit[0].onLower ? (nowTheta[0] - limitSwing[0]) :
	onLimit[0].onUpper ? (nowTheta[0] - limitSwing[1]) : 0.0);

}

void PHBallJoint::Projection(double& f, int k){
	//���̗͐ς̏�Ԃɑ΂��ĉ��炩�̑����^����D
	//k:con[k]��k�̕���(0�`2:���i�C3�`5:��])�Af�͂���ɑΉ�����̓ɂ̂���
	//�̓� = 0 �ɂ��邱�ƂŊ֐߂̍S�������������D
	//�S��������1��0�ɖ߂鎞��LCP�̃�(�g���N)�𖳗����0�ɂ���w�i���x�E�p���x�j�����߂���悤�ɂ���D
	//< fMaxDt, fMinDt�̏����ł͊֐߂��S���������킯�ł͂Ȃ��̂ŁC�P�Ȃ�㏑�����s���D
	Quaterniond propQ = goal * Xjrel.q.Inv();
	Vec3d propV = propQ.RotationHalf();
	if (k==3){
		if(onLimit[0].onLower && limitCount[0] == 0)
			f = max(0.0, f);
		else if(onLimit[0].onUpper||onLimit[0].onLower)
			f = min(0.0, f);
		else if(fMaxDt < f)
			f = fMaxDt;
		else if(f < fMinDt)
			f = fMinDt;
	}

	if (k==5){
		if(onLimit[1].onLower)
			f = max(0.0, f);
		else if(onLimit[1].onUpper)
			f = min(0.0, f);
		else if(fMaxDt < f)
			f = fMaxDt;
		else if(f < fMinDt)
			f = fMinDt;
	}

}
bool PHBallJoint::SetConstLine(char* fileName , int i){
	limitCount[i] = 0;
	std::fstream ConstData(fileName,std::ios::in); //�ǂݍ��݂Ńt�@�C�����J��
	if( ConstData.fail()){std::cout<<"error"<<std::endl; return false;}
	char txt[10];
	ConstData >> txt;
	while( !ConstData.eof()){
		if(i == 1){
			ConstData >> LimitLine.SwingUp[limitCount[i]][0] >> LimitLine.SwingUp[limitCount[i]][1] >> LimitLine.SwingUp[limitCount[i]][2] >> LimitLine.SwingUp[limitCount[i]][3] >> LimitLine.SwingUp[limitCount[i]][4];
		}
		else if(i == 0){
			ConstData >> LimitLine.SwingLow[limitCount[i]][0] >> LimitLine.SwingLow[limitCount[i]][1] >> LimitLine.SwingLow[limitCount[i]][2] >> LimitLine.SwingLow[limitCount[i]][3] >> LimitLine.SwingLow[limitCount[i]][4];
		}
		for(int j = 0; j<5;j++){
			if(i==1)
				LimitLine.SwingUp[limitCount[i]][j] = Rad(LimitLine.SwingUp[limitCount[i]][j]);
			if(i==0)			
				LimitLine.SwingLow[limitCount[i]][j] = Rad(LimitLine.SwingLow[limitCount[i]][j]);
		}
		if(limitCount[i] < 15) limitCount[i]++;
	}
	limitCount[i]--;
	if(i == 0 && limitCount[0] != 0){
		if(LimitLine.SwingLow[limitCount[i]-1][0] != LimitLine.SwingLow[0][0] || LimitLine.SwingLow[limitCount[i]-1][1] != LimitLine.SwingLow[0][1] || LimitLine.SwingLow[limitCount[i]-1][2] != LimitLine.SwingLow[0][2]
		|| LimitLine.SwingLow[limitCount[i]-1][3] != LimitLine.SwingLow[0][3] || LimitLine.SwingLow[limitCount[i]-1][4] != LimitLine.SwingLow[0][4]){
			if(LimitLine.SwingLow[limitCount[i]-1][0] < Rad(360)){
				limitCount[i]++;
			}
			for(int j = 1; j < 5; j++){
				LimitLine.SwingLow[limitCount[i] - 1][j] = LimitLine.SwingLow[0][j];
			}
			LimitLine.SwingLow[limitCount[i] - 1][0] = Rad(360);
		}
	}

	if(i == 1 && limitCount[1] != 0){
		if(LimitLine.SwingUp[limitCount[i]-1][0] != LimitLine.SwingUp[0][0] || LimitLine.SwingUp[limitCount[i]-1][1] != LimitLine.SwingUp[0][1] || LimitLine.SwingUp[limitCount[i]-1][2] != LimitLine.SwingUp[0][2]
		|| LimitLine.SwingUp[limitCount[i]-1][3] != LimitLine.SwingUp[0][3] || LimitLine.SwingUp[limitCount[i]-1][4] != LimitLine.SwingUp[0][4]){
			if(LimitLine.SwingUp[limitCount[i]-1][0] < Rad(360)){
				limitCount[i]++;
			}
			for(int j = 1; j < 5; j++){
				LimitLine.SwingUp[limitCount[i] - 1][j] = LimitLine.SwingUp[0][j];
			}
			LimitLine.SwingUp[limitCount[i] - 1][0] = Rad(360);
		}
	}

	onLimit[0][i] = false;
	onLimit[1][i] = false;
	return true;
}
//----------------------------------------------------------------------------
// PHBallJointNode
void PHBallJointNode::CompJointJacobian(){
	PHBallJoint* j = GetJoint();
	//SwingTwist& angle = (SwingTwist&)(j->position);
	//angle.Jacobian(Jst);
	//Matrix3d test = Jst * Jcinv;
	Quaterniond q = j->Xjrel.q;
	for(int i = 0; i < 3; i++)
		J.col(i).sub_vector(PTM::TSubVectorDim<0,3>()).clear();
	/*J[0].w() = 2.0 * Vec3d(-q.x, -q.y, -q.z);
	J[1].w() = 2.0 * Vec3d( q.w,  q.z, -q.y);
    J[2].w() = 2.0 * Vec3d(-q.z,  q.w,  q.x);
    J[3].w() = 2.0 * Vec3d( q.y, -q.x,  q.w);*/
	J.col(0).sub_vector(PTM::TSubVectorDim<3, 3>()) = Vec3d(1.0, 0.0, 0.0);
	J.col(1).sub_vector(PTM::TSubVectorDim<3, 3>()) = Vec3d(0.0, 1.0, 0.0);
	J.col(2).sub_vector(PTM::TSubVectorDim<3, 3>()) = Vec3d(0.0, 0.0, 1.0);
	PHTreeNodeND<3>::CompJointJacobian();
}

void PHBallJointNode::CompJointCoriolisAccel(){
	//PHBallJoint* j = GetJoint();
	//cj.v().clear();
	//((SwingTwist&)(j->position)).Coriolis(cj.w(), j->velocity);
	//cj.w.clear();
	cj.clear();		//�֐ߍ��W��quaternion�ɂƂ�ꍇ�R���I������0
}

void PHBallJointNode::UpdateJointPosition(double dt){
	PHBallJoint* j = GetJoint();
	j->Xjrel.q += j->Xjrel.q.Derivative(j->vjrel.w()) * dt;
	j->Xjrel.q.unitize();
}

void PHBallJointNode::CompRelativePosition(){
	PHBallJoint* j = GetJoint();
	j->Xjrel.r.clear();
	//j->Xjrel.q��UpdateJointPosition�ōX�V�ς�
}
void PHBallJointNode::CompRelativeVelocity(){
	PHBallJoint* j = GetJoint();
	j->vjrel.v().clear();
	//j->vjrel.w() = ((Quaterniond&)j->position).AngularVelocity((Quaterniond&)j->velocity);
	j->vjrel.w() = j->velocity;
}

void PHBallJointNode::ModifyJacobian(){
	PHBallJoint* j = GetJoint();
	Jq = j->Jcinv;
}

void PHBallJointNode::CompBias(){
	dA.clear();
	db.clear();
}

void PHBallJointNode::Projection(double& f, int k){
	PHBallJoint* j = GetJoint();
	
	for(int i=0; k<2; k++){
		if(j->onLimit[i].onLower)
			f = max(0.0, f);
		if(j->onLimit[i].onUpper)
			f = min(0.0, f);
	}
}

}
