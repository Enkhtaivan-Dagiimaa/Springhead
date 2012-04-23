/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef HI_DEVICE_H
#define HI_DEVICE_H

#include <HumanInterface/SprHIDevice.h>
#include <HumanInterface/SprHIKeyMouse.h>
#include <HumanInterface/SprHIJoyStick.h>
#include <Foundation/Object.h>
#include <vector>

namespace Spr {;

class HIVirtualDevice;

/// �f�o�C�X�̊�{�N���X
class SPR_DLL HIDevice : public NamedObject{
public:
	SPR_OBJECTDEF_ABST(HIDevice);
};

///	���f�o�C�X�̊�{�N���X�DISA�J�[�h1���Ȃǂɑ�������.
class SPR_DLL HIRealDevice: public HIDevice{
public:
	SPR_OBJECTDEF_ABST(HIRealDevice);
	
	/// �񋟂��鉼�z�f�o�C�X�̔z��
	std::vector< UTRef<HIVirtualDevice> >	dvPool;
public:
	///	�������Ɖ��z�f�o�C�X�̓o�^�BSDK�ɓo�^����ۂɌĂ΂��B
	virtual bool Init(){ return false; }

	/// ���z�f�o�C�X�̍쐬
	virtual HIVirtualDeviceIf*	Rent(const IfInfo* ii, const char* name, int portNo);
	/// ���z�f�o�C�X�̕ԋp
	virtual bool Return(HIVirtualDeviceIf* dv);
	///	��Ԃ̍X�V
	virtual void Update(){}
	///	�_���v�o��
	//virtual void Print(std::ostream& o) const;

	virtual bool		AddChildObject(ObjectIf* o);
	virtual ObjectIf*	GetChildObject(size_t i);
	virtual size_t		NChildObject() const;
};

/*
/// �E�B���h�E�Ɋ֘A�t������f�o�C�X�̊�{�N���X
class DRWinDeviceBase : public HIRealDevice{
public:
	SPR_OBJECTDEF_NOIF(DRWinDeviceBase);

public:
	/// �R�[���o�b�N�֐��̓o�^
	virtual void	RegisterCallback(){}
};
*/

//----------------------------------------------------------------------------
//	HIVirtualDevice

///	���z�f�o�C�X�̊�{�N���X�DDA,�J�E���^�Ȃǂ�1�`�����l���ɑ�������.
class SPR_DLL HIVirtualDevice: public HIDevice{
public:
	SPR_OBJECTDEF_ABST(HIVirtualDevice);
	friend class HIVirtualDevicePool;

	/// �g�p���t���O
	bool used;
	/// �|�[�g�ԍ�
	int	portNo;
	/// ���f�o�C�X�ւ̎Q��
	HIRealDevice*	realDevice;
public:
	HIVirtualDevice(HIRealDevice* dev, int ch = -1);
	virtual ~HIVirtualDevice(){}

	/// �f�o�C�X���̐ݒ�
	void SetDeviceName();
	/// �g�p���t���O
	bool IsUsed(){ return used; }
	void SetUsed(bool b=true){ used = b; }
	/// �|�[�g�ԍ�
	int	GetPortNo() const { return portNo; }
	///	���f�o�C�X�ւ̃|�C���^
	HIRealDeviceIf* GetRealDevice(){ return realDevice->Cast(); }
	
	///	�_���v�o��
	//virtual void Print(std::ostream& o) const;
	/// �f�o�C�X�^�C�v
	//virtual const char* GetDeviceType() const = 0;
	/// ��Ԃ̍X�V
	virtual void Update(){}
};

///	���̓|�[�g��1�[�q������킷�D
class SPR_DLL DVInputPort: public HIVirtualDevice, public DVPortEnum{
public:
	///	���̓|�[�g�̓d�����x��(Hi:1, Lo:0) ��Ԃ��D
	virtual int Get()=0;

	///	�f�o�C�X�̎��
	//static const char* GetDeviceTypeStatic() { return "input port"; }
	//virtual const char* GetDeviceType() const{ return GetDeviceTypeStatic(); }

	DVInputPort(HIRealDevice* r, int c):HIVirtualDevice(r, c){}
};

///	�o�̓|�[�g��1�[�q������킷�D
class SPR_DLL DVOutputPort: public HIVirtualDevice, public DVPortEnum{
public:
	///	���̓|�[�g�̓d�����x��(Hi:1, Lo:0) ��Ԃ��D
	virtual void Set(int l)=0;

	///	�f�o�C�X�̎��
	//static const char* GetDeviceTypeStatic() { return "output port"; }
	//virtual const char* GetDeviceType() const{ return GetDeviceTypeStatic(); }

	DVOutputPort(HIRealDevice* r, int c):HIVirtualDevice(r, c){}
};


///	PIO��1�[�q��\���D
class SPR_DLL DVPio: public HIVirtualDevice, public DVPortEnum{
public:
	SPR_OBJECTDEF_ABST(DVPio);
	///	�|�[�g�̃��W�b�N���x���̓��́BHi:true Lo:false
	virtual int Get()=0;
	///	�|�[�g�̃��W�b�N���x���̏o�́BHi:true Lo:false
	virtual void Set(int l)=0;
	
	///	�f�o�C�X�̎��
	//static const char* GetDeviceTypeStatic() { return "PIO"; }
	//virtual const char* GetDeviceType() const{ return GetDeviceTypeStatic(); }

	DVPio(HIRealDevice* r, int c):HIVirtualDevice(r, c){}
};

///	�J�E���^��1�`�����l����\���D
class SPR_DLL DVCounter: public HIVirtualDevice{
public:
	SPR_OBJECTDEF_ABST(DVCounter);
	
	///	�J�E���^�l�̐ݒ�
	virtual void Count(long count)=0;
	///	�J�E���^�l�̓ǂݏo��
	virtual long Count()=0;
	///	�f�o�C�X�̎��
	//static const char* GetDeviceTypeStatic() { return "Counter"; }
	//virtual const char* GetDeviceType() const{ return GetDeviceTypeStatic(); }

	DVCounter(HIRealDevice* r, int c):HIVirtualDevice(r, c){}
};

//----------------------------------------------------------------------------
//	DVAd

///	A/D��1�`�����l����\���D
class DVAd: public HIVirtualDevice{
public:
	SPR_OBJECTDEF_ABST(DVAd);
	///	���̓f�W�^���l�̎擾
	virtual int Digit()=0;
	///	���͓d���̎擾
	virtual float Voltage()=0;
	
	///	�f�o�C�X�̎��
	//static const char* GetDeviceTypeStatic() { return "A/D"; }
	//virtual const char* GetDeviceType() const{ return GetDeviceTypeStatic(); }

	DVAd(HIRealDevice* r, int c):HIVirtualDevice(r, c){}
};

//----------------------------------------------------------------------------
//	DVDa

///	D/A��1�`�����l����\���D
class SPR_DLL DVDa: public HIVirtualDevice{
public:
	SPR_OBJECTDEF_ABST(DVDa);
	
	///	�o�͂���f�W�^���l�̐ݒ�
	virtual void Digit(int d)=0;
	///	�o�͓d���̐ݒ�
	virtual void Voltage(float volt)=0;

	///	�f�o�C�X�̎��
	//static const char* GetDeviceTypeStatic() { return "D/A"; }
	//virtual const char* GetDeviceType() const{ return GetDeviceTypeStatic(); }

	DVDa(HIRealDevice* r, int c):HIVirtualDevice(r, c){}
};

//----------------------------------------------------------------------------
//	DVForce

class SPR_DLL DVForce: public HIVirtualDevice{
public:
	SPR_OBJECTDEF_ABST(DVForce);

	///	���R�x�̎擾
	virtual int GetDOF() = 0;
	///	�͂̎擾
	virtual float GetForce(int ch) = 0;
	///	�͂̎擾
	virtual void GetForce3(Vec3f& f) {
		f[0] = GetForce(0);
		f[1] = GetForce(1);
		f[2] = GetForce(2);
	}
	///	�͂̎擾
	virtual void GetForce6(Vec3f& f, Vec3f& t) {
		f.X() = GetForce(0);
		f.Y() = GetForce(1);
		f.Z() = GetForce(2);
		t.X() = GetForce(3);
		t.Y() = GetForce(4);
		t.Z() = GetForce(5);
	}

	///	�f�o�C�X�̎��
	//static const char* GetDeviceTypeStatic(){ return "Force"; }
	//virtual const char* GetDeviceType() const{ return GetDeviceTypeStatic(); }

	DVForce(HIRealDevice* r):HIVirtualDevice(r){}
};

template<class T>
class DVHandlerContainer{
public:
	typedef std::vector<T*>	Handlers;
	Handlers	handlers;

	void AddHandler(T* h){
		if(find(handlers.begin(), handlers.end(), h) == handlers.end())
			handlers.push_back(h);
	}
	void RemoveHandler(T* h){
		handlers.erase(find(handlers.begin(), handlers.end(), h));
	}
};

///	DVKeyMouse
class SPR_DLL DVKeyMouse: public DVHandlerContainer<DVKeyMouseHandler>, public HIVirtualDevice{
public:
	SPR_OBJECTDEF_ABST(DVKeyMouse);
public:
	// �o�^����Ă���n���h�����Ăяo��
	virtual void OnMouse(int button, int state, int x, int y);
	virtual void OnDoubleClick(int button, int x, int y);
	virtual void OnMouseMove(int button, int x, int y, int zdelta);
	virtual void OnKey(int state, int key, int x, int y);

	virtual int GetKeyState(int key){ return 0; }
	virtual void GetMousePosition(int& x, int& y, int& time, int count=0){}

	///	�f�o�C�X�̎��
	//static const char* GetDeviceTypeStatic() { return "KeyMouse"; }
	//virtual const char* GetDeviceType() const{ return GetDeviceTypeStatic(); }

	DVKeyMouse(HIRealDevice* r, int ch):HIVirtualDevice(r, ch){}
};

//-------------------------------------------------------------------------------------------------
///	DVJoyStick
class SPR_DLL DVJoyStick: public DVHandlerContainer<DVJoyStickHandler>, public HIVirtualDevice{
public:
	SPR_OBJECTDEF_ABST(DVJoyStick);

	int pollInterval;
public:
	// �o�^����Ă���n���h�����Ăяo��
	void OnUpdate(int buttonMask, int x, int y, int z);

	virtual void SetPollInterval(int ms){}
	virtual int  GetPollInterval(){ return pollInterval; }

	DVJoyStick(HIRealDevice* r, int ch):HIVirtualDevice(r, ch){
		pollInterval = 10;
	}
};

//-------------------------------------------------------------------------------------------------
///	������Ԃ��f�o�C�X
class SPR_DLL DVOrientation: public HIVirtualDevice{
public:
	///	�����̎擾(��]�s��ł���킷)
	virtual Vec3f GetEuler() = 0;
	///	�����̎擾(��]�s��ł���킷)
	virtual void GetMatrix(Matrix3f& ori) = 0;
	
	///	�f�o�C�X�̎��
	//static const char* GetDeviceTypeStatic() { return "Orientation"; }
	//virtual const char* GetDeviceType() const{ return GetDeviceTypeStatic(); }

	DVOrientation(HIRealDevice* r):HIVirtualDevice(r){}
};

}	//	namespace Spr

#endif
