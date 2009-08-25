/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

/*
	�V�~�����[�V�����̕����E�B���h�E�\��
	�V�[����Graphic���f����Physic���f����2��ʂɕ\��
	q,ESC	: �A�v���P�[�V�����I��
	r		: �t�@�C���̍ă��[�h	
*/

#ifndef FWMULTIWIN_SAMPLE_H
#define FWMULTIWIN_SAMPLE_H

#include <Framework/SprFWApp.h>
#include <Springhead.h>
#include <vector>

using namespace Spr;

class FWMultiWinSample : public FWApp{
private:
	int				nWin;			//< ���E�B���h�E�̐�
	string			fileName;		//< �ǂݍ��ރt�@�C���̖��O
	vector<string>	winNames;		//< Window�̖��O
public:
	FWMultiWinSample();	
	virtual void Init(int argc, char* argv[]);
	virtual void InitCameraView();
	virtual void Start();
	virtual void Reset();
	static void _cdecl CallBack(int id);
	virtual void Step();
	virtual void Display();
	void DebugDisplay(bool bDebug);
	virtual void Keyboard(int key, int x, int y);	
};

#endif