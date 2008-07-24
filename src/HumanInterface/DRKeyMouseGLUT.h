/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef DRKeyMouseGLUT_H
#define DRKeyMouseGLUT_H

#include <HumanInterface/HIVirtualDevice.h>
#include <HumanInterface/DVKeyMouse.h>
#include <GL/glut.h>

#ifndef GLUTCALLBACK
#define GLUTCALLBACK
#endif

namespace Spr {;

///	DVKeyMouse
class DRKeyMouseGLUT:public HIRealDevice{
public:
	SPR_OBJECTDEF(DRKeyMouseGLUT);
	///	���z�f�o�C�X(KeyMouse)
	class KeyMouse:public DVKeyMouse{
	protected:
		char name[100];
		DRKeyMouseGLUT* glut;
	public:
		KeyMouse(DRKeyMouseGLUT* w);
		///	�f�o�C�X�̖��O
		virtual const char* Name() const{ return name; }
		///	�n���h���̐ݒ�
		virtual void SetHandler(DVKeyMouseHandler* h);
		///	�}�E�X�{�^���E�L�[�{�[�h��Ԏ擾
		virtual int GetKeyState(int key);
		///	�}�E�X�ʒu�擾�֐�	0���ŐV�C1�ȏ�͉ߋ��̗������擾
		virtual DVKeyMouseIf::DVMousePosition GetMousePosition(int count=0);
		///
		virtual HIRealDeviceIf* RealDevice(){ return glut->Cast(); }
	};
protected:
	char name[100];
	DVKeyMouseHandler* handler;
	static DRKeyMouseGLUT* This;
public:
	DRKeyMouseGLUT();
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
	static void GLUTCALLBACK OnMouse(int button, int state, int x, int y);
	static void GLUTCALLBACK OnMotion(int x, int y);
	static void GLUTCALLBACK OnPassiveMotion(int x, int y);
};

}	//	namespace Spr
#endif
