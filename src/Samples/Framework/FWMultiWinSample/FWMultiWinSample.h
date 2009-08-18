/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
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
//	virtual void Reset();
	static void _cdecl CallStep(int id);
	virtual void Step();
	virtual void Display();
	virtual void DebugDisplay(bool bDebug);
	void InitCameraView();
	void Keyboard(int key, int x, int y);	
};

#endif