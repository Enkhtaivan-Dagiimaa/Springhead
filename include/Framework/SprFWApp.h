/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_FWAPP_H
#define SPR_FWAPP_H

#include <Springhead.h>
#include <Framework/SprFWInteractScene.h>
#include <Framework/SprFWGraphicsAdaptee.h>
#include <Framework/SprFWAppInfo.h>
#include <map>
#include <vector>
#include <GL/glut.h>


namespace Spr{;

/** @brief �A�v���P�[�V�����N���X
	Springhead�̃N���X�͊�{�I�Ɍp�������Ɏg�p����悤�ɐ݌v����Ă��邪�C
	FWApp����т��̔h���N���X�͗�O�ł���C���[�U��FWApp���邢�͂��̔h���N���X���p�����C
	���z�֐����I�[�o���C�h���邱�Ƃɂ���ēƎ��@�\����������D
 */
class FWApp : public UTRefCount{
private:
	UTRef<FWSdkIf>			fwSdk;		///< Framework SDK
	UTRef<HISdkIf>			hiSdk;		///< HumanInterface SDK
public:
	FWApp();
	virtual ~FWApp();

protected:
	FWMouseInfo				mouseInfo;		///< �}�E�X���
	FWUICamera				cameraInfo;		///< �J�������
	FWDragger				dragInfo;		///< ���̃h���b�O���

	Wins		wins;				///< �E�B���h�E���
	
	/** @brief �E�B���h�E�ɃV�[����^����
		@param win �V�[�������蓖�Ă�E�B���h�E
		win�ɁC�����̃E�B���h�E�����蓖�Ă��Ă��Ȃ��V�[�������蓖�Ă�D
		�Y������V�[����������Ȃ��ꍇ�C���邢��win�Ɋ��ɃV�[�������蓖�Ă��Ă���ꍇ�͉������Ȃ��D
	*/
	void	AssignScene(FWWin* win);

public:
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

	/** @brief �A�C�h������
		�C�x���g�������Ȃ��ꍇ�Ƀo�b�N�O���E���h�������s���D
	 */
	virtual void IdleFunc(){}

	/** @brief �^�C�}�[����
		�J��Ԃ����s���s���D
	 */
	virtual void TimerFunc(int id);

	/** @brief �`��̈�̃T�C�Y�ύX
		@param w �`��̈�̉���
		@param h �`��̈�̏c��
		���[�U�ɂ���ăE�B���h�E�T�C�Y���ύX���ꂽ�Ƃ��ȂǂɌĂ΂��D
		�f�t�H���g�ł�FWSdk::Reshape���Ă΂��D
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

//�@FWApp�̃C���^�t�F�[�X -----------------------------------------

	/** @brief SDK���擾����
	*/
	FWSdkIf*	GetSdk(){ return fwSdk; }

	/** @brief SDK���쐬����
	*/
	void		CreateSdk();

	/** @brief �E�B���h�E�ɑΉ�����R���e�L�X�g�����
		@param desc	�f�B�X�N���v�^
		�E�B���h�E���쐬����D�Ή����郌���_���͐V�����쐬����C
		�����̃E�B���h�E�����蓖�Ă��Ă��Ȃ��V�[�����֘A�Â�����D
	 */
	FWWin*		CreateWin(const FWWinDesc& desc = FWWinDesc());
	
	/** @brief �E�B���h�E�̏�����
		�E�B���h�E��1�쐬���C����ɃA�N�e�B�u�V�[�������蓖�Ă�ȈՊ֐��D
		����1�ȏ�̃E�B���h�E������ꍇ�͉������Ȃ��D
		AssignScene(CreateWin())�Ɠ����D
		�������Ă��悢���H
	 */
	void		InitWindow();

	/** @brief �E�B���h�E�̐� 
	*/
	int			NWin(){ return (int)wins.size(); }

	/**	@brief �E�B���h�E��ID����T��
		@param wid �E�B���h�EID
		glut�̏ꍇ�Cwid��glutGetWindow���Ԃ����ʎq�D
	*/
	FWWin*		GetWinFromId(int wid);

	/** @brief �E�B���h�E���擾����
		@param index ���Ԗڂ̃E�B���h�E���擾���邩
		index�Ŏw�肳�ꂽ�E�B���h�E���擾����D
		DestroyWin���ĂԂƃC���f�b�N�X�͕ω�����̂Œ��ӂ��K�v�D
	 */
	FWWin*		GetWin(int index);

	/** @brief ���݂̃E�B���h�E���擾����
	*/
	FWWin*		GetCurrentWin();

	/** @brief �E�B���h�E���폜����
	 */
	void DestroyWin(FWWin* win);

	/** @brief ���݂̃E�B���h�E��ݒ肷��
	 */
	void SetCurrentWin(FWWin* win);

	/** @brief �J�����g�E�B���h�E�̃m�[�}���v���[�����C�ĕ`��̕K�v�ɉ����ă}�[�N����
	 */
	void PostRedisplay();

	/// �O������擾����K�v�͂��邩�H�@tazz
	/** @brief �J��������Ԃ�
		@return camInfo
	*/
	FWUICamera*	GetCameraInfo(){return &cameraInfo;}

	// �폜���
	/// ������Ԃɂ���
	void	Clear(){}
	/// ���Z�b�g����
	void	Reset(){}

	/** @brief Ctrl, Shift, Alt�̏�Ԃ�Ԃ�
		�X�̔h���N���X�Ŏ��������
	 */
	int	GetModifier();


// ------------------------------------------------------------------------
	
/** �`��p�[�g */
private:
	UTRef<FWGraphicsAdaptee>	grAdaptee;	//�O���t�B�N�X�V�X�e���̃A�_�v�e�B
public:
	enum grAdapteeType{
		TypeNone,	///< �A�_�v�^����
		TypeGLUT,	///< GLUT
		TypeGLUI,	///< GLUI
	};
	/** @brief �`��̐ݒ�
		FWGraphicsAdaptee��ݒ肷��D�ŏ��ɕK���ĂԁD
	 */
	void SetGRAdaptee(grAdapteeType type);

	/** @brief �`��̐ݒ���擾
		FWGraphicsAdaptee���擾����D�@
	 */
	FWGraphicsAdaptee* GetGRAdaptee(){return grAdaptee;}; //�����I�ɂ͍폜������

	/** @brief FWGraphicsAdaptee�̏�����
		FWGraphicsAdaptee�̏��������s���D�ŏ��ɕK���ĂԁD
	 */
	void GRInit(int argc = 0, char* argv[] = NULL);

/** �R�[���o�b�N�֐�*/
public:
	void CallDisplay();
	void CallReshape(int w, int h);
	void CallIdleFunc();
	void CallKeyboard(int key, int x, int y);
	void CallMouseButton(int button, int state, int x, int y);
	void CallMouseMove(int x, int y);
	void CallJoystick(unsigned int buttonMask, int x, int y, int z);

/** FWInteraction */
///////////////////////////////////////////////////////////////////////////////////
protected:
	FWInteractSceneIf*					curIAScene;
	FWInteractScenes					iaScenes;
	std::vector<UTRef < HIBaseIf > >	humanInterfaces;
public:
	void							CreateHISdk();
	HISdkIf*						GetHISdk();
	void							AddHI(HIBaseIf* hi);
	HIBaseIf*						GetHI(int i);
	FWInteractSceneIf*				CreateIAScene(const FWInteractSceneDesc& desc);
	FWInteractSceneIf*				GetIAScene(int i = -1);
	int								NIAScenes();
	void							ClearIAScenes();
///////////////////////////////////////////////////////////////////////////////////

public:

	/** @brief �^�C�}�[���쐬����
		@param	mode	�^�C�}�̎��
		@return			�^�C�}�I�u�W�F�N�g
	 */
	UTTimerIf*	CreateTimer(UTTimerIf::Mode mode = UTTimerIf::FRAMEWORK);

};

}
#endif
