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
	VMatrixRd matK;			// �S�̍����s��
	VMatrixRd matM;			// �S�̎��ʍs��
	VMatrixRd matMInv;		// �S�̎��ʍs��̋t�s��
	VMatrixRd matC;			// �S�̌����s��
	VVectord xlocalInit;	// �������_�ʒu
	VVectord xlocal;		// �v�Z�p�̒��_�ʒu(u = (x1, ..., xn-1, y1, ..., yn-1, z1, ..., zn-1)
	VVectord vlocal;		// ���_���x
	VVectord flocal;		// �v�Z�p�̊O��

	PHFemVibration(const PHFemVibrationDesc& desc = PHFemVibrationDesc());
	/// ������
	virtual void Init();
	//virtual void CompStiffnessMatrix();
	//virtual void CompMassMatrix();
	//virtual void CompDampingMatrix();
	/// �V�~�����[�V�����X�e�b�v
	virtual void Step();

	/// ���Ԑϕ�
	virtual void ExplicitEuler();
	virtual void ImplicitEuler();
	virtual void NewmarkBeta(const double b = 1.0/6.0);
	virtual void ModalAnalysis(int nmode);

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
	// ���E������������
	bool AddBoundaryCondition(int vtxId, Vec3i dof);
	// ���E������������(���_���j
	bool AddBoundaryCondition(VVector< Vec3i > bcs); 
	// ���_�ɗ͂�������i���[���h���W�n�j
	bool AddVertexForce(int vtxId, Vec3d fW);
	// ���_�Q�ɗ͂�������i���[���h���W�n�j
	bool AddVertexForce(VVector< Vec3d > fWs);
};

}

#endif