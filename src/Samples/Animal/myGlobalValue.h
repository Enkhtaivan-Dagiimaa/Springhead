/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef MYGLOBALVALUE_H
#define MYGLOBALVALUE_H

#include <Springhead.h>
#include <vector>
#include <cstdlib>
#include <gl/glui.h>
//
// --- --- --- --- ---
// using
using namespace Spr;
using namespace std;

// --- --- --- --- ---
// special definition
#define ESC 27
#define SO_NSOLIDS 27
#define JO_NJOINTS 26
#define JO_NBALLJOINTS 14
#define JO_NHINGEJOINTS 8

// --- --- --- --- ---
// �O���[�o���ϐ���`

/// --- Springhead SDK
UTRef<PHSdkIf>   phSdk;								//�����V�~�����[�^��p���邽�߂̕ϐ�
PHSceneIf*       phScene;							//�����V�~�����[�^��ɃI�u�W�F�N�g�i�V�[���O���t�j��o�^���邽�߂̕ϐ�			

UTRef<GRSdkIf>   grSdk;								//�`������邽�߂̕ϐ�
GRDebugRenderIf* grRender;							//�`��֐��������邽�߂̕ϐ�

GRDeviceGLIf*    grDevice;							//OpenGL�f�o�C�X�������邽�߂̕ϐ�

/// --- Springhead �ݒ�
const double simulationPeriod = 8.0;				//glutTimerFunc���ĂԎ����imsec�j
const double dt               = 0.01;				//�V�~�����[�V�����̐ϕ���
const int    niter            = 200;				//LCP�͂�Ղ���������
/// --- Creature�N���X�p�̕ϐ�
UTRef<CRCreatureIf>                creature;		//�N���[�`���[�����p�̕ϐ�
UTRef<CRFourLegsAnimalBodyIf>	   animalBody1;		//�����̑̂������邽�߂̕ϐ�
UTRef<CRTryStandingUpControllerIf> standUpCtrl;		//���Ƃ��Ƃ���R���g���[���������邽�߂̕ϐ�

std::vector<PHSolidIf*>			solids;				//GUI�ɍ��̂̏����������邽�߂̕ϐ�
std::vector<CDBoxIf*>			box;				//GUI�ɍ��̂̔��̏����������邽�߂̕ϐ�
std::vector<PHBallJointIf*>		ballJoints;			//GUI�Ƀ{�[���W���C���g�̏����������邽�߂̕ϐ�
std::vector<PHHingeJointIf*>	hingeJoints;		//GUI�Ƀq���W�W���C���g�̏����������邽�߂̕ϐ�

/// --- GLUI
std::vector<int>	solidsSpace;
std::vector<int>	ballJointsSpace;
std::vector<int>	hingeJointsSpace;
int		nowSolid			  = 0;									//< ���ݕҏW�ł���Solid�̔ԍ�
int		nowBallJoint		  = 0;									//< ���ݕҏW�ł���BallJoint�̔ԍ�
int		nowHingeJoint		  = 0;									//< ���ݕҏW�ł���HingeJoint�̔ԍ�
Vec3f	boxsize				  = Vec3f(0.0, 0.0, 0.0);				//< �ҏW���ă��A���^�C���Œl�̕ς�锠�̑傫��(breadth, height, thickness)
Vec3f	defaultBoxsize;												//< default****[i]��****[i]��������Reset����Ƃ��̂��߂Ɏ���Ă���
float	ballJointData[5]	  = {0.0, 0.0, 0.0, 0.0, 0.0};			//< spinner�ɓ����Ă���l�̃R�s�[�A���A���^�C���ɒl���ς��ispring, damper, limitSwing.upper, limitTwist.lower, limitTwist.upper)
float	defaultBallJointData[5];									//< ���[�h�������́��̗v�f�̒l��ۑ����Ă������߂̔z��
float	rotationBallJoint[16] = {1.0, 0.0, 0.0, 0.0,				//< �֐߂�Quaternion��glMultiMatrix()�ɑ���ł���Affine�s��ɒ������߂̕ϐ��Bglui����ύX����\�������邽�ߗv�f��[16]�̈ꎟ���z��ɂȂ��Ă���
								 0.0, 1.0, 0.0, 0.0,
								 0.0, 0.0, 1.0, 0.0,
								 0.0, 0.0, 0.0, 1.0};				
Affinef				matMadeByQuaterinon;							//< Euler�n�ł�BallJoint��Rotation Matrix�iQuaternion goal��Euler�n�̉�]�s��ɕϊ��������̂�ۑ����Ă����ϐ��j
Affinef				goalEulerMatrix;								//< �ݒ肷�鐧��ڕW��Euler�n�̍s��

float				hingeJointData[4]	  = {0.0, 0.0, 0.0, 0.0};	//< spinner�ɓ����Ă���l�̃R�s�[�A���A���^�C���ɒl���ς��ispring, damper, lower, upper, origin�j
float				defaultHingeJointData[4];						//< ���[�h�������́��̗v�f�̒l��ۑ����Ă������߂̔z��
float				hingeJointGoal;									//< HingeJoint�̐���ڕW�l(rad)
float				defaultHingeJointGoal;							//< ���[�h�������́��̗v�f�̒l��ۑ����Ă������߂̕ϐ�
float				mass = 0;										//< ���ݕҏW�ł���Solid�̎���
float				defaultMass;									//< ���[�h�������́��̗v�f�̒l��ۑ����Ă������߂̕ϐ�
int					dynamicalValue		= 0;						//< Horse Body Simulator�̍����͂̏��ɑ΂��ē������ǂ���
int					counterKeyframe		= 0;						//< ���܂łɓo�^����keyframe�̐�
int					counterStep	= 0;

GLUI*			glui;												//< glui�̃C���X�^���X
GLUI_Rollout*	menu;												//< menu�̃C���X�^���X
GLUI_Panel*		settingModel;										//< menu���̃��f���̐ݒ蕔�����܂Ƃ߂�C���X�^���X

//Solid
GLUI_Rollout*	rollSolid;											//< settingModel����Solid�����̃��[���A�E�g���Ǘ�����C���X�^���X
GLUI_Panel*		solidDataArea;										//< rollSolid����Solid����\������̈���Ǘ�����C���X�^���X
GLUI_Listbox*	listBoxSolid;										//< rollSolid���łǂ�Solid��ҏW���邩��I�ԃ��X�g�{�b�N�X���Ǘ�����C���X�^���X
GLUI_Spinner*	spinnerMass;										//< rollSolid����spinner�ɕ\������鎿�ʏ����Ǘ�����C���X�^���X
GLUI_Spinner*	spinnerBoxsize[3];									//< rollSolid����spinner�ɕ\������锠�̑傫�����Ǘ����邽�߂̃C���X�^���X(Breath,Height,Thickness)

//Ball Joint
GLUI_Rollout*	rollBallJoint;										//< settingModel����BallJoint�����̃��[���A�E�g���Ǘ�����C���X�^���X
GLUI_Panel*		ballJointDataArea;									//< rollBallJoint����BallJoint����\������̈���Ǘ�����C���X�^���X
GLUI_Listbox*	listBoxBallJoint;									//< rollBallJoint���łǂ�BallJoint��ҏW���邩��I�ԃ��X�g�{�b�N�X���Ǘ�����C���X�^���X
GLUI_Spinner*	spinnerDataBallJoint[5];							//< rollBallJoint����spinner�ɕ\�����������Ǘ�����C���X�^���X(spring, damper, swingUpper, twistLower, twistUpper)


//Hinge Joint
GLUI_Rollout*	rollHingeJoint;										//< settingModel����BallJoint�����̃��[���A�E�g���Ǘ�����C���X�^���X
GLUI_Panel*		hingeJointDataArea;									//< rollHingeJoint����BallJoint����\������̈���Ǘ�����C���X�^���X
GLUI_Listbox*	listBoxHingeJoint;									//< rollHingeJoint���łǂ�HingeJoint��ҏW���邩��I�ԃ��X�g�{�b�N�X���Ǘ�����C���X�^���X
GLUI_Spinner*	spinnerDataHingeJoint[4];							//< rollHingeJoint����spinner�ɕ\�����������Ǘ�����C���X�^���X(spring, damper, lower, upper)
GLUI_Spinner*	spinnerDataHingeJointGoal;							//< rollHingeJoint����spinner�ɕ\�����������Ǘ�����C���X�^���X(origin)

int mainWindow	= 0;							//< Horse Body Simulator�̃E�B���h�E���Ǘ�����C���X�^���X

/// --- �O���[�o���t���O
bool bTimer = true;								//	�^�C�}�[�ŃV�~�����[�V������i�߂邩�ǂ���

/// --- ���_
float   camAngle  = 0.0f;						//< Horse Body Simulator�̃J�����̊p�x
float   camZoom   = 2.0f;						//< Horse Body Simulator�̃J�����̔{��
Affinef view;									//< Horse Body Simulator�̃J�����ʒu�̉�]�E�Y�[�����s�����̍s��p�̕ϐ�

////////////////////////////////////////////////////////////////////////////
// Springhead�Ƃ͊֌W�Ȃ��|�C���^�̃������J���͎蓮�ōs��Ȃ���΂Ȃ�Ȃ� //
// �蓮�ōs���͓̂��I�m�ۂ��������Ń|�C���^����,����PressQuit()�ɏ���	  //
////////////////////////////////////////////////////////////////////////////

//Keyboard����Ă΂��֐�
void PressQuit(){
	
	//////////////////////////
	//						//
	// �����Ń������J������ //
	//						//
	//////////////////////////

	exit(0);
}

// GUI����Ă΂��֐�
void PressQuit(int control){
	PressQuit();
}

#endif