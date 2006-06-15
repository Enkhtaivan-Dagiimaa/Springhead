//#ifndef Spr_DEVICE_H
//#define Spr_DEVICE_H
/**	@page Device Device ���C�u����
@section aim �ړI
	���̃p�b�P�[�W�́C�f�o�C�X�h���C�o�𒊏ۉ����邽�߂̂��̂ł��D
�A�v���P�[�V��������f�o�C�X�h���C�o�𒼐ڌĂяo���ƁC�f�o�C�X��ύX�����Ƃ��ɁC
�A�v���P�[�V�������C�����Ȃ���΂Ȃ�Ȃ��Ȃ�܂��D
�����ŁC�Ⴆ��D/A�R���o�[�^�Ȃ�ǂ�D/A�R���o�[�^�ł������悤�ɌĂяo����悤��
���ۉ�����D/A�R���o�[�^�f�o�C�X(�o�[�`�����f�o�C�X)���l���C�A�v���P�[�V������
�o�[�`�����f�o�C�X���g�p����悤�ɂ��܂��D�o�[�`�����f�o�C�X�͎��ۂ̃f�o�C�X
(���A���f�o�C�X)���Ăяo���悤�Ɏ������Ă���܂��D

@section usage �g����
�ȒP�ȃT���v���������܂��D
@verbatim
	DVDeviceManager devMan;				//	�f�o�C�X�Ǘ��N���X�D
	devMan.RPool().Register(new DRAdapio(9));	//	���A���f�o�C�X�̓o�^
	devMan.Init();						//	�f�o�C�X�̏������C������
										//	�o�[�`�����f�o�C�X�����������D
	std::cout << devMan;				//	��Ԃ̕\��
	
	//	�f�o�C�X�Ǘ��N���X����A/D���؂�Ă���
	DVAdBase* ad;
	devMan.VPool().Rent(ad);
	std::cout << ad->Voltage();
@endverbatim
�܂��f�o�C�X�}�l�[�W�����쐬���C���f�o�C�X��o�^���C���������܂��D
����ƃo�[�`�����f�o�C�X�����������̂ŁC�A�v���P�[�V�����́C
�g�������o�[�`�����f�o�C�X���f�o�C�X�}�l�[�W������؂�Ă��܂��D

����̃f�o�C�X�̓���̋@�\���g�������ꍇ�C
��p�̃f�o�C�X�}�l�[�W���I�u�W�F�N�g���쐬���āC�f�o�C�X��o�^�C���������āC
�o�[�`�����f�o�C�X���擾���Ă��������D

�f�o�C�X�}�l�[�W���I�u�W�F�N�g�͕����쐬���Ă���肠��܂���D
*/
/*
#include <sstream>

#include "DVDeviceManager.h"
#include "DVCounterBase.h"
#include "DVDaBase.h"
 #include "DVPioBase.h"

#include "DRRealDevice.h"
#include <stdio.h>

#ifdef _WIN32
 #include <WinBasis/WBWin.h>
 #include <WinBasis/WBPortIO.h>
 #include <WinBasis/WBWinDriver.h>
 #include <WinBasis/WBDllLoader.h>

 #include "DRContecIsaCounter.h"
 #include "DRContecIsaDa.h"
 #include "DRNittaForce.h"
 #include "DRUsbH8Simple.h"
 #include "DRUsb20Simple.h"
 #include "DRUsb20Sh4.h"
#endif

#endif
*/

#include <sstream>
#include <Springhead.h>
#include <Foundation/Object.h>
#include <Base/Affine.h>
#include <stdio.h>

#ifdef _WIN32

// winsock.h��winsock2.h�̎d�l��
// winsock2.h���Ă΂ꂻ�̌��winsock.h���Ă΂ꂽ�ꍇ�A
// ���ƂɌĂ΂ꂽwinsock.h�͖���������邪
// winsock.h���Ă΂ꂽ���winsock2.h���Ă΂ꂽ�ꍇ��
// ���������s���ꓯ���ϐ��̓�d��`�̃G���[���N�����Ă��܂��B
// 
// ������񂱂̎d�l��winsock.h�Ɉ�s���������Β��邪�A
// ������s���̂͂��܂�ǂ����Ƃł͂Ȃ��̂Ŋ����čs��Ȃ��B
// 
// �����ł�WBWin.h��include<winsock2.h>���s���Ă���A
// windows.h��include<winsock.h>���s���Ă���B
// �O�q�����G���[��������邽�߂�
// ���̃t�@�C���ł�include<WinBasis/WBWin.h>���s������A
// include<windows.h>���s�����Ƃɂ���

 #include <WinBasis/WBWin.h>
 #include <WinBasis/WBPortIO.h>
 #include <WinBasis/WBWinDriver.h>
 #include <WinBasis/WBDllLoader.h>

 #include <windows.h>

 #include "DRContecIsaCounter.h"
 #include "DRContecIsaDa.h"
 #include "DRNittaForce.h"
 #include "DRUsbH8Simple.h"
 #include "DRUsb20Simple.h"
 #include "DRUsb20Sh4.h"
#endif

#include "DRADAPIO.h"
//#include "DRContecIsaCounter.h"
//#include "DRContecIsaDa.h"
#include "DREzUSB.h"
#include "DRIHCSh4.h"
//#include "DRNittaForce.h"
#include "DRRealDevice.h"
#include "DRRealDevicePool.h"
#include "DRTokin3D.h"
#include "DRUPP.h"
//#include "DRUsb20Sh4.h"
//#include "DRUsb20Simple.h"
//#include "DRUsbH8Simple.h"
#include "DRUsbH8SimpleDef.h"
#include "DVADBase.h"
#include "DVCounterBase.h"
#include "DVDABase.h"
#include "DVDeviceManager.h"
#include "DVForceBase.h"
#include "DVOrientationBase.h"
#include "DVPIOBase.h"
#include "DVVirtualDevice.h"
#include "DVVirtualDevicePool.h"
#include "Tkinput.h"
#include "Tkinputdef.h"
#include "Tuadapio.h"


