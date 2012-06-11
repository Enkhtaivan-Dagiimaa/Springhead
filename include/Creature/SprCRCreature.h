/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_CRCREATUREIF_H
#define SPR_CRCREATUREIF_H

#include <Foundation/SprScene.h>

namespace Spr{;

//@{
struct PHSceneIf;
struct CRBodyIf;
struct CRBodyDesc;
struct CREngineIf;
struct CREngineDesc;

// ------------------------------------------------------------------------------
/// �^���o�^

void SPR_CDECL CRRegisterTypeDescs();

// ------------------------------------------------------------------------------
/// �N���[�`���̃C���^�t�F�[�X
struct CRCreatureIf : SceneIf{
	SPR_IFDEF(CRCreature);

	/** @brief ���o����񏈗����^�� �̂P�X�e�b�v�����s����
	*/
	void Step();

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

	/** @brief CREngine���쐬����
	*/
	CREngineIf* CreateEngine(const IfInfo* ii, const CREngineDesc& desc);
	template <class T> CREngineIf* CreateEngine(const T& desc){
		return CreateEngine(T::GetIfInfo(), desc);
	}

	/** @brief CREngine���擾����
	*/
	CREngineIf* GetEngine(int i);

	/** @brief CREngine�̐����擾����
	*/
	int NEngines();

	/** @brief �֘A����PHScene���擾����
	*/
	PHSceneIf* GetPHScene();
};

/// �N���[�`���̃f�X�N���v�^
struct CRCreatureDesc{
	SPR_DESCDEF(CRCreature);

	CRCreatureDesc(){
	}
};

//@}

}

#endif // SPR_CRCREATUREIF_H
