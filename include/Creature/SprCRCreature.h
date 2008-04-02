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
	SPR_IFDEF(CRCreature);

	/** @brief �����������s����
	*/
	void Init();

	/** @brief ���o����񏈗����^�� �̂P�X�e�b�v�����s����
	*/
	void Step();

	/** @brief �����V�[���̃{�g���A�b�v���ӂ����Z�b�g����
	*/
	void ClearInternalScene();

	/** @brief �Z���T�[����̓��͂��s��
	*/
	void SensorStep();

	/** @brief �����V�[���̏������s��
	*/
	void InternalSceneStep();

	/** @brief ������s��
	*/
	void ControllerStep();

	/** @brief �{�f�B������
	*/
	CRBodyIf* CreateBody(const IfInfo* ii, const CRBodyDesc& desc);
	template <class T> CRBodyIf* CreateBody(const T& desc){
		return CreateBody(T::GetIfInfo(), desc);
	}

	/** @brief �{�f�B���擾����
	*/
	CRBodyIf* GetBody(int i);

	/** @brief �{�f�B�̐����擾����
	*/
	int NBodies();

	/** @brief ���o�n��ǉ�����i�ˑ�����Z���T�[�͐��Create���Ă����K�v������D�v�΍�j
	*/
	CRSensorIf* CreateSensor(const IfInfo* ii, const CRSensorDesc& desc);
	template <class T> CRSensorIf* CreateSensor(const T& desc){
		return CreateSensor(T::GetIfInfo(), desc);
	}

	/** @brief ���o�n���擾����
	*/
	CRSensorIf* GetSensor(int i);

	/** @brief ���o�n�̐����擾����
	*/
	int NSensors();

	/** @brief �^���R���g���[����ǉ�����i�ˑ�����R���g���[���͐��Create���Ă����K�v������D�v�΍�j
	*/
	CRControllerIf* CreateController(const IfInfo* ii, const CRControllerDesc& desc);
	template <class T> CRControllerIf* CreateController(const T& desc){
		return CreateController(T::GetIfInfo(), desc);
	}

	/** @brief �^���R���g���[�����擾����
	*/
	CRControllerIf* GetController(int i);

	/** @brief �^���R���g���[���̐����擾����
	*/
	int NControllers();

	/** @brief �����V�[��������
	*/
	CRInternalSceneIf* CreateInternalScene(const CRInternalSceneDesc& desc);

	/** @brief �����V�[�����擾����
	*/
	CRInternalSceneIf* GetInternalScene();
};

/// �N���[�`���̃f�X�N���v�^
struct CRCreatureDesc{
	SPR_DESCDEF(CRCreature);

	CRCreatureDesc(){
	}
};
//@}

}

#endif//SPR_CRCreatureIf_H
