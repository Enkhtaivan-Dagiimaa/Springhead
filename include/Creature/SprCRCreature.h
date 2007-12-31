/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_CRCreatureIf_H
#define SPR_CRCreatureIf_H

#include <SprFoundation.h>

namespace Spr{;

//@{

struct CRBodyIf;
struct CRBodyDesc;
struct CRSensorIf;
struct CRSensorDesc;
struct CRControllerIf;
struct CRControllerDesc;
struct CRInternalSceneIf;
struct CRInternalSceneDesc;

// ------------------------------------------------------------------------------
/// �^���o�^

void SPR_CDECL CRRegisterTypeDescs();

// ------------------------------------------------------------------------------
/// �N���[�`���̃C���^�t�F�[�X
struct CRCreatureIf : SceneObjectIf{
	IF_DEF(CRCreature);

	/** @brief �����������s����
	*/
	virtual void Init()= 0;

	/** @brief ���o����񏈗����^�� �̂P�X�e�b�v�����s����
	*/
	virtual void Step()= 0;

	/** @brief �����V�[���̃{�g���A�b�v���ӂ����Z�b�g����
	*/
	virtual void ClearInternalScene()= 0;

	/** @brief �Z���T�[����̓��͂��s��
	*/
	virtual void SensorStep()= 0;

	/** @brief �����V�[���̏������s��
	*/
	virtual void InternalSceneStep()= 0;

	/** @brief ������s��
	*/
	virtual void ControllerStep()= 0;

	/** @brief �{�f�B������
	*/
	virtual CRBodyIf* CreateBody(const IfInfo* ii, const CRBodyDesc& desc)= 0;
	template <class T> CRBodyIf* CreateBody(const T& desc){
		return CreateBody(T::GetIfInfo(), desc);
	}

	/** @brief �{�f�B���擾����
	*/
	virtual CRBodyIf* GetBody(int i)= 0;

	/** @brief �{�f�B�̐����擾����
	*/
	virtual int NBodies()= 0;

	/** @brief ���o�n��ǉ�����i�ˑ�����Z���T�[�͐��Create���Ă����K�v������D�v�΍�j
	*/
	virtual CRSensorIf* CreateSensor(const IfInfo* ii, const CRSensorDesc& desc)= 0;
	template <class T> CRSensorIf* CreateSensor(const T& desc){
		return CreateSensor(T::GetIfInfo(), desc);
	}

	/** @brief ���o�n���擾����
	*/
	virtual CRSensorIf* GetSensor(int i)= 0;

	/** @brief ���o�n�̐����擾����
	*/
	virtual int NSensors()= 0;

	/** @brief �^���R���g���[����ǉ�����i�ˑ�����R���g���[���͐��Create���Ă����K�v������D�v�΍�j
	*/
	virtual CRControllerIf* CreateController(const IfInfo* ii, const CRControllerDesc& desc)= 0;
	template <class T> CRControllerIf* CreateController(const T& desc){
		return CreateController(T::GetIfInfo(), desc);
	}

	/** @brief �^���R���g���[�����擾����
	*/
	virtual CRControllerIf* GetController(int i)= 0;

	/** @brief �^���R���g���[���̐����擾����
	*/
	virtual int NControllers()= 0;

	/** @brief �����V�[��������
	*/
	virtual CRInternalSceneIf* CreateInternalScene(const CRInternalSceneDesc& desc)= 0;

	/** @brief �����V�[�����擾����
	*/
	virtual CRInternalSceneIf* GetInternalScene()= 0;
};

/// �N���[�`���̃f�X�N���v�^
struct CRCreatureDesc{
	DESC_DEF_FOR_OBJECT(CRCreature);

	CRCreatureDesc(){
	}
};
//@}

}

#endif//SPR_CRCreatureIf_H
