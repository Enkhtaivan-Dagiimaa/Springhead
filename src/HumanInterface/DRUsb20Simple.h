/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
// DRUsb20Simple.h: DRUsb20Simple �N���X�̃C���^�[�t�F�C�X
//
//////////////////////////////////////////////////////////////////////

#ifndef DR_USB20SIMPLE_H
#define DR_USB20SIMPLE_H

#include <HumanInterface/HIDevice.h>
#include <HumanInterface/SprHIDRUsb.h>
#include <HumanInterface/HumanInterfaceDecl.hpp>

namespace Spr {

/**
	Cyverse��USB2.0(LDR-SPIDAR-AMP)�̃h���C�o
 **/
class SPR_DLL DRUsb20Simple : public HIRealDevice{
public:
	SPR_OBJECTDEF(DRUsb20Simple);
	SPR_DECLMEMBEROF_DRUsb20SimpleDesc;

	///	���z�f�o�C�X(DA)
	class Da: public DVDa{
	public:
		Da(DRUsb20Simple* r, int c):DVDa(r, c){}
		DRUsb20Simple* GetRealDevice() { return realDevice->Cast(); }
		
		virtual void Voltage(float v){ GetRealDevice()->WriteVoltage(portNo, v); }
		virtual void Digit(int v){ GetRealDevice()->WriteDigit(portNo, v); }
		virtual void Update(){ GetRealDevice()->Update(); }
	};
	///	���z�f�o�C�X(Counter)
	class Counter: public DVCounter{
	public:
		Counter(DRUsb20Simple* r, int c):DVCounter(r, c){}
		DRUsb20Simple* GetRealDevice() { return realDevice->Cast(); }
		
		virtual void Count(long c){ GetRealDevice()->WriteCount(portNo, c); }
		virtual long Count(){ return GetRealDevice()->ReadCount(portNo); }
		virtual void Update(){ GetRealDevice()->Update(); }
	};
	///	���z�f�o�C�X(Pio)
	class Pio: public DVPio{
	public:
		Pio(DRUsb20Simple* r, int c):DVPio(r, c){}
		DRUsb20Simple* GetRealDevice() { return realDevice->Cast(); }

		virtual void Set(int l){ GetRealDevice()->WritePio(portNo, l!=0); }
		virtual int Get(){ return GetRealDevice()->ReadPio(portNo) ? 1 : 0; }
		virtual void Update(){ GetRealDevice()->Update(); }
	};

//----------------------------------------------------------------------------
protected:
	void*	hSpidar;

	int		sign[8]; //DA�o�͗p�̕���
	long	count[8];
	long	countOffset[8];
	int		daOut[8];
	int		adIn[8];
	int		pioLevel[16];

public:
	///	�R���X�g���N�^	ch�͔w�ʂ̃X�C�b�`�ɂȂ�\��
	DRUsb20Simple(const DRUsb20SimpleDesc& d=DRUsb20SimpleDesc());
	virtual ~DRUsb20Simple();

	///	������
	virtual bool Init();
	///	������(�`�����l��, PIDVID���C�ɂ����ɏ�����)
	//virtual bool InitAny();
	///	���z�f�o�C�X�̓o�^
	//void Register(HISdkIf* intf);
	
	///	�d���o��
	void WriteVoltage(int ch, float v);
	///	�d���o��(���l�w��)
	void WriteDigit(int ch, int v);
	///	�J�E���^�l�̐ݒ�
	void WriteCount(int ch, long c);
	///	�J�E���^�l�̓ǂݏo��
	long ReadCount(int ch);
	///	��Ԃ̍X�V
	virtual void Update();
	
	///	PIO�|�[�g�̐ݒ�
	void WritePio(int ch, bool level);
	///	PIO�|�[�g�̓ǂݏo��
	bool ReadPio(int ch);
	///	�X�C�b�`�i�|�[�g�̏��5-8�r�b�g�j�̓ǂݏo��
	int ReadRotarySwitch();

	/// ���Z�b�g
	virtual void Reset();

	///	USB�̃t�@�C���n���h��
	void* GetHandle(){ return hSpidar; };
protected:
	//	�f�o�C�X�������A�`�����l����Ԃ��B�`�����l�����w�肳�ꂽ�ꍇ�A�w�肵���`�����l���̃f�o�C�X�����Ԃ��Ȃ��B
	//	���s�����-1��Ԃ��B
	int FindDevice(int ch=-1);

	virtual unsigned	GetVidPid(){ return 0x0CEC0203; }
	virtual void		UsbReset();
	virtual void		UsbCounterGet();
	virtual void		UsbCounterClear();
	virtual void		UsbDaSet();
	virtual unsigned	UsbVidPid(void* h);
	virtual void*		UsbOpen(int id);
	virtual bool		UsbClose(void*& h);
};

} //namespace Spr

#endif // SPR_DRUSB20SIMPLE_H
