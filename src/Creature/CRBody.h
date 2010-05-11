/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRBODY_H
#define CRBODY_H

#include <Springhead.h>

#include <Foundation/Object.h>

#include <Physics/PHIKActuator.h>

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �{�f�B�̍\���v�f
*/
class CRBodyPart : public SceneObject, public CRBodyPartDesc {
protected:
public:
	SPR_OBJECTDEF(CRBodyPart);
	// ACCESS_DESC(CRBodyPart);

	CRBodyPart(){}
	CRBodyPart(const CRBodyPartDesc& desc) 
		: CRBodyPartDesc(desc)
	{
	}

	/** @brief ���x���i����������������FHand�Ȃǁj�̎擾
	 */
	const char* GetLabel() const { return label.c_str(); }

	/** @brief ���x���i����������������FHand�Ȃǁj�̐ݒ�
	 */
	void SetLabel(const char* str) { label = std::string(str); }
};

class CRSolid : public CRBodyPart, public CRSolidDesc {
protected:
	PHSolidIf* solid;

public:
	SPR_OBJECTDEF(CRSolid);
	ACCESS_DESC(CRSolid);

	CRSolid()
		: solid(NULL)
	{
	}
	CRSolid(const CRSolidDesc& desc) 
		: solid(NULL), CRSolidDesc(desc), CRBodyPart(desc)
	{
	}

	/** @brief PHSolid���擾
	 */
	PHSolidIf* GetPHSolid() { return solid; }

	/** @brief PHSolid��ݒ�
	 */
	void SetPHSolid(PHSolidIf* so) { solid = so; }

	/** @brief �q�v�f�̈���
	*/
	virtual size_t NChildObject() const { if (solid) { return 1; } else { return 0; } }
	virtual ObjectIf* GetChildObject(size_t i) { if (i==0) { return solid; } else { return NULL; } }
	virtual bool AddChildObject(ObjectIf* o) { solid = o->Cast(); return(solid!=NULL); }
	virtual bool DelChildObject(ObjectIf* o) { if (o==solid) { solid = NULL; return true; } return false; }
};

class CRIKSolid : public CRSolid, public CRIKSolidDesc {
protected:
	PHIKEndEffectorIf *ikEndEffector;

public:
	SPR_OBJECTDEF(CRIKSolid);
	ACCESS_DESC(CRIKSolid);

	CRIKSolid()
		: ikEndEffector(NULL)
	{
	}
	CRIKSolid(const CRIKSolidDesc& desc) 
		: ikEndEffector(NULL), CRIKSolidDesc(desc), CRSolid(desc)
	{
	}

	/** @brief IK�G���h�G�t�F�N�^���擾
	 */
	PHIKEndEffectorIf* GetIKEndEffector() {
		return ikEndEffector;
	}

	/** @brief IK�G���h�G�t�F�N�^��ݒ�
	 */
	void SetIKEndEffector(PHIKEndEffectorIf* ikEE) {
		ikEndEffector = ikEE;
	}

	/** @brief �q�v�f�̈���
	*/
	virtual size_t NChildObject() const {
		return( (solid ? 1 : 0) + (ikEndEffector ? 1 : 0) );
	}

	virtual ObjectIf* GetChildObject(size_t i) {
		if (solid) {
			if (i==0) {
				return solid;
			} else if (i==1) {
				return ikEndEffector;
			}
		} else {
			if (i==0) {
				return ikEndEffector;
			}
		}
		return NULL;
	}

	virtual bool AddChildObject(ObjectIf* o) {
		PHSolidIf* so = o->Cast();
		if (so) { solid = so; return true; }

		PHIKEndEffectorIf* ikef = o->Cast();
		if (ikef) { ikEndEffector = ikef; return true; }
		
		return false;
	}

	virtual bool DelChildObject(ObjectIf* o) {
		PHSolidIf* so = o->Cast();
		if (so && so==solid) { solid = NULL; return true; }

		PHIKEndEffectorIf* eef = o->Cast();
		if (eef && eef==ikEndEffector) { ikEndEffector = NULL; return true; }

		return false;
	}
};

class CRJoint : public CRBodyPart, public CRJointDesc {
protected:
	PHJointIf* joint;
	double spring, damper;

public:
	SPR_OBJECTDEF(CRJoint);
	ACCESS_DESC(CRJoint);

	CRJoint()
		: joint(NULL), spring(-1), damper(-1)
	{
	}
	CRJoint(const CRJointDesc& desc) 
		: joint(NULL), CRJointDesc(desc), CRBodyPart(desc), spring(-1), damper(-1)
	{
	}

	/** @brief PHJoint���擾
	 */
	PHJointIf* GetPHJoint() { return joint; }

	/** @brief PHJoint��ݒ�
	 */
	void SetPHJoint(PHJointIf* jo) { joint = jo; }

	/** @brief �o�l�_���p�W���̔{����ݒ�
	 */
	virtual void SetSpringRatio(double springRatio, double damperRatio) {
		if (PHBallJointIf* bj = joint->Cast()) {
			if (spring < 0) {
				spring = bj->GetSpring();
			}
			if (damper < 0) {
				damper = bj->GetDamper();
			}
			bj->SetSpring(spring*springRatio); bj->SetDamper(damper*damperRatio);
			std::cout << "bj_ssr : " << spring * springRatio << std::endl;
		}
		if (PHHingeJointIf* hj = joint->Cast()) {
			if (spring < 0) {
				spring = hj->GetSpring();
			}
			if (damper < 0) {
				damper = hj->GetDamper();
			}
			hj->SetSpring(spring*springRatio); hj->SetDamper(damper*damperRatio);
			std::cout << "hj_ssr : " << spring * springRatio << std::endl;
		}
	}

	/** @brief �q�v�f�̈���
	*/
	virtual size_t NChildObject() const { if (joint) { return 1; } else { return 0; } }
	virtual ObjectIf* GetChildObject(size_t i) { if (i==0) { return joint; } else { return NULL; } }
	virtual bool AddChildObject(ObjectIf* o) { joint = o->Cast(); return(joint!=NULL); }
	virtual bool DelChildObject(ObjectIf* o) { if (o==joint) { joint = NULL; return true; } return false; }
};

class CRIKJoint : public CRJoint, public CRIKJointDesc {
protected:
	PHIKActuatorIf* ikActuator;
	double ikSpring, ikDamper;

public:
	SPR_OBJECTDEF(CRIKJoint);
	ACCESS_DESC(CRIKJoint);

	CRIKJoint()
		: ikActuator(NULL), ikSpring(-1), ikDamper(-1)
	{
	}
	CRIKJoint(const CRIKJointDesc& desc) 
		: ikActuator(NULL), CRIKJointDesc(desc), CRJoint(desc), ikSpring(-1), ikDamper(-1)
	{
	}

	/** @brief IK�A�N�`���G�[�^���擾
	 */
	PHIKActuatorIf* GetIKActuator() { return ikActuator; }

	/** @brief IK�A�N�`���G�[�^��ݒ�
	 */
	void SetIKActuator(PHIKActuatorIf* ikAct) { ikActuator = ikAct; ikSpring = ikAct->GetSpring(); ikDamper = ikAct->GetDamper(); }

	/** @brief �o�l�_���p�W���̔{����ݒ�
	 */
	virtual void SetSpringRatio(double springRatio, double damperRatio) {
		if (PHBallJointIf* bj = joint->Cast()) {
			if (spring < 0) {
				spring = bj->GetSpring();
			}
			if (damper < 0) {
				damper = bj->GetDamper();
			}
			bj->SetSpring(spring*springRatio); bj->SetDamper(damper*damperRatio);
			DCAST(PHIKBallActuator,ikActuator)->jSpring = spring*springRatio;
			DCAST(PHIKBallActuator,ikActuator)->jDamper = damper*damperRatio;
			std::cout << "bj_ssr : " << spring * springRatio << std::endl;
		}
		if (PHHingeJointIf* hj = joint->Cast()) {
			if (spring < 0) {
				spring = hj->GetSpring();
			}
			if (damper < 0) {
				damper = hj->GetDamper();
			}
			hj->SetSpring(spring*springRatio); hj->SetDamper(damper*damperRatio);
			DCAST(PHIKHingeActuator,ikActuator)->jSpring = spring*springRatio;
			DCAST(PHIKHingeActuator,ikActuator)->jDamper = damper*damperRatio;
			std::cout << "hj_ssr : " << spring * springRatio << std::endl;
		}
	}

	/** @brief �o�l�_���p�W���̔{����ݒ�
	 */
	void SetIKSpringRatio(double springRatio, double damperRatio) {
		if (ikSpring < 0) {
			ikSpring = ikActuator->GetSpring();
		}
		if (ikDamper < 0) {
			ikDamper = ikActuator->GetDamper();
		}
		ikActuator->SetSpring(ikSpring*springRatio); ikActuator->SetDamper(ikDamper*damperRatio);
	}

	/** @brief �q�v�f�̈���
	*/
	virtual size_t NChildObject() const {
		return( (joint ? 1 : 0) + (ikActuator ? 1 : 0) );
	}

	virtual ObjectIf* GetChildObject(size_t i) {
		if (joint) {
			if (i==0) {
				return joint;
			} else if (i==1) {
				return ikActuator;
			}
		} else {
			if (i==0) {
				return ikActuator;
			}
		}
		return NULL;
	}

	virtual bool AddChildObject(ObjectIf* o) {
		PHJointIf* jo = o->Cast();
		if (jo) { joint = jo; return true; }

		PHIKActuatorIf* ikact = o->Cast();
		if (ikact) { ikActuator = ikact; return true; }
		
		return false;
	}

	virtual bool DelChildObject(ObjectIf* o) {
		PHJointIf* jo = o->Cast();
		if (jo && jo==joint) { joint = NULL; return true; }

		PHIKActuatorIf* act = o->Cast();
		if (act && act==ikActuator) { ikActuator = NULL; return true; }

		return false;
	}
};

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

public:
	SPR_OBJECTDEF(CRBody);
	ACCESS_DESC(CRBody);

	CRBody(){}
	CRBody(const CRBodyDesc& desc) 
		: CRBodyDesc(desc)
	{
	}

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
