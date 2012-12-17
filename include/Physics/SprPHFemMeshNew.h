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

/// PHFemMeshNew�ւ̃C���^�t�F�[�X
struct PHFemMeshNewIf : public SceneObjectIf{
	SPR_IFDEF(PHFemMeshNew);
};

///	FemMesh�̃X�e�[�g
struct PHFemMeshNewState{};

///	FemMesh�̃f�B�X�N���v�^
struct PHFemMeshNewDesc: public PHFemMeshNewState{
	SPR_DESCDEF(PHFemMeshNew);
	std::vector<Vec3d> vertices;
	std::vector<int> tets;
	std::vector<int> faces;
};

/// Fem�̋��ʌv�Z����
struct PHFemIf : public SceneObjectIf{
	SPR_IFDEF(PHFem);
};

/// Fem�̃f�X�N���v�^
struct PHFemDesc{
};

/// �U���v�Z
struct PHFemVibrationIf : public PHFemIf{
	SPR_IFDEF(PHFemVibration);
};

/// �U���v�Z�̃f�X�N���v�^
struct PHFemVibrationDesc : public PHFemDesc{
	SPR_DESCDEF(PHFemVibration);
	double young;		///< �����O��(Pa, N/m2)
	double poisson;		///< �|�A�\����(*��l�ȗ����̂̏ꍇ�A-1 <= v <= 0.5)
	double density;		///< ���x(kg/m3)
	double alpha;		///< �S��������
	double beta;		///< �\��������
	PHFemVibrationDesc(){
		young = 1.0;
		poisson = 0.5;
		density = 1.0;
		alpha = 0.0;
		beta = 0.0;
	}
};


//@}

}	//	namespace Spr
#endif