/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file SprPHFemMesh.h
 *	@brief FEM�V�~�����[�V�����̂��߂̎l�ʑ̃��b�V��
*/
#ifndef SPR_PHFemMeshIf_H
#define SPR_PHFemMeshIf_H

#include <Foundation/SprObject.h>

/**	\addtogroup gpPhysics	*/
//@{
namespace Spr{;

///	FemMesh�̃X�e�[�g
struct PHFemMeshState{
};
///	FemMesh�̃f�B�X�N���v�^
struct PHFemMeshDesc: public PHFemMeshState{
	std::vector<Vec3d> vertices;
	std::vector<int> tets;
	std::vector<int> faces;
	PHFemMeshDesc();
	void Init();
};

///	FEM�p�̎l�ʑ̃��b�V��
struct PHFemMeshIf : public SceneObjectIf{
	SPR_IFDEF(PHFemMesh);
	//int GetSurfaceVertex(int id);
	//int NSurfaceVertices();
	//void SetVertexTc(int id,double temp);
	//Vec3d GetPose(unsigned id);
	//Vec3d GetSufVtxPose(unsigned id);
};

///	FemMeshThermo�̃f�B�X�N���v�^
struct PHFemMeshThermoDesc: public PHFemMeshDesc{
	double roh;		//	���x
	double thconduct;	//	�M�`����
	double consts;		//	��M
	double heat_trans;	//	�M�`�B��
	PHFemMeshThermoDesc();
	void Init();
};

///	���x��FEM�p�̃��b�V��
struct PHFemMeshThermoIf : public PHFemMeshIf{
	SPR_IFDEF(PHFemMeshThermo);
	int GetSurfaceVertex(int id);
	int NSurfaceVertices();
	void SetVertexTc(int id,double temp);
	void SetVertexTc(int id,double temp,double heatTrans);
	Vec3d GetPose(int id);
	Vec3d GetSufVtxPose(unsigned id);
	unsigned long GetStepCount();		///	�J�E���g�P
	unsigned long GetStepCountCyc();		///	�J�E���g�P�����T�ڂ�	�v�Z��:TotalCount = GetStepCount() + GetStepCountCyc() * (1000 * 1000 * 1000) 
	void SetVertexTemp(unsigned id,double temp);
	void SetvecFAll(unsigned id,double dqdt);
	void SetRohSpheat(double roh,double Cp);		//�f�ތŗL�̕���
};

//@}

}	//	namespace Spr
#endif
