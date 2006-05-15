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

//----------------------------------------------------------------------------
//	PHSolidPair
void PHSolidPair::Init(PHSolidAux* s0, PHSolidAux* s1){
	solid[0] = s0;
	solid[1] = s1;
	int ns0 = solid[0]->solid->shapes.size(), ns1 = solid[1]->solid->shapes.size();
	shapePairs.resize(ns0, ns1);
	for(int i = 0; i < ns0; i++)for(int j = 0; j < ns1; j++){
		PHShapePair& sp = shapePairs.item(i, j);
		sp.shape[0] = solid[0]->solid->shapes[i];
		sp.shape[1] = solid[1]->solid->shapes[j];
	}
	bEnabled = true;
}

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

// �ڐG��́D�ڐG�����̐؂�������߂āC�؂�����\������ʑ��p�`�̒��_��engine�ɍS���Ƃ��Ēǉ�����D	
void PHShapePair::EnumVertex(PHConstraintEngine* engine, unsigned ct, PHSolidAux* solid0, PHSolidAux* solid1){
	//	center �� normal�����ʂƌ�������ʂ����߂Ȃ��Ƃ����Ȃ��D
	//	�ʂ̒��_���ʂ̑��ɂ���ʂ������ΏہD
	//	quick hull �� n log r ������C���`���Ԃŏo���邱�Ƃ͂���Ă����ׂ��D

	//	�e3�p�`�ɂ��āC���_���ǂ��瑤�ɂ��邩���肵�C�����ɂ�����̂�ΏۂƂ���D
	//	������C�@���{���l�̌`�ŕ\������D
	//	���̏����͓ʌ`�󂪎����Ă��ėǂ��D
	//	������̕\���`���Ƃ��āC2���Ȑ��������D���̏ꍇ�C�����͕Ԃ��Ȃ�
	//	��2���Ȑ���Mullar��Preparata�ɂ͓���Ȃ��ŕʂɂ��Ă����D
	CDConvex* conv[2] = { (CDConvex*)shape[0], (CDConvex*)shape[1], };


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
		if (conv[0]->FindCutRing(cutRing, shapePoseW[0]) && conv[1]->FindCutRing(cutRing, shapePoseW[1])){
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
//#define USE_VOLUME

#ifndef USE_VOLUME
bool PHShapePair::Detect(unsigned ct, PHSolidAux* solid0, PHSolidAux* solid1){
	UpdateShapePose(solid0->solid->GetPose() * shape[0]->GetPose(), solid1->solid->GetPose() * shape[1]->GetPose());
	CDConvex* conv[2] = { (CDConvex*)shape[0], (CDConvex*)shape[1], };
	Vec3d sep;
	bool r = FindCommonPoint(conv[0], conv[1], shapePoseW[0], shapePoseW[1], sep, closestPoint[0], closestPoint[1]);
	const double depthEpsilon = 0.01;
	if (r){
		commonPoint = shapePoseW[0] * closestPoint[0];
		if (lastContactCount == unsigned(ct-1)) state = CONTINUE;
		else state = NEW;
		lastContactCount = ct;

		//	�@�������߂�
		if (state == NEW){
			normal = solid0->solid->GetPointVelocity(commonPoint) - solid1->solid->GetPointVelocity(commonPoint);
			double norm = normal.norm();
			if (norm<1e-10){
				normal = solid1->solid->GetCenterPosition() - solid0->solid->GetCenterPosition();
				double norm = normal.norm();
				if (norm<1e-10){
					normal = Vec3d(1,0,0);
				}
			}
			normal.unitize();
			depth = depthEpsilon;
/*
			if (normal.y < 0.8){
				DSTR << std::endl;
				DSTR << (state == NEW ? "new" : "cont");
				DSTR << normal << std::endl;
			} 
*/
		}
		//	�O��̖@���̌����ɓ������āC�ŋߖT�_�����߂�
		if (depth < depthEpsilon) depth = depthEpsilon;
		Posed trans;
		Vec3f n;
		while(1) {
			depth *= 2;							//	�]�T�����āC�[����2�{������
			trans = shapePoseW[1];				//	�������s��
			trans.Pos() += depth * normal;
			FindClosestPoints(conv[0], conv[1], shapePoseW[0], trans, closestPoint[0], closestPoint[1]);
			center = shapePoseW[0] * closestPoint[0];
			n = trans * closestPoint[1] - center;
			if (n.square() > 1e-5) break;
		}
		depth = depth - n.norm();		//	������������ - 2�_�̋���
		normal = n.unit();
		center -= 0.5f*depth*normal;
//		DSTR << "Normal:" << normal << " center:" << center << " depth:" << depth << std::endl;
		#ifdef _DEBUG
		if (!finite(normal.norm())){
			//DSTR << "Error: Wrong normal:" << normal << std::endl;
			//DSTR << trans;
			//DSTR << closestPoint[0] << closestPoint[1] << std::endl;
			FindClosestPoints(conv[0], conv[1], shapePoseW[0], trans, closestPoint[0], closestPoint[1]);
		}
		#endif
	}
	return r;
}
#else
bool PHShapePair::Detect(unsigned ct, PHSolidAux* solid0, PHSolidAux* solid1){
	return CDShapePair::Detect(ct);
}
#endif
//--------------------------------------------------------------------------
#ifndef USE_VOLUME	//	�̐ς��g��Ȃ��ڐG����
bool PHSolidPair::Detect(PHConstraintEngine* engine){
	if(!bEnabled)return false;

	unsigned ct = OCAST(PHScene, engine->GetScene())->GetCount();
	// �����ꂩ��Solid�Ɍ`�󂪊��蓖�Ă��Ă��Ȃ��ꍇ�͐ڐG�Ȃ�
	PHSolid *s0 = solid[0]->solid, *s1 = solid[1]->solid;
	if(s0->NShape() == 0 || s1->NShape() == 0) return false;

	// �S�Ă�shape pair�ɂ��Č����𒲂ׂ�
	bool found = false;
	for(int i = 0; i < (int)(s0->shapes.size()); i++){
		for(int j = 0; j < (int)(s1->shapes.size()); j++){
			PHShapePair& sp = shapePairs.item(i, j);
			//����shape pair�̌�������/�@���ƐڐG�̈ʒu�����߂�D
			if(sp.Detect(ct, solid[0], solid[1])){
				found = true;
				//	��������2�̓ʌ`���ڐG�ʂŐ؂������̐؂���̌`�����߂�
				sp.EnumVertex(engine, ct, solid[0], solid[1]);
				//DSTR << sp.normal << endl;
			}
		}
	}
	return found;
}

//--------------------------------------------------------------------------
#else	//	�̐ς��g���ڐG����
bool PHSolidPair::Detect(PHConstraintEngine* engine){
	if(!bEnabled)return false;
	static CDContactAnalysis analyzer;

	unsigned ct = OCAST(PHScene, engine->GetScene())->GetCount();
	
	// �����ꂩ��Solid�Ɍ`�󂪊��蓖�Ă��Ă��Ȃ��ꍇ�̓G���[
	if(solid[0]->solid->NShape() == 0 || solid[1]->solid->NShape() == 0)
		return false;

	// �S�Ă�shape pair�ɂ��Č����𒲂ׂ�
	bool found = false;
	for(int i = 0; i < (int)(solid[0]->solid->shapes.size()); i++)for(int j = 0; j < (int)(solid[1]->solid->shapes.size()); j++){
		PHShapePair& sp = shapePairs.item(i, j);
		sp.UpdateShapePose(solid[0]->solid->GetPose(), solid[1]->solid->GetPose());

		if(sp.Detect(ct, solid[0], solid[1])){
			found = true;
			analyzer.FindIntersection(&sp);			//�����`��̌v�Z
			analyzer.CalcNormal(&sp);				//�����̖@���ƒ��S�𓾂�

			//�ڐG�_�̍쐬�F
			//�����`����\�����钸�_��analyzer.planes.begin����end�܂ł̓�deleted==false�̂���
			typedef CDQHPlanes<CDContactAnalysisFace>::CDQHPlane Plane;
			static std::vector<ContactVertex> isVtxs;
			isVtxs.clear();
			for(Plane* p = analyzer.planes.begin; p != analyzer.planes.end; p++){
				if(p->deleted) continue;
				isVtxs.push_back(p->normal / p->dist);
			}
			ContactVertex::ex = (-0.1 < sp.normal.z && sp.normal.z < 0.1) ?
				sp.normal ^ Vec3f(0,0,1) : sp.normal ^ Vec3f(1,0,0);
			ContactVertex::ex.unitize();
			ContactVertex::ey = sp.normal ^ ContactVertex::ex;

			//	���ׂĂ̐ڐG�_���܂ލŏ��̓ʑ��p�`
			static CDQHLines<ContactVertex> supportConvex(1000);
			supportConvex.Clear();
			supportConvex.epsilon = 0.01f;

			static std::vector<ContactVertex*> isVtxPtrs;
			isVtxPtrs.clear();
			isVtxPtrs.resize(isVtxs.size());
			for(size_t i=0; i<isVtxPtrs.size(); ++i) isVtxPtrs[i] = &isVtxs[i];
			supportConvex.CreateConvexHull(&isVtxPtrs.front(), &isVtxPtrs.back()+1);
			
			typedef CDQHLines<ContactVertex>::CDQHLine Line;
//#define DEBUG_CONTACTOUT
#ifdef DEBUG_CONTACTOUT
			int n = engine->points.size();
#endif
			for(Line* l = supportConvex.begin; l!=supportConvex.end; ++l){
				//if (l->deleted) continue;
				Vec3d v = *l->vtx[0]+sp.commonPoint;
				engine->points.push_back(DBG_NEW PHContactPoint(&sp, v, solid[0], solid[1]));
			}
#ifdef DEBUG_CONTACTOUT
			//DSTR << engine->points.size()-n << " contacts:";
			for(unsigned i=n; i<engine->points.size(); ++i){
				//DSTR << engine->points[i].pos;
			}
			//DSTR << std::endl;
#endif
		}
	}
	return found;
}
#endif

void PHShapePair::SetState(const PHShapePairState& s){
	lastContactCount = s.lastContactCount;
	depth = s.depth;
	normal = s.normal;
}



//----------------------------------------------------------------------------
// PHConstraintEngineState
PHConstraintEngineState::PHConstraintEngineState(const PHConstraintEngine& ce){
	nSolidPair = 0;
	nShapePair = 0;
	//	solidPairs.item(i,j)�@�� i<j�������g���Ă���̂ł��������ۑ�
	for(int j=0; j<ce.solidPairs.width(); ++j){
		nSolidPair += j;
		for(int i=0; i<j; ++i){
			const PHSolidPair& sp = ce.solidPairs.item(i, j);
			nShapePair += sp.shapePairs.height()*sp.shapePairs.width();
		}
	}
}

//----------------------------------------------------------------------------
// PHConstraintEngine
OBJECTIMP(PHConstraintEngine, PHEngine);

PHConstraintEngine::PHConstraintEngine(){
	max_iter_dynamics = 5;
	max_iter_correction = 3;
	//step_size = 1.0;
	//converge_criteria = 0.00000001;
	max_error = 1.0;
}

PHConstraintEngine::~PHConstraintEngine(){

}

void PHConstraintEngine::Clear(PHScene* s){
	points.clear();
	joints.clear();
	solidPairs.clear();
	solids.clear();
}

void PHConstraintEngine::Add(PHSolid* s){
	if(solids.Find(s) == solids.end()){
		solids.push_back(DBG_NEW PHSolidAux(s));
		
		int N = solids.size();
		assert(solidPairs.height() == N-1 && solidPairs.width() == N-1);
		solidPairs.resize(N, N);
		for(int i = 0; i < N-1; i++)
			solidPairs.item(i, N-1).Init(solids[i], solids[N-1]);
	}
}

void PHConstraintEngine::Remove(PHSolid* s){
	PHSolidAuxs::iterator is = solids.Find(s);
	if(is != solids.end()){
		int idx = is - solids.begin();
		solids.erase(is);
		solidPairs.erase_row(idx);
		solidPairs.erase_col(idx);
	}
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
	default: assert(false);
	}
	joint->SetDesc(desc);
	return joint;
}

PHJoint* PHConstraintEngine::AddJoint(const PHJointDesc& desc){
	PHJoint* joint = CreateJoint(desc);
	joints.push_back(joint);
	return joint;
}

PHJoint* PHConstraintEngine::AddJoint(PHSolid* lhs, PHSolid* rhs, const PHJointDesc& desc){
	PHSolidAuxs::iterator islhs, isrhs;
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

// solidPairs, shapePairs�̍X�V����
//	solid  : AddShape���Ă΂ꂽSolid. �V�K�ǉ����ꂽShape��solid->shapes.back()�Ŏ擾��������.
void PHConstraintEngine::UpdateShapePairs(PHSolid* solid){
	int isolid = (solids.Find(solid) - solids.begin());
	int i, j;
	for(i = 0; i < isolid; i++){
		PHSolidPair& sp = solidPairs.item(i, isolid);
		PHSolid* slhs = sp.solid[0]->solid;
		sp.shapePairs.resize(sp.shapePairs.height(), solid->NShape());
		for(j = 0; j < slhs->NShape(); j++){
			sp.shapePairs.item(j, solid->NShape()-1).shape[0] = slhs->shapes[j];
			sp.shapePairs.item(j, solid->NShape()-1).shape[1] = solid->shapes.back();
		}
	}
	for(i = isolid+1; i < (int)solids.size(); i++){
		PHSolidPair& sp = solidPairs.item(isolid, i);
		PHSolid* srhs = sp.solid[1]->solid;
		sp.shapePairs.resize(solid->NShape(), sp.shapePairs.width());
		for(j = 0; j < srhs->NShape(); j++){
			sp.shapePairs.item(solid->NShape()-1, j).shape[0] = solid->shapes.back();
			sp.shapePairs.item(solid->NShape()-1, j).shape[1] = srhs->shapes[j];
		}
	}
}

void PHConstraintEngine::EnableContact(PHSolid* lhs, PHSolid* rhs, bool bEnable){
	PHSolidAuxs::iterator ilhs = solids.Find(lhs), irhs = solids.Find(rhs);
	if(ilhs == solids.end() || irhs == solids.end())
		return;
	int i = ilhs - solids.begin(), j = irhs - solids.begin();
	if(i > j)swap(i, j);
	assert(i < solidPairs.height() && j < solidPairs.width());
	solidPairs.item(i, j).bEnabled = bEnable;
}

// AABB�Ń\�[�g���邽�߂̍\����
struct Edge{
	float edge;				///<	�[�̈ʒu(�O���[�o���n)
	int	index;				///<	���� solid�̈ʒu
	bool bMin;				///<	���[�Ȃ�true
	bool operator < (const Edge& s) const { return edge < s.edge; }
};
typedef std::vector<Edge> Edges;

bool PHConstraintEngine::DetectPenetration(){
	/* �ȉ��̗���Ō��������߂�
		1. Solid��BBox���x���ł̌�������(z���\�[�g)�D�����̂�����̖����g�����O
		2. �eSolid�̑g�ɂ���
			2a. Shape��BBox���x���ł̌������� (������)
			2b. �eShape�̑g�ɂ���GJK�Ō����`��𓾂�
			2c. �����`�󂩂�@�������߁A�@���Ɋւ��Č`����ˉe���C���̒��_��ڐG�_�Ƃ���
			2d. ����ꂽ�ڐG�_����PHContactPoints�ɋl�߂Ă���
	 */
	points.clear();
	int N = solids.size();

	//1. BBox���x���̏Փ˔���
	Vec3f dir(0,0,1);
	Edges edges;
	edges.resize(2 * N);
	Edges::iterator eit = edges.begin();
	for(int i = 0; i < N; ++i){
		solids[i]->solid->GetBBoxSupport(dir, eit[0].edge, eit[1].edge);
		eit[0].index = i; eit[0].bMin = true;
		eit[1].index = i; eit[1].bMin = false;
		eit += 2;
	}
	std::sort(edges.begin(), edges.end());
	//�[���猩�Ă����āC�ڐG�̉\��������m�[�h�̔��������D
	typedef std::set<int> SolidSet;
	SolidSet cur;							//	���݂�Solid�̃Z�b�g
	bool found = false;
	for(Edges::iterator it = edges.begin(); it != edges.end(); ++it){
		if (it->bMin){						//	���[��������C���X�g���̕��̂Ɣ���
			for(SolidSet::iterator itf=cur.begin(); itf != cur.end(); ++itf){
				int f1 = it->index;
				int f2 = *itf;
				if (f1 > f2) std::swap(f1, f2);
				//2. Solid��Solid�̏Փ˔���
				found |= solidPairs.item(f1, f2).Detect(this);
			}
			cur.insert(it->index);
		}else{
			cur.erase(it->index);			//	�I�[�Ȃ̂ō폜�D
		}
	}
	return found;
}
void PHConstraintEngine::SetupDynamics(double dt){
	solids.SetupDynamics(dt);
	points.SetupDynamics(dt);
	joints.SetupDynamics(dt);
}
void PHConstraintEngine::SetupCorrection(double dt){
	solids.SetupCorrection();
	points.SetupCorrection(dt, max_error);
	joints.SetupCorrection(dt, max_error);
}
void PHConstraintEngine::IterateDynamics(){
	int count = 0;
	while(true){
		if(count == max_iter_dynamics){
			//DSTR << "max count." << " iteration count: " << count << " dfsum: " << dfsum << endl;
			break;
		}
		points.IterateDynamics();
		joints.IterateDynamics();

		count++;
	}
}
void PHConstraintEngine::IterateCorrection(){
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
}

void PHConstraintEngine::UpdateSolids(double dt){
	PHSolidAuxs::iterator is;
	PHSolidAux* aux;
	PHSolid* solid;
	Vec3d vnew, wnew;
	for(is = solids.begin(); is != solids.end(); is++){
		aux = *is;
		solid = aux->solid;
		//velocity update
		vnew = aux->v + aux->dv0 + aux->dv;
		wnew = aux->w + aux->dw0 + aux->dw;
		solid->SetVelocity       (solid->GetOrientation() * vnew);
		solid->SetAngularVelocity(solid->GetOrientation() * wnew);
		//position update
		solid->SetCenterPosition(solid->GetCenterPosition() + solid->GetVelocity() * dt + solid->GetOrientation() * aux->dV);
		solid->SetOrientation(
			(solid->GetOrientation() * Quaterniond::Rot(wnew * dt + aux->dW)).unit()
		);
		//solid->SetOrientation((solid->GetOrientation() + solid->GetOrientation().Derivative(solid->GetOrientation() * is->dW)).unit());
		solid->SetOrientation((solid->GetOrientation() * Quaterniond::Rot(/*solid->GetOrientation() * */aux->dW)).unit());
		solid->SetUpdated(true);
	}
}

//#include <windows.h>

void PHConstraintEngine::Step(){
	//LARGE_INTEGER freq, val[2];
	//QueryPerformanceFrequency(&freq);
	
	//���������m
	//QueryPerformanceCounter(&val[0]);
	DetectPenetration();
	//QueryPerformanceCounter(&val[1]);
	//DSTR << "cd " << (double)(val[1].QuadPart - val[0].QuadPart)/(double)(freq.QuadPart) << endl;

	double dt = OCAST(PHScene, GetScene())->GetTimeStep();

	//QueryPerformanceCounter(&val[0]);
	SetupDynamics(dt);
	//QueryPerformanceCounter(&val[1]);
	//DSTR << "sd " << (double)(val[1].QuadPart - val[0].QuadPart)/(double)(freq.QuadPart) << endl;

	//QueryPerformanceCounter(&val[0]);
	IterateDynamics();
	//DSTR << points.size() << endl;
	//for(PHConstraints::iterator it = joints.begin(); it != joints.end(); it++)
	//	DSTR << (*it)->f << endl;
	//QueryPerformanceCounter(&val[1]);
	//DSTR << "id " << (double)(val[1].QuadPart - val[0].QuadPart)/(double)(freq.QuadPart) << endl;

	//QueryPerformanceCounter(&val[0]);
	SetupCorrection(dt);
	//QueryPerformanceCounter(&val[1]);
	//DSTR << "sc " << (double)(val[1].QuadPart - val[0].QuadPart)/(double)(freq.QuadPart) << endl;

	//QueryPerformanceCounter(&val[0]);
	IterateCorrection();
	//QueryPerformanceCounter(&val[1]);
	//DSTR << "ic " << (double)(val[1].QuadPart - val[0].QuadPart)/(double)(freq.QuadPart) << endl;

	UpdateSolids(dt);

}

size_t PHConstraintEngine::GetStateSize() const {
	PHConstraintEngineState s(*this);
	return s.GetSize();
}
void PHConstraintEngine::ConstructState(void* m) const {
	new (m) PHConstraintEngineState(*this);
}
void PHConstraintEngine::DestructState(void* m) const {
	((PHConstraintEngineState*)m)->~PHConstraintEngineState();
}
bool PHConstraintEngine::GetState(void* s) const {
	PHConstraintEngineState* es = ((PHConstraintEngineState*)s);
	PHSolidPairState* solidStates = es->SolidStates();
	PHShapePairState* shapeStates = es->ShapeStates();
	//	solidPairs.item(i,j)�@�� i<j�������g���Ă���̂ł��������ۑ�
	int solidPos=0;
	int shapePos=0;
	for(int j=0; j<solidPairs.width(); ++j){
		for(int i=0; i<j; ++i){
			const PHSolidPair& sp = solidPairs.item(i, j);
			solidStates[solidPos] = sp;
			++solidPos;
			for(int r=0; r<sp.shapePairs.height(); ++r){
				for(int c=0; c<sp.shapePairs.width(); ++c){
					shapeStates[shapePos] = sp.shapePairs.item(r, c);
					++shapePos;
				}
			}
		}
	}
	return true;
}
void PHConstraintEngine::SetState(const void* s){
	PHConstraintEngineState* es = ((PHConstraintEngineState*)s);
	PHSolidPairState* solidStates = es->SolidStates();
	PHShapePairState* shapeStates = es->ShapeStates();
	//	solidPairs.item(i,j)�@�� i<j�������g���Ă���̂ł��������ۑ�
	int solidPos=0;
	int shapePos=0;
	for(int j=0; j<solidPairs.width(); ++j){
		for(int i=0; i<j; ++i){
			PHSolidPair& sp = solidPairs.item(i, j);
			sp.SetState(solidStates[solidPos]);
			++solidPos;
			for(int r=0; r<sp.shapePairs.height(); ++r){
				for(int c=0; c<sp.shapePairs.width(); ++c){
					sp.shapePairs.item(r, c).SetState(shapeStates[shapePos]);
					++shapePos;
				}
			}
		}
	}
}

}

