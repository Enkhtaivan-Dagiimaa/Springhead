/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "Physics.h"
#pragma hdrstop

#define USE_ROTATEFRICTION 1//��]���C�̐������s�����ǂ���

using namespace PTM;
using namespace std;
namespace Spr{;

PHContactSurface::PHContactSurface(const Matrix3d& local, PHShapePairForLCP* sp, Vec3d p, PHSolid* s0, PHSolid* s1, std::vector<Vec3d> sec){
	shapePair = sp;
	pos = p;
	solid[0] = s0, solid[1] = s1;
	section = sec;
	lastL = lastR = 0;

	//��]���C�p
#if USE_ROTATEFRICTION
	rotateFriction = 0.0;
	segmentationNum = 0;
	interval = 0.5;
	IntegrateArea();
#endif

	Vec3d rjabs[2];
	for(int i = 0; i < 2; i++){
		rjabs[i] = pos - solid[i]->GetCenterPosition();	//���̂̒��S����ڐG�_�܂ł̃x�N�g��
	}
	// local: �ڐG�_�̊֐߃t���[�� �́Cx����@��, y,z����ڐ��Ƃ���
	Quaterniond qlocal;
	qlocal.FromMatrix(local);
	for(int i = 0; i < 2; i++){
		(i == 0 ? poseSocket : posePlug).Ori() = Xj[i].q = solid[i]->GetOrientation().Conjugated() * qlocal;
		(i == 0 ? poseSocket : posePlug).Pos() = Xj[i].r = solid[i]->GetOrientation().Conjugated() * rjabs[i];
	}
}

PHContactSurface::PHContactSurface(PHShapePairForLCP* sp, Vec3d p, PHSolid* s0, PHSolid* s1, std::vector<Vec3d> sec){
	shapePair = sp;
	pos = p;
	solid[0] = s0, solid[1] = s1;
	section = sec;
	lastL = lastR = 0;

	//��]���C�p
	rotateFriction = 0.0;
	segmentationNum = 0;
	interval = 0.5;
	IntegrateArea();

	Vec3d rjabs[2], vjabs[2];
	for(int i = 0; i < 2; i++){
		rjabs[i] = pos - solid[i]->GetCenterPosition();	//���̂̒��S����ڐG�_�܂ł̃x�N�g��
		vjabs[i] = solid[i]->GetVelocity() + solid[i]->GetAngularVelocity() % rjabs[i];	//�ڐG�_�ł̑��x
	}
	//�ڐ��x�N�g��t[0], t[1] (t[0]�͑��Α��x�x�N�g���ɕ��s�ɂȂ�悤�ɂ���)
	Vec3d n, t[2], vjrel, vjrelproj;
	n = shapePair->normal;
	vjrel = vjabs[1] - vjabs[0];
	vjrelproj = vjrel - (n * vjrel) * n;		//���Α��x�x�N�g����@���ɒ������镽�ʂɎˉe�����x�N�g��
	double vjrelproj_norm = vjrelproj.norm();
	if(vjrelproj_norm < 1.0e-10){
		t[0] = n % Vec3d(1.0, 0.0, 0.0);	
		if(t[0].norm() < 1.0e-10)
			t[0] = n % Vec3d(0.0, 1.0, 0.0);
		t[0].unitize();
	}
	else{
		t[0] = vjrelproj / vjrelproj_norm;
	}
	t[1] = n % t[0];
	Matrix3d Rjabs;
	// �ڐG�_�̊֐߃t���[����x��, y����ڐ��Cz����@���Ƃ���
	Rjabs.col(0) = n;
	Rjabs.col(1) = t[0];
	Rjabs.col(2) = t[1];
	Quaterniond qjabs;
	qjabs.FromMatrix(Rjabs);
	for(int i = 0; i < 2; i++){
		(i == 0 ? poseSocket : posePlug).Ori() = Xj[i].q = solid[i]->GetOrientation().Conjugated() * qjabs;
		(i == 0 ? poseSocket : posePlug).Pos() = Xj[i].r = solid[i]->GetOrientation().Conjugated() * rjabs[i];
	}
}

void PHContactSurface::SetConstrainedIndex(int* con){
//	con[0] = con[1] = con[2] = con[3] = con[4] = con[5] = true;
	 for(int i = 0; i<6;i++){
		con[i] = i;
	}
	targetAxis = 6;
}

void PHContactSurface::CompBias(){
	//	correction���ʒuLCP�ŕʂɍs���ꍇ�́A���x��ύX���Ă̈ʒu�␳�͂��Ȃ��B
	if (engine->numIterContactCorrection) return;
	double dtinv = 1.0 / GetScene()->GetTimeStep();
	double overlap = 0.002;

#if 1
	//	�ڐG�p�� correctionRate
	double contactCorrectionRate = 0;

	double e;
	//	���x���������ꍇ�́A���˕Ԃ�Ȃ��B
	if (vjrel[0]*GetScene()->GetTimeStep() > -0.1){
		e = 0;
		contactCorrectionRate = engine->contactCorrectionRate;
	}else{
		//	���˕Ԃ�W��: 2���̂̕��ϒl���g��
		e = 0.5 * (shapePair->shape[0]->GetMaterial().e + shapePair->shape[1]->GetMaterial().e);
		//	���˕Ԃ�Ƃ��͕␳�Ȃ�
		contactCorrectionRate = 0;
	}

	if (overlap > shapePair->depth) overlap = shapePair->depth;
	db[0] = - contactCorrectionRate * (shapePair->depth - overlap) / GetScene()->GetTimeStep() + e * vjrel[0];
#endif

#if 0
//	db.v.x = 0.1*engine->correctionRate * (-shapePair->depth * dtinv + vjrel.v.x);
	/*	hase	�{���� 1e-3�͈�������
		depth ������Correction������ƐڐG���s�A���ɂȂ�̂ŁCdepth-epsilon�ŗǂ����C
		epsilon���傫������D

		����́C�ڐG�ʂ��l�p�`�ŐڐG�_��4�_����ꍇ�ȂǁC
		4�_�Ԃ̗͂̕��U���܂�ɕs�ψ�ɂȂ��]���n�܂�C
		�ڐG�ʂ��E���̏�Ԃƍ����̏�Ԃ̊Ԃōׂ������U���邱�Ƃ����邽�߁D
		����́C���̏ꍇ�ɂ��ڐG�ʂ̌`���ς��Ȃ��悤�ɂ��邱�ƂŁC���艻���Ă���D

		2���̊Ԃ̐ڐG���ЂƂ̐���Ƃ��Ă���킹��悤�ɂȂ�Ή�������Ǝv���D	
	*/
	double err = (shapePair->depth - 1e-3)*dtinv - 0.2*vjrel.v().x;
	if (err < 0) err = 0;
	if (err){
//		HASE_REPORT
//		DSTR << shapePair->state;
//		DSTR << shapePair->shapePoseW[1].Pos();
//		DSTR << "err: " << err << std::edl;
		db.v().x = -err * engine->velCorrectionRate;
	}
#endif

#if 0
	double tmp = 1.0 / (correctionDamper + correctionSpring * GetScene()->GetTimeStep());
	dA[0] = tmp * dtinv;
	db[0] = -correctionSpring * max(0.0, shapePair->depth - overlap) * tmp;
#endif
}

void PHContactSurface::Projection(double& f, int k){
	static double flim;
	if(k == 0){	//�����R�� >= 0�̐���
		f = max(0.0, f);
		//	�ő�Î~���C
		flim = 0.5 * (shapePair->shape[0]->GetMaterial().mu0 + shapePair->shape[1]->GetMaterial().mu0) * f;	}
	else if(k == 1 || k == 2){
		//	�����C�������Ɏ������Ă݂�B
		double fu;
		if (shapePair->shape[0]->GetMaterial().mu0 + shapePair->shape[1]->GetMaterial().mu0 == 0){
			fu = 0;
		}else{
			fu = (shapePair->shape[0]->GetMaterial().mu + shapePair->shape[1]->GetMaterial().mu)
				/ (shapePair->shape[0]->GetMaterial().mu0 + shapePair->shape[1]->GetMaterial().mu0)
				* flim;	
		}
		if (-0.01 < vjrel[1] && vjrel[1] < 0.01){	//	�Î~���C
			if (f > flim) f = fu;
			else if (f < -flim) f = -fu;
		}else{					//	�����C
			if (f > fu) f = fu;
			else if (f < -fu) f = -fu;		
		}
#if 0
		//|���C��| <= �ő�Î~���C�̐���
		//	�E���C�͂̊e�������ő�Î~���C�����������Ă����͂͒�����\��������̂Ŗ{���͂��������B
		//	�E�Î~���C�Ɠ����C�������l�łȂ��ƈ����Ȃ��B
		//���C�W���͗��҂̐Î~���C�W���̕��ςƂ���
		f = min(max(-flim, f), flim);
#endif
		assert(f < 10000);
	}
}

void PHContactSurface::IterateLCP(){
	if(!bEnabled || !bFeasible || bArticulated)
		return;
	FPCK_FINITE(f.v());

	SpatialVector fnew, df;

	for(int j = 0; j < targetAxis; j++){
//		if(!constr[j])continue;
		int i = numCondition[j];
		fnew[i] = f[i] - engine->accelSOR * Ainv[i] * (dA[i] * f[i] + b[i] + db[i] 
				+ J[0].row(i) * solid[0]->dv + J[1].row(i) * solid[1]->dv);

		// �Ƃ肠���������Ȃ��悤�ɊԂɍ��킹�̃R�[�h
		if (!FPCK_FINITE(fnew[i])) fnew[i] = f[i]; //naga ����������ł͊Ԃɍ��킹�̃R�[�h�ł�������
		if (!FPCK_FINITE(fnew[0])){
			FPCK_FINITE(b[0]);
//			DSTR << AinvJ[0].vv << AinvJ[1].vv;
//			DSTR << AinvJ[0].vw << AinvJ[1].vw;
//			DSTR << dA.v[j];
//			DSTR << std::endl;
//			DSTR << "f.v:" << f.v << "b.v:" << b.v << std::endl;
			DSTR << "s0:" << (solid[0]->dv) << std::endl;
			DSTR << "s1:" << (solid[1]->dv)  << std::endl;
		}
		if(j > 2)continue;	//�g���N�͑S�g���N�̌v�Z���I����Ă���AProjection����
		Projection(fnew[i], i);
		df[j] = fnew[i] - f[i];
		CompResponse(df[i],i);
		f[j] = fnew[i];
	}

	//�g���N��Projection�@
	//fnew�S���g���čs��
	ProjectionTorque(fnew);

	//���̂ɉ�����(�e�g���N��Projection�ȍ~�̂��Ƃ��)
	for(int j = 3; j < 6; j++){
		df[j] = fnew[j] - f[j];
		CompResponse(df[j],j);
		f[j] = fnew[j];
	}
}

/**
<< void ProjectionTorque(SpatialVector fnew) >>
====================================================================================
  brief		IterateLCP�ŌĂ΂��g���N�̐������s���֐�
  param		SpatialVector fnew ��������O�̗͂ƃg���N�i�͂͐����ς݁j
  return	�Ȃ�
====================================================================================
�y��{�̗���z
	1. �͂̑傫�����O�̏ꍇ�̓g���N���O�Ƃ��ďI���
	2. �O�łȂ��Ƃ��A�܂���]���C�ƂȂ�g���N�𐧌�����D
	3�DZMP�����߂�
		3-1.�͂ƒ�������g���N�����߂�
		3-2.ZMP�~��=�g���N�̊֌W��,ZMP�͐ڐG���ʏ�ɂ���x���̍��W=0�ł��邱�Ƃ��狁�܂�
	4. ��p�_���ڐG�ʓ����ʊO�����ׁA�ʓ��Ȃ�I���
	5. �ʊO�ɂ���Ƃ��A��ԋ߂��_��V������p�_�Ƃ���
	6. �g���N�����ߒ����āAfnew�ɑ��
*/
void PHContactSurface::ProjectionTorque(SpatialVector& fnew){
	Vec3d F(f[0], f[1], f[2]);
	Vec3d N(fnew[3], fnew[4], fnew[5]);
	
	if(F.norm() != 0.0){	//�͂̑傫����0�łȂ��Ȃ琧������B
		//�܂���]���C�̐���
#if	USE_ROTATEFRICTION
		//��]���C�̎���
		//rotateFriction�ɖ��C�W���ƁA�����R�͂�segmentationNum�Ŋ��������̂�������
		double rotateFrictionLimit;//�ő�Î~��]���C
		rotateFrictionLimit = rotateFriction * 0.5 * (shapePair->shape[0]->GetMaterial().mu0 + shapePair->shape[1]->GetMaterial().mu0);
		if(segmentationNum != 0){
			rotateFrictionLimit *= f[0]/segmentationNum;
		}
		else{
			//cout << "segmentation = 0" << endl;
			rotateFrictionLimit *= f[0];
		}
		double activeRotateFriction;//��]�����C
		if (shapePair->shape[0]->GetMaterial().mu0 + shapePair->shape[1]->GetMaterial().mu0 == 0){
			activeRotateFriction = 0;
		}else{
			activeRotateFriction = (shapePair->shape[0]->GetMaterial().mu + shapePair->shape[1]->GetMaterial().mu)
			/ (shapePair->shape[0]->GetMaterial().mu0 + shapePair->shape[1]->GetMaterial().mu0)
			* rotateFrictionLimit;	
		}
		//�ڐG�ʂ̐��������̊p���x���������ĂȂ��Ȃ�Î~���C�A���Ă���Γ����C
		if (-0.01 < vjrel[3] && vjrel[3] < 0.01){	//	�Î~���C
			if (fnew[3] > rotateFrictionLimit) fnew[3] = activeRotateFriction;
			else if (fnew[3] < -rotateFrictionLimit) fnew[3] = -activeRotateFriction;
		}else{					//	�����C
			if (fnew[3] > activeRotateFriction) fnew[3] = activeRotateFriction;
			else if (fnew[3] < -activeRotateFriction) fnew[3] = -activeRotateFriction;		
		}
#endif
		N.x = fnew[3];
		
		//��]���C�ȊO�̃g���N�̐���
		//ZMP�����߂�		

		//N��F�ƒ������Ă���Ƃ͌���Ȃ�
		//N��F�ɒ������Ă��鐬����ZMP�����߂Đ�������
		Vec3d N_orthogonalF;//N��F�ɒ������Ă��鐬���x�N�g��
		Vec3d N_parallelF;//N��F�����x�N�g��

		N_parallelF = N*F.unit()*F.unit();
		N_orthogonalF = N - N_parallelF;

		fpoint.x = 0.0;
		fpoint.y = -N_orthogonalF.z / F.x;
		fpoint.z = N_orthogonalF.y / F.x;

		//�ʓ����ʊO��
		int L, R;	//�ʂ̂ǂ̗̈�ɍ�p�_�����邩�ێ����邽�߂̕ϐ� L �ʊO���猩�ĕӂ̍����̒��_�@R �E���̒��_
		if(!CheckInside(L, R)){
			//�ʊO�Ȃ�ZMP��ʓ��ɒ���
			//�����ɉ��낹�邩�ǂ����̔���
			if(lastL != lastR){
				//�J��Ԃ��v�Z��2��ڂ���͑O��A�ŋߓ_���������̈悩��T��������B
				//lastL,lastR�͑O��A�ŋߓ_���������̈�������Ă���B
				L = lastL;
				R = lastR;
			}
			if(dot(section[L]-section[R],fpoint-section[R])>=0 && dot(section[R]-section[L],fpoint-section[L])>=0 && CalcSignedArea(section[L],section[R],fpoint) < 0){
				//���������낹��ꍇ
				//ZMP�ɐ����̒����~�ӂ̖@���P�ʃx�N�g���𑫂��Č�_�����߂�
				//�ӂ̖@���P�ʃx�N�g���i�ڐG�ʓ��ւ́j�����߂�
				//��RL�̃x�N�g����(0,section[L].y - section[R].y, section[L].z - section[R].z)
				//����ɒ�������x�N�g����2��ނ����Ă��̂����ڐG�ʓ��ւ̃x�N�g������
				Vec3d side_normal;
				side_normal.x = 0;
				side_normal.y = section[L].z - section[R].z;
				side_normal.z = -(section[L].y - section[R].y);
				side_normal.unitize();

				//�����̒��������߂�
				double dist_of_normal;
				Vec3d tempVec = (section[L]-section[R]) ^ (fpoint-section[R]);
				Vec3d tempVec2(section[L]-section[R]);
				dist_of_normal = tempVec.norm() / tempVec2.norm();

				//�V������p�_�����߂�
				//��p�_�ɐ����̒����{�����ӂ̖@���x�N�g��(������)�𑫂�
				fpoint = fpoint + (dist_of_normal * side_normal);
			}
			else{
				//���낹�Ȃ���΁A�ŋߓ_��T�����ŋߓ_����p�_�Ƃ���
				SearchClosestPoint(L, R);
			}
			//�V����ZMP���琧�����ꂽ�g���N�����߂�
			N_orthogonalF = cross(fpoint, F);
			N = N_orthogonalF;
			fnew[4] = N[1];
			fnew[5] = N[2];
		}
	}
	else{		//�͂̑傫����0�Ȃ�g���N0�ɂ���B
		fnew[3] = fnew[4] = fnew[5] = 0.0;
	}
}

/**
<< void IntegrateArea() >>
====================================================================================
  brief		��]���C�����߂�ׂɐڐG�ʂ̐ϕ����s���֐�
====================================================================================
*/
void PHContactSurface::IntegrateArea()
{
	std::vector<Spr::Vec3d> Factor; //�ڐG�ʂ̕ӂ�\�������̕������̌W���@ay+bz=c��a,b,c��(x,y,z)�ɑΉ�
	std::vector<Spr::Vec2d> RangeY; //������y���W�͈̔�(��,��)��(x,y)�ɑΉ�
	std::vector<Spr::Vec2d> RangeZ; //������z���W�͈̔�(��,��)��(x,y)�ɑΉ�
	double y_min,y_max;//�ϕ��͈�
	double z_min,z_max;//�ϕ��͈�(�ey�ɑ΂��Đݒ肷��)

	Factor.resize(section.size());
	RangeY.resize(section.size());
	RangeZ.resize(section.size());
	for(unsigned int i=0; i<section.size();i++){
		//�W��a,b,�萔c�̐ݒ�
		//�W��a:Factor[i].x,�W��b:Factor[i].y,�萔c:Factor[i].z
		//section��y,z����ݒ�.2�_��ʂ钼���̕���������
		int s0 = i;
		int s1 = i+1;
		if(s1 == section.size()){
			s1 = 0;
		}

		Factor[i].x =(section[s1].z-section[s0].z);
		Factor[i].y = (section[s0].y-section[s1].y);
		Factor[i].z = (section[s0].y*section[s1].z - section[s1].y*section[s0].z);
		
		//y���W�͈̔͂�ݒ�
		//Range.x ���������@Range.y �傫����
		//section��y����ݒ�
		if(section[s0].y<=section[s1].y){
			RangeY[i].x = section[s0].y;
			RangeY[i].y = section[s1].y;
		}
		else{
			RangeY[i].x = section[s1].y;
			RangeY[i].y = section[s0].y;
		}
		//y���W�̑S�͈̂̔� (y_min,y_max)��ݒ�
		if(i==0){
			y_min = RangeY[i].x;
			y_max = RangeY[i].y;
		}
		else{
			if(y_min > RangeY[i].x){
				y_min = RangeY[i].x;
			}
			if(y_max < RangeY[i].y){
				y_max = RangeY[i].y;
			}
		}

		//z���W�͈̔͂�ݒ�
		if(section[s0].z<=section[s1].z){
			RangeZ[i].x = section[s0].z;
			RangeZ[i].y = section[s1].z;
		}
		else{
			RangeZ[i].x = section[s1].z;
			RangeZ[i].y = section[s0].z;
		}
	}

	//ContactSurface�����錴�_����̋�����ϕ�
	double Y,Z;//�ϕ�����Y���W,Z���W
	Y=y_min;
	for(int i=0; Y<=y_max; i++){
		//Y���W������
		Y = y_min + interval*i;
		if(Y > y_max){
			Y = y_max;
		}
		
		std::vector<int> eq;//Y���͈͂ɂ��钼���ԍ�������
		//�ǂ̎���Y�͈̔͂ɂ��邩
		for(unsigned int j=0; j<RangeY.size(); j++){
			if((Y>=RangeY[j].x && Y<=RangeY[j].y)){
				eq.push_back(j);
			}
		}

		//�͈͓���Y������ӂ̎�����Z�͈̔�(z_min,z_max)�����߂�D
		//ay+bz=c z=c/b-(a/b)y
		double temp_z;
		for(unsigned int j=0; j<eq.size(); j++){
			if(fabs(Factor[eq[j]].y) < 1e-10){
				//y�̌W����0�̕ӂ̏ꍇ
				//���̕ӂ�z���W�����̂܂ܔ͈͂ƂȂ�
				z_min = RangeZ[eq[j]].x;
				z_max = RangeZ[eq[j]].y;
				break;
			}
			if(j==0){
				z_min = Factor[eq[j]].z/Factor[eq[j]].y-(Factor[eq[j]].x/Factor[eq[j]].y)*Y;
			}
			else{
				temp_z = Factor[eq[j]].z/Factor[eq[j]].y-(Factor[eq[j]].x/Factor[eq[j]].y)*Y;
				if(z_min <= temp_z){
					z_max = temp_z;
				}
				else{
					z_max = z_min;
					z_min = temp_z;
				}
			}
		}
		
		Z = z_min;
		//Z�͈̔͂Őϕ����s���D
		for(int j=0; Z<=z_max;j++){
			Z = z_min + interval*j;
			if(Z > z_max){
				Z = z_max;
			}
			rotateFriction += sqrt(Y*Y+Z*Z);
			segmentationNum++;
			if(Z==z_max){
				break;
			}
		}
		if(Y==y_max){
			break;
		}
	}
}

/**
<< double CalcSignedArea(Vec3d p, Vec3d q, Vec3d r) >>
====================================================================================
  brief		�O�p�`�̕����t���ʐς̌v�Z���s���֐�
  param		Vec3d	p,q,r	���_
  return	�����t���ʐ�
====================================================================================
*/
double PHContactSurface::CalcSignedArea(Vec3d p, Vec3d q, Vec3d r){
	return ( (p.y-r.y)*(q.z-r.z) + (q.y-r.y)*(r.z-p.z) ) / 2;
}

/**
<< bool CheckInside(void) >> 
====================================================================================
  brief		ZMPfpoint���ڐG�ʓ��ɂ��邩
  param		int& L	��p�_������̈�̒��_���L�^
			int& R�@��p�_������̈�̒��_���L�^
  return	�ʓ��Ftrue	�ʊO�Ffalse
====================================================================================
���ڐG�ʓ��̓_�͌��_�Ƃ��Ă���D
*/
bool PHContactSurface::CheckInside(int& L, int& R){
	int l,m,r;	//m �T���͈͂̒��_
	l = 0;
	r = section.size();	//�T���͈͂̏�����
	while(r-l>1){
		m = (l+r)/2;
		if(CalcSignedArea(section[l], Vec3d(0.0,0.0,0.0), section[m]) <= 0.0){/* if angle(l, g, m)<=180) */
			if(CalcSignedArea(section[l], Vec3d(0.0,0.0,0.0), fpoint) <= 0.0 && CalcSignedArea(fpoint, Vec3d(0.0,0.0,0.0), section[m]) <= 0.0){
				r = m;
			}
			else{
				l = m;
			}
		}
		else{/*angle(l, g, m) > 180*/
			if(CalcSignedArea(section[l], Vec3d(0.0,0.0,0.0), fpoint) >= 0.0 && CalcSignedArea(fpoint, Vec3d(0.0,0.0,0.0), section[m]) >= 0.0){
				l = m;
			}
			else{
				r = m;
			}
		}
	}
	if(r == section.size()){	//section[section.size()] �� section[0]�Ȃ̂�
		r = 0;
	}
	if(CalcSignedArea(section[l], section[r], fpoint) >= 0.0){
		return true;	//�ʓ�
	}
	else{ 
		L = l;
		R = r;
		return false;	//�ʊO
	}
}

/**
<< void PHContactSurface::SearchClosestPoint >>
====================================================================================
  brief		��p�_�̍ŋߓ_�������č�p�_�Ƃ���
  param		int L  ��p�_������̈�̒��_�i�ʊO���猩�č����j
			int R  ��p�_������̈�̒��_�i�ʊO���猩�ĉE���j
  return	�Ȃ�
====================================================================================
  �y��{�̗���z
  ��p�_������̈�̕ӂɍ�p�_���琂�������낹�Ȃ������ꍇ�ɓ���
  1.��p�_������̈�̕ӂ̒��_�ƍ�p�_�Ƃ̋������L�^
  ��p�_������̈�̗אڂ���̈�� ���v���Ɣ����v���̂Q�̏��ŒT�����n�߂�B
  ��p�_������̈�̕ӂ̒��_�ŁA��p�_�Ƃ̋���������Ă��鑤�͒T�������Ȃ��B
  2.�אڂ���̈�̕ӂɐ��������낹�邩���m�F���遨���낹����A���̏����ł̒T�����I����B
  3.���낹�Ȃ������ꍇ�A�ӂ̒��_�ƍ�p�_�Ƃ̋����ƁA�L�^���Ă��鋗�����ׂ�B
  4-1.���Z�������ꍇ�A�������L�^������ɐ�̗̈��2����J��Ԃ��B
  4-2.��蒷�������ꍇ�A�T�����I�����A�Z�����_����p�_�̌��Ƃ��A���̏����ł̒T�����I����B
  
  �����̏����ŒT�����I������
  5.�̈�����v���Ɣ����v���ɒT�����I�������ƂɂȂ�̂ŁA��p�_�̌����ׁA���Z��������p�_�Ƃ���B
*/
void PHContactSurface::SearchClosestPoint(int L, int R){
	
	Vec3d L_fp(fpoint-section[L]);  //���_�����p�_�ւ̃x�N�g��
	Vec3d R_fp(fpoint-section[R]);
	double L_distance = L_fp.norm(); //���_�����p�_�܂ł̋���
	double R_distance = R_fp.norm(); 
	
	Vec3d ClosestPointL=section[L];	//���v���̒T���ł̍ŋߓ_�̃x�N�g��
	Vec3d ClosestPointR=section[R];	//�����v���̒T���ł̍ŋߓ_�̃x�N�g��
	
	int l,r;
	
	for(unsigned int i=0; i<section.size(); i++,L--){	//���v���̒T�� 
		//�ŏ���L�̒��_��R�̒��_����p�_���痣��Ă��邩���ׁA����Ă���ΏI���ɂ���
		if(i==0 && L_distance > R_distance){
			break;
		}
		if(L<0){
			L = section.size()-1;
		}
		if(L==0){
			l = section.size()-1;
			r = L;
		}
		else{
			l = L-1;
			r = L;
		}		
		if(dot(section[l]-section[r],fpoint-section[r])>=0 && dot(section[r]-section[l],fpoint-section[l])>=0 && CalcSignedArea(section[l], section[r], fpoint) < 0){
			//�ʓ���ʂ�Ȃ����������낹��ꍇ			
			
			//�Ӓ������ڐG�ʓ��������P�ʃx�N�g�������߂�
			Vec3d side_normal;
			side_normal.x = 0;
			side_normal.y = section[l].z - section[r].z;
			side_normal.z = -(section[l].y - section[r].y);
			side_normal.unitize();

			//�����̒��������߂�
			double dist_of_normal;
			Vec3d tempVec  = (section[l]-section[r]) ^ (fpoint-section[r]);
			Vec3d tempVec2 = section[l]-section[r];
			dist_of_normal = tempVec.norm() / tempVec2.norm();

			//�V����ZMP�����߂�
			//ZMP�ɐ����̒����{�����ӂ̖@���x�N�g��(������)�𑫂�
			ClosestPointL = fpoint + (dist_of_normal * side_normal);
			break;
		}
		else{
			L_fp = fpoint-section[l];
			if(L_distance > L_fp.norm()){
				//�Z���Ȃ����ꍇ
				L_distance = L_fp.norm();
				ClosestPointL = section[l];
			}
			else{
				//�����Ȃ����ꍇ
				if(i != 0){
					L++;
					if(L == section.size()){
						L = 0;
					}
				}
				break;
			}
		}
	}

	for(unsigned int i=0; i<section.size(); i++,R++){	//�����v���̒T��
		//�ŏ���R�̒��_��L�̒��_����p�_���痣��Ă��邩���ׁA����Ă���ΏI���ɂ���
		if(i==0 && R_distance > L_distance){
			break;
		}
		if(R>=(signed)section.size()){
			R = 0;
		}
		if(R==section.size()-1){
			l = R;
			r = 0;
		}
		else{
			l = R;
			r = R+1;
		}		
		if(dot(section[l]-section[r],fpoint-section[r])>=0 && dot(section[r]-section[l],fpoint-section[l])>=0 && CalcSignedArea(section[l], section[r], fpoint) < 0){
			//�ʓ���ʂ�Ȃ����������낹��ꍇ

			//�ӂɒ������ڐG�ʓ��������P�ʃx�N�g�������߂�
			Vec3d side_normal;
			side_normal.x = 0;
			side_normal.y = section[l].z - section[r].z;
			side_normal.z = -(section[l].y - section[r].y);
			side_normal.unitize();

			//�����̒��������߂�
			double dist_of_normal;
			Vec3d tempVec  = (section[l]-section[r]) ^ (fpoint-section[r]);
			Vec3d tempVec2 = section[l]-section[r];
			dist_of_normal = tempVec.norm() / tempVec2.norm();

			//�V����ZMP�����߂�
			//ZMP�ɐ����̒����{�����ӂ̖@���x�N�g��(������)�𑫂�
			ClosestPointR = fpoint + (dist_of_normal * side_normal);
			break;
		}
		else{
			R_fp = fpoint-section[r];
			if(R_distance > R_fp.norm()){
				//�Z���Ȃ����ꍇ
				R_distance = R_fp.norm();
				ClosestPointR = section[r];
			}
			else{
				//�����Ȃ����ꍇ
				if(i != 0){
					R--;
					if(R < 0){
						R = section.size()-1;
					}
					break;
				}
			}
		}
	}
	
	L_fp = fpoint-ClosestPointL;
	R_fp = fpoint-ClosestPointR;
	
	if(L_fp.norm() <= R_fp.norm()){
		fpoint = ClosestPointL;
		lastL = L-1;
		if(lastL < 0){
			lastL = section.size() - 1;
		}
		lastR = L;
	}
	else{
		fpoint = ClosestPointR;
		lastL = R;
		lastR = R+1;
		if(lastR == section.size()){
			lastR = 0;
		}
	}
}

}
