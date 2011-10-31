/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <Collision/CDDetectorImp.h>
#include <Collision/CDConvexMesh.h>
#include <Collision/CDSphere.h>

namespace Spr {;
const double epsilon = 1e-8;
const double epsilon2 = epsilon*epsilon;

bool bUseContactVolume=true;

bool CDShapePair::Detect(unsigned ct, const Posed& pose0, const Posed& pose1){
	shapePoseW[0] = pose0;
	shapePoseW[1] = pose1;
	
	Vec3d sep;
	bool rv = FindCommonPoint(shape[0], shape[1], shapePoseW[0], shapePoseW[1], sep, closestPoint[0], closestPoint[1]);
	if (rv){
		commonPoint = shapePoseW[0] * closestPoint[0];
		if (lastContactCount == unsigned(ct-1))
			 state = CONTINUE;
		else state = NEW;
		lastContactCount = ct;
	}else{
		state = NONE;
	}
	return rv;
}

void SaveShape(std::ostream& file, CDShape* a);
CDConvex* LoadShape(std::istream& file, PHSdkIf* sdk);
void SaveDetectContinuously(CDShapePair* sp, unsigned ct, const Posed& pose0, const Vec3d& delta0, const Posed& pose1, const Vec3d& delta1){
	std::ofstream file("DetectContinuouslySaveParam.txt");
	file.precision(12);
	file << sp->normal << std::endl;
	file << sp->lastContactCount << std::endl;
	SaveShape(file, sp->shape[0]);
	SaveShape(file, sp->shape[1]);
	file << ct << std::endl;
	file << pose0 << std::endl;
	file << delta0 << std::endl;
	file << pose1 << std::endl;
	file << delta1 << std::endl;	
}
void CallDetectContinuously(std::istream& file, PHSdkIf* sdk){
	CDShapePair* sp = DBG_NEW CDShapePair;
	file >> sp->normal;
	file >> sp->lastContactCount;
	sp->shape[0] = LoadShape(file, sdk);
	sp->shape[1] = LoadShape(file, sdk);
	unsigned ct;
	file >> ct;
	Posed pose0, pose1;
	Vec3d delta0, delta1;
	file >> pose0 >> delta0 >> pose1 >> delta1;
	sp->DetectContinuously(ct, pose0, delta0, pose1, delta1);
	DSTR << sp->normal << std::endl;
}


bool CDShapePair::DetectContinuously(unsigned ct, const Posed& pose0, const Vec3d& delta0, const Posed& pose1, const Vec3d& delta1){
	//	for debug dump
	Vec3d lastNormal = normal;
	int lastLCC = lastContactCount;


	shapePoseW[0] = pose0;
	shapePoseW[1] = pose1;	
	if (lastContactCount == unsigned(ct-1) ){	
		//	�Q��ڈȍ~�̐ڐG�̏ꍇ
		shapePoseW[0].Pos() += delta0;
		shapePoseW[1].Pos() += delta1;	//	�ŏ����猻�݂̈ʒu�Ɉړ�������

		double dist;
		//	�@�������ɔ��肷��Ƃǂꂾ���߂�Ɨ���邩������D
		int res=ContFindCommonPoint(shape[0], shape[1], shapePoseW[0], shapePoseW[1], 
			-normal, -DBL_MAX, 0, normal, closestPoint[0], closestPoint[1], dist);

		if (res <= 0) {	//	�͈͓��ł́A�ڐG���Ă��Ȃ��ꍇ
			return false;
		}
		depth = -dist;
		center = commonPoint = shapePoseW[0] * closestPoint[0] - 0.5*normal*depth;
		goto found;
	}else{
		//	���߂Ă̐ڐG�̏ꍇ
		Vec3d delta = delta1-delta0;
		double end = delta.norm();
		if (end < epsilon){	//	���x���ق�0�̏ꍇ
			shapePoseW[0].Pos() += delta0;
			shapePoseW[1].Pos() += delta1;
		}else{				//	 ���x������ꍇ
			Vec3d dir = delta / end;
			double dist;
			int res=ContFindCommonPoint(shape[0], shape[1], shapePoseW[0], shapePoseW[1], 
				dir, -DBL_MAX, end, normal, closestPoint[0], closestPoint[1], dist);
			if (res <= 0) return false;
			if (!(0.9 < normal.norm() && normal.norm() < 1.1)){
				DSTR << "normal error in " << std::endl;
				int res=ContFindCommonPoint(shape[0], shape[1], shapePoseW[0], shapePoseW[1], 
					dir, -DBL_MAX, end, normal, closestPoint[0], closestPoint[1], dist);
			}

			if (dist >= 0){	//	����̈ړ��ŐڐG���Ă����
				double toi = dist / end;
				shapePoseW[0].Pos() += toi*delta0;
				shapePoseW[1].Pos() += toi*delta1;
				center = commonPoint = shapePoseW[0] * closestPoint[0];
//				shapePoseW[0].Pos() -= dir*1e-8;	//�m���Ɍ�����������邽�� 1e-8�]���ɓ�����
//				shapePoseW[1].Pos() += dir*1e-8;	//�m���Ɍ�����������邽�� 1e-8�]���ɓ�����
				//	�������̌`��̌v�Z�́C�Փˎ��_�̈ʒu�ōs�����Cdepth �͌����_��depth
//				depth = -(1-toi) * delta * normal + 2e-8;
				depth = -(1-toi) * delta * normal;
				goto found;
			}
			//	�ߋ��ɐڐG���Ă����Ƃ�������F��]�ŐڐG���N�����ƍl������B
			//	�Ƃ肠�����A���݂̈ʒu�ŐڐG���Ă��邩�ǂ����m�F����B
			shapePoseW[0].Pos() += delta0;
			shapePoseW[1].Pos() += delta1;
			double tmp;
			if (ContFindCommonPoint(shape[0], shape[1], shapePoseW[0], shapePoseW[1], 
				-dir, -DBL_MAX, 0, normal, closestPoint[0], closestPoint[1], tmp) <= 0)
				return false;	//	�ڐG���Ă��Ȃ��ꍇ�͔�����B
		}
		/*	���x0�̏ꍇ�A�܂��� toi < 0 �̏ꍇ�A�����ɗ���B
			���̂悤�Ȃ��Ƃ��N���錴���ɂ́A���̉\��������B
			- ��]�������ŐڐG���N�������߁A�d�S���x�Ɋ�Â�toi�ł́A�ڐG���o�ł��Ȃ��B
			- ���x������������toi���v�Z�ł��Ȃ��B
			- �ŏ�����ڐG���Ă����B
			- ���[�U�ɂ��񕨗��ړ��������ŐڐG���N�������߁Atoi�ŐڐG���o�ł��Ȃ��B

			�~���̂悤�ȏꍇ�́A�`��̒��S�Ԃ����ԃx�N�g�������@���Ƃ��āA
			�~���@���̌����ŐڐG�@�������߂Ă����{�@���Ƃ���B
			�~�{�@���̌����ŁA�N���ʂƖ@���A�ŋߖT�_���v�Z����B
			�~���̏����ł́A�Ⴆ�΍L�����̏�̏����ȃT�C�R�����������ɔ��ł����Ƃ���
			�~��肪�N����B
				//	���@���i�`��̒��S�����Ԍ����j�̌v�Z
				Vec3d tmpNormal = shapePoseW[1]*shape[1]->CalcCenterOfMass() - shapePoseW[0]*shape[0]->CalcCenterOfMass();
				double norm = tmpNormal.norm();
				if (norm > epsilon) tmpNormal /= norm;
				else tmpNormal = Vec3d(0,1,0);
				double dist;

				//	���@���̌����ŐڐG�@�������߂�B
				int res = ContFindCommonPoint(shape[0], shape[1], shapePoseW[0], shapePoseW[1], 
					-tmpNormal, -DBL_MAX, 0, normal, closestPoint[0], closestPoint[1], dist);
				if (res <= 0) return false;

				//	�@�����X�V���Ă�����x����Ă݂�B
				res = ContFindCommonPoint(shape[0], shape[1], shapePoseW[0], shapePoseW[1], 
					-normal, -DBL_MAX, 0, normal, closestPoint[0], closestPoint[1], dist);
				if (res <= 0) return false;
				depth = -dist;
				center = commonPoint = shapePoseW[0] * closestPoint[0];
				center -= 0.5f*depth*normal;
				goto found;
				
			�����ŁA��L�͂�߂āA
			����Ă����͂��̑O�X�e�b�v�̎p���ɖ߂��āA���ʂ�GJK������Ă݂��
			�@�������܂�͂��B
			���̖@���̌����ɁA���X�e�b�v�̈ʒu����߂��āA�ڐG�_����������
		*/
		//	���ʂ�GJK�Ŗ@�����v�Z
		Posed shapePoseWPrev[2] = {
			shapePoseW[0],
			shapePoseW[1]
		};
		FindClosestPoints(shape[0], shape[1], shapePoseWPrev[0], shapePoseWPrev[1], 
			normal, closestPoint[0], closestPoint[1]);
		double dist;
		//	���߂��@���ŐڐG�ʒu�����߂�
		int res = ContFindCommonPoint(shape[0], shape[1], shapePoseW[0], shapePoseW[1], 
			-normal, -DBL_MAX, 0, normal, closestPoint[0], closestPoint[1], dist);
		if (res <= 0) return false;
		depth = -dist;
		center = commonPoint = shapePoseW[0] * closestPoint[0];
		center -= 0.5f*depth*normal;
		goto found;
	}
found:;
	if (lastContactCount == unsigned(ct-1)){
		state = CONTINUE;
	}else{
		state = NEW;
		static bool bShow = false;
		if (bShow){
			DSTR << "New contact: " << shape[0]->GetName() << "-" << 
				shape[1]->GetName() << std::endl;
		}
	}
	lastContactCount = ct;

	//	debug dump
	if (depth > 10){
		DSTR << "depth=" << depth << std::endl;
		UTRef<CDShapePair> sp = new CDShapePair(*this);
		sp->lastContactCount = lastLCC;
		sp->normal = lastNormal;
		SaveDetectContinuously(sp, ct, pose0, delta0, pose1, delta1);
		DSTR << "SaveDetectDetectContinuously() called" << std::endl;
		assert(0);
	}
	return true;
}

/*	��{���j10.31
�EGJK�̐��x������ �� �@������ �� �ςɐ[���N����
�E�O�̃X�e�b�v�ł��G��Ă� �� GJK�ł͖@�����킩��Ȃ�


�E�O�̃X�e�b�v�i���x�̌����ɏ����]���ɖ߂��Ă��ǂ��j����A
	��]��������������
	1. �ڐG���Ă��Ȃ��ꍇ
		���x�̌����ɐڐG����܂ł����߂�

	2.�ڐG���Ă���ꍇ
		��]������O�̎p���ɖ߂�
		���ʂ�GJK
			- �@����������Ƌ��܂�΁A���̖@�����g��
			- �������߂��� or �ڐG�ς݂̏ꍇ
				- �ȑO�̖@��������O�Ȃ炻����g��
				- �ȑO�̖@����������΁A�ڐG�̐ς���@�������߂�B

*/
bool CDShapePair::DetectContinuously2(unsigned ct, const Posed& pose0, const Posed& pose1, 
		const Vec3d& shapeCenter0, const Vec3d& shapeCenter1, SpatialVector& v0, SpatialVector& v1, Vec3d& cog0, Vec3d cog1, double dt){
	//	for debug dump
	Vec3d lastNormal = normal;
	int lastLCC = lastContactCount;
	shapePoseW[0] = pose0;
	shapePoseW[1] = pose1;	
	if (lastContactCount == unsigned(ct-1) ){	//	�p�������ڐG�̏ꍇ
		//	�@�������ɔ��肷��Ƃǂꂾ���߂�Ɨ���邩���ׂ�D
		double dist;
		int res=ContFindCommonPoint(shape[0], shape[1], shapePoseW[0], shapePoseW[1], 
			-normal, -DBL_MAX, 0, normal, closestPoint[0], closestPoint[1], dist);
		if (res <= 0) {	//	�͈͓��ł́A�ڐG���Ă��Ȃ��ꍇ
			return false;
		}
		depth = -dist;
		center = commonPoint = shapePoseW[0] * closestPoint[0] - 0.5*normal*depth;
		goto found;
	}else{										//	���߂Ă̐ڐG�̏ꍇ
		//	���i�̂�1�X�e�b�v��*���߂�
		double alpha = 2;
		Vec3d delta0 = (v0.v() + (v0.w() ^  (shapeCenter0-cog0)))  * dt * alpha;
		Vec3d delta1 = (v1.v() + (v1.w() ^  (shapeCenter1-cog1)))  * dt * alpha;
		Vec3d delta = delta1-delta0;
		double end = delta.norm();
		Vec3d dir;
		if (end > epsilon){	//	���x������ꍇ
			dir= delta / end;
			shapePoseW[0].Pos() -= delta0;
			shapePoseW[1].Pos() -= delta1;
			double dist;
			int res=ContFindCommonPoint(shape[0], shape[1], shapePoseW[0], shapePoseW[1], 
				dir, 0, end, normal, closestPoint[0], closestPoint[1], dist);
			if (res == 0 || res==-1) return false;	//	�O�̈ʒu���瑬�x�̌����ɓ������Ă��ڐG���Ȃ��ꍇ
			if (res == 1){	//	����̈ړ��ŐڐG���Ă����
				double toi = dist / end;
				shapePoseW[0].Pos() += toi*delta0;
				shapePoseW[1].Pos() += toi*delta1;
				center = commonPoint = shapePoseW[0] * closestPoint[0];
				depth = -(1-toi) * delta * normal;
				if (depth <= 0){
					//	delta�̌����ɐi��ōs���āA�ڐG�����@���� normal������A
					//	normal * delta < 0�ɂȂ�͂������A������ꍇ�A�v�Z�덷��>=0�ɂȂ邱�Ƃ�����B
					DSTR << "depth:" << depth << " delta * normal >= 0" << std::endl;
					return false;
				}
				goto found;
			}
		}
		//	���i�ʒu�����ɖ߂�
		shapePoseW[0].Pos() += delta0;
		shapePoseW[1].Pos() += delta1;
		if (end > epsilon){	//	���x������ꍇ
			//	�Ƃ肠�����A���݂̈ʒu���瑬�x�̋t�����ɃX�C�[�v�����āA�ǂ����ŐڐG���N���Ă邩�m�F
			double tmp;
			if (ContFindCommonPoint(shape[0], shape[1], shapePoseW[0], shapePoseW[1], 
				-dir, -DBL_MAX, 0, normal, closestPoint[0], closestPoint[1], tmp) <= 0)
				return false;	//	�ǂ��ł��ڐG���Ă��Ȃ��ꍇ�͔�����
		}
		//	����̈ړ��ŐڐG���Ȃ����A���łɐڐG���Ă���\��������B
		//	1�X�e�b�v�O�̎p��(=�ڐG����O)�ɖ߂��B
		Posed shapePoseWPrev[2] = { shapePoseW[0], shapePoseW[1] };
		delta0 = (v0.v() + (v0.w() ^  (shapeCenter0-cog0)))  * dt;
		delta1 = (v1.v() + (v1.w() ^  (shapeCenter1-cog1)))  * dt;
		shapePoseWPrev[0].Pos() -= delta0;
		shapePoseWPrev[1].Pos() -= delta1;
		Quaterniond dAng0_ = Quaterniond::Rot(-v0.w() * dt);
		Quaterniond dAng1_ = Quaterniond::Rot(-v1.w() * dt);
		shapePoseWPrev[0].Ori() = dAng0_ * shapePoseWPrev[0].Ori();
		shapePoseWPrev[1].Ori() = dAng1_ * shapePoseWPrev[1].Ori();
		//	���ʂ�GJK�Ŗ@�����v�Z ���߂Ă̐ڐG������A1�X�e�b�v�O�ɂ͐ڐG���Ă��Ȃ������͂��B
		Vec3d tmpNormal;
		double dist = FindClosestPoints(shape[0], shape[1], shapePoseWPrev[0], shapePoseWPrev[1], 
			tmpNormal, closestPoint[0], closestPoint[1]);
		if (dist > 1e-4){
			tmpNormal *= -1.0/dist;
		}else{
			//	�������߂����A���łɏd�Ȃ��Ă��Ă��܂��@�����v�Z�ł��Ȃ������B
			//	���߂ɐڐG���������Ȃ�΁A���̎��̖@�����g���B
			if (lastContactCount!=-2 && ct - lastContactCount <10){
				tmpNormal = lastNormal;
				assert(tmpNormal.norm() > epsilon);
			}else{
				//	���߂Ă̐ڐG�ł��A�ŏ�����ڐG���Ă���ꍇ
				//	�d���Ȃ��̂ŁA�U�����ɂ��炵�ĐڐG���������Ă݂āA��Ԉړ��ʂ����Ȃ��������̗p����B
				Vec3d tmpN[] = {Vec3d(0,0,1), Vec3d(0,0,-1), Vec3d(0,1,0), Vec3d(0,-1,0), Vec3d(1,0,0), Vec3d(-1,0,0)};
				double tmpDist, minDist=-DBL_MAX;
				for(int i=0; i<6; ++i){				
					int res=ContFindCommonPoint(shape[0], shape[1], shapePoseW[0], shapePoseW[1], 
						-tmpN[i], -DBL_MAX, 0, normal, closestPoint[0], closestPoint[1], tmpDist);
					if (res>0){
						if (tmpDist > minDist){
							minDist = tmpDist;
							if (normal*tmpN[i] > 0) tmpNormal = normal;
							else tmpNormal = tmpN[i];
						}
					}
				}
				//DSTR << "minDist:" << minDist << " normal:" << tmpNormal << std::endl;
			}
		}
		//	���߂��@���ŐڐG�ʒu�����߂�
		//	�@�������ɔ��肷��Ƃǂꂾ���߂�Ɨ���邩������D
		int res=ContFindCommonPoint(shape[0], shape[1], shapePoseW[0], shapePoseW[1], 
			-tmpNormal, -DBL_MAX, 0, normal, closestPoint[0], closestPoint[1], dist);
		if (res <= 0) {	//	�@���̌����ɗ����Ă��猻�݈ʒu�܂ŋ߂Â��Ă��ڐG���N���Ȃ��ꍇ�Ȃ̂ŁA�ڐG�Ȃ��B
			return false;
		}
		depth = -dist;
		center = commonPoint = shapePoseW[0] * closestPoint[0] - 0.5*normal*depth;
		if (depth > 5 || depth < 0){
			DSTR << "depth:" << depth << std::endl;
			dist = FindClosestPoints(shape[0], shape[1], shapePoseWPrev[0], shapePoseWPrev[1], 
				tmpNormal, closestPoint[0], closestPoint[1]);
			assert(0);
		}
		goto found;
	}
found:;
	if (lastContactCount == unsigned(ct-1)){
		state = CONTINUE;
	}else{
		state = NEW;
		static bool bShow = false;
		if (bShow){
			DSTR << "New contact: " << shape[0]->GetName() << "-" << 
				shape[1]->GetName() << std::endl;
		}
	}
	lastContactCount = ct;

	//	debug dump
	if (depth > 5 || depth < 0){
		DSTR << "depth=" << depth << std::endl;
		UTRef<CDShapePair> sp = new CDShapePair(*this);
		sp->lastContactCount = lastLCC;
		sp->normal = lastNormal;
		assert(0);
	}
	return true;
}


void CDShapePair::CalcNormal(){
	if (state == NEW){
		//	�ʌ`��̒��S�𗣂����������@���ɂ���D
		normal = shapePoseW[1]*shape[1]->CalcCenterOfMass() - shapePoseW[0]*shape[0]->CalcCenterOfMass();
		double norm = normal.norm();
		if (norm>epsilon) normal /= norm;
		else normal = Vec3d(0,1,0);
	}
	//	�O��̖@���̌����ɓ������āC�ŋߖT�_�����߂�
	Vec3d n = normal;
	int res = ContFindCommonPoint(shape[0], shape[1], shapePoseW[0], shapePoseW[1], 
		-normal, -DBL_MAX, 0, normal, closestPoint[0], closestPoint[1], depth);
	if (res <= 0){
		DSTR << "Error in CalcNormal(): res:" << res << "dist:" << depth << n << std::endl;
		ContFindCommonPointSaveParam(shape[0], shape[1], shapePoseW[0], shapePoseW[1], 
			-n, -DBL_MAX, 0, n, closestPoint[0], closestPoint[1], depth);
	}
	depth *= -1;
	center = shapePoseW[0] * closestPoint[0];
	center -= 0.5f*depth*normal;
}


void CDContactAnalysisFace::Print(std::ostream& os) const {
	os << normal << "*" << dist;
}

Vec3f CDContactAnalysisFace::CommonVtx(int i){
	/**car2.x��dualPlanes[i]->dist��0.000000000�ɂȂ�**/
	double dist = dualPlanes[i]->dist;
#ifdef _DEBUG
	if (dist < CD_EPSILON){
		DSTR << "Error: dist " << dist << std::endl;
		DSTR << "normal:" << dualPlanes[i]->normal << std::endl;
		Vec3d posAv;
		for(int j=0; j<3; ++j){
			DSTR << "Point" << j << ":" << dualPlanes[i]->vtx[j]->GetPos() << std::endl;
			posAv += dualPlanes[i]->vtx[j]->GetPos();
		}
		DSTR << "PosAve:" << posAv << std::endl;
		Vec3d pos0 = dualPlanes[i]->vtx[0]->GetPos();
		Vec3d pos1 = dualPlanes[i]->vtx[1]->GetPos();
		Vec3d pos2 = dualPlanes[i]->vtx[2]->GetPos();
		Vec3d a = pos1 - pos0;
		Vec3d b = pos2 - pos0;
		DSTR << "a:" << a << "b:" << b << std::endl;
		Vec3d normal = a ^ b;
		assert(normal.norm());
		normal.unitize();
		dist = pos0 * normal;
	}
#endif
	return Vec3f(dualPlanes[i]->normal / dist);
}

bool CDContactAnalysisFace::CalcDualVtx(Vec3f* base){
	normal = (base[face->vtxs[1]] - base[face->vtxs[0]]) ^ 
		(base[face->vtxs[2]] - base[face->vtxs[0]]);
	DEBUG_EVAL( if ( normal.norm() < CD_EPSILON || !finite(normal.norm()) ){
		DSTR << "normal is too small." << std::endl; } 
	)
	normal.unitize();
	dist = normal * base[face->vtxs[0]];
#if 1	//	debug	hase
	if (dist < -1e-3){
		DSTR << "Error: distance=" << dist << " < 0" << std::endl;
		return false; 
	}
#endif
	if (dist < CD_EPSILON) dist = CD_EPSILON;
	if (dist > CD_INFINITE) dist = CD_INFINITE;
	return true;
}

//----------------------------------------------------------------------------
//	CDContactAnalysis
//
#define CONTACT_ANALYSIS_BUFFER	2000
CDContactAnalysis::VtxBuffer CDContactAnalysis::vtxBuffer(CONTACT_ANALYSIS_BUFFER);
CDContactAnalysis::Vtxs CDContactAnalysis::vtxs(CONTACT_ANALYSIS_BUFFER);
CDQHPlanes<CDContactAnalysisFace> CDContactAnalysis::planes(CONTACT_ANALYSIS_BUFFER);
CDContactAnalysisFace** CDContactAnalysis::FindIntersection(CDShapePair* cp){
	planes.Clear();
	vtxBuffer.clear();
	vtxs.clear();
	if (bUseContactVolume){
		if (DCAST(CDConvexMesh, cp->shape[0]) && DCAST(CDConvexMesh, cp->shape[1])){
			isValid = true;
			CDConvexMesh* poly[2];
			poly[0] = (CDConvexMesh*) cp->shape[0];
			poly[1] = (CDConvexMesh*) cp->shape[1];
			for(int i=0; i<2; ++i){
				Posed afw = cp->shapePoseW[i];
				afw.Pos() -= cp->commonPoint;
				tvtxs[i].resize(poly[i]->base.size());
				for(unsigned v=0; v<tvtxs[i].size(); ++v){
					tvtxs[i][v] = afw * poly[i]->base[v];
				}
				for(CDFaces::iterator it = poly[i]->faces.begin();
					it != poly[i]->faces.begin() + poly[i]->nPlanes; ++it){
					vtxBuffer.push_back(CDContactAnalysisFace());
					vtxBuffer.back().face = &*it;
					vtxBuffer.back().id = i;
					if (!vtxBuffer.back().CalcDualVtx(&*tvtxs[i].begin())){
						DSTR << "Common Local: " << cp->shapePoseW[i].Inv() * cp->commonPoint 
							<< std::endl;
						for(unsigned int v=0; v<poly[i]->base.size(); ++v){
							DSTR << poly[i]->base[v] << std::endl;
						}
						assert(0);
					}
				}
			}
			for(VtxBuffer::iterator it=vtxBuffer.begin(); it != vtxBuffer.end(); ++it){
				vtxs.push_back(&*it);
			}
			planes.CreateConvexHull(&*vtxs.begin(), &*vtxs.begin() + vtxs.size());
			for(CDContactAnalysisFace** it = &*vtxs.begin(); it != &*vtxs.begin() + vtxs.size(); ++it){
				(*it)->dualPlanes.clear();
			}
			for(CDQHPlane<CDContactAnalysisFace>* it = planes.begin; it != planes.end; ++it){
				if(it->deleted) continue;
				for(int i=0; i!=3; ++i){
					it->vtx[i]->dualPlanes.push_back(it);
				}
			}
			for(CDContactAnalysisFace** itv = &*vtxs.begin(); itv != planes.vtxBegin; ++itv){
				for(CDContactAnalysisFace::DualPlanes::iterator itp = (*itv)->dualPlanes.begin();
					itp != (*itv)->dualPlanes.end(); ++itp){
					CDContactAnalysisFace::DualPlanes::iterator next = itp;
					++next;
					int i;
					for(i=0; i<3; ++i){
						if ((*itp)->vtx[i] == *itv) break;
					}
					for(CDContactAnalysisFace::DualPlanes::iterator it2 = next;
						it2 != (*itv)->dualPlanes.end(); ++it2){
						if ((*it2) == (*itp)->neighbor[i]){
							std::swap(*next, *it2);
							goto nextFace;
						}
					}
					nextFace:;
				}
			}
		}else{
			isValid = false;
		}
		#if 0
		DSTR << "ConvexHull�Ɏg�p���ꂽ���_:" << std::endl;
		int i=1;
		for(CDFace** f = vtxs.begin(); f != planes.vtxBegin; ++f){
			DSTR << "p" << i++ << ":\t" << 1/(*f)->dist << (*f)->normal << std::endl;
			Sleep(1);
		}
		DSTR << "ConvexHull���\�������:" << std::endl;
		i=1;
		for(CDQHPlane<CDFace>* p = planes.begin; p != planes.end; ++p){
			if (p->deleted) continue;
			DSTR << "v"  << i++ << ":\t" << p->normal/p->dist + cp->commonPoint << std::endl;
			Sleep(1);
		}
		#endif
	}else{	//	bUseContactVolume
		isValid = false;
	}
	return planes.vtxBegin;
}
void CDContactAnalysis::IntegrateNormal(CDShapePair* cp){
	cp->iNormal = Vec3d();
	if (isValid){	//	�����|���S���̏ꍇ
		double areaForCenter=0;
		for(Vtxs::iterator it = vtxs.begin(); it != vtxs.end(); ++it){
			CDContactAnalysisFace& qhVtx = **it;
			if (qhVtx.NCommonVtx() < 3) continue;
			Vec3f p0 = qhVtx.CommonVtx(0);
			Vec3f p1;
			Vec3f p2 = qhVtx.CommonVtx(1);
			for(unsigned i=2; i<qhVtx.NCommonVtx(); ++i){
				p1 = p2;
				p2 = qhVtx.CommonVtx(i);
				Vec3d n = (p2-p0) ^ (p1-p0);
				if (qhVtx.id==0){
					cp->iNormal += n;
				}else{
					cp->iNormal -= n;
				}
				areaForCenter += n.norm();
			}
		}
	}else{
		//	�����ǂ������ׂ�D
		CDSphere* sp[2];
		Vec3f center[2];
		for(int i=0; i<2; ++i){
			sp[i] = DCAST(CDSphere, cp->shape[i]);
			if (sp[i]) center[i] = cp->shapePoseW[i].Pos();
		}
		if (sp[0] && sp[1]){	//	�������̏ꍇ
			cp->iNormal = (center[1] - center[0]).unit();
		}else{
			if (sp[0]){
				cp->iNormal = (cp->commonPoint - center[0]).unit();
			}else if (sp[1]){
				cp->iNormal = (center[1] - cp->commonPoint).unit();
			}else{
//				assert(0);	//	�m��Ȃ��ʌ`��D
				//cp->iNormal = (af[1].Pos() - af[0].Pos()).unit();
				cp->iNormal = (cp->shapePoseW[1].Pos() - cp->shapePoseW[0].Pos()).unit();
			}
		}
	}
	if (cp->iNormal.square() < 1e-20){
		DEBUG_EVAL( DSTR << "iNormal error."; )
//		if (lastINormal.square() > 1e-20){
//			cp->iNormal = lastINormal;
//		}else{
			cp->iNormal = Vec3f(0,1,0);
			DEBUG_EVAL( DSTR << "Set (0,1,0)"; )
//		}
		DEBUG_EVAL( DSTR << std::endl; )
	}
	cp->iNormal.unitize();
#ifdef _DEBUG
	if (!finite(cp->iNormal.norm())){
		DSTR << "Error: iNormal is " << cp->iNormal << std::endl;
	}
#endif
}


void CDContactAnalysis::CalcNormal(CDShapePair* cp){
	if (cp->state == cp->NEW) {
		//	�V���ȐڐG�̏ꍇ�́C�@����ϕ����ď����l�����߂�
		IntegrateNormal(cp);
		cp->normal = cp->iNormal;
		cp->depth = 1e-2;
	}
	//	�O��̖@���̌����ɓ������āC�ŋߖT�_�����߂�
	Vec3d n = cp->normal;
	int res = ContFindCommonPoint(cp->shape[0], cp->shape[1], cp->shapePoseW[0], cp->shapePoseW[1], 
		-cp->normal, -DBL_MAX, 0, cp->normal, cp->closestPoint[0], cp->closestPoint[1], cp->depth);
	if (res <= 0){
		DSTR << "Error in CDContactAnalysis::CalcNormal(): res:" << res << "dist:" << cp->depth << -n << std::endl;
		DSTR << "cp:" << cp->shapePoseW[0]*cp->closestPoint[0] << cp->shapePoseW[1]*cp->closestPoint[1] << std::endl; 
		ContFindCommonPointSaveParam(cp->shape[0], cp->shape[1], cp->shapePoseW[0], cp->shapePoseW[1], 
			-n, -DBL_MAX, 0, n, cp->closestPoint[0], cp->closestPoint[1], cp->depth);
	}
	cp->depth *= -1;
	cp->center = cp->shapePoseW[0] * cp->closestPoint[0];
	cp->center -= 0.5f*cp->depth*cp->normal;
}

	
/*
void CDContactAnalysis::Draw(CDShapePair& cp, Posed afw, SGScene* s){
	GRRender* render;
	s->GetRenderers().Find(render);
	if (!render || !render->CanDraw()) return;
	render->SetModelMatrix(afw);
	GRMaterialData mat(
		Vec4f(0, 0, 1, 1),
		Vec4f(0, 0, 1, 1),
		Vec4f(0, 0, 1, 1),
		Vec4f(0, 0, 1, 1),
		0.0f);
	render->SetDepthFunc(GRRender::DF_ALWAYS);
	render->SetMaterial(mat);
	for(Vtxs::iterator it = vtxs.begin(); it != vtxs.end(); ++it){
		CDFace& face = **it;
		if (face.dualPlanes.size() < 3) continue;
		Vec3f vbuf[3];
		vbuf[0] = face.dualPlanes[0]->normal / face.dualPlanes[0]->dist + cp->commonPoint;
		vbuf[2] = face.dualPlanes[1]->normal / face.dualPlanes[1]->dist + cp->commonPoint;

		for(unsigned int i=2; i<face.dualPlanes.size(); ++i){
			vbuf[1] = vbuf[2];
			vbuf[2] = face.dualPlanes[i]->normal / face.dualPlanes[i]->dist + cp->commonPoint;
			render->DrawDirect(GRRender::TRIANGLES, vbuf, 3);
		}
	}
	GRMaterialData mat2(
		Vec4f(0, 1, 1, 1),
		Vec4f(0, 1, 1, 1),
		Vec4f(0, 1, 1, 1),
		Vec4f(0, 1, 1, 1),
		0.0f);
	render->SetMaterial(mat2);
	Vec3f vtx(cp->commonPoint);
	render->SetLineWidth(3);
	render->DrawDirect(GRRender::POINTS, &vtx, 1);
	render->SetDepthFunc(GRRender::DF_LESS);
}
*/
//-----------------------------------------------------------------------------
//	�ڐG����֐��Ccenter, extent �̓��[�J��(�t���[����)���W�n�Ŏw��
//
#define ABS(x) ((x)>0 ? (x) : -(x))
bool FASTCALL BBoxIntersection(Posed poseA, Vec3f centerA, Vec3f extentA,
					  Posed poseB, Vec3f centerB, Vec3f extentB){ 
	Matrix3f postureA, postureB;
	poseA.Ori().ToMatrix(postureA);
	poseB.Ori().ToMatrix(postureB);


    // compute difference of box centers, D = C1-C0
    Vec3f kD = postureA*centerA - postureB*centerB;

    float aafC[3][3];     // matrix C = A^T B, c_{ij} = Dot(A_i,B_j)
    float aafAbsC[3][3];  // |c_{ij}|
    float afAD[3];        // Dot(A_i,D)
    float fR0, fR1, fR;   // interval radii and distance between centers
    float fR01;           // = R0 + R1
    
    // axis C0+t*A0
    aafC[0][0] = postureA.Ex()*(postureB.Ex());
    aafC[0][1] = postureA.Ex()*(postureB.Ey());
    aafC[0][2] = postureA.Ex()*(postureB.Ez());
    afAD[0] = postureA.Ex()*(kD);
    aafAbsC[0][0] = ABS(aafC[0][0]);
    aafAbsC[0][1] = ABS(aafC[0][1]);
    aafAbsC[0][2] = ABS(aafC[0][2]);
    fR = ABS(afAD[0]);
    fR1 = extentB[0]*aafAbsC[0][0]+extentB[1]*aafAbsC[0][1]+extentB[2]*aafAbsC[0][2];
    fR01 = extentA[0] + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A1
    aafC[1][0] = postureA.Ey()*(postureB.Ex());
    aafC[1][1] = postureA.Ey()*(postureB.Ey());
    aafC[1][2] = postureA.Ey()*(postureB.Ez());
    afAD[1] = postureA.Ey()*(kD);
    aafAbsC[1][0] = ABS(aafC[1][0]);
    aafAbsC[1][1] = ABS(aafC[1][1]);
    aafAbsC[1][2] = ABS(aafC[1][2]);
    fR = ABS(afAD[1]);
    fR1 = extentB[0]*aafAbsC[1][0]+extentB[1]*aafAbsC[1][1]+extentB[2]*aafAbsC[1][2];
    fR01 = extentA[1] + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A2
    aafC[2][0] = postureA.Ez()*(postureB.Ex());
    aafC[2][1] = postureA.Ez()*(postureB.Ey());
    aafC[2][2] = postureA.Ez()*(postureB.Ez());
    afAD[2] = postureA.Ez()*(kD);
    aafAbsC[2][0] = ABS(aafC[2][0]);
    aafAbsC[2][1] = ABS(aafC[2][1]);
    aafAbsC[2][2] = ABS(aafC[2][2]);
    fR = ABS(afAD[2]);
    fR1 = extentB[0]*aafAbsC[2][0]+extentB[1]*aafAbsC[2][1]+extentB[2]*aafAbsC[2][2];
    fR01 = extentA[2] + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*B0
    fR = ABS(postureB.Ex()*(kD));
    fR0 = extentA[0]*aafAbsC[0][0]+extentA[1]*aafAbsC[1][0]+extentA[2]*aafAbsC[2][0];
    fR01 = fR0 + extentB[0];
    if ( fR > fR01 )
        return false;

    // axis C0+t*B1
    fR = ABS(postureB.Ey()*(kD));
    fR0 = extentA[0]*aafAbsC[0][1]+extentA[1]*aafAbsC[1][1]+extentA[2]*aafAbsC[2][1];
    fR01 = fR0 + extentB[1];
    if ( fR > fR01 )
        return false;

    // axis C0+t*B2
    fR = ABS(postureB.Ez()*(kD));
    fR0 = extentA[0]*aafAbsC[0][2]+extentA[1]*aafAbsC[1][2]+extentA[2]*aafAbsC[2][2];
    fR01 = fR0 + extentB[2];
    if ( fR > fR01 )
        return false;

    // axis C0+t*A0xB0
    fR = ABS(afAD[2]*aafC[1][0]-afAD[1]*aafC[2][0]);
    fR0 = extentA[1]*aafAbsC[2][0] + extentA[2]*aafAbsC[1][0];
    fR1 = extentB[1]*aafAbsC[0][2] + extentB[2]*aafAbsC[0][1];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A0xB1
    fR = ABS(afAD[2]*aafC[1][1]-afAD[1]*aafC[2][1]);
    fR0 = extentA[1]*aafAbsC[2][1] + extentA[2]*aafAbsC[1][1];
    fR1 = extentB[0]*aafAbsC[0][2] + extentB[2]*aafAbsC[0][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A0xB2
    fR = ABS(afAD[2]*aafC[1][2]-afAD[1]*aafC[2][2]);
    fR0 = extentA[1]*aafAbsC[2][2] + extentA[2]*aafAbsC[1][2];
    fR1 = extentB[0]*aafAbsC[0][1] + extentB[1]*aafAbsC[0][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A1xB0
    fR = ABS(afAD[0]*aafC[2][0]-afAD[2]*aafC[0][0]);
    fR0 = extentA[0]*aafAbsC[2][0] + extentA[2]*aafAbsC[0][0];
    fR1 = extentB[1]*aafAbsC[1][2] + extentB[2]*aafAbsC[1][1];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A1xB1
    fR = ABS(afAD[0]*aafC[2][1]-afAD[2]*aafC[0][1]);
    fR0 = extentA[0]*aafAbsC[2][1] + extentA[2]*aafAbsC[0][1];
    fR1 = extentB[0]*aafAbsC[1][2] + extentB[2]*aafAbsC[1][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A1xB2
    fR = ABS(afAD[0]*aafC[2][2]-afAD[2]*aafC[0][2]);
    fR0 = extentA[0]*aafAbsC[2][2] + extentA[2]*aafAbsC[0][2];
    fR1 = extentB[0]*aafAbsC[1][1] + extentB[1]*aafAbsC[1][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A2xB0
    fR = ABS(afAD[1]*aafC[0][0]-afAD[0]*aafC[1][0]);
    fR0 = extentA[0]*aafAbsC[1][0] + extentA[1]*aafAbsC[0][0];
    fR1 = extentB[1]*aafAbsC[2][2] + extentB[2]*aafAbsC[2][1];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A2xB1
    fR = ABS(afAD[1]*aafC[0][1]-afAD[0]*aafC[1][1]);
    fR0 = extentA[0]*aafAbsC[1][1] + extentA[1]*aafAbsC[0][1];
    fR1 = extentB[0]*aafAbsC[2][2] + extentB[2]*aafAbsC[2][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    // axis C0+t*A2xB2
    fR = ABS(afAD[1]*aafC[0][2]-afAD[0]*aafC[1][2]);
    fR0 = extentA[0]*aafAbsC[1][2] + extentA[1]*aafAbsC[0][2];
    fR1 = extentB[0]*aafAbsC[2][1] + extentB[1]*aafAbsC[2][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return false;

    return true;
}

}
