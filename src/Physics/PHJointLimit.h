/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHJOINTLIMIT_H
#define PHJOINTLIMIT_H

#include <Base/TQuaternion.h>
#include <Physics/PhysicsDecl.hpp>
#include <Physics/PHAxisIndex.h>

#include <Physics/SprPHJointLimit.h>
#include <Foundation/Object.h>

namespace Spr{;

class PHScene;
class PHConstraintEngine;
class PH1DJoint;
class PHBallJoint;

///	1���R�x�֐߂̉���S��
class PH1DJointLimit : public SceneObject {
public:
	SPR_OBJECTDEF(PH1DJointLimit);
	SPR_DECLMEMBEROF_PH1DJointLimitDesc;

	/// ����S���̑ΏۂƂȂ�֐�
	PH1DJoint* joint;

	/// ���͈͊O�ɏo�Ă��邩
	bool bOnLimit;

	/// ���͈͊O�ɏo�Ă����
	double diff;

	/// �R���X�g���N�^
	PH1DJointLimit(const PH1DJointLimitDesc& desc = PH1DJointLimitDesc()) { SetDesc(&desc); }
	
	// ----- Limit�̋@�\

	virtual void SetupAxisIndex();
	virtual void CompBias();

	// ----- �C���^�t�F�[�X�̎���
	void    SetRange(Vec2d  range)    { this->range = range; }
	void    GetRange(Vec2d &range)    { range = this->range; }
	void    SetSpring(double spring)  { this->spring = spring; }
	double  GetSpring()               { return spring; }
	void    SetDamper(double damper)  { this->damper = damper; }
	double  GetDamper()               { return damper; }
	bool    IsOnLimit()               { return bOnLimit; }
};

// -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  ----- 

///	���֐߂̉���S���i�x�[�X�N���X�j
class PHBallJointLimit : public SceneObject {
public:
	SPR_OBJECTDEF(PHBallJointLimit);
	SPR_DECLMEMBEROF_PHBallJointLimitState;
	SPR_DECLMEMBEROF_PHBallJointLimitDesc;

	/// ����S���̑ΏۂƂȂ�֐�
	PHBallJoint* joint;

	/// ���͈͊O�ɏo�Ă����
	Vec3d diff;

	// ----- Joint�{�̂ƈقȂ���W�n���g�����߂̓Ǝ���Jacobian��LCP�v�Z�ϐ�

	/// �S�����R�r�A��
	Matrix3d J[2];

	/// T = M.inv() * J^t �K�E�X�U�C�f���Ŏg�p
	Matrix3d T[2];
	
	/// LCP��b�x�N�g���Ƃ��̕␳��
	Vec3d b, db, B;
	
	/// LCP��A�s��̑Ίp�����Ƃ��̕␳�ʁC�t��
	Vec3d A, dA, Ainv;	

	/// Projection�p�̊e����Min/Max
	double fMaxDt[3], fMinDt[3];

	/// �S�����Ǘ��N���X
	AxisIndex<3> axes;

	/// �R���X�g���N�^
	PHBallJointLimit();
	
	// ----- PHBallJoint����Ăяo�����@�\

	/// LCP�������O�i�K�̌v�Z
	void SetupLCP();

	/// LCP�̌J��Ԃ��v�Z
	void IterateLCP();

	// ----- ���̃N���X�Ŏ�������@�\

	/// A�̑Ίp�������v�Z����DA = J * M^-1 * J^T
	void CompResponseMatrix();

	/// LCP�̕␳�l�̌v�Z�D�덷�C���p
	void CompBias();

	/// �S���͕ω���(df)�ɑ΂�������x�ω���(dv�̍���)���v�Z���Ĕ��f
	void CompResponse(double df, int i);

	// ----- �h���N���X�Ŏ�������@�\

	/// �S�����W�n��Jabocian���v�Z
	virtual void CompJacobian(){}

	/// ���搧���ɂ������Ă��邩�m�F���ǂ̎��R�x�𑬓x�S�����邩��ݒ�
	virtual void SetupAxisIndex(){}

	// ----- �C���^�t�F�[�X�̎���
	void    SetSpring(double spring)  { this->spring = spring; }
	double  GetSpring()               { return spring; }
	void    SetDamper(double damper)  { this->damper = damper; }
	double  GetDamper()               { return damper; }
	void SetLimitDir(Vec3d limDir)    { this->limitDir = limDir; }
	Vec3d GetLimitDir()               { return limitDir; }

	virtual bool IsOnLimit()          { return false; }
};

// -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  ----- 

///	���֐߂̉~�`����S��
class PHBallJointConeLimit : public PHBallJointLimit{
public:
	SPR_OBJECTDEF(PHBallJointConeLimit);
	SPR_DECLMEMBEROF_PHBallJointConeLimitDesc;

	/// ���͈͊O�ɏo�Ă��邩
	bool bOnLimit;

	/// �R���X�g���N�^
	PHBallJointConeLimit(const PHBallJointConeLimitDesc& desc = PHBallJointConeLimitDesc()) {SetDesc(&desc);}
	
	// ----- PHBallJointLimit�̔h���N���X�Ŏ�������@�\

	/// �S�����W�n��Jabocian���v�Z
	virtual void CompJacobian();

	/// ���搧���ɂ������Ă��邩�m�F���ǂ̎��R�x�𑬓x�S�����邩��ݒ�
	virtual void SetupAxisIndex();

	// ----- �C���^�t�F�[�X�̎���
	virtual bool IsOnLimit() { return bOnLimit; }
	void SetSwingRange(Vec2d range) { limitSwing = range; }
	void GetSwingRange(Vec2d& range){ range = limitSwing; }
	void SetTwistRange(Vec2d range) { limitTwist = range; }
	void GetTwistRange(Vec2d& range){ range = limitTwist; }
};

// -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  ----- 


/// �X�v���C���Ȑ���̓_�i����_�ł͂Ȃ��j
class ClosedSplineCurve;
struct SplinePoint {
	/// �e
	ClosedSplineCurve* spline;

	/// �_�̍��W�i�ɍ��W : Swing, SwingDir�j
	Vec2d pos;

	/// �Ӕԍ�
	int edge;

	/// �p�����[�^t
	double t;

	/// �l�̍X�V�Dt��0�ȉ���1�ȏゾ������ׂ̕ӏ�̓_�Ɉڂ�
	void Update();

	/// pos����i��t���t�Z����
	void ReverseUpdate();

	/// �ڐ���x���Ƃ��č��W�n�����
	Matrix3d CompJacobian();
};

/// �X�v���C���Ȑ�
class ClosedSplineCurve {
public:
	/// �R���X�g���N�^
	ClosedSplineCurve(std::vector<PHSplineLimitNode>* nds) { nodes = nds; }

	/// ����_�̔z��i�ւ̃|�C���^�D���̂�Limit��Desc�������Ă���j
	std::vector<PHSplineLimitNode>* nodes;
	
	/// �ӂ̐�
	int NEdges() { return nodes->size()-1; /* �n�_�ƏI�_�����̂� */ }

	/// i�Ԗڂ̕ӂ�Swing�X�v���C���W����Ԃ�
	Vec4d GetEdgeSwingCoeff(int i);

	/// i�Ԗڂ̕ӂ�SwingDir�X�v���C���W����Ԃ�
	Vec4d GetEdgeSwingDirCoeff(int i);

	/// ���_��ǉ�(pos���w�肳�ꂽ�ꍇ�͑}��)����
	void AddNode(PHSplineLimitNode node, int pos=-1);

	/// i�Ԗڂ̕ӂ̃p�����[�^t�ɂ�����_�̋ɍ��W��Ԃ�
	SplinePoint GetPointOnEdge(int i, double t);
};

///	���֐߂̃X�v���C���Ȑ�����S��
class PHBallJointSplineLimit : public PHBallJointLimit{
public:
	SPR_OBJECTDEF(PHBallJointSplineLimit);
	SPR_DECLMEMBEROF_PHBallJointSplineLimitDesc;

	/// ����Ȑ�
	ClosedSplineCurve limitCurve;

	/// ���͈͊O�ɏo�Ă��邩
	bool bOnSwing, bOnTwist;
	
	/// �Ō�ɉ�����ɋ����Ƃ��̈ʒu�iin Swing/SwingDir/Twist�j
	Vec3d lastPos;

	/// ���݂̊֐߃|�W�V�����iin Swing/SwingDir/Twist�j
	Vec3d currPos;

	/// �ŋߖT�_
	SplinePoint neighbor;

	/// �R���X�g���N�^
	PHBallJointSplineLimit(const PHBallJointSplineLimitDesc& desc = PHBallJointSplineLimitDesc()) : limitCurve(&nodes) {
		SetDesc(&desc);

		bOnSwing = false;
		bOnTwist = false;

		lastPos = Vec3d(0,0,0);
	}

	// ----- PHBallJointLimit�̔h���N���X�Ŏ�������@�\

	/// �S�����W�n��Jabocian���v�Z
	virtual void CompJacobian();

	/// �ǂ̎��R�x�𑬓x�S�����邩��ݒ�
	virtual void SetupAxisIndex();

	// ----- ���̃N���X�̋@�\

	/// Swing-SwingDir�̉���`�F�b�N�ibOnSwing�i��neighbor�j�����肷��j
	void CheckSwingLimit();

	/// Twist�̉���`�F�b�N�ibOnTwist�����肷��j
	void CheckTwistLimit();

	/// ��_�`�F�b�N
	int CheckIntersection(Vec2d base, Vec2d curr, double& minDist);

	// ----- �C���^�t�F�[�X�̎���
	virtual bool IsOnLimit() { return(bOnSwing || bOnTwist); }
	void AddNode(PHSplineLimitNode node)          { limitCurve.AddNode(node, -1);  }
	void AddNode(PHSplineLimitNode node, int pos) { limitCurve.AddNode(node, pos); }
	void AddNode(double S, double SD, double dS, double dSD, double tMin, double tMax) {
		AddNode(PHSplineLimitNode(S,SD,dS,dSD,tMin,tMax));
	}
	void AddNode(double S, double SD, double dS, double dSD, double tMin, double tMax, int pos) {
		AddNode(PHSplineLimitNode(S,SD,dS,dSD,tMin,tMax), pos);
	}
	void SetPoleTwistRange(Vec2d range) { poleTwist = range; }
	void GetPoleTwistRange(Vec2d& range){ range = poleTwist; }
};

// ----- ���[�e�B���e�B

/// �R���������������֐�
Vec3d SolveCubicFunction(Vec4d eq3);

/// Swing-SwingDir�ɍ��W�����[�N���b�h���W�ɒ����֐�
Vec3d FromPolar(Vec2d pos);

}

#endif
