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
#include <Creature/SprCRBodyPart.h>

#include <map>

//@{
namespace Spr{;

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �{�f�B
*/
class CRBody : public SceneObject, public CRBodyDesc {
protected:
	/// �{�f�B���\�����鍄��
	typedef std::vector< UTRef<CRSolidIf> > CRSolids;
	CRSolids solids;	
	
	/// �{�f�B���\������֐�
	typedef std::vector< UTRef<CRJointIf> > CRJoints;
	CRJoints joints;

	/// ���x������\���v�f�ւ̃}�b�v
	typedef std::map<UTString, CRBodyPartIf*> LabelMap;
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
	virtual CRBodyPartIf* FindByLabel(UTString label);

	/** @brief �{�f�B�Ɋւ���X�e�b�v�������s��
	*/
	virtual void Step();

	/** @brief ���̂̐��𓾂�
	*/
	virtual int	NSolids() { return solids.size(); }

	/** @brief i�Ԗڂ̍��̂𓾂�
	*/
	virtual CRSolidIf* GetSolid(int i) { return solids[i]; }

	/** @brief �֐߂̐��𓾂�
	*/
	virtual int NJoints() { return joints.size(); }

	/** @brief i�Ԗڂ̊֐߂𓾂�
	*/
	virtual CRJointIf* GetJoint(int i) { return joints[i]; }

	/** @brief �{�f�B�̏d�S���W�𓾂�
	*/
	virtual Vec3d GetCenterOfMass();
	
	/** @brief �{�f�B�̎��ʂ𓾂�
	*/
	virtual double GetSumOfMass();

	/** @brief �q�v�f�̈���
	*/
	virtual size_t NChildObject() const { return solids.size()+joints.size(); }
	virtual ObjectIf* GetChildObject(size_t i);
	virtual bool AddChildObject(ObjectIf* o);
	virtual bool DelChildObject(ObjectIf* o);

	virtual Matrix3d CalcBoxInertia(Vec3d boxsize, double mass);
};
}
//@}

#endif//CRBODY_H
