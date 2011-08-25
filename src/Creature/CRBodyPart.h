/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRBODYPART_H
#define CRBODYPART_H

#include <Foundation/Object.h>
#include <Creature/SprCRBodyPart.h>

//@{
namespace Spr{;

struct PHIKActuatorIf;

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �{�f�B�\���v�f�̃x�[�X�N���X
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
	virtual const char* GetLabel() const { return label.c_str(); }

	/** @brief ���x���i����������������FHand�Ȃǁj�̐ݒ�
	 */
	void SetLabel(const char* str) { label = std::string(str); }

	/** @brief �P�X�e�b�v
	*/
	virtual void Step() {}
};

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �{�f�B�\������
*/
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
	virtual size_t NChildObject() const;
	virtual ObjectIf* GetChildObject(size_t i);
	virtual bool AddChildObject(ObjectIf* o);
	virtual bool DelChildObject(ObjectIf* o);
};

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �{�f�B�\�����́i���j
*/
class CRIKSolid : public CRSolid, public CRIKSolidDesc {
private:
	/// �\���V�~�����[�V�����p�̏�Ԋi�[�ϐ�
	// UTRef<ObjectStatesIf> state;

	/// �o�ߎ���
	float time;

	/// ���B�ڕW����
	float timeLimit;

	/// �ŏI�y�я����̖ڕW�ʒu�E���x�E�p���E�p���x
	Vec3d		finalPos, initPos; bool bCtlPos;
	Quaterniond	finalOri, initOri; bool bCtlOri;
	Vec3d		finalVel, initVel; bool bCtlVel;
	Vec3d		finalAvl, initAvl; bool bCtlAvl;

	/// �L����
	bool bEnable;

	/// ���̒T���G���A
	Posed		relativePose;
	double		horizRange;
	double		vertRange;
	std::vector<PHSolidIf*> foundSolids;

	virtual void InitVars() {
		ikEndEffector = NULL;
		bCtlPos = bCtlOri = bCtlVel = bCtlAvl = false;
		bEnable = false;

		relativePose = Posed();
		horizRange = vertRange = -1;
		foundSolids.clear();

		// state = ObjectStatesIf::Create();
	}

protected:
	PHIKEndEffectorIf *ikEndEffector;

public:
	SPR_OBJECTDEF(CRIKSolid);
	ACCESS_DESC(CRIKSolid);

	CRIKSolid()
		: ikEndEffector(NULL)
	{
		InitVars();
	}
	CRIKSolid(const CRIKSolidDesc& desc) 
		: CRIKSolidDesc(desc), CRSolid(desc)
	{
		InitVars();
	}

	// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
	// ��{�@�\

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

	/** @brief �P�X�e�b�v
	*/
	virtual void Step();

	/** @brief �q�v�f�̈���
	*/
	virtual size_t NChildObject() const;
	virtual ObjectIf* GetChildObject(size_t i);
	virtual bool AddChildObject(ObjectIf* o);
	virtual bool DelChildObject(ObjectIf* o);

	// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
	// �O���^��

	/** @brief �ڕW�ʒu�̐ݒ�
		@param pos �ڕW�ʒu
	*/
	void SetTargetPos(Vec3d pos);

	/** @brief �ڕW�p���̐ݒ�
		@param ori �ڕW�p��
	*/
	void SetTargetOri(Quaterniond ori);

	/** @brief �ڕW�ʒu�E�p���̐ݒ�
		@param pose �ڕW�ʒu�E�p��
	*/
	void SetTargetPose(Posed pose);

	/** @brief �ڕW���B�����̐ݒ�
		@param timeLimit ���B�܂ł̖ڕW�o�ߎ���
	*/
	void SetTimeLimit(float timeLimit);

	/** @brief �P�X�e�b�v
	*/
	void StepTrajectory();

	/** @brief ����J�n
	*/
	void Start();

	/** @brief �����~
	*/
	void Stop();

	/** @brief �O�����v�悷��
	*/
	void Plan();

	// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
	// ���̒T��

	/** @brief ���̒T���p�̉~���`�̃G���A��ݒu����
		@param relativePose �~����u�����΍��W�n��ݒ�F���_���~���̒��_�AZ��+���~���̎��AY��+�������
		@param horizRange ���������̕��i���W�A���j �}�C�i�X�Ȃ疳��
		@param vertRange  ���������̕��i���W�A���j �}�C�i�X�Ȃ疳��
	*/
	void SetViewArea(Posed relativePose, double horizRange, double vertRange);

	/** @brief �ݒ肳�ꂽ�~���`�̃G���A���ɑ��݂��鍄�̂̐�
	*/
	int NVisibleSolids();

	/** @brief �ݒ肳�ꂽ�~���`�̃G���A���ɑ��݂��鍄�̂�Ԃ�
	*/
	PHSolidIf* GetVisibleSolid(int i);

	/** @brief �P�X�e�b�v
	*/
	void StepSearchArea();
};

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �{�f�B�\���֐�
*/
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
	virtual void SetSpringRatio(double springRatio, double damperRatio);
	/** @brief �q�v�f�̈���
	*/
	virtual size_t NChildObject() const;
	virtual ObjectIf* GetChildObject(size_t i);
	virtual bool AddChildObject(ObjectIf* o);
	virtual bool DelChildObject(ObjectIf* o);
};

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �{�f�B�\���֐߁i���j
*/
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
	void SetIKActuator(PHIKActuatorIf* ikAct);

	/** @brief �o�l�_���p�W���̔{����ݒ�
	 */
	virtual void SetSpringRatio(double springRatio, double damperRatio);

	/** @brief �o�l�_���p�W���̔{����ݒ�
	 */
	void SetIKSpringRatio(double springRatio, double damperRatio);

	/** @brief �q�v�f�̈���
	*/
	virtual size_t NChildObject() const;
	virtual ObjectIf* GetChildObject(size_t i);
	virtual bool AddChildObject(ObjectIf* o);
	virtual bool DelChildObject(ObjectIf* o);
};

}
//@}

#endif//CRBODYPART_H
