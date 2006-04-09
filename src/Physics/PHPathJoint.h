#ifndef PHPATHJOINT_H
#define PHPATHJOINT_H

#include <SprPhysics.h>
#include "PHConstraint.h"

namespace Spr{;

/*	2�̍��̊Ԃ̑��Έʒu�E�p����Ԃ��֐�
		[p; q] = f(s)
	������Ƃ���D������p�͑��Έʒu, q�͑���quaternion�Cs�͈�ʉ����W��\�킷�X�J���[�D
	��������
		[p'; q'] = (df/ds)(s)s'�D
	quaternion�̎��Ԕ�������p���x�𓾂�s���E(q)�Ƃ����
		[v; w] = [I O; O E(q(s))](df/ds)(s)s' =: J(s)s'
	��̎����s��s'�����܂�Α��Α��x[v; w]�����܂�D
	������S�������̌`�ɕς���D6�����x�N�g��J(s)�͑��Α��x�̌�����\�킵�Ă���̂ŁC
	����ɒ�������s���J~(s)�Ƃ���ƁC�S��������
		J~(s)[v; w] = 0

	Correction�́C
	velocity update��̑��Έʒu�ɍł��߂��O��im(f)��̓_���擾�C���̓_�ɍ��킹��D
	�{���̍S��������[p; q] in im(f)��������`�Ȃ̂ň����Ȃ��D
 */

/// �֐߂̋O���̃L�[�t���[���������C�⊮���ĕԂ����胄�R�r�A�����v�Z�����肷��N���X
struct PHPathPointWithJacobian : public PHPathPoint{
	Matrix6d	J;
};

class PHPath : public std::vector<PHPathPointWithJacobian>, public InheritSceneObject<PHPathIf, SceneObject>{
	iterator Find(double s);
	bool bLoop;	//[-pi, pi]�̖�����]�֐�
public:
	virtual void SetLoop(bool OnOff = true){bLoop = OnOff;}
	virtual bool IsLoop(){return bLoop;}
	void AddPoint(double s, const Posed& pose);
	void CompJacobian();
	void GetPose(double s, Posed& pose);
	void GetJacobian(double s, Matrix6d& J);
};

class PHPathJoint : public InheritJoint1D<PHPathJointIf, PHJoint1D>{
	UTRef<PHPath> path;
public:
	double	q, qd;				// �֐ߕψʂƊ֐ߊp�x
	//Matrix53d	Jcvrel, Jcwrel;	// ���Α��x����S�����x�C���Ίp���x����S�����x�ւ̃��R�r�s��

	virtual bool AddChildObject(ObjectIf* o);
	virtual PHConstraintDesc::ConstraintType GetConstraintType(){return PHConstraintDesc::PATHJOINT;}
	virtual double GetPosition();
	virtual double GetVelocity();
	virtual void CompConstraintJacobian();
	virtual void CompBias(double dt);
	virtual void CompError(double dt);
	virtual void ProjectionDynamics(double& f, int k);
	virtual void ProjectionCorrection(double& F, int k);
	PHPathJoint();
};

}

#endif