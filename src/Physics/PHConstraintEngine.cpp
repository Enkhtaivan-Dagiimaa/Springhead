#include "Physics.h"
#pragma hdrstop
#include <float.h>
#include <Collision/CDDetectorImp.h>


using namespace PTM;
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

	unsigned ct = ((PHScene*)(engine->GetScene()))->GetCount();
	
	PHSolid *s0 = engine->solids[is0], *s1 = engine->solids[is1];

	// �S�Ă�shape pair�ɂ��Č����𒲂ׂ�
	bool found = false;
	for(int i = 0; i < s0->shapes.size(); i++)for(int j = 0; j < s1->shapes.size(); j++){
		CDShapePair& sp = shapePairs.item(i, j);
		sp.UpdateShapePose(s0->GetPose(), s1->GetPose());
		//����shape pair�̌�������
		if(sp.Detect(ct)){
			found = true;
			//�����`��̌v�Z
			analyzer.FindIntersection(&sp);
			//�����̖@���ƒ��S�𓾂�
			analyzer.CalcNormal(&sp);

			//���C�W���͗��҂̐Î~���C�W���̕��ςƂ���
			double mu = (sp.shape[0]->material.mu0 + sp.shape[1]->material.mu0) * 0.5;
			//�ڐG���쐬
			engine->contacts.push_back(PHContact(is0, is1, i, j, sp.normal, sp.center, mu));

			//�ڐG�_�̍쐬�F
			//�����`����\�����钸�_��analyzer.planes.begin����end�܂ł̓�deleted==false�̂���
			typedef CDQHPlanes<CDContactAnalysisFace>::CDQHPlane Plane;
			Vec3d v, vproj;
			for(Plane* p = analyzer.planes.begin; p != analyzer.planes.end; p++){
				if(p->deleted)
					continue;
				//sp.center��ʂ�sp.normal��@���Ƃ��镽�ʏ�ɒ��_���ˉe
				//�@���͐��K������Ă���Ƃ���
				v = p->normal;
                vproj = v - ((v - sp.center) * sp.normal) * sp.normal;

				engine->points.push_back(PHContactPoint(engine->contacts.size() - 1, vproj));
			}
		}
	}
	return found;
}

//----------------------------------------------------------------------------

OBJECTIMP(PHConstraintEngine, PHEngine);

PHConstraintEngine::PHConstraintEngine(){
	bReady = false;
}

PHConstraintEngine::~PHConstraintEngine(){

}

void PHConstraintEngine::AddSolid(PHSolid* s){
	if(solids.Find(s) == 0){
		solids.push_back(s);
		bReady = false;
	}
}

void PHConstraintEngine::RemoveSolid(PHSolid* s){
	if(solids.Erase(s))
		bReady = false;
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

	bReady = true;
}

void PHConstraintEngine::Detect(){
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
	for(Edges::iterator it = edges.begin(); it != edges.end(); ++it){
		if (it->bMin){						//	���[��������C���X�g���̕��̂Ɣ���
			for(SolidSet::iterator itf=cur.begin(); itf != cur.end(); ++itf){
				int f1 = it->index;
				int f2 = *itf;
				if (f1 > f2) std::swap(f1, f2);
				//2. Solid��Solid�̏Փ˔���
				solidPairs.item(f1, f2).Detect(f1, f2, this);
			}
			cur.insert(it->index);
		}else{
			cur.erase(it->index);			//	�I�[�Ȃ̂ō폜�D
		}
	}
}


void PHConstraintEngine::SetupLCP(){
	double dt = ((PHSceneIf*)GetScene())->GetTimeStep();

	//LCP�\�z
	//�eSolid�Ɋ֌W����ϐ�
	for(int i = 0; i < solids.size(); i++){
		solidAuxs[i].dVlin_nc = solids[i]->GetVelocity() + solidAuxs[i].minv * solids[i]->GetForce() * dt;
		Vec3d w = solids[i]->GetAngularVelocity();
		solidAuxs[i].dVang_nc = w + solidAuxs[i].Iinv * (solids[i]->GetTorque() - w % (solids[i]->GetInertia() * w)) * dt;
	}

	//�eContact�Ɋ֌W����ϐ�
	Vec3d n, c, r[2], v[2], vrel, t[2];
	Matrix3d rcross[2];
	Posed q[2];
	PHSolid* solid[2];
	PHSolidAux* solidaux[2];
	int icon = 0;
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
			}
		}
		c = ip->pos;	//�ڐG�_
		for(int i = 0; i < 2; i++){
			r[i] = c - q[i].pos;	//���̂̒��S����ڐG�_�܂ł̃x�N�g��
			rcross[i] = Matrix3d::Cross(r[i]);
			v[i] = solid[i]->GetVelocity() + solid[i]->GetAngularVelocity() % r[i];	//�ڐG�_�ł̑��x
		}
		//�ڐ��x�N�g��t[0], t[1]
		// *t[0]�͑��Α��x�x�N�g���ɕ��s�ɂȂ�悤�ɂ���(�Ƃ����炵��)
		vrel = v[1] - v[0];
		//t[0] = (n % vrel) % n ���ό`
		t[0] = vrel - (n * vrel) * n;
		t[1] = t[0] % n;

		for(int i = 0; i < 2; i++){
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
			ip->Tlin[i] = solidaux[i]->minv * ip->Jlin[i].trans();
			ip->Tang[i] = solidaux[i]->Iinv * ip->Jang[i].trans();
		}
		ip->b = ip->Jlin[0] * solidaux[0]->dVlin_nc + ip->Jang[0] * solidaux[0]->dVang_nc +
				ip->Jlin[1] * solidaux[1]->dVlin_nc + ip->Jang[1] * solidaux[1]->dVang_nc;
	}

	//A�s��̐ݒ�
	/*
	��(lhs(i),lhs(j)) * (1/m_lhs(i) J_lin(i,lhs(i)) J_lin(j,lhs(j))' + J_ang(i,lhs(i)) I_lhs(i)^-1 J_ang(j,lhs(j))') +
	��(lhs(i),rhs(j)) * (1/m_lhs(i) J_lin(i,lhs(i)) J_lin(j,rhs(j))' + J_ang(i,lhs(i)) I_lhs(i)^-1 J_ang(j,rhs(j))') +
	��(rhs(i),lhs(j)) * (1/m_rhs(i) J_lin(i,rhs(i)) J_lin(j,lhs(j))' + J_ang(i,rhs(i)) I_rhs(i)^-1 J_ang(j,lhs(j))')
	��(rhs(i),rhs(j)) * (1/m_rhs(i) J_lin(i,rhs(i)) J_lin(j,rhs(j))' + J_ang(i,rhs(i)) I_rhs(i)^-1 J_ang(j,rhs(j))') +
	*/
	//A�͑Ώ̍s��Ȃ̂őΊp����я�O�p�v�f�̂݌v�Z
	A.resize(points.size(), points.size());
	PHContactPoints::iterator ip_row, ip_col;
	PHLCPMatrix::iterator iA;
	int icon_row, icon_col, solid_row[2], solid_col[2];
	icon_row = 0;
	for(ip_row = points.begin(); ip_row != points.end(); ip_row++){
		if(ip_row->contact != icon_row){
			icon_row = ip_row->contact;
			PHContact& con = contacts[icon_row];
			solid_row[0] = con.solid[0];
			solid_row[1] = con.solid[1];
		}
		int i = ip_row - points.begin();
		iA = &A.item(i, i);
		icon_col = icon_row;
		for(ip_col = ip_row; ip_col != points.end(); ip_col++){
			if(ip_col->contact != icon_col){
				icon_col = ip_col->contact;
				PHContact& con = contacts[icon_col];
				solid_col[0] = con.solid[0];
				solid_col[1] = con.solid[1];
			}		
			iA->clear();
			if(solid_row[0] == solid_col[0])
				*iA += ip_row->Jlin[0] * ip_col->Tlin[0] + ip_row->Jang[0] * ip_col->Tang[0];
			if(solid_row[0] == solid_col[1])
				*iA += ip_row->Jlin[0] * ip_col->Tlin[1] + ip_row->Jang[0] * ip_col->Tang[1];
			if(solid_row[1] == solid_col[0])
				*iA += ip_row->Jlin[1] * ip_col->Tlin[0] + ip_row->Jang[1] * ip_col->Tang[0];
			if(solid_row[1] == solid_col[1])
				*iA += ip_row->Jlin[1] * ip_col->Tlin[1] + ip_row->Jang[1] * ip_col->Tang[1];
			iA++;
		}
	}

	//�e�ڐG�_��b��Jlin, Jang��A�̑Ίp�v�f�Ŋ���
	iA = A.begin();
	for(PHContactPoints::iterator ip = points.begin(); ip != points.end(); ip++){
		for(int i = 0; i < 3; i++){
			//0����`�F�b�N�́H
			double diag_inv = 1.0 / (*iA)[i][i];
			ip->b[i] *= diag_inv;
			ip->Jlin[0].row(i) *= diag_inv;
			ip->Jlin[1].row(i) *= diag_inv;
			ip->Jang[0].row(i) *= diag_inv;
			ip->Jang[1].row(i) *= diag_inv;
		}
		iA += points.size() + 1;	//���̑Ίp�v�f��
	}
}

void PHConstraintEngine::SetInitialValue(){
	//���x�ω��ʂ͖����ׂ̏ꍇ�ŏ�����
	for(PHSolidAuxArray::iterator is = solidAuxs.begin(); is != solidAuxs.end(); is++){
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
	return e < 0.001;
}

void PHConstraintEngine::UpdateLCP(){
	PHContactPoints::iterator ip;
	PHContact* con;
	PHSolidAux* solidaux[2];
	//�ڐG��f�̍X�V
	Vec3d fnew;
	int icon = 0;
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
	}
	//���x�ω�dV�̍X�V
	icon = 0;
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

void PHConstraintEngine::Step(){
	if(!bReady)
		Init();

	//���������m
	Detect();

	//LCP��ݒ�
	SetupLCP();

	//����ϐ��̏����l��ݒ�
	SetInitialValue();

	while(true){
		//terminate condition
		if(CheckConvergence())
			break;
		
		UpdateLCP();
	}

}

}
