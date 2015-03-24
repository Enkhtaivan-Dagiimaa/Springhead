#ifndef PHOpGroup_H
#define PHOpGroup_H

#include <Physics/SprPHOpObj.h>
#include <Physics/PHOpParticle.h>

namespace Spr{;

class PHOpGroup
{
public:
	//���q�O���[�v��id
	int gMyIndex;
	//���̃O���[�v�Ɋ܂ޗ��q��index
	std::vector<int> gPInd;
	//�O���[�v�̃��[�����g�s��
	Matrix3f gAgroup;
	//�O���[�v�̌ŗL�x�N�g���Q
	Matrix3f gEigen;
	//�O���[�v�̎���
	float gtotalMass;
	//�������S
	Vec3f gOrigCenter;
	//���ݒ��S
	Vec3f gCurrCenter;
	//���̃O���[�v�Ɋ܂ޗ��q�̐�
	int gNptcl;

	PHOpGroup()
	{
		gCurrCenter = gOrigCenter = Vec3f(0,0,0);
		gAgroup = gAgroup.Zero();
		gtotalMass = 0;
	
		gNptcl = 0;
		gEigen = gEigen.Unit();
	}
	void addPInds(int gPIndex);
	void delPInds(int gPIndex);
	void preCalculation();

	void groupTimeStep();
	
	void projectPos(PHOpParticle *objPArr);
	void gravityStep();
	void SwapPtclInGroup();
	

	int getParticleGlbIndex(int pindex)
	{
		return this->gPInd[pindex];
	}

	
};


}//namespace






#endif