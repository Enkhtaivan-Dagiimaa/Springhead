/*
 *  Copyright (c) 2003-2012, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file SprPHFemMeshNew.h
 *	@brief ����
*/
#ifndef SPR_PH_FEMMESH_NEW_IF_H
#define SPR_PH_FEMMESH_NEW_IF_H

#include <Foundation/SprObject.h>

/**	\addtogroup gpPhysics	*/
//@{
namespace Spr{;

struct PHSolidIf;
struct PHFemVibrationIf;

///	FemMesh�̃X�e�[�g
struct PHFemMeshNewState{};
///	FemMesh�̃f�B�X�N���v�^
struct PHFemMeshNewDesc: public PHFemMeshNewState{
	std::vector<Vec3d> vertices;
	std::vector<int> tets;
	std::vector<int> faces;
};
/// PHFemMeshNew�ւ̃C���^�t�F�[�X
struct PHFemMeshNewIf : public SceneObjectIf{
	SPR_IFDEF(PHFemMeshNew);
	void SetPHSolid(PHSolidIf* s);
	PHSolidIf* GetPHSolid();
	PHFemVibrationIf* GetPHFemVibration();
	int NVertices();
	int NFaces();
	double CompTetVolume(int tetID, bool bDeform);
	bool AddVertexDisplacementW(int vtxId, Vec3d disW);
	bool AddVertexDisplacementL(int vtxId, Vec3d disL);
	bool SetVertexPositionW(int vtxId, Vec3d posW);
	bool SetVertexPositionL(int vtxId, Vec3d posL);
};

/// Fem�̃f�X�N���v�^
struct PHFemBaseDesc{};
/// Fem�̋��ʌv�Z����
struct PHFemBaseIf : public SceneObjectIf{
	SPR_IFDEF(PHFemBase);
};

/// �U���v�Z�̃f�X�N���v�^
struct PHFemVibrationDesc : public PHFemBaseDesc{
	enum ANALYSIS_MODE{
		ANALYSIS_DIRECT,
		ANALYSIS_MODAL
	};
	enum INTEGRATION_MODE{
		INT_EXPLICIT_EULER,
		INT_IMPLICIT_EULER,
		INT_SIMPLECTIC,
		INT_NEWMARK_BETA,
	};
	double young;		///< �����O��(Pa, N/m2)
	double poisson;		///< �|�A�\����(*��l�ȗ����̂̏ꍇ�A-1 <= v <= 0.5)
	double density;		///< ���x(kg/m3)
	double alpha;		///< �S��������
	double beta;		///< �\��������
	PHFemVibrationDesc(){
		// �A���~�̕���
		// �|�A�\����:0.35,�����O�� 70GPa, ���x2.70g/cm3
		// ������͓K���ɐݒ�
		poisson = 0.35;
		young = 70 * 1e6;
		density =  2.7 * 1e3; 
		alpha = 0.001;
		beta = 0.0001;
	}
};

/// �U���v�Z
struct PHFemVibrationIf : public PHFemBaseIf{
	SPR_IFDEF(PHFemVibration);
	void SetTimeStep(double dt);
	double GetTimeStep();
	void SetYoungModulus(double value);
	double GetYoungModulus();
	void SetPoissonsRatio(double value);
	double GetPoissonsRatio();
	void SetDensity(double value);
	double GetDensity();
	void SetAlpha(double value);
	double GetAlpha();
	void SetBeta(double value);
	double GetBeta();
	void SetAnalysisMode(PHFemVibrationDesc::ANALYSIS_MODE mode);
	void SetIntegrationMode(PHFemVibrationDesc::INTEGRATION_MODE mode);
	// ���E������������
	bool AddBoundaryCondition(int vtxId, Vec3i dof);
	// ���E������������(���_���j
	bool AddBoundaryCondition(PTM::VVector< Vec3i > bcs); 
	// ���_�ɗ͂�������i���[���h���W�n�j
	bool AddVertexForceW(int vtxId, Vec3d fW);
	// ���_�Q�ɗ͂�������i���[���h���W�n�j
	bool AddVertexForceW(PTM::VVector< Vec3d > fWs);
};

//@}

}	//	namespace Spr
#endif