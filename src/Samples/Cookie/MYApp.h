/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef MYAPP_H
#define MYAPP_H


#include <Springhead.h>
#include <SprFramework.h>
#include <Framework/SprFWAppGLUT.h>

#include <GL/glut.h>
#include <vector>
#include <windows.h>

using namespace Spr;

/** @brief MYApp�N���X��FWAppGLUT�܂łŒ�`����Ă���֐�������
		   �I�[�o�[���C�h���Ďg�����߂̃N���X
*/
class MYApp : public FWAppGLUT, public UTRefCount{

public:
	//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	//�����o�ϐ�
	FWWin*								window;			//< �\������E�B���h�E�̃n���h��
	FWSceneIf*							fwScene;		//< FWScene(= PHScene + GRScene)�̃C���X�^���X
	PHSceneIf*							phScene;		//< PHScene�̃C���X�^���X
	GRRenderIf*							grRender;		//< GRRender�̃C���X�^���X
	GRDeviceGLIf*						deviceGL;		//< GL�̃f�o�C�X�n���h��
	float								camHeight;		//< �J�����̍���(TopView)
	float								camTopAngle;	//< TopView�̃J�����p�x
	double								dt;				//< �ϕ���
	double								nIter;			//< LCP������
	bool								bStep;			//< �V�~�����[�V�������s�����ǂ���
	bool								flagDebugMode;	//< �V�[�����f�o�b�O���[�h�ŕ`�悷�邩�̃t���O
	const char*							filePass;		//< xFile���u���Ă���t�H���_�ւ̑��΃p�X
	const char*							fileName;		//< ���[�h����t�@�C����
	const char*							phCookieName;	//< �N�b�L�[�̏Փ˔���`��̖��O
	const char*							grCookieName;	//< �N�b�L�[�̌����ڂ̖��O
	std::vector< UTRef< FWObjectIf > >	copiedCookies;	//< �R�s�[���ꂽ�N�b�L�[���i�[����z��

	//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	//�R���X�g���N�^�E�f�X�g���N�^
	MYApp();

	//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	//�V������`�����֐��̐錾
	static MYApp* GetInstance(){
		return (MYApp*) instance;
	}
	bool CopyTheCookie(const char* phName, const char* grName);		//< //�����̖��O�̍��̂�T���Ă��āC�����f�[�^�̂��̂𕡐�����֐�
	void ResetScene();
	//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	//�I�[�o�[���C�h
	//FW�Ɋւ���֐�
	void Init(int argc, char* argv[]);				//< �������֐��̃I�[�o�[���C�h�錾
	void Step();									//< ���X�e�b�v�V�~�����[�V�����Ƃ��ČĂ΂��֐��̃I�[�o�[���C�h�錾
	//GLUT����̊֐�
	void Keyboard(unsigned char key, int x, int y);	//< �L�[�{�[�h�C�x���g����������֐��̃I�[�o�[���C�h�錾
	void Display();									//< �`��C�x���g����������֐��̃I�[�o�[���C�h�錾
};

#endif