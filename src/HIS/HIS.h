/*
#ifndef VR_HIS_H
#define VR_HIS_H
*/
/**	@page HIS Human Interface System ���C�u����

@section HISAim �ړI
	���̃p�b�P�[�W�́C�q���[�}���C���^�t�F�[�X�V�X�e���𓮂������߂̃\�t�g�E�G�A
�̃��C�u�����ł��D�A�v���P�[�V�����́C���̃p�b�P�[�W�̃N���X�𗘗p���܂��D

@section HISStructure �\��
@subsection HISSpidar SPIDAR
SPIDAR�𓮂������߂̃\�t�g�E�G�A�̃��C�u�����ł��D
4�{��SPIDAR�p Spr::HISpidar4, SPIDAR-G�p Spr::HISpidarG6 �Ȃǂ̃N���X������ق��C
�����SPIDAR�iSPIDAR-G CType�p�j Spr::HISpidarG6C �̂悤�ȃN���X������܂��D
�V����SPIDAR���������C���̃f�o�C�X�p�ɃN���X���ЂƂ��悤�ɂ��Ă��������D

@subsection HISMOVE �ړ����̓C���^�t�F�[�X
TBW

*/
/*
#ifdef _WIN32
 #include <WinBasis/WBWin.h>
 #include <mmsystem.h>
#endif
#include <Device/DRRealDevice.h>
#include "HIBase.h"
#include "HIHapticDevice.h"
#include "HISpidarCalc3DOF.h"
#include "HISPidarCalc6DOF.h"
#include "HISpidarCalcBase.h"
#include "HISpidarMotor.h"
#include <fstream>
#include <Springhead.h>
#include <Foundation/Object.h>

#endif
*/
#include <Springhead.h>
#include <Foundation/Object.h>
#include <Base/Affine.h>
#include <fstream>
#include <Device/DRRealDevice.h>

#ifdef _WIN32
#include <WinBasis/WBWin.h>
#include <mmsystem.h>
#endif

#include "HIBase.h"
#include "HIHapticDevice.h"
#include "HIMouse.h"
#include "HIOrientation.h"
#include "HIRUIBase.h"
#include "HIRUIKuma.h"
#include "HISpidar2.h"
#include "HISpidar3.h"
#include "HISpidar4.h"
#include "HISpidar4Angle.h"
#include "HISpidar4D.h"
#include "HISpidar8.h"
#include "HISpidar8MotorTest.h"
#include "HISpidarCalc3DOF.h"
#include "HISpidarCalc6DOF.h"
#include "HISpidarCalcBase.h"
#include "HISpidarCanoe.h"
#include "HISpidarFishing.h"
#include "HISpidarFishingRod.h"
#include "HISpidarG6.h"
#include "HISpidarG6C.h"
#include "HISpidarG6Jun.h"
#include "HISPidarG6O.h"
#include "HISpidarG6WB.h"
#include "HISpidarG6X2.h"
#include "HISPidarG6X3.h"
#include "HISpidarG6X.h"
#include "HISpidarI7.h"
#include "HISpidarMotor.h"
#include "HISpidarMotorAngle.h"
#include "HIStepWalk.h"
#include "HITurnTable.h"