#ifndef PHPARAMETRICJOINT_H
#define PHPARAMETRICJOINT_H

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
struct PHJointTrajectoryPoint{
	double		q;
	Posed		pose;
	Matrix6d	J;
};
class PHJointTrajectory : public std::vector<PHJointTrajectoryPoint>, public Object{
	iterator Find(double q);
	bool bRevolutive;	//[-pi, pi]�̖�����]�֐�
public:
	void SetRevolutive(bool OnOff = true){bRevolutive = OnOff;}
	bool IsRevolutive(){return bRevolutive;}
	void AddPoint(const Posed& pose, double q);
	void CreateDB();
	void GetPose(Posed& pose, double q);
	void GetJacobian(Matrix6d& J, double q);
};

class PHParametricJoint : public InheritJoint1D<PHParametricJointIf, PHJoint1D>{
	UTRef<PHJointTrajectory> trajectory;
public:
	double	q, qd;				// �֐ߕψʂƊ֐ߊp�x
	//Matrix53d	Jcvrel, Jcwrel;	// ���Α��x����S�����x�C���Ίp���x����S�����x�ւ̃��R�r�s��

	virtual PHConstraintDesc::ConstraintType GetConstraintType(){return PHConstraintDesc::PARAMETRICJOINT;}
	virtual double GetPosition();
	virtual double GetVelocity();
	virtual void CompConstraintJacobian();
	virtual void CompBias(double dt);
	virtual void CompError(double dt);
	virtual void ProjectionDynamics(double& f, int k);
	virtual void ProjectionCorrection(double& F, int k);
	PHParametricJoint();
};

}

#endif