/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef MYAPP_H
#define MYAPP_H

#include <Framework/SprFWAppGLUT.h>
#include <Springhead.h>
#include <cstdlib>
#include <vector>

using namespace Spr;
using namespace std;

class MYApp : public FWAppGLUT, public UTRefCount{
	
private:
	double					dt;				//< �ϕ���
	double					nIter;			//< LCP������
	bool					bTimer;

	int								numWindow;		//< ���E�B���h�E�̐�
	vector< UTRef< FWWin > > 		windows;		//< ������E�B���h�E
	vector< UTRef< FWSceneIf > >	fwScenes;		//< Physics�̃V�[���O���t
	vector< string >				winNames;		//< Window�̖��O
	vector< string >				fileNames;		//< �ǂݍ��ރt�@�C���̖��O

	vector< Affinef	>				views;			//< �J�����ʒu�̉�]�E�Y�[�����s�����̍s��p�̕ϐ�

public:
	
	// �R���X�g���N�^
	MYApp();

	// ���̃N���X���ŐV�����錾����֐�
	void NumOfClassMembers(std::ostream& out);

	// �C���^�t�F�[�X�֐�
	double	GetTimeStep()					{return dt;}
	void	SetTimeStep(double data)		{dt = data;}
	double	GetNumIteration()				{return nIter;}
	void	SetNumIteration(double data)	{nIter = data;}
	
	// ���̃N���X�Ő錾���ꂽ�֐��̂����I�[�o�[���[�h����֐�
	void Init(int argc, char* argv[]);
	void Keyboard(int key, int x, int y);
	void Display();
	void Step();
	
};

#endif