#ifndef SPR_UTPATH_H
#define SPR_UTPATH_H
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Base/BaseUtility.h>

namespace Spr {;

class SPR_DLL UTPath{
protected:
	UTString path;
public:
	///	�J�����g�f�B���N�g�����擾����
	static UTString GetCwd();
	///	�J�����g�f�B���N�g����ݒ肷��
	static bool SetCwd(UTString cwd);
	/// �f�B���N�g�����쐬����
	static void CreateDir(UTString dirname);

	///	�t�@�C���p�X�̎擾
	UTString Path() const { return path; }
	operator UTString() const { return Path(); }
	///	�t�@�C���p�X�̐ݒ�
	void Path(UTString p);
	///	�t�@�C���p�X�̃h���C�u��
	UTString Drive();
	///	�t�@�C���p�X�̃h���C�u��
	UTString Dir();
	///	�t�@�C���p�X�̃t�@�C������
	UTString File();
	///	�t�@�C���p�X�̃t�@�C�����̊g���q��������������Ԃ��D
	UTString Main();
	///	�t�@�C���p�X�̊g���q��Ԃ��D
	UTString Ext();

	/**	@brief �t���p�X���擾����D
		���X��΃p�X�̏ꍇ�͂��̂܂ܕԂ��D
		���΃p�X�̏ꍇ�̓J�����g�f�B���N�g����t�����ăt���p�X�ɂ���D
	 */
	UTString FullPath();

	/** @brief ���΃p�X���擾����D
		���X���΃p�X�̏ꍇ�͂��̂܂ܕԂ��D
		��΃p�X�̏ꍇ�̓J�����g�f�B���N�g���Ƃ̋��ʕ����������đ��΃p�X�ɂ���D
	 */
	UTString RelPath();

	///	�f�B���N�g�����t���p�X�ŕԂ�
	UTString FullDir();

	///	�t�@�C���������ϐ� PATH ���猟�����āC�t�@�C���p�X��ݒ�D
	bool Search(UTString file);

	UTPath(){}
	UTPath(UTString filename){ Path(filename); }
};

}	//	namespace Spr

#endif

