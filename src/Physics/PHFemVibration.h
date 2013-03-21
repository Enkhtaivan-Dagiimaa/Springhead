/*
 *  Copyright (c) 2003-2012, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#ifndef PH_FEM_VIBRATION_H
#define PH_FEM_VIBRATION_H

#include <Physics/PHFemBase.h>
#include <Foundation/Object.h>

namespace Spr{;
using namespace PTM;

class PHFemVibration : public PHFemVibrationDesc, public PHFemBase{
public:
	SPR_OBJECTDEF(PHFemVibration);
	ACCESS_DESC(PHFemVibration);
	typedef double element_type;
	typedef VMatrixRow< element_type > VMatrixRe;
	typedef VVector< double > VVectord;
	#define element_limit std::numeric_limits< element_type >::max_digits10 	// stream�̐��x

	double vdt;
	PHFemVibrationDesc::ANALYSIS_MODE analysis_mode;
	PHFemVibrationDesc::INTEGRATION_MODE integration_mode;
	bool bRecomp;		// �S�̍s�񂪍Čv�Z���ꂽ�ꍇ�̃t���O
	VMatrixRe matKIni;	// �S�̍����s��̏����l
	VMatrixRe matMIni;	// �S�̎��ʍs��̏����l
	VMatrixRe matCIni;	// �S�̌����s��̏����l
	VVectord xdl;		// ���_�ψ�(u = (u0, v0, w0, ..., un-1, vn-1, wn-1))
	VVectord vl;		// ���_���x
	VVectord al;		// ���_�����x�i�ꕔ�̐ϕ��ł͂���Ȃ��j
	VVectord fl;		// �v�Z�p�̊O��
	VMatrixRe matKp;	// �S�̍����s��̈ꕔ
	VMatrixRe matMp;	// �S�̎��ʍs��̈ꕔ
	VMatrixRe matCp;	// �S�̌����s��̈ꕔ
	VVectord xdlp;		// ���_�ψʂ̈ꕔ(u = (u0, v0, w0, ..., un-1, vn-1, wn-1))
	VVectord vlp;		// ���_���x�̈ꕔ
	VVectord alp;		// ���_�����x�̈ꕔ
	VVectord flp;		// �v�Z�p�̊O�͂̈ꕔ
//	VMatrixRe evalue;	// �ŗL�l
//	VMatrixRe evector;	// �ŗL�x�N�g��
	double dampingRatio[2];	// ������
	int nMode;			// ���[�h��

	VVector< int > boundary;	// ���E����ID

	PHFemVibration(const PHFemVibrationDesc& desc = PHFemVibrationDesc());
	/// ������
	virtual void Init();
	virtual void CompStiffnessMatrix();
	virtual void CompMassMatrix();
	virtual void CompRayleighDampingMatrix();
	virtual void CompRayleighDampingMatrixByDampingRatio();

	/// �V�~�����[�V�����X�e�b�v
	virtual void Step();
	// ���Ԑϕ����@�̃X�C�b�`�i�s��j
	virtual void InitNumericalIntegration(const VMatrixRe& _M, const VMatrixRe& _K, const VMatrixRe& _C, const double& _dt, VMatrixRe& _SInv);
	virtual void NumericalIntegration(const VMatrixRe& _M, const VMatrixRe& _K, const VMatrixRe& _C, 
		const VVectord& _f, const double& _dt, VVectord& _xd, VVectord& _v, VVectord& _a);
	// ���Ԑϕ����@�̃X�C�b�`(1���R�x)
	virtual void InitNumericalIntegration(const double& _m, const double& _k, const double& _c, const double& _dt, double& _sInv);
	virtual void NumericalIntegration(const double& _sInv, const double& _k, const double& _c, 
		const double& _f, const double& _dt, double& _x, double& _v, double& _a);
	virtual void ModalAnalysis(const VMatrixRe& _M, const VMatrixRe& _K, const VMatrixRe& _C, 
		const VVectord& _f, const double& _dt, bool& bFirst, VVectord& _xd, VVectord& _v, VVectord& _a, const int nmode);


	/// �ŗL�l���(Kx = lamda M x)������
	virtual void CompEigenValue(const VMatrixRd& _M, const VMatrixRd& _K, const int start, const int interval, VVectord& e, VMatrixRd& v);
	virtual void CompEigenVibrationFrequency(const VVectord& e, VVectord& w);
	virtual void CompEigenAngularVibrationFrequency(const VVectord& e, VVectord& wrad);
	double CompModalDampingRatio(double wrad);
	void CompRayleighDampingCoeffcient(double wrad[2], double ratio[2], double& a, double& b);
	// Subspace�@�̎���
	virtual void SubSpace(const VMatrixRe& _K, const VMatrixRe& _M, 
							const int nmode, const double epsilon, VVectord& e, VMatrixRe& v);

	/// �e��ݒ�A�v�Z
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
	void SetAnalysisMode(PHFemVibrationDesc::ANALYSIS_MODE mode);
	void SetIntegrationMode(PHFemVibrationDesc::INTEGRATION_MODE mode);

	// FemVertex���璸�_�ψʂ��擾���A�v�Z�ł���`�ɕϊ�����
	void GetVerticesDisplacement(VVectord& _xd);
	// FemVertex�ɒ��_�ψʂ�������
	void UpdateVerticesPosition(VVectord& _xd);

	std::vector< int > FindVertices(const int vtxId, const Vec3d _vecl);
	// ���E������������(�s��Ɣԍ��j
	bool AddBoundaryCondition(VMatrixRe& mat, const int id);
	// ���E������������(���_�j
	bool AddBoundaryCondition(const int vtxId, const Vec3i dof);
	// ���E������������(���_���j
	bool AddBoundaryCondition(const VVector< Vec3i >& bcs);
	// ���E�����ɉ����čs��̎��R�x���팸����
	void ReduceMatrixSize(VMatrixRe& mat, const VVector< int >& bc);
	// ���E�����ɉ����čs�񎩗R�x���팸����
	void ReduceMatrixSize(VMatrixRe& _M, VMatrixRe& _K, VMatrixRe& _C, const VVector< int >& bc);
	// ���E�����ɉ����ăx�N�g���̎��R�x���팸����
	void ReduceVectorSize(VVectord& r, const VVector< int >& bc);
	// ���E�����ɉ����ăx�N�g���̎��R�x���팸����
	void ReduceVectorSize(VVectord& _xd, VVectord& _v, VVectord& _a, VVectord& _f,const VVector< int >& bc);
	// �팸�����x�N�g�������̃x�N�g���ɖ߂�
	void AssignVector(const VVectord& from, VVectord& to, const VVector< int >& bc);

	// ���_�ɗ͂�������i���[�J�����W�n�j
	bool AddVertexForceL(int vtxId, Vec3d fL);
	// ���_�ɗ͂�������i���[���h���W�n�j
	bool AddVertexForceW(int vtxId, Vec3d fW);
	// ���_�Q�ɗ͂�������i���[���h���W�n�j
	bool AddVertexForceW(VVector< Vec3d > fWs);
	// �͂�������
	bool AddForce(int tetId, Vec3d posW, Vec3d fW);

	// �`��֐����g���ĔC�ӂ̓_�̕ψʂ��擾����
	bool GetDisplacement(int tetId, Vec3d posW, Vec3d& disp, bool bDeform);

	/// ������
	bool FindClosestPointOnMesh( const Vec3d& posW, const Vec3d pos[3], Vec3d& cp, double& dist, bool bDeform);
	/// ����_����߂��ʂƖʏ�̓_��T��
	bool FindNeighborFaces(Vec3d posW, std::vector< int >& faceIds, std::vector< Vec3d >& cpWs, bool bDeform);
	/// ����_����߂��l�ʑ̂Ǝl�ʑ̏�̓_��T��
	bool FindNeighborTetrahedron(Vec3d posW, int& tetId, Vec3d& cpW, bool bDeform);

	/// �s��̃t�@�C���o��
	void MatrixFileOut(VMatrixRe mat, std::string filename);
};

}

#endif