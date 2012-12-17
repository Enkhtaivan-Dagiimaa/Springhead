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

	double timeStep;
	VMatrixRow< double > matK;		// �S�̍����s��
	VMatrixRow< double > matM;		// �S�̎��ʍs��
	VMatrixRow< double > matMInv;	// �S�̎��ʍs��̋t�s��
	VMatrixRow< double > matC;		// �S�̌����s��
	VVector< double > xlocalLast;	// �O�X�e�b�v�̒��_�ʒu
	VVector< double > xlocal;		// �v�Z�p�̒��_�ʒu(u = (x1, ..., xn-1, y1, ..., yn-1, z1, ..., zn-1)
	VVector< double > vlocalLast;	// �O�X�e�b�v�̒��_���x
	VVector< double > vlocal;		// �v�Z�p�̒��_���x
	VVector< double > flocal;		// �v�Z�p�̊O��

	PHFemVibration(const PHFemVibrationDesc& desc = PHFemVibrationDesc());
	/// ������
	virtual void Init();
	/// �V�~�����[�V�����X�e�b�v
	virtual void Step();

	/// ���Ԑϕ�
	virtual void ExplicitEuler();
	virtual void ImplicitEuler();

	/// �e��ݒ�
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
	bool AddBoundaryCondition(int vtxId, Vec3i dof);
};

}

#endif