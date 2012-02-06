/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_FWAPP_H
#define SPR_FWAPP_H

#include <Foundation/SprUTTimer.h>
#include <HumanInterface/SprHIKeyMouse.h>
#include <Framework/SprFWWin.h>

namespace Spr{;

struct FWSdkIf;
class FWGraphicsAdaptee;

/** @brief �A�v���P�[�V�����N���X
	Springhead�̃N���X�͊�{�I�Ɍp�������Ɏg�p����悤�ɐ݌v����Ă��邪�C
	FWApp����т��̔h���N���X�͗�O�ł���C���[�U��FWApp���邢�͂��̔h���N���X���p�����C
	���z�֐����I�[�o���C�h���邱�Ƃɂ���ēƎ��@�\����������D
 */
class FWApp : public UTRefCount, public DVKeyMouseHandler{
protected:
	static FWApp*				instance;	///< �B���FWApp�C���X�^���X
	UTRef<FWSdkIf>				fwSdk;		///< Framework SDK	
	
	// �E�B���h�E
	typedef std::vector< UTRef<FWWinIf> > Wins;
	Wins		wins;
	
	/** @brief �E�B���h�E�ɃV�[����^����
		@param win �V�[�������蓖�Ă�E�B���h�E
		win�ɁC�����̃E�B���h�E�����蓖�Ă��Ă��Ȃ��V�[�������蓖�Ă�D
		�Y������V�[����������Ȃ��ꍇ�C���邢��win�Ɋ��ɃV�[�������蓖�Ă��Ă���ꍇ�͉������Ȃ��D
	*/
	void	AssignScene(FWWinIf* win);

public:
	FWApp();
	virtual ~FWApp();

// �h���N���X�Œ�`����K�v�����鉼�z�֐� -----------------------------

	/** @brief ������
		FWApp�I�u�W�F�N�g�̏��������s���D�ŏ��ɕK���ĂԁD
	 */
	virtual void Init(int argc = 0, char* argv[] = NULL);

	/** @brief �V�[���̕`��
		�V�[�����\�������Ƃ��ɌĂ΂��D
		�`�揈�����J�X�^�}�C�Y�������ꍇ�ɃI�[�o���C�h����D
		�f�t�H���g�ł�FWSdk::Draw���Ă΂��D
	 */
	virtual void Display();

	/** @brief IdleFunc�̌Ăяo����L����/����������
		glut�̏ꍇ�CglutIdleFunc�ɑΉ�����D

		����ʐ����������A�����Ƃ��킹�邽�߂�EnableIdleFunc��ǉ����܂����B
		DisableIdleFunc���c���Ă��܂���obsolete�Ƃ������Ǝv���܂��B
	 */
	void DisableIdleFunc(){ EnableIdleFunc(false); }
	void EnableIdleFunc(bool on = true);
	
	/** @brief ���C�����[�v�̎��s
		glut�̏ꍇ�CglutmainLoop�̎��s
	 */
	void StartMainLoop();

// �h���N���X�Œ�`���邱�Ƃ̂ł��鉼�z�֐� -----------------------------
	/** @brief ���[�U�֐�
		���C�����[�v���s���Ƀ��[�U�����������������ꍇ�C�����ɋL�q����D
		�����V�~�����[�V�����̃X�e�b�v�̑O�ɌĂ΂��D
	 */
	virtual void UserFunc(){}

	/** @brief �A�C�h������
		�C�x���g�������Ȃ��ꍇ�Ƀo�b�N�O���E���h�������s���D
	 */
	virtual void IdleFunc(){}

	/** @brief �^�C�}�[����
		�J��Ԃ����s���s���D
		�f�t�H���g�ł�id = 0�ŃR�[���o�b�N
	 */
	virtual void TimerFunc(int id);

	/** @brief �`��̈�̃T�C�Y�ύX
		@param w �`��̈�̉���
		@param h �`��̈�̏c��
		���[�U�ɂ���ăE�B���h�E�T�C�Y���ύX���ꂽ�Ƃ��ȂǂɌĂ΂��D
	 */
	virtual void Reshape(int w, int h);

	/** @brief �L�[�{�[�h�C�x���g�̃n���h��
		@param key �L�[�R�[�h
		@param x �J�[�\�������W
		@param y �J�[�\�������W
		����L�[�̃L�[�R�[�h�͎g�p����O���t�B�N�X���C�u�����Ɉˑ�����D
		
		��GLUT�̏ꍇ�CglutKeyboardFunc�R�[���o�b�N��glutSpecialFunc�R�[���o�b�N��
		�L�[�R�[�h���d�����Ă���D������glutSpecialFunc�R�[���o�b�N�ɓn���ꂽ�L�[�R�[�h�ɂ�
		256�����Z�����Keyboard�ɓn�����D
	 */
	virtual void Keyboard(int key, int x, int y){}

	/** @brief �}�E�X�C�x���g�̃n���h��
	 */
	virtual void MouseButton(int button, int state, int x, int y);

	/** @brief �}�E�X�C�x���g�̃n���h��
		�f�t�H���g�ł͍��h���b�O�Ŏ��_�ύX�C�E�h���b�O�ŃY�[���C��/�A�E�g
	 */
	virtual void MouseMove(int x, int y);

	/** @brief �W���C�X�e�B�b�N�̃n���h��
	*/
	virtual void Joystick(unsigned int buttonMask, int x, int y, int z){}

	/** @brief �I�������n���h��
	 */
	virtual void AtExit(){}

	/** DVKeyMouseHandler�̉��z�֐�
		�f�t�H���g�ł�FWApp�̏]���̃C�x���g�n���h�����Ăт����̂݁D
		������𒼐ڃI�[�o���C�h���Ă��悢�D
	 **/
	virtual bool OnMouse(int button, int state, int x, int y){
		MouseButton(button, state, x, y);
		return true;
	}
	virtual bool OnMouseMove(int state, int x, int y, int zdelta){
		MouseMove(x, y);
		return true;
	}
	virtual bool OnDoubleClick(int button, int x, int y){
		return false;
	}
	virtual bool OnKey(int state, int key, int x, int y){
		Keyboard(key, x, y);
		return true;
	}

	/** @brief GUI�n�C�x���g�n���h��
		@param ctrl	�X�V���������R���g���[��
		�X�V�C�x���g�̔��������͈ȉ��̂Ƃ���F
		FWButtonIf:				�{�^���������ꂽ
		FWTextBoxIf:			�e�L�X�g���ύX���ꂽ
		FWRotationControlIf:	��]����
		FWTranslationControlIf:	���s�ړ�����
	 **/
	virtual void OnControlUpdate(FWControlIf* ctrl){}

	//�@FWApp�̃C���^�t�F�[�X -----------------------------------------

	/** @brief FWApp�C���X�^���X���擾���� */
	static FWApp* GetApp(){ return instance; }

	/** @brief SDK���擾����
	*/
	FWSdkIf*	GetSdk(){ return fwSdk; }

	/** @brief SDK���쐬����
	 */
	void		CreateSdk();

	/** @brief �E�B���h�E�ɑΉ�����R���e�L�X�g�����
		@param desc		�f�B�X�N���v�^
		@param parent	�e�E�B���h�E
		�E�B���h�E���쐬����D�Ή����郌���_���͐V�����쐬����C
		�����̃E�B���h�E�����蓖�Ă��Ă��Ȃ��V�[�����֘A�Â�����D
		parent���w�肳�ꂽ�ꍇ��parent��e�E�B���h�E�Ƃ���q�E�B���h�E���쐬����
	 */
	FWWinIf*	CreateWin(const FWWinDesc& desc = FWWinDesc(), FWWinIf* parent = 0);
	/** @brief �E�B���h�E�̐� */
	int			NWin(){ return (int)wins.size(); }
	
	/**	@brief �E�B���h�E��ID����T��
		@param wid �E�B���h�EID
		glut�̏ꍇ�Cwid��glutGetWindow���Ԃ����ʎq�D
	*/
	FWWinIf*	GetWinFromId(int wid);

	/** @brief �E�B���h�E���擾����
		@param index ���Ԗڂ̃E�B���h�E���擾���邩
		index�Ŏw�肳�ꂽ�E�B���h�E���擾����D
		DestroyWin���ĂԂƃC���f�b�N�X�͕ω�����̂Œ��ӂ��K�v�D
	 */
	FWWinIf*	GetWin(int index);

	/** @brief ���݂̃E�B���h�E���擾����
	*/
	FWWinIf*	GetCurrentWin();

	/** @brief �E�B���h�E���폜����
	 */
	void DestroyWin(FWWinIf* win);

	/** @brief ���݂̃E�B���h�E��ݒ肷��
	 */
	void SetCurrentWin(FWWinIf* win);

	/** @brief �J�����g�E�B���h�E�̃m�[�}���v���[�����C�ĕ`��̕K�v�ɉ����ă}�[�N����
	 */
	void PostRedisplay();

	/** @brief Ctrl, Shift, Alt�̏�Ԃ�Ԃ�
		�X�̔h���N���X�Ŏ��������
	 */
	int	GetModifier();
	
	enum{
		TypeNone,	///< �A�_�v�^����
		TypeGLUT,	///< GLUT
		TypeGLUI,	///< GLUI
	};
	/** @brief �`��̐ݒ�
		FWGraphicsAdaptee��ݒ肷��D�ŏ��ɕK���ĂԁD
	 */
	void SetGRAdaptee(int type);

	/** @brief FWGraphicsAdaptee�̏�����
		FWGraphicsAdaptee�̏��������s���D�ŏ��ɕK���ĂԁD
	 */
	/*	SetGRAdaptee��GRInit�𕪗�����Ӗ����قƂ�ǖ����̂ŁC
		������̈����Ń^�C�v���w��ł���悤�ɂ����D
		GRInit�ɐ旧����SetGRAdaptee�őI������Ă���ꍇ�͂������D�悷��

		tazz
	 */
	void GRInit(int argc = 0, char* argv[] = NULL, int type = TypeGLUT);

public:
	// �^�C�}�[
	typedef std::vector< UTRef<UTTimerIf> > Timers;
	Timers timers;

	/** @brief �^�C�}�[���쐬����
		@param	mode	�^�C�}�̎��
		@return			�^�C�}�I�u�W�F�N�g
	 */
	UTTimerIf* CreateTimer(UTTimerIf::Mode mode = UTTimerIf::FRAMEWORK);

	/** @breif �^�C�}�[���擾����
		@param �^�C�}�[�ԍ�
		@return �^�C�}�[�I�u�W�F�N�g
	*/
	UTTimerIf* GetTimer(int i);
	int	NTimers(){ return (int)timers.size(); }

public:
	/**  �폜���API  **/
	/// �E�B���h�E��1�����쐬
	void	InitWindow(){ if(!NWin()) CreateWin(); }
	/// ������Ԃɂ���
	void	Clear(){}
	/// ���Z�b�g����
	void	Reset(){}

};

}
#endif
