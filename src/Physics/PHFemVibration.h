/*
 *  Copyright (c) 2003-2012, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#ifndef PH_FEM_VIBRATION_H
#define PH_FEM_VIBRATION_H

#include <Physics/PHFemMeshNew.h>
#include <Foundation/Object.h>

namespace Spr{;

class PHFemVibration : public PHFemVibrationDesc, public PHFem{
public:
	SPR_OBJECTDEF(PHFemVibration);
	ACCESS_DESC(PHFemVibration);
	typedef VMatrixRow< double > VMatrixRd;
	typedef VVector< double > VVectord;

	double vdt;
	PHFemVibrationDesc::INTEGRATION_MODE integration_mode;
	VMatrixRd matKIni;	// �S�̍����s��̏����l
	VMatrixRd matMIni;	// �S�̎��ʍs��̏����l
	VMatrixRd matMInv;	// �S�̎��ʍs��̋t�s��
	VMatrixRd matCIni;	// �S�̌����s��̏����l
	VVectord xdl;		// ���_�ψ�(u = (u0, v0, w0, ..., un-1, vn-1, wn-1))
	VVectord vl;		// ���_���x
	VVectord fl;		// �v�Z�p�̊O��
	VMatrixRd matKp;	// �S�̍����s��̈ꕔ
	VMatrixRd matMp;	// �S�̎��ʍs��̈ꕔ
	VMatrixRd matCp;	// �S�̌����s��̈ꕔ
	VVectord xdlp;		// ���_�ψʂ̈ꕔ(u = (u0, v0, w0, ..., un-1, vn-1, wn-1))
	VVectord vlp;		// ���_���x�̈ꕔ
	VVectord flp;		// �v�Z�p�̊O�͂̈ꕔ

	VVector< int > boundary;	// ���E����ID

	PHFemVibration(const PHFemVibrationDesc& desc = PHFemVibrationDesc());
	/// ������
	virtual void Init();
	//virtual void CompStiffnessMatrix();
	//virtual void CompMassMatrix();
	//virtual void CompDampingMatrix();
	/// �V�~�����[�V�����X�e�b�v
	virtual void Step();

	/// ���Ԑϕ�
	/// _M:���ʍs��A_K:�����s��A_C:�����s��A_f:�O�́A_dt:�ϕ����݁A_xd:�ψʁA_v:���x
	virtual void ExplicitEuler(const VMatrixRd& _MInv, const VMatrixRd& _K, const VMatrixRd& _C, 
		const VVectord& _f, const double& _dt, VVectord& _xd, VVectord& _v);
	virtual void ImplicitEuler(const VMatrixRd& _MInv, const VMatrixRd& _K, const VMatrixRd& _C, 
		const VVectord& _f, const double& _dt, VVectord& _xd, VVectord& _v);
	virtual void NewmarkBeta(const VMatrixRd& _M, const VMatrixRd& _K, const VMatrixRd& _C, 
		const VVectord& _f, const double& _dt, VVectord& _xd, VVectord& _v, const double b = 1.0 /6.0);
	virtual void ModalAnalysis(const VMatrixRd& _M, const VMatrixRd& _K, const VMatrixRd& _C, 
		const VVectord& _f, const double& _dt, VVectord& _xd, VVectord& _v, const int nmode);

	/// �ŗL�l���
	virtual void SubSpace(const VMatrixRd& _M, const VMatrixRd& _K, 
							const int nmode, const double epsilon, VVectord& e, VMatrixRd& v);

	/// �e��ݒ�
	void SetTimeStep(double dt){ vdt = dt; }
	double GetTimeStep(){ return vdt; }
	void SetYoungModulus(double value){ young = value; }
	double GetYoungModulus(){ return young; }
	void SetPoissonsRatio(double value){ poisson = value; }
	double GetPoissonsRatio(){ return poisson; }
	void SetDensity(double value) { density = value; }
	double GetDensity(){ return density; }
	void SetAlpha(double value){ alpha = value; }
	double GetAlpha(){ return alpha; }
	void SetBeta(double value){ beta = value; }
	double GetBeta(){ return beta; }
	void SetIntegrationMode(PHFemVibrationDesc::INTEGRATION_MODE mode);

	// FemVertex���璸�_�ψʂ��擾���A�v�Z�ł���`�ɕϊ�����
	void GetVerticesDisplacement(VVectord& _xd);
	// FemVertex�ɒ��_�ψʂ�������
	void UpdateVerticesPosition(VVectord& _xd);

	// ���E������������(�s��Ɣԍ��j
	bool AddBoundaryCondition(VMatrixRd& base, VMatrixRd& mat, const int id);
	// ���E������������(���_�j
	bool AddBoundaryCondition(const int vtxId, const Vec3i dof);
	// ���E������������(���_���j
	bool AddBoundaryCondition(const VVector< Vec3i > bcs);
	// ���E�����𔽉f�����邽�߂̍s����v�Z����
	void CompBoundaryMatrix(VMatrixRd& _L, VMatrixRd& _R, const VVector< int > bc);
	// ���E�����ɉ����čs��̎��R�x���팸����
	void ReduceMatrixSize(VMatrixRd& mat, const VVector< int > bc);
	// ���E�����ɉ����čs�񎩗R�x���팸����
	void ReduceMatrixSize(VMatrixRd& _M, VMatrixRd& _K, VMatrixRd& _C, const VVector< int > bc);
	// ���E�����ɉ����ăx�N�g���̎��R�x���팸����
	void ReduceVectorSize(VVectord& r, const VVector< int > bc);
	// ���E�����ɉ����ăx�N�g���̎��R�x���팸����
	void ReduceVectorSize(VVectord& _xd, VVectord& _v, VVectord& _f,const VVector< int > bc);
	// �x�N�g���̎��R�x�����ɖ߂�
	void GainVectorSize(VVectord& r, const VVector< int > bc);
	// �x�N�g���̎��R�x�����ɖ߂�
	void GainVectorSize(VVectord& _xd, VVectord& _v, const VVector< int > bc);

	// ���_�ɗ͂�������i���[���h���W�n�j
	bool AddVertexForce(int vtxId, Vec3d fW);
	// ���_�Q�ɗ͂�������i���[���h���W�n�j
	bool AddVertexForce(VVector< Vec3d > fWs);

	/// scilab�f�o�b�N
	bool IsScilabStarted;
	void ScilabDeterminant(VMatrixRd& _M, VMatrixRd& _K, VMatrixRd& _C);
	void ScilabEigenValueAnalysis(VMatrixRd& _M, VMatrixRd& _K);

	/// �s��̃t�@�C���o��
	void MatrixFileOut(VMatrixRd mat, std::string filename);
};

}

#endif