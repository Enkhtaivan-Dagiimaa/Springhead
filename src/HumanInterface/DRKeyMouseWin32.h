/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef DRKEYMOUSEWIN32_H
#define DRKEYMOUSEWIN32_H

#include <HumanInterface/HIVirtualDevice.h>
#include <HumanInterface/DVKeyMouse.h>
namespace Spr {;

///	DVKeyMouse
class DRKeyMouseWin32:public HIRealDevice, public 
	DRKeyMouseWin32IfInit{
public:
	SPR_OBJECTDEF(DRKeyMouseWin32, HIRealDevice);
	///	���z�f�o�C�X(KeyMouse)
	class KeyMouse:public DVKeyMouse{
	protected:
		char name[100];
		DRKeyMouseWin32* win32;
	public:
		KeyMouse(DRKeyMouseWin32* w);
		///	�f�o�C�X�̖��O
		virtual const char* Name() const{ return name; }
		///	�n���h���̐ݒ�
		virtual void SetHandler(DVKeyMouseHandler* h);
		///	�}�E�X�{�^���E�L�[�{�[�h��Ԏ擾
		virtual int GetKeyState(int key);
		///	�}�E�X�ʒu�擾�֐�	0���ŐV�C1�ȏ�͉ߋ��̗������擾
		virtual DVKeyMouseIf::DVMousePosition GetMousePosition(int count=0);
		///
		virtual HIRealDeviceIf* RealDevice(){ return win32->Cast(); }
	};
protected:
	char name[100];
	DVKeyMouseHandler* handler;
public:
	DRKeyMouseWin32();
	///	������
	virtual bool Init();
	///	���z�f�o�C�X�̓o�^
	void Register(HISdkIf* intf);
	///	�f�o�C�X�̖��O
	virtual const char* Name() const { return name; }
	///	�f�o�C�X�̎��
	static const char* TypeS() { return "KeyMouse"; }
	///	�f�o�C�X�̎��
	virtual const char* Type() const{ return TypeS(); }
	///	�n���h���̐ݒ�
	virtual void SetHandler(DVKeyMouseHandler* h){
		handler = h;
	}
	///	Win32���b�Z�[�W�����āC�}�E�X�{�^���̃C�x���g����������D
	bool PreviewMessage(void* m);
};

}	//	namespace Spr
#endif
