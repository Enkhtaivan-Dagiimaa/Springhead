#include "Physics.h"
#pragma hdrstop
#include <float.h>
#include <Collision/CDDetectorImp.h>


using namespace PTM;
namespace Spr{;


/*
//	�Փˌ��o�ƐڐG�͂̌v�Z
bool PHConstraintEngine::PHSolidPair::Detect(PHConstraintEngine* engine){
	bool rv = false;
	int ct = ((PHScene*)engine->GetScene())->GetCount();
	double dt = ((PHScene*)engine->GetScene())->GetTimeStep();

	//	Shape���m�̐ڐG����
	typedef std::vector<PHShapePair*> Contacts;
	Contacts contacts;
	for(int i=0; i<shapePairs.height(); ++i){
		for(int j=0; j<shapePairs.width(); ++j){
			PHShapePair* shapePair = shapePairs.item(i,j);
			shapePair->UpdateShapePose(solid[0].solid->GetPose(), solid[1].solid->GetPose());
			if ( shapePair->Detect(ct) ){
				rv = true;
				contacts.push_back(shapePair);
				static CDContactAnalysis analyzer;
				analyzer.FindIntersection(shapePair);	//	�ڐG�`��̉��
				analyzer.CalcNormal(shapePair);			//	�@���x�N�g���̌v�Z

			}
		}
	}

	return rv;
}
*/

//----------------------------------------------------------------------------
OBJECTIMP(PHConstraintEngine, PHEngine);

void PHConstraintEngine::AddSolid(PHSolid* s){
	if(solids.Find(s) == 0)
		solids.push_back(s);
	Init();
}

void PHConstraintEngine::RemoveSolid(PHSolid* s){
	solids.Erase(s);
	Init();
}

void PHConstraintEngine::Init(){
	int N = solids.size();
	minv.resize(N);
	Iinv.resize(N);
	for(int i = 0; i < N; i++){
		minv[i] = solids[i]->GetMassInv();
		Iinv[i] = solids[i]->GetInertiaInv();
	}

}

/**	Solid�̒[�̈ʒu�������C�\�[�g��������	*/
struct PHSolidEdge{
	float edge;				///<	�[�̈ʒu(�O���[�o���n)
	int solid;				///<	���� solid�̈ʒu
	bool bMin;				///<	���[�Ȃ�true
	bool operator < (const PHSolidEdge& s) const { return edge < s.edge; }
};

void PHConstraintEngine::Step(){
/*
	//BBox���x���̏Փ˔���
	Vec3f dir(0,0,1);
	typedef std::vector<PHSolidEdge> Edges;
	Edges edges;
	edges.resize(solids.size()*2);
	Edges::iterator eit = edges.begin();
	for(unsigned int i = 0; i < solids.size(); ++i){
		solids[i]->GetBBoxSupport(dir, eit[0].edge, eit[1].edge);
		if (!_finite(eit[0].edge) || !_finite(eit[1].edge)){
			DSTR << solids[i]->GetName() << " min:" << eit[0].edge << " max:" << eit[1].edge << std::endl;
			solids[i]->GetBBoxSupport(dir, eit[0].edge, eit[1].edge);
		}
		eit[0].solid = i; eit[0].bMin = true;
		eit[1].solid = i; eit[1].bMin = false;
		eit += 2;
	}
	if (edges.size() > 200){
		DSTR << "strange edges" << std::endl;
		DSTR << (unsigned)&*edges.begin() << (unsigned)&*edges.end();
	}
	std::sort(edges.begin(), edges.end());
	//�[���猩�Ă����āC�ڐG�̉\��������m�[�h�̔��������D
	typedef std::set<int> SolidSet;
	SolidSet cur;							//	���݂�Solid�̃Z�b�g
	for(Edges::iterator it = edges.begin(); it != edges.end(); ++it){
		if (it->bMin){						//	���[��������C���X�g���̕��̂Ɣ���
			for(SolidSet::iterator itf=cur.begin(); itf != cur.end(); ++itf){
				int f1 = it->solid;
				int f2 = *itf;
				if (f1 > f2) std::swap(f1, f2);
				PHSolidPair* pair =  solidPairs.item(f1, f2);
				//	Solid��Solid�̏Փ˔���
				pair->Detect(this);	//	�ċA�I�ɏՓ˔���(GJK���g�p)
			}
			cur.insert(it->solid);
		}else{
			cur.erase(it->solid);			//	�I�[�Ȃ̂œ��Y�t���[�����폜�D
		}
	}
*/
	//�����܂ł̏�����PHContact�̔z��𓾂�.
	//��������Ƃ�LCP�\�z
	//���R�r�s��ɒl���l�߂�
	Vec3d n, c, r[2], v[2], vrel, t[2];
	Matrix3d rcross[2];
	Posed q[2];
	PHSolid* solid[2];
	for(PHContacts::iterator iv = contacts.begin(); iv != contacts.end(); iv++){
		n = iv->normal;	//�@��
		for(PHContactPoints::iterator ip = iv->points.begin(); ip != iv->points.end(); ip++){
			c = ip->pos;	//�ڐG�_
			for(int i = 0; i < 2; i++){
				solid[i] = solids[iv->solids[i]];
                q[i] = solid[i]->GetPose();
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

			ip->Jlin[0].row(0) = -n;
			ip->Jlin[0].row(1) = -t[0];
			ip->Jlin[0].row(2) = -t[1];
			ip->Jang[0].row(0) = -rcross[0] * n;
			ip->Jang[0].row(1) = -rcross[0] * t[0];
			ip->Jang[0].row(2) = -rcross[0] * t[1];
			ip->Jlin[1].row(0) =  n;
			ip->Jlin[1].row(1) =  t[0];
			ip->Jlin[1].row(2) =  t[1];
			ip->Jang[1].row(0) =  rcross[1] * n;
			ip->Jang[1].row(1) =  rcross[1] * t[0];
			ip->Jang[1].row(2) =  rcross[1] * t[1];
		}
	}

	//LCP������
	/*	u		: �e���̂̑��x����ׂ��x�N�g��
		udot	: u�̎��Ԕ���
		w		: �ڐG�_�ł̑��x
		J		: u����w��^���郄�R�r�s��. w = J u
		M		: �����s��
		dt		: �I�C���[�ϕ��̎��ԃX�e�b�v
		��		: �ڐG�� * dt
		Fext	: �O�͂���уR���I����

		�^����������
			M udot = J' ��/dt + Fext
		�I�C���[�ϕ��ɂ��
			u(t+1) = u(t) + M^-1 J' �� + dt M^-1 Fext
		������J��������
			w = J M^-1 J' �� + J (u(t) + dt M^-1 Fext)
		A = J M^-1 J'
		b = J (u(t) + dt M^-1 Fext)
		�Ƃ�����
			w = A �� + b
		����ɁC�����R�́C�Î~���C�C�����C�Ȃǂ��l�����邱�Ƃɂ��
		�ȉ��̐�������𓾂�
			��_i = ��_lo_i				 ==> w_i >= 0
			��_i = ��_hi_i				 ==> w_i <= 0
			��_lo_i < ��_i < ��_hi_i ==> w_i = 0
		����̉��ɂ�LCP(A, b, ��_hi, ��_lo)�Ə���

		������@:
		A = D - N�Ƃ����Ƃ��C�K���ȃ�[0]�������l�Ƃ��āC�Q����
			��[k+1] = LCP(D, -N ��[k] + b, ��_hi, ��_lo)
		�ɂ���[k]���X�V���Ă����D���̂Ƃ�
			��[k+1] = ��[k]
		�����藧�Ȃ��
			��[k] = LCP(A, b, ��_hi, ��_lo)
		�ƂȂ茳�̖�肪���������ƂɂȂ�
		���āCD���Ίp�s��Ȃ�΁CLCP(D, -N ��[k] + b, ��_hi, ��_lo)�̉���
			��[k+1] = min(max(��_lo, D^-1(N ��[k] - b)), ��_hi)
		�œ�����D�v����ɂ��̎����J��Ԃ��v�Z����΂悢�D
	 */

	/* �ȉ��͏ڍׂȎ��W�J
		N : ���̂̐�
		K : �ڐG�̐�
		lhs(i) : i�Ԗڂ̐ڐG�́u�����v�̍��̂̔ԍ�
		rhs(i) : i�Ԗڂ̐ڐG�́u�E���v�̍��̂̔ԍ�
			lhs(i) < rhs(i)

		[�\���m��]
		�s��̐�C = A B�ɂ���
		C(i,j) = Arow(i)' Bcol(j) = ��_k=1^n A(i,k) B(k,j)		(n = A�̗� = B�̍s��)

		[�e�s���3x3�u���b�N�̓��e]
		����s��A��(i,j)�Ԗڂ�3x3�����s���A(i,j)�Ə����D

		size(M) = (6N, 6N)
		M(i,j) =
			O		(i != j)
			m_k * 1	(i = 2k)	m_k��k�Ԗڂ̍��̂̎���, 1��3x3�P�ʍs��
			I_k		(i = 2k+1)	I_k��k�Ԗڂ̍��̂̊����e���\��
		
		size(J) = (3K, 6N)
		J(i,j) =
			J_lin(i,lhs(i))		(j == 2lhs(i))
			J_ang(i,lhs(i))		(j == 2lhs(i)+1)
			J_lin(i,rhs(i))		(j == 2rhs(i))
			J_ang(i,rhs(i))		(j == 2rhs(i)+1)
			O					otherwise

        T := M^-1 J'
		T(i,j)
			= ��_k=1^2N M^-1(i,k) J'(k,j)
			= ��_k=1^2N M(i,k)^-1 J(j,k)'
			= M(i,i)^-1 J(j,i)'
			=	1/m_lhs(j)  J_lin(j,lhs(j))'	(i == 2lhs(j))
				I_lhs(j)^-1 J_ang(j,lhs(j))'	(i == 2lhs(j)+1)
				1/m_rhs(j)  J_lin(j,rhs(j))'	(i == 2rhs(j))
				I_rhs(j)^-1 J_ang(j,rhs(j))'	(i == 2rhs(j)+1)
				O							otherwise

		A = J M^-1 J' = J T
		size(A) = (3K, 3K)
		A(i,j) = ��_k=1^2N J(i,k) T(k,j) =
			��(lhs(i),lhs(j)) * (1/m_lhs(i) J_lin(i,lhs(i)) J_lin(j,lhs(j))' + J_ang(i,lhs(i)) I_lhs(i)^-1 J_ang(j,lhs(j))') +
			��(rhs(i),rhs(j)) * (1/m_rhs(i) J_lin(i,rhs(i)) J_lin(j,rhs(j))' + J_ang(i,rhs(i)) I_rhs(i)^-1 J_ang(j,rhs(j))') +
			��(lhs(i),rhs(j)) * (1/m_lhs(i) J_lin(i,lhs(i)) J_lin(j,rhs(j))' + J_ang(i,lhs(i)) I_lhs(i)^-1 J_ang(j,rhs(j))') +
			��(rhs(i),lhs(j)) * (1/m_rhs(i) J_lin(i,rhs(i)) J_lin(j,lhs(j))' + J_ang(i,rhs(i)) I_rhs(i)^-1 J_ang(j,lhs(j))')
		�������̓N���l�b�J�[�f���^(��(i,j) == 1 if i == j or 0 otherwise)

		b = J Vnc		(Vnc = u(t) + dt M^-1 Fext)
		size(b) = 3K
		b(i) = ��_k=1^2N J(i,k) Vnc(k) =
			J_lin(i,lhs(i)) Vnc(2lhs(i)) + J_ang(i,lhs(i)) Vnc(2lhs(i)+1) +
			J_lin(i,rhs(i)) Vnc(2rhs(i)) + J_ang(i,rhs(i)) Vnc(2rhs(i)+1)
	*/


	//precomputation
	

	while(true){
		//terminate condition

		//linear equation

		//projection

	}

}

}
