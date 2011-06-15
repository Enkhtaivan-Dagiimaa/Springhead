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
	PHFemMeshDesc();
	void Init();
};

///	FEM�p�̎l�ʑ̃��b�V��
struct PHFemMeshIf : public SceneObjectIf{
	SPR_IFDEF(PHFemMesh);
};

///	FemMeshThermo�̃f�B�X�N���v�^
struct PHFemMeshThermoDesc: public PHFemMeshDesc{
	PHFemMeshThermoDesc();
	void Init();
};

///	���x��FEM�p�̃��b�V��
struct PHFemMeshThermoIf : public PHFemMeshIf{
	SPR_IFDEF(PHFemMeshThermo);
};

//@}

}	//	namespace Spr
#endif
