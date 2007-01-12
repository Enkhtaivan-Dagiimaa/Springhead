#ifndef DVKEYMOUSE_H
#define DVKEYMOUSE_H

#include <HumanInterface/HIVirtualDevice.h>
namespace Spr {;

///	DVKeyMouse
class SPR_DLL DVKeyMouse:public HIVirtualDevice, public DVKeyMouseIfInit{
public:
	OBJECTDEF_ABST(DVKeyMouse, HIVirtualDevice);
	///	�f�o�C�X�̖��O
	virtual const char* Name() const =0;
	///	�f�o�C�X�̎��
	static const char* TypeS() { return "KeyMouse"; }
	///	�f�o�C�X�̎��
	virtual const char* Type() const{ return TypeS(); }
	///	�n���h���̐ݒ�
	virtual void SetHandler(DVKeyMouseHandler* h)=0;
	///	�}�E�X�{�^���E�L�[�{�[�h��Ԏ擾
	virtual int GetKeyState(int key)=0;
	///	�}�E�X�ʒu�擾�֐�	0���ŐV�C1�ȏ�͉ߋ��̗������擾
	virtual DVKeyMouseIf::DVMousePosition GetMousePosition(int count=0){
		return DVKeyMouseIf::DVMousePosition();
	}
};

}	//	namespace Spr
#endif
