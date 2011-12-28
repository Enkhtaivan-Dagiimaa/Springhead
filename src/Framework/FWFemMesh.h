/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef FWFEMMESH_H
#define FWFEMMESH_H

#include <Framework/SprFWFemMesh.h>
#include <Framework/FWObject.h>
#include <Physics/PHFemMesh.h>
#include "FrameworkDecl.hpp"

namespace Spr{;

class GRMesh;

/**	Framework��ł�FEM���b�V���B
	���X�e�b�v�APHFemMesh��FEM�V�~�����[�V�������ʂ�GRMesh�ɔ��f������B
	���������ɂ́AGRMesh����PHFemMesh�𐶐����A�����PHFemMesh����GRMesh�𐶐����AgrMesh��grFrame�̉���GRMesh�ƒu��������B*/
class FWFemMesh: public FWObject{
	SPR_OBJECTDEF(FWFemMesh);		
	SPR_DECLMEMBEROF_FWFemMeshDesc;	//	FWFemMeshDesc�̃����o��GetDesc(), GetDescSize()�̎���
protected:
	GRMesh* grMesh;
	UTRef<PHFemMesh> phMesh;
	std::vector<int> vertexIdMap;	//	grMesh����phMesh�ւ̒��_�̑Ή��\
public:
	FWFemMesh(const FWFemMeshDesc& d=FWFemMeshDesc());		//�R���X�g���N�^
	///	���[�h��ɌĂ΂��BgrMesh����phMesh�𐶐����AgrMesh��phMesh�ɍ��킹�����̂ɒu��������
	void Loaded(UTLoadContext* );
	///	�O���t�B�N�X�\���O�̓�������
	void Sync(bool ph2gr);
	///	�q�I�u�W�F�N�g�̐�
	virtual size_t NChildObject() const;
	///	�q�I�u�W�F�N�g�̎擾
	virtual ObjectIf* GetChildObject(size_t pos);
	///	�q�I�u�W�F�N�g�̒ǉ�
	virtual bool AddChildObject(ObjectIf* o);
	/// phMesh���擾
	PHFemMeshIf* GetPHMesh(){return phMesh->Cast();	};

//protected:
	//	TetGen��p���āAGRMesh���l�ʑ̃��b�V�������AphMesh�Ɋi�[����B
	virtual bool CreatePHFromGR();
	//	phMesh����GRMesh�𐶐�����B�}�e���A���Ȃǂ�grMesh����E���B
	void CreateGRFromPH();
	
};
}

#endif
