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
	UTRef<FWSdkIf>						fwSdk;
	UTRef<HISdkIf>						hiSdk;	
public:
	virtual ~FWApp();
	Wins								wins;		///< �E�B���h�E���
	UTRef<FWVFuncBridge>				vfBridge;	///< ������(Ruby�Ȃ�)�փ|�[�g����ۂɉ��z�֐����K�؂ɌĂ΂��悤�ɂ��邽�߂̃u���b�W

protected:
	MouseInfo							mouseInfo;	///< �}�E�X���
	CameraInfo							cameraInfo;	///< �J�������
	std::map<FWSceneIf*, DragInfo>		dragInfo;	///< ���̃h���b�O���

public:
// �h���N���X�Œ�`����K�v�����鉼�z�֐� -----------------------------

	/** @brief ������
		FWApp�I�u�W�F�N�g�̏��������s���D�ŏ��ɕK���ĂԁD
	 */
	virtual void Init(int argc = 0, char* argv[] = NULL)=0;

	/** @brief �V�[���̕`��
		�V�[�����\�������Ƃ��ɌĂ΂��D
		�`�揈�����J�X�^�}�C�Y�������ꍇ�ɃI�[�o���C�h����D
		�f�t�H���g�ł�FWSdk::Draw���Ă΂��D
	 */
	virtual void Display()=0;

// �h���N���X�Œ�`���邱�Ƃ̂ł��鉼�z�֐� -----------------------------

	/** @brief �A�C�h������
		�C�x���g�������Ȃ��ꍇ�Ƀo�b�N�O���E���h�������s���D
	 */
	virtual void IdleFunc(){}

	/** @brief �^�C�}�[����
		�J��Ԃ����s���s���D
	 */
	virtual void TimerFunc(int id){}
	
	/** @brief ���C�����[�v�̎��s
		glut�̏ꍇ�CglutIdleFunc,glutmainLoop�̎��s
	 */
	virtual void StartMainLoop();

	/** @brief �`��̈�̃T�C�Y�ύX
		@param w �`��̈�̉���
		@param h �`��̈�̏c��
		���[�U�ɂ���ăE�B���h�E�T�C�Y���ύX���ꂽ�Ƃ��ȂǂɌĂ΂��D
		�f�t�H���g�ł�FWSdk::Reshape���Ă΂��D
	 */
	virtual void Reshape(int w, int h);

	/** @brief �L�[�{�[�h�C�x���g�̃n���h��
	 */
	virtual void Keyboard(int key, int x, int y){}

	/** @brief �L�[�{�[�h�C�x���g�̃n���h��
		Keyboard�֐��̂͂��߂ɌĂ�
	 */
	void BeginKeyboard();

	/** @brief �L�[�{�[�h�C�x���g�̃n���h��
		Keyboard�֐��̏I���ɌĂ�
	 */
	void EndKeyboard();

	/** @brief �}�E�X�C�x���g�̃n���h��
	 */
	virtual void MouseButton(int button, int state, int x, int y);

	/** @brief �}�E�X�C�x���g�̃n���h��
		�f�t�H���g�ł͍��h���b�O�Ŏ��_�ύX�C�E�h���b�O�ŃY�[���C��/�A�E�g
	 */
	virtual void MouseMove(int x, int y);

	/** @brif �W���C�X�e�B�b�N�̃n���h��
	*/
	virtual void Joystick(unsigned int buttonMask, int x, int y, int z){}


//�@FWApp�̃C���^�t�F�[�X -----------------------------------------

	/** @brief SDK���擾����
	*/
	FWSdkIf*	GetSdk(){ return fwSdk; }

	/** @brief SDK���쐬����
	*/
	void		CreateSdk();

	/** @brief window�ɃV�[����^����
	*/
	void		AssignScene(FWWin* win);

	/** @brief �E�B���h�E�ɑΉ�����R���e�L�X�g�����
		@param desc	�f�B�X�N���v�^
		�E�B���h�E���쐬����D�Ή����郌���_���͐V�����쐬����C
		�V�[���̓A�N�e�B�u�ȃV�[�����֘A������D
	 */
	FWWin*		CreateWin(const FWWinDesc& desc = FWWinDesc());
	
	/** @brief �E�B���h�E�̏�����
	 */
	void		InitWindow();

	/** @brief �E�B���h�E�̐� 
	*/
	int			NWin(){ return (int)wins.size(); }

	/**	@brief �E�B���h�E��ID����T��
	*/
	FWWin*		GetWinFromId(int wid);

	/** @brief �E�B���h�E���擾����
		index�Ŏw�肳�ꂽ�E�B���h�E���擾����D
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

	/** @brief �J��������Ԃ�
		@return camInfo
	*/
	CameraInfo	GetCameraInfo(){return cameraInfo;}

	/** @brief Ctrl, Shift, Alt�̏�Ԃ�Ԃ�
		�X�̔h���N���X�Ŏ��������
	 */

	virtual int	GetModifier();

// ------------------------------------------------------------------------
	
/** �`��p�[�g */
private:
	UTRef<FWGraphicsAdaptee>	grAdaptee;	//�O���t�B�N�X�V�X�e���̃A�_�v�e�B
public:
	enum grAdapteeType{
		TypeGLUT,
		TypeGLUI,
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
	static FWApp* instance;
	void CallDisplay();
	void CallReshape(int w, int h);
	void CallTimerFunc(int id);
	void CallIdleFunc();
	void CallKeyboard(int key, int x, int y);
	void CallMouseButton(int button, int state, int x, int y);
	void CallMouseMove(int x, int y);
	void CallJoystick(unsigned int buttonMask, int x, int y, int z);

/** FWInteraction */
///////////////////////////////////////////////////////////////////////////////////
public:
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


/** �^�C�}�@*/
protected:
	vector<UTRef<FWTimer> > fwTimers;
public:

	/** @brief �^�C�}�[���쐬����
		�ŏ��ɍ쐬���ꂽ�^�C�}��ID��0�D���̌��1����������DReleaseTimer�ɂ���Ċ����̃^�C�}��ID���ω����邱�Ƃ͖����D
		�^�C�}�����̏����l��0�D������0�̏ꍇ�̓A�N�e�B�u�ȃV�[����time step���^�C�}�����ƂȂ�D
		�قȂ������ݒ肷��ꍇ��FWTimer::SetInterval���g�p����D
	 */
	int CreateTimer(FWTimer::TimerType t = FWTimer::GLUT);

	/** @brief �^�C�}�[�̎��ԊԊu�̐ݒ�
	*/
	void SetInterval(int id, unsigned ms);	
	/** @brief �^�C�}�[�̎��ԊԊu�̎擾
	*/
	unsigned GetInterval(int id);
	/** @brief �^�C�}�[�̕���\�̐ݒ�
	*/
	void SetResolution(int id, unsigned r);	
	/** @brief �^�C�}�[�̕���\�̎擾
	*/
	unsigned GetResolution(int id);
	/** @brief �^�C�}�[���������
	*/
	void ReleaseTimer(int id);
	/** @brief �^�C�}�[���č\������
	*/
	void RecreateTimer(int id);
	/** @brief �S�Ẵ^�C�}�[���������
	*/
	void ReleaseAllTimer();
	/** @brief �S�Ẵ^�C�}�[���č\������
	*/
	void CreateAllTimer();
	/** @brief �S�Ẵ^�C�}�[���폜����
	*/
	void ClearAllTimer();

	/** @brief �^�C�}�[���擾���� */
	FWTimer* GetTimer(int id);
};

}
#endif
