#include "Physics.h"
#pragma hdrstop
#include <float.h>
#include <Collision/CDDetectorImp.h>


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
			//���C�W���͗��҂̐Î~���C�W���̕��ςƂ���
			con.mu = (sp.shape[0]->material.mu0 + sp.shape[1]->material.mu0) * 0.5;
			
			//�ڐG�_�̍쐬�F
			//�����`����\�����钸�_��analyzer.planes.begin����end�܂ł̓�deleted==false�̂���
			typedef CDQHPlanes<CDContactAnalysisFace>::CDQHPlane Plane;
			Vec3d v, vsum, vproj;
			int numv = 0;
			for(Plane* p = analyzer.planes.begin; p != analyzer.planes.end; p++){
				if(p->deleted)
					continue;
				numv++;
				//sp.center��ʂ�sp.normal��@���Ƃ��镽�ʏ�ɒ��_���ˉe
				//�@���͐��K������Ă���Ƃ���
				v = p->normal / p->dist + sp.commonPoint;
				vsum += v;

				engine->points.push_back(PHContactPoint(engine->contacts.size(), v));
			}
			con.center = vsum / (double)numv;
			engine->contacts.push_back(con);
		}
	}
	return found;
}

//----------------------------------------------------------------------------

OBJECTIMP(PHConstraintEngine, PHEngine);

PHConstraintEngine::PHConstraintEngine(){
	ready = false;
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


void PHConstraintEngine::SetupLCP(double dt){
	//LCP�\�z
	//�eSolid�Ɋ֌W����ϐ�
	for(int i = 0; i < (int)(solids.size()); i++){
		solidAuxs[i].dVlin_nc = /*solids[i]->GetVelocity() + */solidAuxs[i].minv * solids[i]->GetForce() * dt;
		Vec3d w = solids[i]->GetAngularVelocity();
		solidAuxs[i].dVang_nc = /*w + */solidAuxs[i].Iinv * (solids[i]->GetTorque() - w % (solids[i]->GetInertia() * w)) * dt;
	}

	//�eContact�Ɋ֌W����ϐ�
	Vec3d n, c, p, r[2], v[2], vrel, t[2];
	Matrix3d rcross[2];
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
			c = con.center; //�d�S
			for(int i = 0; i < 2; i++){
				solid[i]    =  solids[con.solid[i]];
				solidaux[i] = &solidAuxs[con.solid[i]];
				q[i] = solid[i]->GetPose();
			}
		}
		//���̎��_�ł�pos�͌����`��̒��_�Ȃ̂ŁC����������`��̏d�S��ʂ镽�ʂ֎ˉe����
		ip->pos = ip->pos - ((ip->pos - c) * n) * n;

		for(int i = 0; i < 2; i++){
			r[i] = ip->pos - q[i].pos;	//���̂̒��S����ڐG�_�܂ł̃x�N�g��
			rcross[i] = Matrix3d::Cross(r[i]);
			v[i] = solid[i]->GetVelocity() + solid[i]->GetAngularVelocity() % r[i];	//�ڐG�_�ł̑��x
		}
		//�ڐ��x�N�g��t[0], t[1]
		// *t[0]�͑��Α��x�x�N�g���ɕ��s�ɂȂ�悤�ɂ���(�Ƃ����炵��)
		vrel = v[1] - v[0];
		//t[0] = (n % vrel) % n ���ό`
		t[0] = (vrel - (n * vrel) * n).unit();
		t[1] = t[0] % n;

		for(int i = 0; i < 2; i++){
			// J�s��
			ip->Jlin[i].row(0) = n;
			ip->Jlin[i].row(1) = t[0];
			ip->Jlin[i].row(2) = t[1];
			ip->Jang[i].row(0) = rcross[0] * n;
			ip->Jang[i].row(1) = rcross[0] * t[0];
			ip->Jang[i].row(2) = rcross[0] * t[1];
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
		// b�x�N�g��
		ip->b = ip->Jlin[0] * (solid[0]->GetVelocity() + solidaux[0]->dVlin_nc) + 
				ip->Jang[0] * (solid[0]->GetAngularVelocity() + solidaux[0]->dVang_nc) +
				ip->Jlin[1] * (solid[1]->GetVelocity() + solidaux[1]->dVlin_nc) +
				ip->Jang[1] * (solid[1]->GetAngularVelocity() + solidaux[1]->dVang_nc);

		// Jlin, Jang, b��A�̑Ίp�v�f�Ŋ���
		for(int i = 0; i < 3; i++){
			//0����`�F�b�N�́H
			double diag_inv = 1.0 / ip->A[i][i];
			ip->b[i] *= diag_inv;
			ip->Jlin[0].row(i) *= diag_inv;
			ip->Jlin[1].row(i) *= diag_inv;
			ip->Jang[0].row(i) *= diag_inv;
			ip->Jang[1].row(i) *= diag_inv;
		}
	}
}

void PHConstraintEngine::SetInitialValue(){
	//���x�ω��ʂ͖����ׂ̏ꍇ�ŏ�����
	for(PHSolidAuxs::iterator is = solidAuxs.begin(); is != solidAuxs.end(); is++){
		is->dVlin = is->dVlin_nc;
		is->dVang = is->dVang_nc;
	}
	//�ڐG�͂�0�ŏ�����
	for(PHContactPoints::iterator ip = points.begin(); ip != points.end(); ip++){
		ip->f.clear();
	}
}

bool PHConstraintEngine::CheckConvergence(){
	double e = 0.0;
	for(PHContactPoints::iterator ip = points.begin(); ip != points.end(); ip++)
		e += ip->df.norm();
	DSTR << e << endl;
	return e < 0.001;
}

void PHConstraintEngine::Iteration(){
	PHContactPoints::iterator ip;
	PHContact* con;
	PHSolidAux* solidaux[2];
	//�ڐG��f�̍X�V
	Vec3d fnew;
	int icon = -1;
	for(ip = points.begin(); ip != points.end(); ip++){
		if(icon != ip->contact){
			icon = ip->contact;
			con = &contacts[icon];
			for(int i = 0; i < 2; i++)
				solidaux[i] = &solidAuxs[con->solid[i]];
		}
		fnew = ip->f - (ip->b +
			ip->Jlin[0] * solidaux[0]->dVlin + ip->Jang[0] * solidaux[0]->dVang +
			ip->Jlin[1] * solidaux[1]->dVlin + ip->Jang[1] * solidaux[1]->dVang);

		//fmin = {  0, -mu * f[0], -mu * f[0]};
		//fmax = {Inf,  mu * f[0],  mu * f[0]};
		//fnew[i] = min(max(fmin[i], f[i]), fmax[i]);
		//�������F
		//	�E���C�͂̊e�������ő�Î~���C�����������Ă����͂͒�����\��������̂Ŗ{���͂��������B
		//	�E�Î~���C�Ɠ����C�������l�łȂ��ƈ����Ȃ��B
		
		//�����R�� >= 0�̐���
		fnew[0] = Spr::max(0.0, fnew[0]);
		
		//|���C��| <= �ő�Î~���C�̐���
		double flim = con->mu * fnew[0];		//�ő�Î~���C
		fnew[1] = Spr::min(Spr::max(-flim, fnew[1]), flim);
		fnew[2] = Spr::min(Spr::max(-flim, fnew[2]), flim);		

		ip->df = fnew - ip->f;
		ip->f = fnew;
		DSTR << ip->df << ';' << ip->f << endl;
	}
	//���x�ω�dV�̍X�V
	icon = -1;
	for(ip = points.begin(); ip != points.end(); ip++){
		if(icon != ip->contact){
			icon = ip->contact;
			PHContact& con = contacts[icon];
			for(int i = 0; i < 2; i++)
				solidaux[i] = &solidAuxs[con.solid[i]];
		}
		solidaux[0]->dVlin = ip->Tlin[0] * ip->f;
		solidaux[0]->dVang = ip->Tang[0] * ip->f;
		solidaux[1]->dVlin = ip->Tlin[1] * ip->f;
		solidaux[1]->dVang = ip->Tang[1] * ip->f;
	}
}

void PHConstraintEngine::UpdateSolids(double dt){
	PHSolids::iterator is;
	PHSolidAuxs::iterator isaux;
	for(is = solids.begin(), isaux = solidAuxs.begin(); is != solids.end(); is++, isaux++){
		(*is)->SetVelocity((*is)->GetVelocity() + (isaux->dVlin_nc + isaux->dVlin) * dt);
		(*is)->SetAngularVelocity((*is)->GetAngularVelocity() + (isaux->dVang_nc + isaux->dVang) * dt);
		(*is)->SetUpdated(true);
	}
}

void PHConstraintEngine::PrintContacts(){
	PHContactPoints::iterator ip;
	int icon = -1;
	for(ip = points.begin(); ip != points.end(); ip++){
		if(icon != ip->contact){
			icon = ip->contact;
			PHContact& con = contacts[icon];
			DSTR << "contact: " << icon << " normal: " << con.normal << " center: " << con.center << endl;
		}
		DSTR << "point: " << ip->pos << endl;
	}
}

void PHConstraintEngine::Step(){
	if(!ready)
		Init();

	//���������m
	if(!Detect())
		return;

	PrintContacts();

	double dt = OCAST(PHScene, GetScene())->GetTimeStep();

	//LCP��ݒ�
	SetupLCP(dt);

	//����ϐ��̏����l��ݒ�
	SetInitialValue();

	while(true){
		//����
		Iteration();
		//�I������
		if(CheckConvergence())
			break;		
	}

	//Solid�ɔ��f
	UpdateSolids(dt);

}

}
