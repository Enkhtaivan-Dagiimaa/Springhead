/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_FIFile_H
#define SPR_FIFile_H
#include <Foundation/SprObject.h>

/**	\defgroup gpFileIO	�t�@�C�����o��SDK	*/
//@{
namespace Spr{;

class UTTypeDescDb;
///	�t�@�C�����[�_�E�Z�[�o
struct FIFileIf: public ObjectIf{
	SPR_IFDEF(FIFile);
	/**	@brief �V�[�����t�@�C�����烍�[�h����
		@param objs ���[�h����V�[���̐e�ƂȂ�I�u�W�F�N�g�̃X�^�b�N
		@param fn	�t�@�C����
		@return		����������true�C���s������false
	 */
	bool Load(ObjectIfs& objs, const char* fn);

	/** @brief �V�[�����t�@�C���փZ�[�u����
		@param objs	�Z�[�u����V�[���̍ŏ�ʃI�u�W�F�N�g���i�[�����z��
		@param fn	�t�@�C����
		@return		����������true, ���s������false
	 */
	bool Save(const ObjectIfs& objs, const char* fn);

	/** @brief	�C���|�[�g���̐ݒ�
		@param	�C���|�[�g���
		�V�[�����Z�[�u����ۂɁC�ꕔ��ʃt�@�C���փG�N�X�|�[�g�������ꍇ��
		Save�ɐ旧����SetImport���ĂԁD
	 */
	void SetImport(ImportIf* import);

	/**	@brief  �C���|�[�g���̎擾
		@return	�C���|�[�g���
		�V�[���̃��[�h��ɃC���|�[�g���擾�������ۂɎg�p����D
	 */
	ImportIf* GetImport();

	///	����DSTR�̏o�̗͂L��
	/// �ėp�����Ⴂ�̂�ObjectIf�Ƀf�o�b�O�o�̓��x����ݒ肷��API������ׂ����Htazz
	void SetDSTR(bool f);
};
///	Spr�t�@�C���̃t�@�C�����[�_�ƃZ�[�o
struct FIFileSprIf: public FIFileIf{
	SPR_IFDEF(FIFileSpr);
	///	���[�h
	bool Load(ObjectIfs& objs, const char* fn);
	///	�Z�[�u
	bool Save(const ObjectIfs& objs, const char* fn);
};

///	DirectX�t�@�C���̃t�@�C�����[�_�ƃZ�[�o
struct FIFileXIf: public FIFileIf{
	SPR_IFDEF(FIFileX);
	///	���[�h
	bool Load(ObjectIfs& objs, const char* fn);
	///	�Z�[�u
	bool Save(const ObjectIfs& objs, const char* fn);
};

///	DirectX�t�@�C���̃t�@�C�����[�_�ƃZ�[�o
struct FIFileVRMLIf: public FIFileIf{
	SPR_IFDEF(FIFileVRML);
	///	���[�h
	bool Load(ObjectIfs& objs, const char* fn);
	///	�Z�[�u
	bool Save(const ObjectIfs& objs, const char* fn);
};


///	COLLADA�t�@�C���̃t�@�C�����[�_�ƃZ�[�o
struct FIFileCOLLADAIf: public FIFileIf{
	SPR_IFDEF(FIFileCOLLADA);
	///	���[�h
	bool Load(ObjectIfs& objs, const char* fn);
	///	�Z�[�u
	bool Save(const ObjectIfs& objs, const char* fn);
};

///	�o�C�i���t�@�C���̃t�@�C�����[�_�ƃZ�[�o
struct FIFileBinaryIf: public FIFileIf{
	SPR_IFDEF(FIFileBinary);
	///	���[�h
	bool Load(ObjectIfs& objs, const char* fn);
	///	�Z�[�u
	bool Save(const ObjectIfs& objs, const char* fn);
};

}

#endif
