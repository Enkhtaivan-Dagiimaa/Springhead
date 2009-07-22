/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PH3Element_H
#define PH3Element_H

#include <SprPhysics.h>
#include <Foundation/Object.h>
#include <Physics/PHJoint.h>

namespace Spr{;

struct SpatialVectorArray2{
	SpatialVector v[2];
	SpatialVector& operator[](int i){
		return v[i];
	}
};
struct PH3ElementState{
	SpatialVectorArray2 xs;				///(3�v�f���f����p)�΂˕��̋���

};

struct PH3ElementCommonData: public PH3ElementState{
	SpatialVector xd;				///(3�v�f���f����p)�_���p���̋���
	SpatialVector ws;					///(3�v�f���f����p)�΂˕��̑���
	//SpatialVector fs;					///(3�v�f���f����p)�΂˕��̗�
};
class PH3Element : public PHJoint, public PH3ElementCommonData{
	Vec3d spring, damper,secondDamper;
	double springOri, damperOri, yieldStress, hardnessRate;
	Vec3d  I;							///�f�ʂQ�����[�����g
	bool   yieldFlag;
public:
	SPR_OBJECTDEF(PH3Element);

	/// �C���^�t�F�[�X�̎���
	virtual void SetSpring(const Vec3d& s){spring = s;}
	virtual Vec3d GetSpring(){return spring;}
	virtual void SetDamper(const Vec3d& d){damper = d;}
	virtual Vec3d GetDamper(){return damper;}
	virtual void SetSecondDamper(const Vec3d& d2){secondDamper = d2;}
	virtual Vec3d GetSecondDamper(){return secondDamper;}

	virtual void SetSpringOri(const double spring){springOri = spring;}
	virtual double GetSpringOri(){return springOri;}
	virtual void SetDamperOri(const double damper){damperOri = damper;}
	virtual double GetDamperOri(){return damperOri;}

	virtual void SetDesc(const void* desc);
	virtual void SetConstrainedIndex(bool* con);
	virtual void SetConstrainedIndexCorrection(bool* con);

	virtual void SetYieldStress(const double yS){yieldStress = yS;}
	virtual double GetYieldStress(){return yieldStress;}
	virtual void SetHardnessRate(const double hR){hardnessRate = hR;}
	virtual double GetHardnessRate(){return hardnessRate;}
	virtual void SetI(const Vec3d i){I = i;}
	virtual Vec3d GetI(){return I;}

	virtual void CompBias();

	//�R���X�g���N�^������
	PH3Element(const PH3ElementDesc& desc = PH3ElementDesc());
};

}

#endif
