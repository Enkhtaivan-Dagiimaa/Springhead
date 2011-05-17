/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef FWFEMMESH_H
#define FWFEMMESH_H

#include <Springhead.h>
#include <Framework/FWObject.h>
#include <Graphics/GRMesh.h>

namespace Spr{;

//���̃N���X�̋@�\�́A���H
//
class FWFemMesh: public FWObject, public FWFemMeshDesc{
protected:
	UTRef<GRMesh> grMesh;
public:
	SPR_OBJECTDEF(FWFemMesh);
	ACCESS_DESC(FWFemMesh);
	FWFemMesh(const FWFemMeshDesc& d=FWFemMeshDesc());
	///	�q�I�u�W�F�N�g�̐�
	virtual size_t NChildObject() const;
	///	�q�I�u�W�F�N�g�̎擾
	virtual ObjectIf* GetChildObject(size_t pos);
	///	�q�I�u�W�F�N�g�̒ǉ�
	virtual bool AddChildObject(ObjectIf* o);
	//TetGen�Ŏl�ʑ̃��b�V����
	virtual bool IntoTetGen(GRMesh* grm);
};

}

#endif
