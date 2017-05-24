/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_FWCONSOLEDEBUGMONITOR_H
#define SPR_FWCONSOLEDEBUGMONITOR_H

#include <Framework/SprFWApp.h>

namespace Spr{;
/** \addtogroup gpFramework */
//@{

class FWConsoleDebugMonitor {
	std::string line;						//	�R���\�[�����͂̍s
	std::vector<std::string> lineHistory;	//	���͗���
	int lineHistoryCur;						//	���݂̈ʒu
public:
	FWConsoleDebugMonitor();
	///	Idel�ŌĂяo���ƃR���\�[���L�[���͂��`�F�b�N����B�u���b�N���Ȃ��B
	virtual void KeyCheck();
	///	�L�[���͂���������
	virtual bool ProcessKey(int key);
	///	�R�}���h���s
	virtual bool ExecCommand(std::string cmd, std::string arg, std::vector<std::string> args);
	///	�R�}���h�̌��(i=0 �R�}���h, i=1.. arg1...)�I�[�o���C�h�p
	virtual const std::vector<std::string>& Candidates(int i) {
		static std::vector<std::string> rv;
		return rv;
	}
};

//@}
}

#endif SPR_FWCONSOLEDEBUGMONITOR_H
