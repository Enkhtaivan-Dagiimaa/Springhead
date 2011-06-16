#ifndef SPR_FW_GRAPHICSADAPTEE_H
#define SPR_FW_GRAPHICSADAPTEE_H

#include <Framework/SprFWWin.h>

namespace Spr{;

struct GRDeviceIf;

struct FWGraphicsAdapteeDesc{};

/** FWGraphicsAdaptee
	�����n�Ɉˑ������E�B���h�E�̍쐬�E�Ǘ��@�\
	���@�\���l�����FWWindowManager�̕��������C������ tazz
 */

struct FWGraphicsAdapteeIf : ObjectIf{	
	SPR_IFDEF(FWGraphicsAdaptee);

	///	���������s���B�ŏ��ɂ�����ĂԕK�v������B
	// �� char* argv[]�Ƃ���������swig�����Ή��̖͗l
	void Init(int argc = 0, char** argv = NULL);
	
	/// mainloop���ĂсC�^�C�}�[���X�^�[�g����
	void StartMainLoop();

	/// Idle callback�̗L����/������
	void EnableIdleFunc(bool on = true);

	/// �t���X�N���[�����[�h�ւ̐؂�ւ��DAPI���͎b��I��GLUT�ɏ���
	void EnterGameMode();
	void LeaveGameMode();

	/** �E�B���h�E */
	///	�E�B���h�E���쐬���A�E�B���h�E ID��Ԃ�
	FWWinIf* CreateWin(const FWWinDesc& d);
	///	�E�B���h�E��j������
	void DestroyWin(FWWinIf* w);
	///	�J�����g�E�B���h�E��ݒ肷��
	void SetCurrentWin(FWWinIf* w);
	///	�J�����g�E�B���h�E��Ԃ��B
	FWWinIf* GetCurrentWin();
	///	�J�����g�E�B���h�E��Ԃ��B
	int GetWinFromId();
	/// �J�����g�E�B���h�E�̃m�[�}���v���[�����C�ĕ`��̕K�v�ɉ����ă}�[�N����
	void PostRedisplay();
	/// Shift,Ctrl,Alt�̃X�e�[�g��Ԃ�
	int GetModifiers();

	/// �O���t�B�N�X�f�o�C�X�̎擾
	GRDeviceIf*	GetGRDevice();
};

}

#endif