/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_FIImport_H
#define SPR_FIImport_H

/**	\defgroup gpFileIO */
//@{
namespace Spr{;

/// Import�m�[�h
//  UTImportHandler������
/*struct Import{
	std::string		path;	
};*/

struct ImportDesc{
	std::string		path;	
};

/**
	�t�@�C���Ԃ̃C���|�[�g�֌W���Ǘ�����N���X�D
	�c���[�\�������D
 */
struct ImportIf : ObjectIf{
	SPR_IFDEF(Import);

	/**	@brief ���[�h��p���[�h��On/Off����
		@param on
		���[�h��p���[�h�̏ꍇ�C�ۑ����Ɏq�I�u�W�F�N�g��ʃt�@�C���֕ۑ������C
		�{�̃t�@�C����Import�m�[�h�̂ݏ������ށD
		Import�Ώۂ��O���A�v���ō쐬���ꂽ���b�V���f�[�^�Ȃǂ̂Ƃ��Ɏg�p����D
	 */
	void SetLoadOnly(bool on = true);
	/*
	/// �C���|�[�g�Ώۂ̃I�u�W�F�N�g��ǉ�
	void AddObject(ObjectIf* obj);

	/// �C���|�[�g�Ώۂ̃I�u�W�F�N�g���擾
	//ObjectIf* GetTarget();

	/// �C���|�[�g/�G�N�X�|�[�g���鑊�΃p�X��ݒ�
	void SetPath(UTString path);
	
	/// �C���|�[�g/�G�N�X�|�[�g���鑊�΃p�X���擾
	UTString GetPath();

	/// �q�C���|�[�g�G���g�����쐬
	FIImportIf* CreateImport();
	*/
};

}

#endif
