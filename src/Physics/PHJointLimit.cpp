/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <Physics/PHJointLimit.h>
#include <Physics/PHJoint.h>
#include <Physics/PHBallJoint.h>
#include <Physics/PHConstraintEngine.h>

using namespace PTM;
using namespace std;
namespace Spr{;

PHJointLimit1D::PHJointLimit1D(){
	f = F = 0.0;
	onLower = onUpper = false;
	A = Ainv = dA = b = db = 0;
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
			joint->fMaxDt[joint->movableAxes[0]] = FLT_MAX;
			joint->fMinDt[joint->movableAxes[0]] = 0;
		}
		if(theta >= u){
			onUpper = true;
			diff = joint->GetPosition() - u;
			joint->fMaxDt[joint->movableAxes[0]] = 0;
			joint->fMinDt[joint->movableAxes[0]] = -FLT_MAX;
		}
	}
	if(onLower || onUpper){
		double tmp = 1.0 / (joint->rangeDamper + joint->rangeSpring * joint->GetScene()->GetTimeStep());
		dA = tmp * joint->GetScene()->GetTimeStepInv();
//		db = tmp * (joint->rangeSpring * diff);
		db = tmp * (joint->rangeSpring * diff + joint->GetSpring() * (joint->GetPosition() - joint->GetTargetPosition())
			- joint->GetDamper() * joint->targetVelocity - joint->offsetForce * joint->GetScene()->GetTimeStepInv());
		A  = joint->A[joint->movableAxes[0]];
		b  = joint->b[joint->movableAxes[0]];
		Ainv = 1.0 / (A + dA);
		f *= joint->engine->shrinkRate;

		joint->CompResponse(f, 0);

		int i = joint->movableAxes[0];
		joint->dA[i] = dA;
		joint->A[i] = A;
		joint->db[i] = db;
		joint->b[i] = b;
		joint->Ainv[i] = Ainv;
		joint->f[i] = f;
		joint->constrainedAxes[i] = joint->movableAxes[0];
		joint->targetAxis = 6;
	}
	else f = 0.0;
}

void PHJointLimit1D::IterateLCP(){
	if(!onLower && !onUpper)
		return;

	int j = joint->movableAxes[0];
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
	constNumCondition = 0;
}

bool PHBallJointLimit::SetConstLine(char* fileName , bool i){
	int n = 0;
	int Num;
	if(i) Num = 0;
	else Num = 1;
	std::fstream ConstData(fileName,std::ios::in); //�ǂݍ��݂Ńt�@�C�����J��
	if( ConstData.fail()){
		std::cout<<"error"<<std::endl;
		return false;
	}
	char txt[10];
	ConstData >> txt;
	ConstData >> joint-> poleTwist[0] >> joint-> poleTwist[1];
	while( !ConstData.eof()){
		ConstData
			>> splinePoints[Num].swingUpper[n][0]
			>> splinePoints[Num].swingUpper[n][1]
			>> splinePoints[Num].swingUpper[n][2]
			>> splinePoints[Num].swingUpper[n][3]
			>> splinePoints[Num].swingUpper[n][4]
			>> splinePoints[Num].swingUpper[n][5];//0����A�X�C���O���ʊp�A�X�C���O�p�A�X�C���O���ʊp�̌X���A�X�C���O�p�̌X���A�c�C�X�g�p�̍ő�A�c�C�X�g�p�̍ŏ�
		for(int j = 0; j<6;j++){
			splinePoints[Num].swingUpper[n][j] = Rad(splinePoints[Num].swingUpper[n][j]);
		}
		if(n < 15) n++;
	}
	n--;
	if(n != 0){
		if(splinePoints[Num].swingUpper[n-1][Num] != splinePoints[Num].swingUpper[0][0] || splinePoints[Num].swingUpper[n-1][1] != splinePoints[Num].swingUpper[0][1] || splinePoints[Num].swingUpper[n-1][2] != splinePoints[Num].swingUpper[0][2]
		|| splinePoints[Num].swingUpper[n-1][3] != splinePoints[Num].swingUpper[0][3] || splinePoints[Num].swingUpper[n-1][4] != splinePoints[Num].swingUpper[0][4]){
			if(splinePoints[Num].swingUpper[n-1][0] < Rad(360)){
				n++;
			}
			for(int j = 1; j < 6; j++){
				splinePoints[Num].swingUpper[n - 1][j] = splinePoints[Num].swingUpper[0][j];
			}
			splinePoints[Num].swingUpper[n - 1][0] = Rad(360);
		}
	}
	splinePoints[Num].UorL = i;
	splinePoints[Num].limitCount = n;
	onLimit[0][0] = false;
	onLimit[1][0] = false;
	onLimit[2][0] = false;
	onLimit[0][1] = false;
	onLimit[1][1] = false;
	onLimit[2][1] = false;
	return true;
}
void PHBallJointLimit::SetConstPoint(int num,int way,double a){//�X�v���C���ōS������Ƃ��ɍS���_�̐����̏C���Ɏg�p
	if(a >= 6.28 && way == 0){
		splinePoints[0].limitCount = num+1;
	}
	splinePoints[0].swingUpper[num][way] = a;
}

// �d���R�[�h���֐���
inline void CalcFrame(Matrix3d& R, const Vec3d& zdir, const Vec3d& xdir){
	R.Ez() = zdir;
	R.Ex() = xdir;
	R.Ey() = cross(R.Ez(), R.Ex());
	R.Ex() = cross(R.Ey(), R.Ez());		
}

inline Vec3d CalcPosition(double SwingDir,double Swing){
	Vec3d Position;
	Position[0] = sin(Swing) * cos(SwingDir);
	Position[1] = sin(Swing) * sin(SwingDir);
	Position[2] = cos(Swing);
	return Position;
}
Vec4d PHBallJointLimit::CalcParameter(int i,int j,bool flag){
	Vec4d eq3Para;
		eq3Para[0] = 2 * splinePoints[0].swingUpper[i-1][j] - 2 * splinePoints[0].swingUpper[i][j] + splinePoints[0].swingUpper[i][j+2] + splinePoints[0].swingUpper[i-1][j+2];
		eq3Para[1] = 3 * splinePoints[0].swingUpper[i][j] - 3 * splinePoints[0].swingUpper[i-1][j] - 2 * splinePoints[0].swingUpper[i-1][j+2] - splinePoints[0].swingUpper[i][j+2];
		eq3Para[2] = splinePoints[0].swingUpper[i-1][j+2];
		eq3Para[3] = splinePoints[0].swingUpper[i-1][j]; 
	return eq3Para;
}

void PHBallJointLimit::CheckLimit(){
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
	Jc = joint->Jc;
	Jcinv = joint->Jcinv;
	double cosLange = dot(joint->limitDir, Jc.Ez());
	if(cosLange > 1.0){
		cosLange = 1.0;
	}
	else if(cosLange < -1.0){
		cosLange = -1.0;
	}
	currentEulerAngle[0]	= acos(cosLange);			///< Swing�p�̌v�Z	

	Vec3d PolarCoord;
	PolarCoord = Jc.Ez() * limDir.trans();					///< �|�������̌v�Z�J�n
	if(PolarCoord.x == 0){
		if(PolarCoord.y >= 0) currentEulerAngle[1] = M_PI / 2;
		else currentEulerAngle[1] = 3 * M_PI / 2;
	}
	else {
		currentEulerAngle[1] = atan(PolarCoord.y / PolarCoord.x);
	}
	if (PolarCoord.x < 0) currentEulerAngle[1] += M_PI;								///< 0 <= currentEulerAngle[2] <= 2�΂͈̔͂ɕύX
	else if(PolarCoord.x > 0 && PolarCoord.y < 0) currentEulerAngle[1] += 2 * M_PI;

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

	Quaterniond twistQ = qSwing * joint->Xjrel.q;
	currentEulerAngle[2] = twistQ.Theta();						///< Twist�p�̌v�Z���s���Ă���	
	if (twistQ.z < 0)
		currentEulerAngle[2]  *= -1;								///< Twist��]�������΂��������Ƃ�����̂ł��̑΍�
	
	if(currentEulerAngle[2] < Rad(-180)) currentEulerAngle[2] += Rad(360);
	if(currentEulerAngle[2] > Rad( 180)) currentEulerAngle[2] -= Rad(360);
	joint->position = currentEulerAngle;
	
	// ���搧���ɂ������Ă��邩�̔���
	switch(joint->ConstMode){
		case 1:
			SwingTwistLimit();
			break;
		case 2:
			SplineLimit();
			break;
	}

	// �ǂ��������搧���ɂ������Ă����true
	if((onLimit[0].onUpper || onLimit[0].onLower) ||
	   (onLimit[2].onUpper || onLimit[2].onLower) ||
	   (onLimit[1].onUpper || onLimit[1].onLower))  
	   anyLimit = true;
	else anyLimit = false;

	if(anyLimit){
		//���R�r�A���̎擾���X�V
		//J[0] = joint->J[0];
		//J[1] = joint->J[1];
		//J[0].wv() = Jcinv * joint->J[0].wv();
		//J[0].ww() = Jcinv * joint->J[0].ww();
		//J[1].wv() = Jcinv * joint->J[1].wv();
		//J[1].ww() = Jcinv * joint->J[1].ww();
		J[0] = Jcinv * joint->J[0].ww();
		J[1] = Jcinv * joint->J[1].ww();
	}
	else neighborAngle[0] = currentEulerAngle[2];

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

void PHBallJointLimit::SwingTwistLimit(){
	// swing�p�̉��搧���̊m�F
	{
		if (joint->limitSwing[0]<(FLT_MAX*0.1) && currentEulerAngle[0] < joint->limitSwing[0]){
			onLimit[0].onLower = true;
			penetration[0] = currentEulerAngle[0] - joint->limitSwing[0];
			joint->limitf[0] = max(0.0,joint->limitf[0]);
			constrainedAxes[constNumCondition] = 0;
			constNumCondition++;
		}
		else onLimit[0].onLower = false;
	}
	if(!onLimit[0].onLower){
		if(joint->limitSwing[1]<(FLT_MAX*0.1) && currentEulerAngle[0] > joint->limitSwing[1]){
			onLimit[0].onUpper = true;
			penetration[0] = currentEulerAngle[0] - joint->limitSwing[1];
			joint->limitf[0] = min(0.0,joint->limitf[0]);
			constrainedAxes[constNumCondition] = 0;
			constNumCondition++;
		}
		else onLimit[0].onUpper = false;
	}
	// swingDir�p�̉��搧���̊m�F
	{
		if (joint->limitSwingDir[0]<(FLT_MAX*0.1) && currentEulerAngle[1] < joint->limitSwingDir[0]){
			onLimit[1].onLower = true;
			penetration[1] = currentEulerAngle[1] - joint->limitSwingDir[0];
			joint->limitf[1] = max(0.0,joint->limitf[1]);
			constrainedAxes[constNumCondition] = 1;
			constNumCondition++;
		}
		else onLimit[1].onLower = false;
	}
	if(!onLimit[1].onLower){
		if(joint->limitSwingDir[1]<(FLT_MAX*0.1) && currentEulerAngle[1] > joint->limitSwingDir[1]){
			onLimit[1].onUpper = true;
			penetration[1] = currentEulerAngle[1] - joint->limitSwingDir[1];
			joint->limitf[1] = min(0.0,joint->limitf[1]);
			constrainedAxes[constNumCondition] = 1;
			constNumCondition++;
		}
		else onLimit[1].onUpper = false;
	}
	// twist�p�̉��搧���̊m�F
	if(joint->limitTwist[0]<(FLT_MAX*0.1) && currentEulerAngle[2] < joint->limitTwist[0]){
		penetration[2] = currentEulerAngle[2] - joint->limitTwist[0];
		joint->limitf[2] = max(0.0,joint->limitf[1]);
		onLimit[2].onLower = true;
		onLimit[2].onUpper = false;
		constrainedAxes[constNumCondition] = 2;
		constNumCondition++;
	}
	else if(joint->limitTwist[1]<(FLT_MAX*0.1) && currentEulerAngle[2] > joint->limitTwist[1]){
		penetration[2] = currentEulerAngle[2] - joint->limitTwist[1];
		joint->limitf[2] = min(0.0,joint->limitf[2]);
		onLimit[2].onLower = false;
		onLimit[2].onUpper = true;
		constrainedAxes[constNumCondition] = 2;
		constNumCondition++;
	}
	else{
		onLimit[2].onLower = false;
		onLimit[2].onUpper = false;
	}


}

void PHBallJointLimit::SplineLimit(){
	//�S���̎菇
	//���݂̎p�����S�������ׂ�
	onLimit[0].onLower = false;
	Vec3d para;
	Vec4d swing,swingDir,eq3;
	//swing�p0���𒴂���ꍇ�ɂ�0���ŋ�؂���2�񃋁[�v���񂷁B
	int exception;
	Vec3d exceptionVec;
	if(abs(lastEulerAngle[0]-currentEulerAngle[0]) > M_PI){//�ړ��̃x�N�g���͍����̏����������Ƃ�
		exception = 2;
		exceptionVec = currentEulerAngle;
		if(currentEulerAngle[0] > M_PI){
			currentEulerAngle[1] = (lastEulerAngle[0] * currentEulerAngle[1] - (currentEulerAngle[0]-2*M_PI) * lastEulerAngle[1])/(lastEulerAngle[0]-(currentEulerAngle[0]-2*M_PI));
			currentEulerAngle[0] = 0;
		}
		else{
			currentEulerAngle[1] = ((lastEulerAngle[0]-2*M_PI) * currentEulerAngle[1] - currentEulerAngle[0] * lastEulerAngle[1])/((lastEulerAngle[0]-2*M_PI)-currentEulerAngle[0]);
			currentEulerAngle[0] = 2*M_PI;
		}
	}
	else exception = 1;

	if(!onLimit[0].onUpper){ //1�X�e�b�v�ňړ����������ƍS���Ȑ��̊ԂɌ�_�����邩���`�F�b�N����B
		for(int m=0;m<exception;m++){
		
			//�O��̓_�ƍ���̓_�̈ʒu�̐���
			if(m==1){
				lastEulerAngle = currentEulerAngle;
				currentEulerAngle = exceptionVec;
				if(lastEulerAngle[0] == 0)	lastEulerAngle[0] = 2*M_PI;
				else lastEulerAngle = 0;
			}
			double Line[2];//1�X�e�b�v�ňړ������ʂ̐����̌W��
			if(currentEulerAngle[1] == lastEulerAngle[1]){
				Line[0] = 0;
				Line[1] = currentEulerAngle[0];
			}
			else{
				Line[0] = (currentEulerAngle[0] - lastEulerAngle[0])/(currentEulerAngle[1] - lastEulerAngle[1]);	//��:�O��̈ʒu�ƍ���̈ʒu��x�̒l���ς���Ă��邱�Ƃ��m�F����ׂ�
				Line[1] = (currentEulerAngle[1]*lastEulerAngle[0] - currentEulerAngle[0]*lastEulerAngle[1])/(currentEulerAngle[1] - lastEulerAngle[1]);
			}
			for(int funcNum = 1; funcNum < splinePoints[0].limitCount;funcNum++){
				//�G���~�[�g�X�v���C���̎�
				swingDir = CalcParameter(funcNum,0,1);//�X�C���O���ʊp��3���������𗧂Ă�
				swing = CalcParameter(funcNum,1,1);//�X�C���O�p��3���������𗧂Ă�
				eq3 = swing - Line[0] * swingDir;//�X�C���O�p�A�X�C���O���ʊp��3����������1�X�e�b�v�ł̈ړ��x�N�g����1�����ɑ������
				eq3[3] -= Line[1];
				para = SolveQubicFunction(eq3);//���߂�ꂽ3���������̉��𒲂ׂ�

				double nearAngleDistance = FLT_MAX;
				for(int j = 0;j < 3;j++){
					if(para[j] <= 1 && para[j] > 0){//������`��ɂ��邩���ׂ�(0<t<=1)
						Vec2d limit;
						limit[1] = swingDir[0] * para[j] * para[j] * para[j] + swingDir[1] * para[j] * para[j] + swingDir[2] * para[j] + swingDir[3];  
						limit[0] = swing[0] * para[j] * para[j] * para[j] + swing[1] * para[j] * para[j] + swing[2] * para[j] + swing[3];
						if(limit[0] < currentEulerAngle[0]){
							onLimit[0].onUpper = true;
							if( fabs(currentEulerAngle[1]-limit[1]) + fabs(currentEulerAngle[0]-limit[0]) < nearAngleDistance){
								neighborAngle[0] = limit[0];	//��_��swing�p
								neighborAngle[1] = limit[1];	//��_��swingDir�p
								neighborAngle[2] = funcNum;		//�g�p���Ă���֐�
								neighborAngle[3] = para[j];		//�}��ϐ�
								nearAngleDistance = fabs(currentEulerAngle[1]-limit[1]) + fabs(currentEulerAngle[0]-limit[0]);
							}
						}
					}
				}
			}
		}
	}
	if(onLimit[0].onUpper){
		onLimit[0].onUpper = false;
		penetration.x = 10;
		for(int FuncNum = 1; FuncNum < splinePoints[0].limitCount;FuncNum++){
			swingDir = CalcParameter(FuncNum,0,1);
			swingDir[3] -= currentEulerAngle[1];
			para = SolveQubicFunction(swingDir);
			for(int j = 0;j <3;j++){
				if(para[j] > 0 && para[j] < 1){
					swing = CalcParameter(FuncNum,1,1);
					joint->limitSwing.upper = swing[0] * para[j] * para[j] * para[j] + swing[1] * para[j] * para[j] + swing[2] * para[j] + swing[3];
					if(joint->limitSwing.upper < currentEulerAngle[0]) {
						onLimit[0].onUpper = !onLimit[0].onUpper;
						if(abs(penetration.x) > abs(joint->limitSwing.upper - lastEulerAngle[0])){
							neighborAngle[2] = FuncNum;
							neighborAngle[3] = para[j];
						}
						penetration.x = joint->limitSwing.upper - currentEulerAngle[0];
					}
				}
			}
		}
	}
	Vec3d changeJacobian = Vec3d(0.0,0.0,0.0);
	//�S�����Ă���ꍇ �ߖT�_�����߂�B	��_�ɖ߂��͂������悤�ɕύX����B(�ߖT�_�ł͂Ȃ���_�ɖ߂��悤�ɕς���b������B)
	if(onLimit[0].onUpper){
		double delta;
		delta = 0.001;//(currentEulerAngle[1] - lastEulerAngle[1]) / 20;
		neighborAngle[3] -= delta * 10;
		while(neighborAngle[3] > 1){
			neighborAngle[3] -= 1;
			neighborAngle[2]++;
			if(neighborAngle[2] > splinePoints[0].limitCount) neighborAngle[2] = 0;
		}
		while(neighborAngle[3] < 0){
			neighborAngle[3] +=1;
			neighborAngle[2] --;
			if(neighborAngle[2] < 1) neighborAngle[2] = splinePoints[0].limitCount;
		}
		Vec4d para;
		para = neighborAngle;
		para[0] = 10;//diff
		swingDir = CalcParameter((int)para[2],0,1);
		swing = CalcParameter((int)para[2],1,1);
		Vec2d limit;
		for(int i=0;i<20;i++){
			para[3] += delta;
			if(para[3] > 1){
				para[3] -= 1;
				para[2]++;
				if(para[2] > splinePoints[0].limitCount) para[2] = 0;
				swingDir = CalcParameter((int)para[2],0,1);
				swing = CalcParameter((int)para[2],1,1);
			}
			if(para[3] < 0){
				para[3] +=1;
				para[2] --;
				if(para[2] < 1) para[2] = splinePoints[0].limitCount;
				swingDir = CalcParameter((int)para[2],0,1);
				swing = CalcParameter((int)para[2],1,1);
			}
			limit[0] = swing[0] * para[3] * para[3] * para[3] + swing[1] * para[3] * para[3] + swing[2] * para[3] + swing[3] - currentEulerAngle[0];
			limit[1] = swingDir[0] * para[3] * para[3] * para[3] + swingDir[1] * para[3] * para[3] + swingDir[2] * para[3] + swingDir[3] - currentEulerAngle[1];
			if(para[0] > limit.norm()){
				para[0] = limit.norm();
				neighborAngle[0] = limit[0];
				neighborAngle[1] = limit[1];
				neighborAngle[2] = para[2];
				neighborAngle[3] = para[3];
			}
		}
		penetration.x = para[0];
		//DSTR << penetration.x << std::endl;
		if(penetration.x > 0.5) {
			penetration.x = 0.5;
		}//�l���傫������Ɣ��U����\�������邽��
		constrainedAxes[constNumCondition] = 0;
		constNumCondition++;
		onLimit[1].onUpper = true;
		//���W�ϊ�
		limit[0] = swing[0] * neighborAngle[3] * neighborAngle[3] * neighborAngle[3] + swing[1] * neighborAngle[3] * neighborAngle[3] + swing[2] * neighborAngle[3] + swing[3] - currentEulerAngle[0];
		limit[1] = swingDir[0] * neighborAngle[3] * neighborAngle[3] * neighborAngle[3] + swingDir[1] * neighborAngle[3] * neighborAngle[3] + swingDir[2] * neighborAngle[3] + swingDir[3] - currentEulerAngle[1];
		changeJacobian = Vec3d(-limit[1],-limit[0],0);
		changeJacobian = changeJacobian / changeJacobian.norm();
		Jc.Ex() = changeJacobian * Jc;
		joint->limitf[0] = max(0.0,joint->limitf[0]);
	}
	if(joint->poleTwist.x < FLT_MAX*0.1 && joint->poleTwist.x != joint->poleTwist.y){
		if(!onLimit[0].onUpper){
			neighborAngle[0] = currentEulerAngle[0];
			neighborAngle[1] = currentEulerAngle[1];
			for(int funNum = 1;funNum < splinePoints[0].limitCount;funNum++){
				if(neighborAngle[1] > splinePoints[0].swingUpper[funNum-1][0] && neighborAngle[1] <= splinePoints[0].swingUpper[funNum][0]){
					neighborAngle[2] = funNum;
				}
			}
			swingDir = CalcParameter((int)neighborAngle[2],0,1);
			swing    = CalcParameter((int)neighborAngle[2],1,1);
			swingDir[3] -= neighborAngle[1];
			para = SolveQubicFunction(swingDir);
			int j = 0;
			for (;j < 3;j++){
				if(para[j] <= 1 && para[j] > 0){
					neighborAngle[3] = para[j];
				}
			}
		}
		else{
			neighborAngle[0] += currentEulerAngle[0];
			neighborAngle[1] += currentEulerAngle[1];
		}
		double limit = swing[0] * neighborAngle[3] * neighborAngle[3] * neighborAngle[3] + swing[1] * neighborAngle[3] * neighborAngle[3] + swing[2] * neighborAngle[3] + swing[3];
		joint->limitTwist[0] = splinePoints[0].swingUpper[(int)neighborAngle[2]-1][5]*(1-neighborAngle[3])+splinePoints[0].swingUpper[(int)neighborAngle[2]][5]*neighborAngle[3];
		joint->limitTwist[1] = splinePoints[0].swingUpper[(int)neighborAngle[2]-1][4]*(1-neighborAngle[3])+splinePoints[0].swingUpper[(int)neighborAngle[2]][4]*neighborAngle[3];
		if(!onLimit[0].onUpper){
			joint->limitTwist[0] = joint->limitTwist[0] * (neighborAngle[0]/limit) * (neighborAngle[0]/limit)+ joint->poleTwist[0] * (1.0-(neighborAngle[0]/limit)*(neighborAngle[0]/limit));
			joint->limitTwist[1] = joint->limitTwist[1] * (neighborAngle[0]/limit) * (neighborAngle[0]/limit)+ joint->poleTwist[1] * (1.0-(neighborAngle[0]/limit)*(neighborAngle[0]/limit));
		}
		//�����܂�limitTwist�̉�����v�Z
		// twist�p�̉��搧���̊m�FSwingTwist�p�S���Ɠ��l
		if(joint->limitTwist[0]<(FLT_MAX*0.1) && currentEulerAngle[2] < joint->limitTwist[0]){
			penetration[2] = currentEulerAngle[2] - joint->limitTwist[0];
			joint->limitf[2] = max(0.0,joint->limitf[1]);
			onLimit[2].onLower = true;
			onLimit[2].onUpper = false;
			constrainedAxes[constNumCondition] = 2;
			constNumCondition++;
		}
		else if(joint->limitTwist[1]<(FLT_MAX*0.1) && currentEulerAngle[2] > joint->limitTwist[1]){
			penetration[2] = currentEulerAngle[2] - joint->limitTwist[1];
			joint->limitf[2] = min(0.0,joint->limitf[2]);
			onLimit[2].onLower = false;
			onLimit[2].onUpper = true;
			constrainedAxes[constNumCondition] = 2;
			constNumCondition++;
		}
		else{
			onLimit[2].onLower = false;
			onLimit[2].onUpper = false;
		}
	}

	lastEulerAngle = currentEulerAngle;
}

void PHBallJointLimit::SetupLCP(){
	if(joint->ConstMode == 0 || (joint->ConstMode==1 && (joint->limitTwist[0]<(FLT_MAX*0.1)||joint->limitTwist[1]<(FLT_MAX*0.1)))){ //�֐߂̍S���̃��[�h��-1�̂Ƃ��͍S���̌v�Z�������Ȃ�Ȃ��B//���[�h�̔ԍ����C�����������悳����
		return;
	}
	// ���͈̓`�F�b�N
	constNumCondition = 0;
	CheckLimit();

	// vJc : Jc�ɂ���Ďʑ������S�����W�n���猩��Plug�̊p���x
	Vec3d vJc = Jc * joint->vjrel.w();
	double dtinv = joint->GetScene()->GetTimeStepInv();
	double corRate = joint->engine->GetVelCorrectionRate();

	// ����t���O�̎w��onLimit[0]: swing, onLimit[1]: swingdir, onLimit[2]: twist
	// currentEulerAngle[0]: swing, currentEulerAngle[1]: twist
	// ���搧�����z���Ă�����AdA:�֐߂��_�炩�����鐬����0�ɂ���Adb:�N�����Ă������������ɖ߂�	

	// �ȉ���PHBallJoint�ɂ��Ƃ��Ƃ������R�[�h
	dA.clear();
	db.clear();
	// Swing�p���͈�
	for(int i=0;i<3;i++){
		if(onLimit[i].onLower){
			db[i] = penetration[i] * dtinv * corRate;
		}
		else if(onLimit[i].onUpper){
			db[i] = penetration[i] * dtinv * corRate;
		}
	}
	A = CompResponseMatrix();
	b = (J[0] * joint->solid[0]->v.w() + J[1] * joint->solid[1]->v.w());
	for(int i = 0; i < 3; i++){
		if(A[i]+dA[i]==0) Ainv[i] = FLT_MAX;
		else{
			Ainv[i] = 1.0 / (A[i] + dA[i]);
			joint->f[i] *= joint->engine->shrinkRate;
			joint->CompResponse(joint->f[i], i);
		}
	}
}

void PHBallJointLimit::Projection(double& f, int k){
	if (onLimit[k].onLower)
		f = max(0.0, f);
	else if(onLimit[k].onUpper)
		f = min(0.0, f);
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
	for(int n = 0; n < constNumCondition; n++){
		int i = constrainedAxes[n];
		if((onLimit[i].onUpper||onLimit[i].onLower)){
			fnew[i] = joint->limitf[i] - joint->engine->accelSOR * Ainv[i] * (dA[i] * joint->limitf[i] + b[i] + db[i]
				+ J[0].row(i) * joint->solid[0]->dv.w() + J[1].row(i) * joint->solid[1]->dv.w());	
			Projection(fnew[i], i);
	//		joint->CompResponse(fnew[i] - joint->limitf[i], i);	
			SpatialVector dfs;
			for(int k = 0; k < 2; k++){
				if(!joint->solid[k]->IsDynamical() || !joint->IsInactive(k))continue;
				if(joint->solid[k]->IsArticulated()){
					dfs.v().clear();
					dfs.w() = J[k].row(i) * (fnew[i] - joint->limitf[i]);
					joint->solid[k]->treeNode->CompResponse(dfs, true, false);
				}
				else joint->solid[k]->dv.w() += T[k].row(i) * (fnew[i] - joint->limitf[i]);
			}
		}
		else fnew[i] = 0;
		joint ->limitf[i] = fnew[i];
	}
}

Vec3d PHBallJointLimit::CompResponseMatrix(){
	int i, j;
//	SpatialVector A;
	Vec3d A;	//���֐߂�3���R�x�̂���3�~3�s��ł���
	PHRootNode* root[2];
	if(joint->solid[0]->IsArticulated())
		root[0] = joint->solid[0]->treeNode->GetRootNode();
	if(joint->solid[1]->IsArticulated())
		root[1] = joint->solid[1]->treeNode->GetRootNode();

	SpatialVector df;
	for(i = 0; i < 2; i++){
		if(joint->solid[i]->IsDynamical()){
			if(joint->solid[i]->IsArticulated()){
				for(j = 0; j < 3; j++){
					df.v().clear();
					df.w() = J[i].row(j);
					joint->solid[i]->treeNode->CompResponse(df, false, false);
					A[j] += J[i].row(j) * joint->solid[i]->treeNode->da.w();
					int ic = !i;
					//�����Е��̍��̂�����̃c���[�ɑ�����ꍇ�͂��̉e���������Z
					if(joint->solid[ic]->IsArticulated() && root[i] == root[ic])
						A[j] += J[ic].row(j) * joint->solid[ic]->treeNode->da.w();
				}
			}
			else{
				// T = M^-1 * J^T
				//T[i].vv() = J[i].vv() * joint->solid[i]->minv;
				//T[i].vw() = J[i].vw() * joint->solid[i]->Iinv;
				//T[i].wv() = J[i].wv() * joint->solid[i]->minv;
				//T[i].ww() = J[i].ww() * joint->solid[i]->Iinv;
				T[i] = J[i] * joint-> solid[i]->Iinv;
				for(j = 0; j < 3; j++){
					// A == �_������J * M^-1 * J^T, Gauss Seidel�@��D
					A[j] += J[i].row(j) * T[i].row(j);
				}
			}
		}
	}
	/** �ő�̑Ίp�v�f�Ƃ̔䂪eps�����������Ίp�v�f������ꍇ�C
		���l�I�s���萫�̌����ƂȂ�̂ł��̐����͍S���Ώۂ��珜�O����
		��eps��傫���Ƃ�ƁC�K�v�ȍS���܂Ŗ���������Ă��܂��̂ŁA�����͐T�d�ɁB
	 */
	const double eps = 0.000001, epsabs = 1.0e-10;
	double Amax = 0.0, Amin;
	for(j = 0; j < constNumCondition; j++)
//		if(joint->constr[j] && A[j] > Amax)
		if(A[constrainedAxes[j]] > Amax)
			Amax = A[constrainedAxes[j]];
	Amin = Amax * eps;
	for(j = 0; j < constNumCondition; j++){
//		if(!joint->constr[j+3])continue;
		int i = constrainedAxes[j];
		if(A[i] < Amin || A[i] < epsabs){
//			joint->constr[j+3] = false;
			DSTR << j << "-th constraint ill-conditioned! disabled." << endl;
		}
		else
			Ainv[i] = 1.0 / (A[i] + dA[i]);
	}
	return A;
}

//3���������������֐�//�X�v���C���Ȑ��ł̍S���Ŏg�p
Vec3d PHBallJointLimit::SolveQubicFunction(Vec4d eq3){	//�S���̊m�F�̂��߂�3���������������K�v������
	double z = 0;
	bool FlagQ = true;
	double D;
	Vec3d Para;
	double eq2[3];
	
	if(eq3[0] != 0){
		if(eq3[3] == 0) Para[0] = 0;
		else {
			double variable[2];
			variable[0] = (eq3[2]/eq3[0] - eq3[1]*eq3[1] / (3.0*eq3[0]*eq3[0])) / 3.0;//P
			variable[1] = (eq3[3]/eq3[0]-eq3[1]*eq3[2]/(3.0*eq3[0]*eq3[0])+2.0*pow(eq3[1],3)/(27.0*pow(eq3[0],3)))/2.0;//Q
			
			if(variable[1] == 0) z = 0;
			else{
				if(variable[1] < 0){
					FlagQ = false;
					variable[1] *= -1;
				}
				D = pow(variable[0],3) + pow(variable[1],2);
				if(D < 0)
					z = - 2 * sqrt( -variable[0] ) * cos(atan(sqrt(-D) / variable[1]) / 3.0);
				else if(variable[0] < 0)
					z = -pow(variable[1] + sqrt(D),0.333333) - pow(variable[1] - sqrt(D),0.333333);
				else if(variable[0] >= 0)
					z = pow(sqrt(D) - variable[1],0.333333) - pow(variable[1] + sqrt(D),0.333333);
				if(FlagQ == false)
					z *= -1;				
			}	
		Para[0] = z - eq3[1] / (3 * eq3[0]);
		}
		eq2[0] = eq3[0];
		for(int  i=0; i<2; i++){
			eq2[i+1] = eq3[i+1] + Para[0] * eq2[i];
		}
	}
	else {
		Para[0] = FLT_MAX;
		eq2[0] = eq3[1];
		eq2[1] = eq3[2];
		eq2[2] = eq3[3];
	}
	if(eq2[0] != 0){
		D = eq2[1] * eq2[1] - 4.0 * eq2[0] * eq2[2];
		if (D > 0){
			Para[1] = (- eq2[1] + sqrt(D))/ (2 * eq2[0]);
			Para[2] = (- eq2[1] - sqrt(D))/ (2 * eq2[0]);
		}
		else Para[1] = Para[2] = FLT_MAX;
	}
	else if(eq2[1] != 0){
		Para[1] = -eq2[2]/eq2[1];
		Para[2] = FLT_MAX;
	}
	else{
		Para[1] = Para[2] = FLT_MAX;
	}
	return	Para;
}

}
