/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRBodyController_H
#define CRBodyController_H

#include <Creature/CREngine.h>
#include <Creature/SprCRBody.h>
#include <Creature/SprCRBodyController.h>

#include <vector>

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �e�p�[�c�̓��B�ڕW���Ǘ�����\����
*/
struct CRBCTargetValue {
	/// �Ώۍ���
	CRIKSolidIf* solid;

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

	CRBCTargetValue() {
		bEnable = false;
	}
};


// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �S�g�^���R���g���[��
*/
class CRBodyController : public CREngine, public CRBodyControllerDesc {
private:
	/// �\���V�~�����[�V�����p�̏�Ԋi�[�ϐ�
	// UTRef<ObjectStatesIf> state;

	/// ����Ώۂ̃N���[�`���g��
	CRBodyIf* body;

	/// �e�p�[�c�̓��B�ڕW���X�g
	std::vector<CRBCTargetValue*> targetValues;

	/// �ڕW���X�g����w�胉�x�������p�[�c�Ɋւ���ڕW�l�Z�b�g��T���i�Ȃ���΍��j
	CRBCTargetValue* FindTargetValue(UTString label);

	/// �f�o�b�O�\���p����
	PHSolidIf* soPoint;

public:
	SPR_OBJECTDEF(CRBodyController);
	ACCESS_DESC(CRBodyController);

	CRBodyController(){
		body = NULL;
		soPoint = NULL;
	}
	CRBodyController(const CRBodyControllerDesc& desc) 
		: CRBodyControllerDesc(desc)
	{
		body = NULL;
		soPoint = NULL;
		Init();
	}

	/** @ brief �����������s����
	*/
	virtual void Init();

	/** @ brief ����̃X�e�b�v�����s����
	*/
	virtual void Step();

	/** @brief �ڕW�ʒu�̐ݒ�
		@param effector ���삳����p�[�c�̃��x��
		@param pos �ڕW�ʒu
	*/
	virtual void SetTargetPos(UTString effector, Vec3d pos);

	/** @brief �ڕW�p���̐ݒ�
		@param effector ���삳����p�[�c�̃��x��
		@param ori �ڕW�p��
	*/
	virtual void SetTargetOri(UTString effector, Quaterniond ori);

	/** @brief �ڕW�ʒu�E�p���̐ݒ�
		@param effector ���삳����p�[�c�̃��x��
		@param pose �ڕW�ʒu�E�p��
	*/
	virtual void SetTargetPose(UTString effector, Posed pose);

	/** @brief �ڕW���B�����̐ݒ�
		@param effector ���삳����p�[�c�̃��x��
		@param timeLimit ���B�܂ł̖ڕW�o�ߎ���
	*/
	virtual void SetTimeLimit(UTString effector, float timeLimit);

	/** @brief �O�����v�悷��
	*/
	virtual void Plan();

	/** @brief �^���J�n
		@param effector ���삳����p�[�c�̃��x��
	*/
	virtual void Restart(UTString effector);

	/** @brief �q�v�f�̈���
	*/
	virtual size_t NChildObject() const { return body ? 1 : 0; }
	virtual ObjectIf* GetChildObject(size_t i) { return( ((i==0) ? body : NULL) ); }
	virtual bool AddChildObject(ObjectIf* o){ CRBodyIf* b=o->Cast(); if(b){body=b; return true;} return false; }
	virtual bool DelChildObject(ObjectIf* o){ if (o==body) { body = NULL; return true;} return false; }
};

}
//@}

#endif//CRBodyController_H
