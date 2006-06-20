/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CDPhysicalMaterial_H
#define CDPhysicalMaterial_H

#include <SceneGraph/SGObject.h>

namespace Spr {;

/// �����}�e���A���̃f�[�^
class CDPhysicalMaterialData{
public:
	CDPhysicalMaterialData(){ InitData(); }
	///
	void InitData();
	///	�R�͂̂΂ˌW��
	float reflexSpring;
	///	�R�͂̃_���p�W��
	float reflexDamper;
	///	���C�͂̂΂ˌW��
	float frictionSpring;
	///	���C�͂̃_���p�W��
	float frictionDamper;
	///	�ő�Î~���C�W��
	float staticFriction;
	///	�����C�W��
	float dynamicFriction;
};

/// �����}�e���A��
class CDPhysicalMaterial:public SGObject{
public:
	SGOBJECT_DEF(CDPhysicalMaterial);
	/// �����}�e���A���̃f�[�^
	CDPhysicalMaterialData pMatData;
	CDPhysicalMaterial(){
	};	
};

}
#endif
