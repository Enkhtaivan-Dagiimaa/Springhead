/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "Physics.h"
#pragma hdrstop

using namespace PTM;
using namespace std;
namespace Spr{;

PHJointLimit1D::PHJointLimit1D(){
	f = F = 0.0;
}

void PHJointLimit1D::SetupLCP(){
	//���͈͂̔���
	onLower = onUpper = false;
	double l = joint->lower, u = joint->upper;
	double theta = joint->GetPosition();
	double diff;
	if(l < u){
		if(theta <= l){
			onLower = true;
			diff = joint->GetPosition() - l;
		}
		if(theta >= u){
			onUpper = true;
			diff = joint->GetPosition() - u;
		}
	}
	if(onLower || onUpper){
		double tmp = 1.0 / (joint->rangeDamper + joint->rangeSpring * joint->GetScene()->GetTimeStep());
		dA = tmp * joint->GetScene()->GetTimeStepInv();
		db = tmp * (joint->rangeSpring * diff);
		A  = joint->A[joint->axisIndex[0]];
		b  = joint->b[joint->axisIndex[0]];
		Ainv = 1.0 / (A + dA);
		f *= joint->engine->shrinkRate;

		joint->CompResponse(f, 0);
	}
	else f = 0.0;
}

void PHJointLimit1D::IterateLCP(){
	if(!onLower && !onUpper)
		return;

	int j = joint->axisIndex[0];
	double fnew = f - joint->engine->accelSOR * Ainv * (dA * f + b + db
			 + joint->J[0].row(j) * joint->solid[0]->dv + joint->J[1].row(j) * joint->solid[1]->dv);	

	if(onLower)
		fnew = max(0.0, fnew);
	if(onUpper)
		fnew = min(0.0, fnew);

	joint->CompResponse(fnew - f, 0);
	f = fnew;
}

void PHJointLimit1D::SetupCorrectionLCP(){

}

void PHJointLimit1D::IterateCorrectionLCP(){
	if(onLower)
		F = max(0.0, F);
	if(onUpper)
		F = min(0.0, F);
}

///////////////////////////////////////////////////////////////////////////////

PHBallJointLimit::PHBallJointLimit(){
	// limit��ɋ��邩�ǂ����̃t���O�̏�����
	for(int i=0; i<3; ++i){
		onLimit[i].onLower = false;
		onLimit[i].onUpper = false;
	}
	anyLimit = false;
	limitCount[0]	= 0;
	limitCount[1]	= 0;
}

bool PHBallJointLimit::SetConstLine(char* fileName , int i){
	limitCount[i] = 0;
	std::fstream ConstData(fileName,std::ios::in); //�ǂݍ��݂Ńt�@�C�����J��
	if( ConstData.fail()){
		std::cout<<"error"<<std::endl;
		return false;
	}
	char txt[10];
	ConstData >> txt;
	while( !ConstData.eof()){
		if(i == 1){
			ConstData
				>> limitLine.SwingUp[limitCount[i]][0]
				>> limitLine.SwingUp[limitCount[i]][1]
				>> limitLine.SwingUp[limitCount[i]][2]
				>> limitLine.SwingUp[limitCount[i]][3]
				>> limitLine.SwingUp[limitCount[i]][4];
		}
		else if(i == 0){
			ConstData
				>> limitLine.SwingLow[limitCount[i]][0]
				>> limitLine.SwingLow[limitCount[i]][1]
				>> limitLine.SwingLow[limitCount[i]][2]
				>> limitLine.SwingLow[limitCount[i]][3]
				>> limitLine.SwingLow[limitCount[i]][4];
		}
		for(int j = 0; j<5;j++){
			if(i==1)
				limitLine.SwingUp[limitCount[i]][j] = Rad(limitLine.SwingUp[limitCount[i]][j]);
			if(i==0)			
				limitLine.SwingLow[limitCount[i]][j] = Rad(limitLine.SwingLow[limitCount[i]][j]);
		}
		if(limitCount[i] < 15) limitCount[i]++;
	}
	limitCount[i]--;
	if(i == 0 && limitCount[0] != 0){
		if(limitLine.SwingLow[limitCount[i]-1][0] != limitLine.SwingLow[0][0] || limitLine.SwingLow[limitCount[i]-1][1] != limitLine.SwingLow[0][1] || limitLine.SwingLow[limitCount[i]-1][2] != limitLine.SwingLow[0][2]
		|| limitLine.SwingLow[limitCount[i]-1][3] != limitLine.SwingLow[0][3] || limitLine.SwingLow[limitCount[i]-1][4] != limitLine.SwingLow[0][4]){
			if(limitLine.SwingLow[limitCount[i]-1][0] < Rad(360)){
				limitCount[i]++;
			}
			for(int j = 1; j < 5; j++){
				limitLine.SwingLow[limitCount[i] - 1][j] = limitLine.SwingLow[0][j];
			}
			limitLine.SwingLow[limitCount[i] - 1][0] = Rad(360);
		}
	}

	if(i == 1 && limitCount[1] != 0){
		if(limitLine.SwingUp[limitCount[i]-1][0] != limitLine.SwingUp[0][0] || limitLine.SwingUp[limitCount[i]-1][1] != limitLine.SwingUp[0][1] || limitLine.SwingUp[limitCount[i]-1][2] != limitLine.SwingUp[0][2]
		|| limitLine.SwingUp[limitCount[i]-1][3] != limitLine.SwingUp[0][3] || limitLine.SwingUp[limitCount[i]-1][4] != limitLine.SwingUp[0][4]){
			if(limitLine.SwingUp[limitCount[i]-1][0] < Rad(360)){
				limitCount[i]++;
			}
			for(int j = 1; j < 5; j++){
				limitLine.SwingUp[limitCount[i] - 1][j] = limitLine.SwingUp[0][j];
			}
			limitLine.SwingUp[limitCount[i] - 1][0] = Rad(360);
		}
	}

	onLimit[0][i] = false;
	onLimit[1][i] = false;
	return true;
}

// �d���R�[�h���֐���
inline void CalcFrame(Matrix3d& R, const Vec3d& zdir, const Vec3d& xdir){
	R.Ez() = zdir;
	R.Ex() = xdir;
	R.Ey() = cross(R.Ez(), R.Ex());
	R.Ex() = cross(R.Ey(), R.Ez());		
}
inline Vec3d CalcDir(double theta, double phi){
	return Vec3d(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
}

void PHBallJointLimit::CheckLimit(){
	/**
		�ԉ�����ւ̓`���F

		�������e���c���ł��Ȃ������̂œ��얢�m�F��Ԃň�U�R�~�b�g���܂��B

		�f�X�N���v�^�̓��e�����������Ă��镔��������܂����A�f�X�N���v�^�͓ǂݎ���p�ł��B
		�萔�ł͂Ȃ���Ԃł���Ȃ�Ώꏊ���ڂ��ׂ��ł��B	tazz
	
		con�̐ݒ�͕s�v�ɂȂ�܂���

		���������͊֐������ăR�[�h���y�ʉ����Ă��������B���肢���܂��B
	*/

	/*
	// -----bool* con�̈Ӗ�----------------------------------
	// �S�����鎲�ɂ��Ẵt���O
	// con[0]-con[2]:���i�^���ix,y,z�j
	// con[3]-con[5]:��]�^���ix���܂��,y���܂��,z���܂��j
	// true:�S������Afalse:�S�����Ȃ�
	//-------------------------------------------------------

	//con[0] = con[1] = con[2] = true;				
	//con[3] = con[4] = con[5] = false;
	*/

	// ����̃`�F�b�N
	// ���݂�Socket��Plug�Ƃ̊Ԃ̊p�x���v�Z
	nowTheta[0]	= acos(dot(joint->limitDir, joint->Jc.Ez()));			///< Swing�p�̌v�Z	

	Vec3d PolarCoord;
	PolarCoord = joint->Jc.Ez() * limDir.trans();					///< �|�������̌v�Z�J�n
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
	Vec3d half =  0.5 * (Vec3d(0.0, 0.0, 1.0) + joint->Jc.Ez());	
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

	Quaterniond twistQ = qSwing * joint->Xjrel.q;
	nowTheta[1] = twistQ.Theta();						///< Twist�p�̌v�Z���s���Ă���	
	if (twistQ.z < 0)
		nowTheta[1]  *= -1;								///< Twist��]�������΂��������Ƃ�����̂ł��̑΍�
	
	if(nowTheta[1] < Rad(-180)) nowTheta[1] += Rad(360);
	if(nowTheta[1] > Rad( 180)) nowTheta[1] -= Rad(360);
	
	// ���搧���ɂ������Ă��邩�̔���

	//swing�p�̉��搧���̊m�F
	if (limitCount[1] != 0){
		FunNum = 1;
		while(limitLine.SwingUp[FunNum][0] < nowTheta[2]){				//���݂̎p�����S���Ȑ��̂ǂ̋�Ԃɂ���̂����ׂ�B
			FunNum++;
			if(limitCount[1] < FunNum) break;
		}
		double Herx,Swing;
		// �X�C���O�����̍S�����W�̐����l�����߂�
		Herx = (nowTheta[2] - limitLine.SwingUp[FunNum-1][0]) / (limitLine.SwingUp[FunNum][0] - limitLine.SwingUp[FunNum-1][0]);				//�X�v���C���Ȑ�����S�����W�����߂�
		
		joint->limitSwing[1] = (2 * limitLine.SwingUp[FunNum-1][1] - 2 * limitLine.SwingUp[FunNum][1] + limitLine.SwingUp[FunNum][4] + limitLine.SwingUp[FunNum-1][4]) * Herx * Herx * Herx
					+ (3 * limitLine.SwingUp[FunNum][1] - 3 * limitLine.SwingUp[FunNum-1][1] - 2 * limitLine.SwingUp[FunNum-1][4] - limitLine.SwingUp[FunNum][4]) * Herx * Herx
					+ limitLine.SwingUp[FunNum-1][4] * Herx + limitLine.SwingUp[FunNum-1][1];
		if(nowTheta[0] >= joint->limitSwing[1]){
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
			Neighbor = CalcDir(joint->limitSwing[1], nowTheta[2]);	//�\�P�b�g���W�n�ł̋ߖT�_�̍��W
			Irrupt = (cross(Neighbor, joint->Jc.Ez())).norm();				//���݂̒l�ƋߖT�_�̊p�x�̍��@sin(Rad)

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

				while(nowTheta[2] > limitLine.SwingUp[FunNum][0]){ //�S���̋Ȑ��̊֐��̋�Ԃ𒴂��Ă����玟�̋�Ԃ�
					FunNum++;
				}
				while(nowTheta[2] < limitLine.SwingUp[FunNum-1][0]){
					FunNum--;
				}
				Herx = (nowTheta[2] - limitLine.SwingUp[FunNum-1][0]) / (limitLine.SwingUp[FunNum][0] - limitLine.SwingUp[FunNum-1][0]);	//�͈͂�0-1�ɂ����Ƃ���x���W
				Swing = (2 * limitLine.SwingUp[FunNum-1][1] - 2 * limitLine.SwingUp[FunNum][1] + limitLine.SwingUp[FunNum][4] + limitLine.SwingUp[FunNum-1][4]) * Herx * Herx * Herx
							+ (3 * limitLine.SwingUp[FunNum][1] - 3 * limitLine.SwingUp[FunNum-1][1] - 2 * limitLine.SwingUp[FunNum-1][4] - limitLine.SwingUp[FunNum][4]) * Herx * Herx
							+ limitLine.SwingUp[FunNum-1][4] * Herx + limitLine.SwingUp[FunNum-1][1];	//�v�Z�ʒu�ł�Swing�p�̐���
				Neighbor = CalcDir(Swing, nowTheta[2]);				//�ߖT�_�̍��W�i���S����̃x�N�g���j���v�Z
				IrruptUp = (cross(Neighbor, joint->Jc.Ez())).norm();		//���݂̒l�Ƃ̋�����cos�����߂�
				if(Irrupt > IrruptUp){								//�O�̈ʒu���߂��ߖT�_�𓾂�ꂽ���������
					Irrupt = IrruptUp;
					minDif = nowTheta[2];
					FunStack = FunNum;
				}
			}
			nowTheta[2] = minDif;			//�ߖT�_�̃X�C���O���ʊp�ł̍S���ʒu�ւ̃x�N�g�����v�Z����
			FunNum = FunStack;
			Herx = (nowTheta[2] - limitLine.SwingUp[FunNum-1][0]) / (limitLine.SwingUp[FunNum][0] - limitLine.SwingUp[FunNum-1][0]);
			if(Herx > 1 || Herx < 0){
				FunNum = 1;
				while(limitLine.SwingUp[FunNum][0] < nowTheta[2]){
					FunNum++;
					if(limitCount[1] < FunNum) break;
				}
				Herx = (nowTheta[2] - limitLine.SwingUp[FunNum-1][0]) / (limitLine.SwingUp[FunNum][0] - limitLine.SwingUp[FunNum-1][0]);
			}
			joint->limitSwing[1]
				= (2 * limitLine.SwingUp[FunNum-1][1] - 2 * limitLine.SwingUp[FunNum][1] + limitLine.SwingUp[FunNum][4] + limitLine.SwingUp[FunNum-1][4]) * Herx * Herx * Herx
				+ (3 * limitLine.SwingUp[FunNum][1] - 3 * limitLine.SwingUp[FunNum-1][1] - 2 * limitLine.SwingUp[FunNum-1][4] - limitLine.SwingUp[FunNum][4]) * Herx * Herx
				+ limitLine.SwingUp[FunNum-1][4] * Herx + limitLine.SwingUp[FunNum-1][1];
			Neighbor = CalcDir(joint->limitSwing[1], nowTheta[2]);	//�ߖT�_�̍��W�i���S����̃x�N�g���j���v�Z
			Irrupt = (cross(Neighbor, joint->Jc.Ez())).norm();				//�ߖT�_�܂ł̍ŒZ�ƂȂ�p�x�����߂�
			Irrupt = asin(Irrupt);
			//�����܂�
			//�ڐ����������߂�
			tanLine.x = 3 * (2 * limitLine.SwingUp[FunNum-1][1] - 2 * limitLine.SwingUp[FunNum][1] + limitLine.SwingUp[FunNum][4] + limitLine.SwingUp[FunNum-1][4]) * Herx * Herx
							+ 2 * (3 * limitLine.SwingUp[FunNum][1] - 3 * limitLine.SwingUp[FunNum-1][1] - 2 * limitLine.SwingUp[FunNum-1][4] - limitLine.SwingUp[FunNum][4]) * Herx
							+ limitLine.SwingUp[FunNum-1][4]; //��'�̌v�Z
			tanLine.z = -tanLine.x * sin(joint->limitSwing[1]);
			tanLine.x *= cos(joint->limitSwing[1]);
			tanLine.y = tanLine.x * sin(nowTheta[2]) + Neighbor.x;
			tanLine.x = tanLine.x * cos(nowTheta[2]) - Neighbor.y;
			tanLine.unitize();

			// ���W�ϊ��������ɍs��
			CalcFrame(joint->Jc, joint->Xjrel.q * Vec3d(0.0, 0.0, 1.0), tanLine);
			joint->Jcinv = joint->Jc.trans();
		}
		else{
			onLimit[0].onUpper = false;
		}
	}
	else{
		onLimit[0].onUpper = false;
	}
	//Swing�p�̍ŏ��l�����߂�
	if (limitCount[0] != 0 && !onLimit[0].onUpper){
		FunNum = 1;
		while(limitLine.SwingLow[FunNum][0] < nowTheta[2]){
			FunNum++;
			if(limitCount[0] < FunNum) break;
		}
		double Herx,Swing;
		// �X�C���O�����̍S�����W�̐����l�����߂�
		Herx = (nowTheta[2] - limitLine.SwingLow[FunNum-1][0]) / (limitLine.SwingLow[FunNum][0] - limitLine.SwingLow[FunNum-1][0]);
		joint->limitSwing[0] = (2 * limitLine.SwingLow[FunNum-1][1] - 2 * limitLine.SwingLow[FunNum][1] + limitLine.SwingLow[FunNum][4] + limitLine.SwingLow[FunNum-1][4]) * Herx * Herx * Herx
					+ (3 * limitLine.SwingLow[FunNum][1] - 3 * limitLine.SwingLow[FunNum-1][1] - 2 * limitLine.SwingLow[FunNum-1][4] - limitLine.SwingLow[FunNum][4]) * Herx * Herx
					+ limitLine.SwingLow[FunNum-1][4] * Herx + limitLine.SwingLow[FunNum-1][1];
		if(nowTheta[0] <= joint->limitSwing[0]){

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
			while(limitLine.SwingLow[FunNum][0] < nowTheta[2]){
				FunNum++;
				if(limitCount[0] < FunNum + 1) break;
			}
			FunStack = FunNum;
			minDif = nowTheta[2];
			nowTheta[2] = BefPos[2];
			Herx = (nowTheta[2] - limitLine.SwingLow[FunNum-1][0]) / (limitLine.SwingLow[FunNum][0] - limitLine.SwingLow[FunNum-1][0]);
			joint->limitSwing[0] = (2 * limitLine.SwingLow[FunNum-1][1] - 2 * limitLine.SwingLow[FunNum][1] + limitLine.SwingLow[FunNum][4] + limitLine.SwingLow[FunNum-1][4]) * Herx * Herx * Herx
						+ (3 * limitLine.SwingLow[FunNum][1] - 3 * limitLine.SwingLow[FunNum-1][1] - 2 * limitLine.SwingLow[FunNum-1][4] - limitLine.SwingLow[FunNum][4]) * Herx * Herx
						+ limitLine.SwingLow[FunNum-1][4] * Herx + limitLine.SwingLow[FunNum-1][1];
			Neighbor = CalcDir(joint->limitSwing[0], nowTheta[2]);	//�\�P�b�g���W�n�ł̋ߖT�_�̍��W
			Irrupt = (cross(Neighbor, joint->Jc.Ez())).norm();				//���݂̒l�ƋߖT�_�̕ψڊp�x�@sin(Rad)
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
				while(nowTheta[2] > limitLine.SwingLow[FunNum][0]){
					FunNum++;	//��Ԃ𒴂��Ă����玟�̋�Ԃ�
				}
				while(nowTheta[2] < limitLine.SwingLow[FunNum-1][0]){
					FunNum--;
				}
				Herx = (nowTheta[2] - limitLine.SwingLow[FunNum-1][0]) / (limitLine.SwingLow[FunNum][0] - limitLine.SwingLow[FunNum-1][0]);	//�͈͂�0-1�ɂ����Ƃ���x���W
				Swing = (2 * limitLine.SwingLow[FunNum-1][1] - 2 * limitLine.SwingLow[FunNum][1] + limitLine.SwingLow[FunNum][4] + limitLine.SwingLow[FunNum-1][4]) * Herx * Herx * Herx
							+ (3 * limitLine.SwingLow[FunNum][1] - 3 * limitLine.SwingLow[FunNum-1][1] - 2 * limitLine.SwingLow[FunNum-1][4] - limitLine.SwingLow[FunNum][4]) * Herx * Herx
							+ limitLine.SwingLow[FunNum-1][4] * Herx + limitLine.SwingLow[FunNum-1][1];	//�v�Z�ʒu�ł�Swing�p�̐���
				Neighbor = CalcDir(Swing, nowTheta[2]);				//�ߖT�_�̍��W�i���S����̃x�N�g���j���v�Z		
				IrruptUp = (cross(Neighbor, joint->Jc.Ez())).norm();		//���݂̒l�Ƃ̋�����sin�����߂�
				if(Irrupt > IrruptUp){
					Irrupt = IrruptUp;
					minDif = nowTheta[2];
					FunStack = FunNum;
				}
			}
			nowTheta[2] = minDif;			//�ߖT�_�̃X�C���O���ʊp�ł̍S���ʒu�ւ̃x�N�g�����v�Z����
			FunNum = FunStack;
			Herx = (nowTheta[2] - limitLine.SwingLow[FunNum-1][0]) / (limitLine.SwingLow[FunNum][0] - limitLine.SwingLow[FunNum-1][0]);
			if(Herx > 1 || Herx < 0){
				FunNum = 1;
				while(limitLine.SwingLow[FunNum][0] < nowTheta[2]){
					FunNum++;
					if(limitCount[1] < FunNum) break;
				}
				Herx = (nowTheta[2] - limitLine.SwingLow[FunNum-1][0]) / (limitLine.SwingLow[FunNum][0] - limitLine.SwingLow[FunNum-1][0]);
			}
			joint->limitSwing[1] = (2 * limitLine.SwingLow[FunNum-1][1] - 2 * limitLine.SwingLow[FunNum][1] + limitLine.SwingLow[FunNum][4] + limitLine.SwingLow[FunNum-1][4]) * Herx * Herx * Herx
						+ (3 * limitLine.SwingLow[FunNum][1] - 3 * limitLine.SwingLow[FunNum-1][1] - 2 * limitLine.SwingLow[FunNum-1][4] - limitLine.SwingLow[FunNum][4]) * Herx * Herx
						+ limitLine.SwingLow[FunNum-1][4] * Herx + limitLine.SwingLow[FunNum-1][1];
			Neighbor = CalcDir(joint->limitSwing[1], nowTheta[2]);	//�ߖT�_�̍��W�i���S����̃x�N�g���j���v�Z
			Irrupt = (cross(Neighbor, joint->Jc.Ez())).norm();				//�ߖT�_�܂ł̍ŒZ�ƂȂ�p�x�����߂�
			Irrupt = asin(Irrupt);
			//�����܂�
			//�ڐ����������߂�
			tanLine.x = 3 * (2 * limitLine.SwingLow[FunNum-1][1] - 2 * limitLine.SwingLow[FunNum][1] + limitLine.SwingLow[FunNum][4] + limitLine.SwingLow[FunNum-1][4]) * Herx * Herx
							+ 2 * (3 * limitLine.SwingLow[FunNum][1] - 3 * limitLine.SwingLow[FunNum-1][1] - 2 * limitLine.SwingLow[FunNum-1][4] - limitLine.SwingLow[FunNum][4]) * Herx
							+ limitLine.SwingLow[FunNum-1][4]; //��'�̌v�Z
			tanLine.z = -tanLine.x * sin(joint->limitSwing[1]);
			tanLine.x *= cos(joint->limitSwing[1]);
			tanLine.y = tanLine.x * sin(nowTheta[2]) + Neighbor.x;
			tanLine.x = tanLine.x * cos(nowTheta[2]) - Neighbor.y;
			tanLine.unitize();

			// ���W�ϊ��������ɍs��
			CalcFrame(joint->Jc, joint->Xjrel.q * Vec3f(0.0, 0.0, 1.0), -tanLine);
			joint->Jcinv = joint->Jc.trans();
		}
		else{
			onLimit[0].onLower = false;
		}
	}
	else{
		onLimit[0].onLower = false;
	}
	BefPos = nowTheta;

	// swing�p�̉��搧���̊m�F
	if (limitCount[0] == 0 && (!onLimit[0].onUpper)){
		if (joint->limitSwing[0]!=FLT_MAX && nowTheta[0] < joint->limitSwing[0]){
			onLimit[0].onLower = true;
			Irrupt = nowTheta[0] - joint->limitSwing[0];
		}
		else onLimit[0].onLower = false;
	}	
	if(limitCount[1] == 0 && !onLimit[0].onLower){
		if(joint->limitSwing[1]!=FLT_MAX && nowTheta[0] > joint->limitSwing[1]){
			onLimit[0].onUpper = true;
			Irrupt = nowTheta[0] - joint->limitSwing[1];
		}
		else onLimit[0].onUpper = false;
	}
	//�c�C�X�g�p�̉��搧���̊m�F
	if(joint->poleTwist.upper != FLT_MAX || joint->poleTwist.lower != FLT_MAX){
		double Herx,Theta;
		Vec2d LSwing,LimTwistL,LimTwistU;
		if(limitCount[1] != 0){		//�X�C���O�p�̍ő�l������Ƃ��ő�l�ƍő�c�C�X�g�p�����߂�
			FunNum = 1;
			while(limitLine.SwingUp[FunNum][0] < nowTheta[2]){
				FunNum++;
				if(limitCount[1] < FunNum) break;
			}
			Herx = (nowTheta[2] - limitLine.SwingUp[FunNum-1][0]) / (limitLine.SwingUp[FunNum][0] - limitLine.SwingUp[FunNum-1][0]);
			LimTwistU.upper = (2 * limitLine.SwingUp[FunNum-1][3] - 2 * limitLine.SwingUp[FunNum][3] + limitLine.SwingUp[FunNum][4] + limitLine.SwingUp[FunNum-1][4]) * Herx * Herx * Herx
						+ (3 * limitLine.SwingUp[FunNum][3] - 3 * limitLine.SwingUp[FunNum-1][3] - 2 * limitLine.SwingUp[FunNum-1][4] - limitLine.SwingUp[FunNum][4]) * Herx * Herx
						+ limitLine.SwingUp[FunNum-1][4] * Herx + limitLine.SwingUp[FunNum-1][3];
			LSwing[1] = (2 * limitLine.SwingUp[FunNum-1][1] - 2 * limitLine.SwingUp[FunNum][1] + limitLine.SwingUp[FunNum][4] + limitLine.SwingUp[FunNum-1][4]) * Herx * Herx * Herx
						+ (3 * limitLine.SwingUp[FunNum][1] - 3 * limitLine.SwingUp[FunNum-1][1] - 2 * limitLine.SwingUp[FunNum-1][4] - limitLine.SwingUp[FunNum][4]) * Herx * Herx
						+ limitLine.SwingUp[FunNum-1][4] * Herx + limitLine.SwingUp[FunNum-1][1];
			LimTwistU.lower = (2 * limitLine.SwingUp[FunNum-1][2] - 2 * limitLine.SwingUp[FunNum][2] + limitLine.SwingUp[FunNum][4] + limitLine.SwingUp[FunNum-1][4]) * Herx * Herx * Herx
						+ (3 * limitLine.SwingUp[FunNum][2] - 3 * limitLine.SwingUp[FunNum-1][2] - 2 * limitLine.SwingUp[FunNum-1][4] - limitLine.SwingUp[FunNum][4]) * Herx * Herx
						+ limitLine.SwingUp[FunNum-1][4] * Herx + limitLine.SwingUp[FunNum-1][2];
		}
		else{
			LimTwistU.upper = joint->poleTwist.upper;
			LSwing[1] = M_PI;
			LimTwistU.lower = joint->poleTwist.lower;
		}		
		//�X�C���O�p�̍ŏ��l������Ƃ�
		if(limitCount[0] != 0){
			FunNum = 1;
			while(limitLine.SwingUp[FunNum][0] < nowTheta[2]){
				FunNum++;
				if(limitCount[1] < FunNum) break;
			}
			Herx = (nowTheta[2] - limitLine.SwingLow[FunNum-1][0]) / (limitLine.SwingLow[FunNum][0] - limitLine.SwingLow[FunNum-1][0]);
			LimTwistL.lower = (2 * limitLine.SwingLow[FunNum-1][2] - 2 * limitLine.SwingLow[FunNum][2] + limitLine.SwingLow[FunNum][4] + limitLine.SwingLow[FunNum-1][4]) * Herx * Herx * Herx
						+ (3 * limitLine.SwingLow[FunNum][2] - 3 * limitLine.SwingLow[FunNum-1][2] - 2 * limitLine.SwingLow[FunNum-1][4] - limitLine.SwingLow[FunNum][4]) * Herx * Herx
						+ limitLine.SwingLow[FunNum-1][4] * Herx + limitLine.SwingLow[FunNum-1][2];
			LSwing[0] = (2 * limitLine.SwingLow[FunNum-1][1] - 2 * limitLine.SwingLow[FunNum][1] + limitLine.SwingLow[FunNum][4] + limitLine.SwingLow[FunNum-1][4]) * Herx * Herx * Herx
						+ (3 * limitLine.SwingLow[FunNum][1] - 3 * limitLine.SwingLow[FunNum-1][1] - 2 * limitLine.SwingLow[FunNum-1][4] - limitLine.SwingLow[FunNum][4]) * Herx * Herx
						+ limitLine.SwingLow[FunNum-1][4] * Herx + limitLine.SwingLow[FunNum-1][1];
			LimTwistL.upper = (2 * limitLine.SwingLow[FunNum-1][3] - 2 * limitLine.SwingLow[FunNum][3] + limitLine.SwingLow[FunNum][4] + limitLine.SwingLow[FunNum-1][4]) * Herx * Herx * Herx
						+ (3 * limitLine.SwingLow[FunNum][3] - 3 * limitLine.SwingLow[FunNum-1][3] - 2 * limitLine.SwingLow[FunNum-1][4] - limitLine.SwingLow[FunNum][4]) * Herx * Herx
						+ limitLine.SwingLow[FunNum-1][4] * Herx + limitLine.SwingLow[FunNum-1][3];
		}
		else{
			LimTwistL.upper = joint->poleTwist.upper;
			LSwing[0] = 0;
			LimTwistL.lower = joint->poleTwist.lower;
		}
		if(nowTheta[0] > LSwing[1]) Theta = LSwing[1];
		else if(nowTheta[0] < LSwing[0]) Theta = LSwing[0];
		else Theta = nowTheta[0];
		for(int i = 0;i<2;i++){
			if(joint->poleTwist[i] != FLT_MAX){
				if(LSwing.lower - LSwing.upper){
					joint->limitTwist[i] = ((LimTwistL[i] - LimTwistU[i]) * Theta + (LSwing.lower * LimTwistU[i] - LSwing.upper * LimTwistL[i])) / (LSwing.lower - LSwing.upper);
				}
				else if(i==0) joint->limitTwist[i] = min(LimTwistL[i] , LimTwistU[i]);
				else joint->limitTwist[i] = max(LimTwistL[i] , LimTwistU[i]);
			}
		}
	}

	// twist�p�̉��搧���̊m�F
	if(joint->limitTwist[0] != FLT_MAX && nowTheta[1] < joint->limitTwist[0])
		onLimit[1].onLower = true;
	else if(joint->limitTwist[1] != FLT_MAX && nowTheta[1] > joint->limitTwist[1])
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

	/* con�̐ݒ�͕s�v�ɂȂ���
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
	*/
}

void PHBallJointLimit::SetupLCP(){
	// ���͈̓`�F�b�N
	CheckLimit();

	// vJc : Jc�ɂ���Ďʑ������S�����W�n���猩��Plug�̊p���x
	Vec3d vJc = joint->Jc * joint->vjrel.w();
	double dtinv = joint->GetScene()->GetTimeStepInv();
	double corRate = joint->engine->GetVelCorrectionRate();

	// ����t���O�̎w��onLimit[0]: swing, onLimit[1]: twist
	// nowTheta[0]: swing, nowTheta[1]: twist
	// ���搧�����z���Ă�����AdA:�֐߂��_�炩�����鐬����0�ɂ���Adb:�N�����Ă������������ɖ߂�	
	// x�������ɍS����������ꍇ	
	if(onLimit[0].onUpper || onLimit[0].onLower){
		double dbLCP = (Irrupt) * dtinv * corRate;
		double fCheck;
		//fCheck�͉���\���Ă���̂ł����H
		//double bSame = b[3] + J[0].row(3) * solid[0]->dv + J[1].row(3) * solid[1]->dv;
		//fCheck = (A.w().x + dA.w().x) * A.w()[0] *
		//			(A.w()[0] * (dA.w()[0] * f.w()[0] + db.w()[0]) - dA.w()[0] * (dbLCP + bSame));
		if(fCheck > 0 ){
			db[0] += dbLCP;
		}
		else{
			dA[0] = 0;
			db[0] = dbLCP;
		}
	}

	//z�������ɍS����������ꍇ
	if(onLimit[1].onLower && (vJc.z < 0)){
		double dbLCP = (nowTheta[1] - joint->limitTwist[0]) * dtinv * corRate;
		double fCheck;
		//double bSame = b[5] + J[0].row(5) * solid[0]->dv + J[1].row(5) * solid[1]->dv;
		//fCheck = (A.w().z + dA.w().z) * A.w()[2] *
		//			(A.w()[2] * (dA.w()[2] * f.w()[2] + db.w()[2]) - dA.w()[2] * (dbLCP + bSame));
		if(fCheck > 0 ){
			db[2] += dbLCP;
		}
		else{
			dA[2] = 0;
			db[2] = dbLCP;
		}
	}
	if(onLimit[1].onUpper && (vJc.z > 0)){
		double dbLCP = (nowTheta[1] - joint->limitTwist[1]) * dtinv * corRate;
		double fCheck;
		//double bSame = b[5] + J[0].row(5) * solid[0]->dv + J[1].row(5) * solid[1]->dv;
		//fCheck = (A.w().z + dA.w().z) * A.w()[2] *
		//			(A.w()[2] * (dA.w()[2] * f.w()[2] + db.w()[2]) - dA.w()[2] * (dbLCP + bSame));	
		if(fCheck > 0 ){																			
			db[2] += dbLCP;
		}
		else{
			dA[2] = 0;
			db[2] = dbLCP;
		}
	}

	// limDir : Z����limitDir�Ɍ������W�n�DX����Socket��Z���ƒ�������悤�ɂƂ�
	//limitDir.unitize();
	limDir.Ez() = joint->limitDir.unit();
	if(limDir.Ez() != Vec3d(0.0,0.0,1.0))		///< NG: Vec3d��r
		 limDir.Ex() = cross(Vec3d(0.0,0.0,1.0),limDir.Ez());
	else limDir.Ex() = Vec3d(1.0,0.0,0.0);
	limDir.Ey() = cross(limDir.Ez(), limDir.Ex());
	BefPos.clear();

	// ����t���O�̎w��onLimit[0]: swing, onLimit[1]: twist
	// nowTheta[0]: swing, nowTheta[1]: twist
	// ���搧�����z���Ă�����AdA:�֐߂��_�炩�����鐬����0�ɂ���Adb:�N�����Ă������������ɖ߂�	

/*
	// �ȉ���PHBallJoint�ɂ��Ƃ��Ƃ������R�[�h
	dA.clear();
	db.clear();
	// Swing�p���͈�
	if(onLimit[0].onLower){
		dA[0] = 0;
		db[0] = (nowTheta[0] - joint->limitSwing[0]) * dtinv * corRate;
	}
	else if(onLimit[0].onUpper){
		dA[0] = 0;
		db[0] = (nowTheta[0] - joint->limitSwing[1]) * dtinv * corRate;
	}

	// Twist�p���͈�
	if(onLimit[1].onLower && (vJc.z < 0)){
		dA[2] = 0;
		db[2] = (nowTheta[1] - joint->limitTwist[0]) * dtinv * corRate;
	}
	else if(onLimit[1].onUpper && (vJc.z > 0)){
		dA[2] = 0;
		db[2] = (nowTheta[1] - joint->limitTwist[1]) * dtinv * corRate;
	}

	for(int i = 0; i < 3; i++){
		A[i]  = joint->A[joint->axisIndex[i]];
		b[i]  = joint->b[joint->axisIndex[i]];
		Ainv[i] = 1.0 / (A[i] + dA[i]);
		f[i] *= joint->engine->shrinkRate;
		joint->CompResponse(f[i], i);
	}
*/
}

void PHBallJointLimit::Projection(double& f, int k){
	if (k == 0){
		if(onLimit[0].onLower && limitCount[0] == 0)
			f = max(0.0, f);
		else if(onLimit[0].onUpper || onLimit[0].onLower)	///< ���ł͂Ȃ����H
			f = min(0.0, f);
	}
	if (k == 2){
		if(onLimit[1].onLower)
			f = max(0.0, f);
		else if(onLimit[1].onUpper)
			f = min(0.0, f);
	}
}

void PHBallJointLimit::IterateLCP(){
	//���̗͐ς̏�Ԃɑ΂��ĉ��炩�̑����^����D
	//k:con[k]��k�̕���(0�`2:���i�C3�`5:��])�Af�͂���ɑΉ�����̓ɂ̂���
	//�̓� = 0 �ɂ��邱�ƂŊ֐߂̍S�������������D
	//�S��������1��0�ɖ߂鎞��LCP�̃�(�g���N)�𖳗����0�ɂ���w�i���x�E�p���x�j�����߂���悤�ɂ���D
	//< fMaxDt, fMinDt�̏����ł͊֐߂��S���������킯�ł͂Ȃ��̂ŁC�P�Ȃ�㏑�����s���D

	if(!anyLimit)
		return;

	Vec3d fnew;
	for(int i = 0; i < 3; i++){
		int j = joint->axisIndex[i];
		fnew[i] = joint->limitf[i] - joint->engine->accelSOR * Ainv[i] * (dA[i] * joint->limitf[i] + b[i] + db[i]
				+ joint->J[0].row(j) * joint->solid[0]->dv + joint->J[1].row(j) * joint->solid[1]->dv);	

		Projection(fnew[i], i);
		
		joint->CompResponse(fnew[i] - joint->limitf[i], i);
		joint->limitf[i] = fnew[i];
	}
	
}

}