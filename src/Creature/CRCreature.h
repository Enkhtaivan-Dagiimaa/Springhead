/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRCREATURE_H
#define CRCREATURE_H

#include <Springhead.h>

#include <Foundation/Object.h>

#include <vector>

//@{
namespace Spr{;

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �N���[�`���̎���
*/
class CRCreature : public SceneObject, public CRCreatureDesc {

protected:
	/** @brief ���o�n
	*/
	std::vector<CRSensorIf*> sensors;

	/** @brief �^���R���g���[��
	*/
	std::vector<CRControllerIf*> controllers;

	/** @brief �����V�[���i�L���̗ށj
	*/
	CRInternalSceneIf* internalScene;

	/** @brief �{�f�B�i�������Ă�j
	*/
	std::vector< UTRef<CRBodyIf> > body;

public:
	SPR_OBJECTDEF(CRCreature);
	ACCESS_DESC(CRCreature);

	CRCreature(){
		CRRegisterTypeDescs();
	}
	CRCreature(const CRCreatureDesc& desc, SceneIf* s=NULL) : CRCreatureDesc(desc) {
		CRRegisterTypeDescs();
		internalScene = NULL;
		if(s){SetScene(s);}
	}

	/** @brief �����������s����
	*/
	virtual void Init();

	/** @brief ���o����񏈗����^�� �̂P�X�e�b�v�����s����
	*/
	virtual void Step();

	/** @brief �����V�[���̃{�g���A�b�v���ӂ����Z�b�g����
	*/
	virtual void ClearInternalScene();

	/** @brief �Z���T�[����̓��͂��s��
	*/
	virtual void SensorStep();

	/** @brief �����V�[���̏������s��
	*/
	virtual void InternalSceneStep();

	/** @brief ������s��
	*/
	virtual void ControllerStep();

	/** @brief �{�f�B������
	*/
	virtual CRBodyIf* CreateBody(const IfInfo* ii, const CRBodyDesc& desc);

	/** @brief �{�f�B���擾����
	*/
	virtual CRBodyIf* GetBody(int i);

	/** @brief �{�f�B�̐����擾����
	*/
	virtual int NBodies();

	/** @brief ���o�n��ǉ�����
	*/
	virtual CRSensorIf* CreateSensor(const IfInfo* ii, const CRSensorDesc& desc);

	/** @brief ���o�n���擾����
	*/
	virtual CRSensorIf* GetSensor(int i);

	/** @brief ���o�n�̐����擾����
	*/
	virtual int NSensors();

	/** @brief �^���R���g���[����ǉ�����
	*/
	virtual CRControllerIf* CreateController(const IfInfo* ii, const CRControllerDesc& desc);

	/** @brief �^���R���g���[�����擾����
	*/
	virtual CRControllerIf* GetController(int i);

	/** @brief �^���R���g���[���̐����擾����
	*/
	virtual int NControllers();

	/** @brief �����V�[��������
	*/
	virtual CRInternalSceneIf* CreateInternalScene(const CRInternalSceneDesc& desc);

	/** @brief �����V�[�����擾����
	*/
	virtual CRInternalSceneIf* GetInternalScene();
};
}
//@}

#endif//CRCREATURE_H
