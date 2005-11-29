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

void PHConstraintEngine::Add(PHSolid* s){
	solids.push_back(s);
	Init();
}

void PHConstraintEngine::Init(){
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
	//�����܂ł̏�����PHContactVolume�̔z��𓾂�.
	//��������Ƃ�LCP�\�z
	//���R�r�s��ɒl���l�߂�
	Vec3d n, c, r[2], v[2], vrel, t[2];
	Matrix3d rcross[2];
	Posed q[2];
	PHSolid* solid[2];
	for(PHContactVolumes::iterator iv = contacts.begin(); iv != contacts.end(); iv++){
		n = iv->normal;	//�@��
		for(PHContactPoints::iterator ip = iv->points.begin(); ip != iv->points.end(); ip++){
			c = ip->point;	//�ڐG�_
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

			ip->J[0][0].row(0) = -n;
			ip->J[0][0].row(1) = -t[0];
			ip->J[0][0].row(2) = -t[1];
			ip->J[0][1].row(0) = -rcross[0] * n;
			ip->J[0][1].row(1) = -rcross[0] * t[0];
			ip->J[0][1].row(2) = -rcross[0] * t[1];
			ip->J[1][0].row(0) =  n;
			ip->J[1][0].row(1) =  t[0];
			ip->J[1][0].row(2) =  t[1];
			ip->J[1][1].row(0) =  rcross[1] * n;
			ip->J[1][1].row(1) =  rcross[1] * t[0];
			ip->J[1][1].row(2) =  rcross[1] * t[1];
		}
	}

	//LCP������
	//precomputation

	while(true){
		//terminate condition

		//linear equation

		//projection

	}

}

}
