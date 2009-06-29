/*
 *  Copyright (c) 2003-2009, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_CRSDKIF_H
#define SPR_CRSDKIF_H

namespace Spr{;

//@{

struct CRCreatureIf;
struct CRCreatureDesc;

/// �N���[�`��Sdk�̃C���^�t�F�[�X
struct CRSdkIf : SdkIf {
	SPR_IFDEF(CRSdk);

	/** @brief SDK�I�u�W�F�N�g���쐬����
	 */
	static CRSdkIf* SPR_CDECL CreateSdk();

	/** @brief �N���[�`������̍쐬����
	 */
	CRCreatureIf* CreateCreature(const IfInfo* ii, const CRCreatureDesc& desc);
	template <class T> CRCreatureIf* CreateCreature(const T& desc){
		return CreateCreature(T::GetIfInfo(), desc);
	}

	/** @brief �N���[�`���̌��i�l���H�j���擾����
	 */
	int NCreatures() const;

	/** @brief �N���[�`����̂��擾����
	 */
	CRCreatureIf* GetCreature(int index);

	/** @brief �S�N���[�`���ɂ��ĂP�X�e�b�v���������s����
	 */
	void Step();

	static void SPR_CDECL RegisterSdk();
};

/// �N���[�`��Sdk�̃f�X�N���v�^
struct CRSdkDesc {
	SPR_DESCDEF(CRSdk);

	CRSdkDesc(){
	}
};

//@}

}

#endif // SPR_CRSDKIF_H