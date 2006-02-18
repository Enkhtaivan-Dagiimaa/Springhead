#include "Physics.h"
#pragma hdrstop
#include <float.h>
#include <Collision/CDDetectorImp.h>
#include <Collision/CDQuickHull2D.h>
#include <Collision/CDQuickHull2DImp.h>


using namespace PTM;
using namespace std;
namespace Spr{;

// AABB�Ń\�[�g���邽�߂̍\����
struct Edge{
	float edge;				///<	�[�̈ʒu(�O���[�o���n)
	int	index;				///<	���� solid�̈ʒu
	bool bMin;				///<	���[�Ȃ�true
	bool operator < (const Edge& s) const { return edge < s.edge; }
};
typedef std::vector<Edge> Edges;

//----------------------------------------------------------------------------
// PHContact

//----------------------------------------------------------------------------
// PHConstraintEngine

//----------------------------------------------------------------------------
//	PHSolidPair

void PHConstraintEngine::PHSolidPair::Init(PHSolid* s0, PHSolid* s1){
	int ns0 = s0->shapes.size(), ns1 = s1->shapes.size();
	shapePairs.resize(ns0, ns1);
	for(int i = 0; i < ns0; i++)for(int j = 0; j < ns1; j++){
		CDShapePair& sp = shapePairs.item(i, j);
		sp.shape[0] = s0->shapes[i];
		sp.shape[1] = s1->shapes[j];
	}
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

bool PHConstraintEngine::PHSolidPair::Detect(int is0, int is1, PHConstraintEngine* engine){
	// ��������Shape�ɂ���BBox���x�����������Α����Ȃ邩���H
	static CDContactAnalysis analyzer;

	unsigned ct = OCAST(PHScene, engine->GetScene())->GetCount();
	
	// �����ꂩ��Solid�Ɍ`�󂪊��蓖�Ă��Ă��Ȃ��ꍇ�̓G���[
	PHSolid *s0 = engine->solids[is0], *s1 = engine->solids[is1];
	if(s0->NShape() == 0 || s1->NShape() == 0)
		return false;

	// �S�Ă�shape pair�ɂ��Č����𒲂ׂ�
	bool found = false;
	for(int i = 0; i < (int)(s0->shapes.size()); i++)for(int j = 0; j < (int)(s1->shapes.size()); j++){
		CDShapePair& sp = shapePairs.item(i, j);
		sp.UpdateShapePose(s0->GetPose(), s1->GetPose());
		//����shape pair�̌�������
		if(sp.Detect(ct)){
			found = true;
			//�����`��̌v�Z
			analyzer.FindIntersection(&sp);
			//�����̖@���ƒ��S�𓾂�
			analyzer.CalcNormal(&sp);

			//�ڐG���쐬
			PHContact con;
			con.shape[0] = i;
			con.shape[1] = j;
			con.solid[0] = is0;
			con.solid[1] = is1;
			con.normal = sp.normal;
			con.depth = sp.depth;
			//���C�W���͗��҂̐Î~���C�W���̕��ςƂ���
			con.mu = (sp.shape[0]->material.mu0 + sp.shape[1]->material.mu0) * 0.5;
			
			//�ڐG�_�̍쐬�F
			//�����`����\�����钸�_��analyzer.planes.begin����end�܂ł̓�deleted==false�̂���
			typedef CDQHPlanes<CDContactAnalysisFace>::CDQHPlane Plane;
			static std::vector<ContactVertex> isVtxs;
			isVtxs.clear();
			for(Plane* p = analyzer.planes.begin; p != analyzer.planes.end; p++){
				if(p->deleted) continue;
				isVtxs.push_back(p->normal / p->dist);
			}
			ContactVertex::ex = (-0.1<con.normal.z && con.normal.z < 0.1) ?
				con.normal ^ Vec3f(0,0,1) : con.normal ^ Vec3f(1,0,0);
			ContactVertex::ex.unitize();
			ContactVertex::ey = con.normal ^ ContactVertex::ex;

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
				engine->points.push_back(PHContactPoint(engine->contacts.size(), v));
			}
#ifdef DEBUG_CONTACTOUT
			DSTR << engine->points.size()-n << " contacts:";
			for(unsigned i=n; i<engine->points.size(); ++i){
				DSTR << engine->points[i].pos;
			}
			DSTR << std::endl;
#endif
			engine->contacts.push_back(con);
		}
	}
	return found;
}

//----------------------------------------------------------------------------

OBJECTIMP(PHConstraintEngine, PHEngine);

PHConstraintEngine::PHConstraintEngine(){
	ready = false;
	max_iter_dynamics = 10;
	max_iter_correction = 10;
	step_size = 1.0;
	converge_criteria = 0.00000001;
}

PHConstraintEngine::~PHConstraintEngine(){

}

void PHConstraintEngine::Add(PHSolid* s){
	if(solids.Find(s) == 0){
		solids.push_back(s);
		Invalidate();
	}
}

void PHConstraintEngine::Remove(PHSolid* s){
	if(solids.Erase(s))
		Invalidate();
}

void PHConstraintEngine::Init(){
	int N = solids.size();

	solidAuxs.resize(N);
	for(int i = 0; i < N; i++){
		solidAuxs[i].minv = solids[i]->GetMassInv();
		solidAuxs[i].Iinv = solids[i]->GetInertiaInv();
	}

	//�o�^����Ă���Solid�̐��ɍ��킹��solidPairs��shapePairs��resize
	solidPairs.resize(N, N);
	for(int i = 0; i < N; i++)for(int j = i+1; j < N; j++){
		PHSolidPair& sp = solidPairs.item(i, j);
		sp.Init(solids[i], solids[j]);
	}

	ready = true;
}

bool PHConstraintEngine::Detect(){
	/* �ȉ��̗���Ō��������߂�
		1. Solid��BBox���x���ł̌�������(z���\�[�g)�D�����̂�����̖����g�����O
		2. �eSolid�̑g�ɂ���
			2a. Shape��BBox���x���ł̌������� (������)
			2b. �eShape�̑g�ɂ���GJK�Ō����`��𓾂�
			2c. �����`�󂩂�@�������߁A�@���Ɋւ��Č`����ˉe���C���̒��_��ڐG�_�Ƃ���
			2d. ����ꂽ�ڐG�_����PHContactPoints�ɋl�߂Ă���
	 */

	contacts.clear();
	points.clear();
	int N = solids.size();

	//1. BBox���x���̏Փ˔���
	Vec3f dir(0,0,1);
	Edges edges;
	edges.resize(2 * N);
	Edges::iterator eit = edges.begin();
	for(int i = 0; i < N; ++i){
		solids[i]->GetBBoxSupport(dir, eit[0].edge, eit[1].edge);
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
				found |= solidPairs.item(f1, f2).Detect(f1, f2, this);
			}
			cur.insert(it->index);
		}else{
			cur.erase(it->index);			//	�I�[�Ȃ̂ō폜�D
		}
	}
	return found;
}

/*void PHConstraintEngine::PrintContacts(){
	PHContactPoints::iterator ip;
	int icon = -1;
	for(ip = points.begin(); ip != points.end(); ip++){
		if(icon != ip->contact){
			icon = ip->contact;
			PHContact& con = contacts[icon];
			DSTR << "contact: " << icon << " normal: " << con.normal << " center: " << con.center << endl;
		}
		DSTR << "point: " << ip->pos <<
			" normal: " << ip->Jlin[0].row(0) << "tangent0: " << ip->Jlin[0].row(1) << endl;
	}
}*/

//LCP�\�z
void PHConstraintEngine::SetupDynamics(double dt){
	//�eSolid�Ɋ֌W����ϐ�
	{
		Quaterniond q;
		Vec3d v, w, f, t;
		for(int i = 0; i < (int)(solids.size()); i++){
			q = solids[i]->GetOrientation();
			v = q.Conjugated() * solids[i]->GetVelocity();
			w = q.Conjugated() * solids[i]->GetAngularVelocity();
			f = q.Conjugated() * solids[i]->nextForce;
			t = q.Conjugated() * solids[i]->nextTorque;
			solidAuxs[i].Vlin0 = v + solidAuxs[i].minv * f * dt;
			solidAuxs[i].Vang0 = w + solidAuxs[i].Iinv * (t - w % (solids[i]->GetInertia() * w)) * dt;
			solidAuxs[i].dVlin.clear();
			solidAuxs[i].dVang.clear();
		}
	}
	//�eContact�Ɋ֌W����ϐ�
	Vec3d n, p, r[2], v[2], vrel, vrelproj, t[2];
	Matrix3d rcross[2], R[2];
	Posed q[2];
	PHSolid* solid[2];
	PHSolidAux* solidaux[2];
	int icon = -1;
	for(PHContactPoints::iterator ip = points.begin(); ip != points.end(); ip++){
		//�ڐG�̃C���f�b�N�X��K�v�Ȃ�X�V
		if(icon != ip->contact){
			icon = ip->contact;
			PHContact& con = contacts[icon];
			n = con.normal;	//�@��
			for(int i = 0; i < 2; i++){
				solid[i]    =  solids[con.solid[i]];
				solidaux[i] = &solidAuxs[con.solid[i]];
				q[i] = solid[i]->GetPose();
				q[i].Ori().ToMatrix(R[i]);
			}
		}

		for(int i = 0; i < 2; i++){
			r[i] = ip->pos - q[i].Pos();	//���̂̒��S����ڐG�_�܂ł̃x�N�g��
			rcross[i] = Matrix3d::Cross(r[i]);
			v[i] = solid[i]->GetVelocity() + solid[i]->GetAngularVelocity() % r[i];	//�ڐG�_�ł̑��x
		}
		//�ڐ��x�N�g��t[0], t[1]
		// *t[0]�͑��Α��x�x�N�g���ɕ��s�ɂȂ�悤�ɂ���(�Ƃ����炵��)
		vrel = v[1] - v[0];
		vrelproj = vrel - (n * vrel) * n;	//���Α��x�x�N�g����@���ɒ������镽�ʂɎˉe�����x�N�g��
		double vrelproj_norm = vrelproj.norm();
		if(vrelproj_norm < 1.0e-10){
			t[0] = n % Vec3d(1.0, 0.0, 0.0);	
			if(t[0].norm() < 1.0e-10)
				t[0] = n % Vec3d(0.0, 1.0, 0.0);
			t[0].unitize();
		}
		else{
			t[0] = vrelproj / vrelproj_norm;
		}
		t[1] = t[0] % n;
		for(int i = 0; i < 2; i++){
			// J�s��
			ip->Jlin[i].row(0) = n;
			ip->Jlin[i].row(1) = t[0];
			ip->Jlin[i].row(2) = t[1];
			ip->Jang[i] = ip->Jlin[i] * (-rcross[i]);
			ip->Jlin[i] = ip->Jlin[i] * solid[i]->GetRotation();
			ip->Jang[i] = ip->Jang[i] * solid[i]->GetRotation();
			if(i == 0){
				ip->Jlin[i] *= -1.0;
				ip->Jang[i] *= -1.0;
			}
			// T�s��
			ip->Tlin[i] = solidaux[i]->minv * ip->Jlin[i].trans();
			ip->Tang[i] = solidaux[i]->Iinv * ip->Jang[i].trans();
		}
		// A�s��
		ip->A = ip->Jlin[0] * ip->Tlin[0] + ip->Jang[0] * ip->Tang[0] +
				ip->Jlin[1] * ip->Tlin[1] + ip->Jang[1] * ip->Tang[1];
		ip->Ainv = ip->A.inv();
		// b�x�N�g��
		ip->b = ip->Jlin[0] * (solidaux[0]->Vlin0) + 
				ip->Jang[0] * (solidaux[0]->Vang0) +
				ip->Jlin[1] * (solidaux[1]->Vlin0) +
				ip->Jang[1] * (solidaux[1]->Vang0);
		ip->b = ip->Ainv * ip->b;
		ip->Jlin[0] = ip->Ainv * ip->Jlin[0];
		ip->Jang[0] = ip->Ainv * ip->Jang[0];
		ip->Jlin[1] = ip->Ainv * ip->Jlin[1];
		ip->Jang[1] = ip->Ainv * ip->Jang[1];
		ip->f.clear();
		// Jlin, Jang, b��A�̑Ίp�v�f�Ŋ���
		/*double diag_inv;
		for(int i = 0; i < 3; i++){
			//0����`�F�b�N�́H
			diag_inv = 1.0 / ip->A[i][i];
			ip->b[i] *= diag_inv;
			ip->Jlin[0].row(i) *= diag_inv;
			ip->Jlin[1].row(i) *= diag_inv;
			ip->Jang[0].row(i) *= diag_inv;
			ip->Jang[1].row(i) *= diag_inv;
		}*/
	}
}

void PHConstraintEngine::SetupCorrection(){
	PHContactPoints::iterator ip;
	PHContact* con;
	PHSolidAux* solidaux[2];
	Vec3d Vlin[2], Vang[2];
	//Dynamics�̉e�����l��������ł̊e�ڐG�_�ł̌����[�x
	int icon = -1;
	for(ip = points.begin(); ip != points.end(); ip++){
		if(icon != ip->contact){
			icon = ip->contact;
			con = &contacts[icon];
			for(int i = 0; i < 2; i++){
				solidaux[i] = &solidAuxs[con->solid[i]];
				Vlin[i] = solidaux[i]->Vlin0 + solidaux[i]->dVlin;
				Vang[i] = solidaux[i]->Vang0 + solidaux[i]->dVang;
			}
		}
		ip->B = -con->depth +
			ip->Jlin[0].row(0) * Vlin[0] + ip->Jang[0].row(0) * Vang[0] + ip->Jlin[1].row(0) * Vlin[1] + ip->Jang[1].row(0) * Vang[1];
		ip->B *= (0.1 / ip->A[0][0]);
		ip->F = 0.0;
		//ip->b = ip->Ainv * ip->b;
	}
	for(PHSolidAuxs::iterator is = solidAuxs.begin(); is != solidAuxs.end(); is++){
		is->dVlin.clear();
		is->dVang.clear();
	}
}

void PHConstraintEngine::IterateDynamics(){
	PHContactPoints::iterator ip;
	PHContact* con;
	PHSolidAux* solidaux[2];
	Vec3d fnew;
	double dfsum;
	int count = 0;
	while(true){
		if(count == max_iter_dynamics){
			DSTR << "max count." << " iteration count: " << count << " dfsum: " << dfsum << endl;
			break;
		}
		dfsum = 0.0;
		//����
		//�ڐG��f�̍X�V
		int icon = -1;
		for(ip = points.begin(); ip != points.end(); ip++){
			if(icon != ip->contact){
				icon = ip->contact;
				con = &contacts[icon];
				for(int i = 0; i < 2; i++)
					solidaux[i] = &solidAuxs[con->solid[i]];
			}

			fnew = ip->f - step_size * (ip->b +
				ip->Jlin[0] * solidaux[0]->dVlin + ip->Jang[0] * solidaux[0]->dVang +
				ip->Jlin[1] * solidaux[1]->dVlin + ip->Jang[1] * solidaux[1]->dVang);

			//�����R�� >= 0�̐���
			fnew[0] = Spr::max(0.0, fnew[0]);
			
			//|���C��| <= �ő�Î~���C�̐���
			//	�E���C�͂̊e�������ő�Î~���C�����������Ă����͂͒�����\��������̂Ŗ{���͂��������B
			//	�E�Î~���C�Ɠ����C�������l�łȂ��ƈ����Ȃ��B
			double flim = con->mu * fnew[0];		//�ő�Î~���C
			fnew[1] = Spr::min(Spr::max(-flim, fnew[1]), flim);
			fnew[2] = Spr::min(Spr::max(-flim, fnew[2]), flim);		

			ip->df = fnew - ip->f;
			dfsum += ip->df.square();
			ip->f = fnew;
			
			solidaux[0]->dVlin += (ip->Tlin[0] * ip->df);
			solidaux[0]->dVang += (ip->Tang[0] * ip->df);
			solidaux[1]->dVlin += (ip->Tlin[1] * ip->df);
			solidaux[1]->dVang += (ip->Tang[1] * ip->df);

		}
		count++;
		//�I������
		if(dfsum < converge_criteria){
			DSTR << "converged." << " iteration count: " << count << " dfsum: " << dfsum << endl;
			break;
		}
	}
}

void PHConstraintEngine::IterateCorrection(){
	PHContactPoints::iterator ip;
	PHContact* con;
	PHSolidAux* solidaux[2];
	double Fnew, dFsum;
	int count = 0;
	while(true){
		if(count == max_iter_correction){
			DSTR << "max count." << " iteration count: " << count << " dFsum: " << dFsum << endl;
			break;
		}
		dFsum = 0.0;
		
		int icon = -1;
		for(ip = points.begin(); ip != points.end(); ip++){
			if(icon != ip->contact){
				icon = ip->contact;
				con = &contacts[icon];
				for(int i = 0; i < 2; i++)
					solidaux[i] = &solidAuxs[con->solid[i]];
			}

			Fnew = ip->F - step_size * (ip->B +
				ip->Jlin[0].row(0) * solidaux[0]->dVlin + ip->Jang[0].row(0) * solidaux[0]->dVang +
				ip->Jlin[1].row(0) * solidaux[1]->dVlin + ip->Jang[1].row(0) * solidaux[1]->dVang);

			//�����R�� >= 0�̐���
			Fnew = Spr::max(0.0, Fnew);
			
			ip->dF = Fnew - ip->F;
			dFsum += ip->dF * ip->dF;
			ip->F = Fnew;
			
			solidaux[0]->dVlin += (ip->Tlin[0].col(0) * ip->dF);
			solidaux[0]->dVang += (ip->Tang[0].col(0) * ip->dF);
			solidaux[1]->dVlin += (ip->Tlin[1].col(0) * ip->dF);
			solidaux[1]->dVang += (ip->Tang[1].col(0) * ip->dF);

		}
		count++;
		//�I������
		if(dFsum < converge_criteria){
			DSTR << "converged." << " iteration count: " << count << " dFsum: " << dFsum << endl;
			break;
		}
	}
}

void PHConstraintEngine::UpdateDynamics(double dt){
	PHSolids::iterator is;
	PHSolidAuxs::iterator isaux;
	for(is = solids.begin(), isaux = solidAuxs.begin(); is != solids.end(); is++, isaux++){
		PHSolid* s = *is;
		//velocity update
		s->SetVelocity       (s->GetOrientation() * (isaux->Vlin0 + isaux->dVlin));
		s->SetAngularVelocity(s->GetOrientation() * (isaux->Vang0 + isaux->dVang));
		//position update
		s->SetCenterPosition(s->GetCenterPosition() + s->GetVelocity() * dt);
		s->SetOrientation(s->GetOrientation() + s->GetOrientation().Derivative(s->GetAngularVelocity()) * dt);

		DSTR << s->GetVelocity() << " ; " << s->GetCenterPosition() << endl;

		(*is)->SetUpdated(true);
	}
}

void PHConstraintEngine::UpdateCorrection(){
	PHSolids::iterator is;
	PHSolidAuxs::iterator isaux;
	for(is = solids.begin(), isaux = solidAuxs.begin(); is != solids.end(); is++, isaux++){
		PHSolid* s = *is;
		//position update
		s->SetCenterPosition(s->GetCenterPosition() + s->GetOrientation() * isaux->dVlin);
		s->SetOrientation(s->GetOrientation() + s->GetOrientation().Derivative(s->GetOrientation() * isaux->dVang));

		(*is)->SetUpdated(true);
	}
}

void PHConstraintEngine::Step(){
	if(!ready)
		Init();

	//���������m
	if(!Detect())
		return;

	double dt = OCAST(PHScene, GetScene())->GetTimeStep();

	//PrintContacts();

	DSTR << "dynamics: " << endl;
	SetupDynamics(dt);
	IterateDynamics();
	UpdateDynamics(dt);
	
	DSTR << "correction: " << endl;
	SetupCorrection();
	IterateCorrection();
	UpdateCorrection();

}

}
