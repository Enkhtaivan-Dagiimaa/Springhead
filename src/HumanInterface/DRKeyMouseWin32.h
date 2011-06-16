/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef DRKEYMOUSEWIN32_H
#define DRKEYMOUSEWIN32_H

#include <HumanInterface/HIDevice.h>

namespace Spr {;

///	DVKeyMouse
class DRKeyMouseWin32: public HIRealDevice{
public:
	SPR_OBJECTDEF(DRKeyMouseWin32);
	///	���z�f�o�C�X(KeyMouse)
	class DV: public DVKeyMouse{
	public:
		DV(DRKeyMouseWin32* dr, int ch):DVKeyMouse(dr, ch){}
		DRKeyMouseWin32* GetRealDevice(){ return realDevice->Cast(); }

		///	�}�E�X�{�^���E�L�[�{�[�h��Ԏ擾
		virtual int GetKeyState(int key){ return GetRealDevice()->GetKeyState(key); }
		///	�}�E�X�ʒu�擾�֐�	0���ŐV�C1�ȏ�͉ߋ��̗������擾
		virtual void GetMousePosition(int& x, int& y, int& time, int count=0){
			return GetRealDevice()->GetMousePosition(x, y, time, count);
		}
	};
public:
	DRKeyMouseWin32(const DRKeyMouseWin32Desc& desc = DRKeyMouseWin32Desc());
	///	������
	virtual bool Init();
	///	���z�f�o�C�X�̓o�^
	//virtual void Register(HISdkIf* intf);
	virtual HIVirtualDeviceIf*	Rent(const IfInfo* ii, const char* name, int portNo);

	/// windows���z�L�[�R�[�h����Springhead���ʃL�[�R�[�h�ւ̕ϊ�
	int	ConvertKeyCode(int key, bool spr_to_win32);
	
	int		GetKeyState(int key);
	void	GetMousePosition(int& x, int& y, int& time, int count);

	///	Win32���b�Z�[�W�����āC�}�E�X�{�^���̃C�x���g����������D
	bool PreviewMessage(void* m);
};

}	//	namespace Spr
#endif
