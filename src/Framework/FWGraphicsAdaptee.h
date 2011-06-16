#ifndef FW_GRAPHICSADAPTEE_H
#define FW_GRAPHICSADAPTEE_H

#include <Framework/SprFWGraphicsAdaptee.h>
#include <Foundation/Object.h>

namespace Spr{;

class FWApp;

class FWGraphicsAdaptee : public Object, public FWGraphicsAdapteeDesc{
public:
	SPR_OBJECTDEF_ABST(FWGraphicsAdaptee);
	FWApp*			fwApp;			///< �A�v���P�[�V�����N���X�ւ̎Q��
	GRDeviceIf*		grDevice;		///< �O���t�B�N�X�f�o�C�X�ւ̎Q��
public:
	///	���������s���B�ŏ��ɂ�����ĂԕK�v������B
	virtual void Init(int argc = 0, char** argv = NULL){}
	
	/// mainloop���ĂсC�^�C�}�[���X�^�[�g����
	virtual void StartMainLoop()=0;

	/// Idle callback�̗L����/������
	virtual void EnableIdleFunc(bool on = true)=0;

	/// �t���X�N���[�����[�h�ւ̐؂�ւ��DAPI���͎b��I��GLUT�ɏ���
	virtual void EnterGameMode()=0;
	virtual void LeaveGameMode()=0;

	/** �E�B���h�E */
	///	�E�B���h�E���쐬���A�E�B���h�E ID��Ԃ�
	virtual FWWinIf* CreateWin(const FWWinDesc& d){return NULL;};
	///	�E�B���h�E��j������
	virtual void DestroyWin(FWWinIf* w){};
	///	�J�����g�E�B���h�E��ݒ肷��
	virtual void SetCurrentWin(FWWinIf* w){};
	///	�J�����g�E�B���h�E��Ԃ��B
	virtual FWWinIf* GetCurrentWin(){return NULL;};
	///	�J�����g�E�B���h�E��Ԃ��B
	virtual int GetWinFromId(){return NULL;};
	/// �J�����g�E�B���h�E�̃m�[�}���v���[�����C�ĕ`��̕K�v�ɉ����ă}�[�N����
	virtual void PostRedisplay(){};
	/// Shift,Ctrl,Alt�̃X�e�[�g��Ԃ�
	virtual int GetModifiers(){return NULL;};

	GRDeviceIf* GetGRDevice(){ return grDevice; }

	FWGraphicsAdaptee(FWApp* a){ fwApp = a; }
};

}

#endif