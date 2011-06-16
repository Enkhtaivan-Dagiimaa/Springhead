/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef DRKeyMouseGLUT_H
#define DRKeyMouseGLUT_H

#include <HumanInterface/HIDevice.h>
#include <GL/glut.h>
#include <deque>

#ifndef GLUTCALLBACK
#define GLUTCALLBACK
#endif

namespace Spr {;

/** GLUT�ŃL�[�{�[�h�E�}�E�X�̎���

	�C�x���g�����̗���F
	�Estatic�̃R�[���o�b�N��glut�ɓo�^����
	�Estatic�R�[���o�b�N������f�o�C�X�̃C���X�^���X�i�V���O���g���j�̃R�[���o�b�N���Ă�
	�E���}�E�X�̃R�[���o�b�N����C�|�[�g�ԍ��ƃJ�����g�E�B���h�EID����v���鉼�z�f�o�C�X��T���C
	�@���̃R�[���o�b�N���Ă�

 */
class DRKeyMouseGLUT: public HIRealDevice{
public:
	SPR_OBJECTDEF(DRKeyMouseGLUT);
	///	���z�f�o�C�X(KeyMouse)
	class DV: public DVKeyMouse{
	public:
		// �}�E�X�ʒu���L�����郊���O�o�b�t�@
		std::deque<Vec2i>	mousePoints;
	public:

		DV(DRKeyMouseGLUT* dr, int ch):DVKeyMouse(dr, ch){
			mousePoints.resize(64);
		}
		DRKeyMouseGLUT* GetRealDevice(){ return realDevice->Cast(); }
		
		virtual void OnMouse(int button, int state, int x, int y);
		virtual void OnMouseMove(int button, int x, int y, int zdelta);
		virtual void GetMousePosition(int& x, int& y, int& time, int count=0);
	};

	// �}�E�X�{�^���̏��
	int	buttonState;

public:
	DRKeyMouseGLUT(const DRKeyMouseGLUTDesc& desc = DRKeyMouseGLUTDesc());
	
	///	HIRealDevice�̉��z�֐�
	virtual bool Init();
	//virtual void Register(HISdkIf* intf);
	virtual HIVirtualDeviceIf*	Rent(const IfInfo* ii, const char* name, int portNo);

	/// DRWinDeviceBase�̉��z�֐�
	//virtual void RegisterCallback();

	/// GLUT�L�[�R�[�h����Springhead���ʃL�[�R�[�h�ւ̕ϊ�
	int	ConvertKeyCode(int key, bool spr_to_glut, bool special);

	///
	void OnMouse		(int button, int state, int x, int y);
	void OnMotion		(int x, int y);
	void OnPassiveMotion(int x, int y);
	void OnKey			(unsigned char ch, int x, int y);
	void OnSpecialKey	(int ch, int x, int y);

	/// �E�B���h�EID�ƃf�o�C�X�Ƃ̑Ή�
	//typedef std::map<int, DRKeyMouseGLUT*> KeyMouseMap;
	static 	DRKeyMouseGLUT* instance;
	/// static�̃R�[���o�b�N�֐�
	static void GLUTCALLBACK OnMouseStatic			(int button, int state, int x, int y);
	static void GLUTCALLBACK OnMotionStatic			(int x, int y);
	static void GLUTCALLBACK OnPassiveMotionStatic	(int x, int y);
	static void GLUTCALLBACK OnKeyStatic			(unsigned char ch, int x, int y);
	static void GLUTCALLBACK OnSpecialKeyStatic		(int ch, int x, int y);
};

}	//	namespace Spr
#endif
