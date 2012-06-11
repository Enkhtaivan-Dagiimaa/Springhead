/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRBODY_H
#define CRBODY_H

#include <Foundation/Object.h>
#include <Creature/SprCRBody.h>
#include <Creature/SprCRBone.h>

#include <map>

//@{
namespace Spr{;

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �{�f�B
*/
class CRBody : public SceneObject, public CRBodyDesc {
protected:
	/// �{�f�B���\������{�[��
	typedef std::vector< UTRef<CRBoneIf> > CRBones;
	CRBones bones;	
	
	/// ���x������\���v�f�ւ̃}�b�v
	typedef std::map<UTString, CRBoneIf*> LabelMap;
	LabelMap labelMap;

public:
	SPR_OBJECTDEF(CRBody);
	ACCESS_DESC(CRBody);

	CRBody(){}
	CRBody(const CRBodyDesc& desc) 
		: CRBodyDesc(desc)
	{
	}

	/** @brief ���x������\���v�f��T��
	*/
	virtual CRBoneIf* FindByLabel(UTString label);

	/** @brief �{�[���̐��𓾂�
	*/
	virtual int	NBones() { return bones.size(); }

	/** @brief i�Ԗڂ̃{�[���𓾂�
	*/
	virtual CRBoneIf* GetBone(int i) {
		if (0<=i && i<(int)bones.size()) {
			return bones[i];
		} else {
			return NULL;
		}
	}

	/** @brief �{�f�B�̏d�S���W�𓾂�
	*/
	virtual Vec3d GetCenterOfMass();
	
	/** @brief �{�f�B�̎��ʂ𓾂�
	*/
	virtual double GetSumOfMass();

	/** @brief �q�v�f�̈���
	*/
	virtual size_t NChildObject() const { return bones.size(); }
	virtual ObjectIf* GetChildObject(size_t i) { return GetBone(i); }
	virtual bool AddChildObject(ObjectIf* o);
	virtual bool DelChildObject(ObjectIf* o);

	virtual Matrix3d CalcBoxInertia(Vec3d boxsize, double mass);
};
}
//@}

#endif//CRBODY_H
