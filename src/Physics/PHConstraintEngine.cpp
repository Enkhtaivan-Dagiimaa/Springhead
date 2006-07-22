/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "Physics.h"
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif
#include <float.h>
#include <Collision/CDDetectorImp.h>
#include <Collision/CDQuickHull2D.h>
#include <Collision/CDQuickHull2DImp.h>
#include <Collision/CDCutRing.h>

using namespace PTM;
using namespace std;
namespace Spr{;

class ContactVertex: public Vec3d{
public:
	static Vec3d ex, ey;
	ContactVertex(){}
	ContactVertex(const Vec3d& v):Vec3d(v){}
	Vec2d GetPos(){
		return Vec2d( (*(Vec3d*)this)*ex, (*(Vec3d*)this)*ey );
	}
};
Vec3d ContactVertex::ex;
Vec3d ContactVertex::ey;

void PHShapePairForLCP::CalcNormal(PHSolid* solid0, PHSolid* solid1){
	const double depthEpsilon = 0.01;
	if (state == NEW){
		normal = solid0->GetPointVelocity(commonPoint) - solid1->GetPointVelocity(commonPoint);
		double norm = normal.norm();
		if (norm<1e-10){
			normal = solid1->GetCenterPosition() - solid0->GetCenterPosition();
			double norm = normal.norm();
			if (norm<1e-10){
				normal = Vec3d(1,0,0);
			}
		}
		normal.unitize();
		depth = depthEpsilon;
	}
	//	�O��̖@���̌����ɓ������āC�ŋߖT�_�����߂�
	if (depth < depthEpsilon) depth = depthEpsilon;
	Posed trans;
	Vec3f n;
	while(1) {
		depth *= 2;							//	�]�T�����āC�[����2�{������
		trans = shapePoseW[1];				//	�������s��
		trans.Pos() += depth * normal;
		FindClosestPoints(shape[0], shape[1], shapePoseW[0], trans, closestPoint[0], closestPoint[1]);
		center = shapePoseW[0] * closestPoint[0];
		n = trans * closestPoint[1] - center;
		if (n.square() > 1e-5) break;
	}
	depth = depth - n.norm();		//	������������ - 2�_�̋���
	normal = n.unit();
	center -= 0.5f*depth*normal;
#ifdef _DEBUG
	if (!finite(normal.norm())){
		FindClosestPoints(shape[0], shape[1], shapePoseW[0], trans, closestPoint[0], closestPoint[1]);
	}
#endif
}

// �ڐG��́D�ڐG�����̐؂�������߂āC�؂�����\������ʑ��p�`�̒��_��engine�ɍS���Ƃ��Ēǉ�����D	
void PHShapePairForLCP::EnumVertex(PHConstraintEngine* engine, unsigned ct, PHSolidInfoForLCP* solid0, PHSolidInfoForLCP* solid1){
	//	center �� normal�����ʂƌ�������ʂ����߂Ȃ��Ƃ����Ȃ��D
	//	�ʂ̒��_���ʂ̑��ɂ���ʂ������ΏہD
	//	quick hull �� n log r ������C���`���Ԃŏo���邱�Ƃ͂���Ă����ׂ��D

	//	�e3�p�`�ɂ��āC���_���ǂ��瑤�ɂ��邩���肵�C�����ɂ�����̂�ΏۂƂ���D
	//	������C�@���{���l�̌`�ŕ\������D
	//	���̏����͓ʌ`�󂪎����Ă��ėǂ��D
	//	������̕\���`���Ƃ��āC2���Ȑ��������D���̏ꍇ�C�����͕Ԃ��Ȃ�
	//	��2���Ȑ���Mullar��Preparata�ɂ͓���Ȃ��ŕʂɂ��Ă����D

	//2D�ւ̕ϊ�������D�ǂ�����H
	//	�K���ɑ��x�H
	Vec3d v0 = solid0->solid->GetPointVelocity(center);
	Vec3d v1 = solid1->solid->GetPointVelocity(center);
	Matrix3d local;	//	contact coodinate system �ڐG�̍��W�n
	local.Ex() = normal;
	local.Ey() = v1-v0;
	local.Ey() -= local.Ey() * normal * normal;
	if (local.Ey().square() > 1e-6){
		local.Ey().unitize(); 
	}else{
		if (Square(normal.x) < 0.5) local.Ey()= (normal ^ Vec3f(1,0,0)).unit();
		else local.Ey() = (normal ^ Vec3f(0,1,0)).unit();
	}
	local.Ez() =  local.Ex() ^ local.Ey();
	if (local.det() < 0.99) {
		DSTR << "Error: local coordinate error." << std::endl;
		assert(0);
	}

	// �ʑ��ʓ��m�̏ꍇ�́A�ʂƖʂ��G���ꍇ������̂ŁA�ڐG���ʑ��p�`�ɂȂ邱�Ƃ�����B
	// ���Ɠʌ`��̐ڐG�͕K���P�_�ɂȂ�B
	CDSphere* sp[2];
	sp[0] = DCAST(CDSphere, shape[0]);	// CDSphere�փ_�C�i�~�b�N�L���X�g
	sp[1] = DCAST(CDSphere, shape[1]);
	// �ڐG��͂��s���Q�̕��̂̕Е�or���������̏ꍇ
	if (sp[0] || sp[1]) {		
		// �ڐG�_�̔z��(engine->points)�ɁA���̍Ői���_�Ƒ���̍Ői���_�̒��_center��ǉ�����
		engine->points.push_back(DBG_NEW PHContactPoint(local, this, center, solid0, solid1));
	}
	// �ڐG��͂��s���Q�̕��̂��ǂ���Ƃ����ł͂Ȃ��ꍇ
	else {						
		//	�؂�������߂�P�F�؂�����\����������̗�
		CDCutRing cutRing(center, local);
		if (shape[0]->FindCutRing(cutRing, shapePoseW[0]) && shape[1]->FindCutRing(cutRing, shapePoseW[1])){
			//	�؂��������ꍇ�i���l�덷�Ő؂����������Ȃ��ꍇ������j
			//	�؂�������߂�Q�F�������Ȃ��ŗւ����
			cutRing.MakeRing();
			//	cutRing.Print(DSTR);
			//	DSTR << "contact center:" << center << " normal:" << normal << "  vtxs:" << std::endl;
			int nPoint = engine->points.size();
			for(CDQHLine<CDCutLine>* vtx = cutRing.vtxs.begin; vtx!=cutRing.vtxs.end; ++vtx){
				if (vtx->deleted) continue;
				assert(finite(vtx->dist));

				Vec3d pos;
				pos.sub_vector(1, Vec2d()) = vtx->normal / vtx->dist;
				pos = cutRing.local * pos;
				Matrix3d local;
				cutRing.local.Ori().ToMatrix(local);
				engine->points.push_back(DBG_NEW PHContactPoint(local, this, pos, solid0, solid1));
			//	DSTR << "  " << pos << std::endl;
			}
			if (nPoint == engine->points.size()){	//	�ЂƂ��ǉ����Ă��Ȃ���ConvexHull�����Ȃ������D
				//	������1�_�ŐڐG���Ă���D
				engine->points.push_back(DBG_NEW PHContactPoint(local, this, center, solid0, solid1));
			}
		}
	}
}

void PHSolidPairForLCP::OnDetect(PHShapePairForLCP* sp, PHConstraintEngine* engine, unsigned ct, double dt){
	//	�@�������߂�
	sp->CalcNormal(solid[0]->solid, solid[1]->solid);

	//	��������2�̓ʌ`���ڐG�ʂŐ؂������̐؂���̌`�����߂�
	sp->EnumVertex(engine, ct, solid[0], solid[1]);
}			

//----------------------------------------------------------------------------
// PHConstraintEngine
OBJECT_IMP(PHConstraintEngine, PHEngine);

PHConstraintEngine::PHConstraintEngine(){
	numIteration	= 15;
	correctionRate	= 0.3;
	shrinkRate		= 0.7;
}

PHConstraintEngine::~PHConstraintEngine(){

}

void PHConstraintEngine::Clear(){
	base_type::Clear();
	points.clear();
	joints.clear();
}

PHJoint* PHConstraintEngine::CreateJoint(const PHJointDesc& desc){
	PHJoint* joint;
	switch(desc.type){
	case PHConstraintDesc::HINGEJOINT:
		joint = DBG_NEW PHHingeJoint();
		break;
	case PHConstraintDesc::SLIDERJOINT:
		joint = DBG_NEW PHSliderJoint();
		break;
	case PHConstraintDesc::BALLJOINT:
		joint = DBG_NEW PHBallJoint();
		break;
	case PHConstraintDesc::PATHJOINT:
		joint = DBG_NEW PHPathJoint();
		break;
	case PHConstraintDesc::SPRING:
		joint = DBG_NEW PHSpring();
		break;
	default: assert(false);
	}
	joint->SetScene(GetScene());
	joint->SetDesc(desc);
	return joint;
}

PHJoint* PHConstraintEngine::AddJoint(const PHJointDesc& desc){
	PHJoint* joint = CreateJoint(desc);
	joints.push_back(joint);
	return joint;
}

PHJoint* PHConstraintEngine::AddJoint(PHSolid* lhs, PHSolid* rhs, const PHJointDesc& desc){
	PHSolidInfos<PHSolidInfoForLCP>::iterator islhs, isrhs;
	islhs = solids.Find(lhs);
	isrhs = solids.Find(rhs);
	if(islhs == solids.end() || isrhs == solids.end())
		return NULL;
	
	PHJoint* joint = CreateJoint(desc);
	joint->solid[0] = *islhs;
	joint->solid[1] = *isrhs;
	joints.push_back(joint);

	//�֐߂łȂ���ꂽ���̊Ԃ̐ڐG�͖�����
	//EnableContact(lhs, rhs, false);

	return joint;
}

void PHConstraintEngine::SetupDynamics(double dt){
	PHSolidInfos<PHSolidInfoForLCP>::iterator it;
	for(it = solids.begin(); it != solids.end(); it++)
		(*it)->SetupDynamics(dt);
	points.SetupDynamics(dt, correctionRate, shrinkRate);
	joints.SetupDynamics(dt, correctionRate, shrinkRate);
}
/*void PHConstraintEngine::SetupCorrection(double dt){
	PHSolidInfos<PHSolidInfoForLCP>::iterator it;
	for(it = solids.begin(); it != solids.end(); it++)
		(*it)->SetupCorrection();
	points.SetupCorrection(dt, max_error);
	joints.SetupCorrection(dt, max_error);
}*/
void PHConstraintEngine::IterateDynamics(){
	int count = 0;
	while(true){
		if(count == numIteration){
			//DSTR << "max count." << " iteration count: " << count << " dfsum: " << dfsum << endl;
			break;
		}
		points.IterateDynamics();
		joints.IterateDynamics();

		count++;
	}
}
/*void PHConstraintEngine::IterateCorrection(){
	int count = 0;
	while(true){
		if(count == max_iter_correction){
			//DSTR << "max count." << " iteration count: " << count << " dFsum: " << dFsum << endl;
			break;
		}
		points.IterateCorrection();
		joints.IterateCorrection();

		count++;
	}
}*/

void PHConstraintEngine::UpdateSolids(double dt){
	PHSolidInfos<PHSolidInfoForLCP>::iterator is;
	PHSolidInfoForLCP* info;
	PHSolid* solid;
	Vec3d vnew, wnew;
	for(is = solids.begin(); is != solids.end(); is++){
		info = *is;
		solid = info->solid;
		//velocity update
		vnew = info->v + info->dv0 + info->dv;
		wnew = info->w + info->dw0 + info->dw;
		solid->SetVelocity       (solid->GetOrientation() * vnew);
		solid->SetAngularVelocity(solid->GetOrientation() * wnew);
		//position update
		solid->SetCenterPosition(solid->GetCenterPosition() + solid->GetVelocity() * dt/* + solid->GetOrientation() * info->dV*/);
		solid->SetOrientation(
			(solid->GetOrientation() * Quaterniond::Rot(wnew * dt/* + info->dW*/)).unit()
		);
		//solid->SetOrientation((solid->GetOrientation() + solid->GetOrientation().Derivative(solid->GetOrientation() * is->dW)).unit());
		//solid->SetOrientation((solid->GetOrientation() * Quaterniond::Rot(/*solid->GetOrientation() * */info->dW)).unit());
		solid->SetUpdated(true);
	}
}

#include <windows.h>

void PHConstraintEngine::Step(){
	PHScene* scene = DCAST(PHScene, GetScene());
	unsigned int ct = scene->GetCount();
	double dt = scene->GetTimeStep();
	Dynamics(dt, ct);
	//Correction(dt, ct);
	UpdateSolids(dt);
}
void PHConstraintEngine::Dynamics(double dt, int ct){
	LARGE_INTEGER freq, val[2];
	QueryPerformanceFrequency(&freq);

	//���������m
	QueryPerformanceCounter(&val[0]);
	points.clear();
	if(bContactEnabled)
		Detect(ct, dt);
	QueryPerformanceCounter(&val[1]);
	//DSTR << "cd " << (double)(val[1].QuadPart - val[0].QuadPart)/(double)(freq.QuadPart) << endl;
	
	QueryPerformanceCounter(&val[0]);
	SetupDynamics(dt);
	QueryPerformanceCounter(&val[1]);
	//DSTR << "sd " << (double)(val[1].QuadPart - val[0].QuadPart)/(double)(freq.QuadPart) << endl;

	QueryPerformanceCounter(&val[0]);
	IterateDynamics();
	QueryPerformanceCounter(&val[1]);
	//DSTR << "id " << (double)(val[1].QuadPart - val[0].QuadPart)/(double)(freq.QuadPart) << endl;
}

/*void PHConstraintEngine::Correction(double dt, int ct){
	LARGE_INTEGER freq, val[2];
	QueryPerformanceFrequency(&freq);

	QueryPerformanceCounter(&val[0]);
	SetupCorrection(dt);
	QueryPerformanceCounter(&val[1]);
	//DSTR << "sc " << (double)(val[1].QuadPart - val[0].QuadPart)/(double)(freq.QuadPart) << endl;

	QueryPerformanceCounter(&val[0]);
	IterateCorrection();
	QueryPerformanceCounter(&val[1]);
	//DSTR << "ic " << (double)(val[1].QuadPart - val[0].QuadPart)/(double)(freq.QuadPart) << endl;
}*/


}

