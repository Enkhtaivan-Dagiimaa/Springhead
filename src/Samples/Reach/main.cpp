/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 Springhead2/src/Samples/Joints/main.cpp

�y�T�v�z�֐ߋ@�\�̃f��
*/

#include <ctime>
#include <string>
#include <GL/glut.h>
#include <sstream>
#include <windows.h>

#include <Springhead.h>		//	Springhead�̃C���^�t�F�[�X

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif
using namespace Spr;

#define ESC		27

PHSdkIf* phSdk;			// SDK�C���^�t�F�[�X
GRSdkIf* grSdk;
PHSceneIf* scene;		// Scene�C���^�t�F�[�X
GRDebugRenderIf* render;
GRDeviceGLIf* device;

double simulationPeriod = 32.0;
int sceneNo;			// �V�[���ԍ�

PHSolidDesc descFloor;					//�����̂̃f�B�X�N���v�^
PHSolidDesc descBox;					//�����̂̃f�B�X�N���v�^
//CDConvexMeshIf* meshBox;				//���`��̃C���^�t�F�[�X
CDShapeIf* shapeBox;
CDShapeIf* shapeSphere;

std::vector<PHSolidIf*> soBox;			//�����̂̃C���^�t�F�[�X
std::vector<PHJointIf*> jntLink;		//�֐߂̃C���^�t�F�[�X

std::vector<PHSolidIf*> soTarget;		


/** �����p�ϐ� **/
const double dt = 0.05	;					//�ϕ���
const int niter = 200;					//LCP�͂�Ղ���������

bool bTimer = true;			//	�^�C�}�[�ŃV�~�����[�V������i�߂邩�ǂ���


void step(){
	scene->ClearForce();
	scene->GenerateForce();
	scene->Integrate();
	glutPostRedisplay();
}
void timer(int id){
	if (!bTimer) return;
	glutTimerFunc(simulationPeriod, timer, 0);
	step();
}

// �V�[��0 : ���̃f���Bspace key�Ŕ���������
void BuildScene(){	
	soBox.clear();
	jntLink.clear();
	//	��
	CDBoxDesc bd;
	bd.boxsize = Vec3f(0.19, 0.077, 0.137);
	CDShapeIf* shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	//�󒆂ɌŒ肷��
	soBox.back()->SetFramePosition(Vec3f(0.0, 0.0, 0.0));
	soBox.back()->SetDynamical(false);

	//	��
	bd.boxsize = Vec3f(0.15, 0.08, 0.108);
	shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);

	//	�W���C���g
	PHHingeJointDesc jd;
	jd.poseSocket.Pos() = Vec3d(0,0.04, 0);
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'x');
	jd.posePlug.Pos() = Vec3d(0, -0.04, 0);
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'x');
	jd.damper = 60.0;
	jd.spring = 160.0;
	jntLink.push_back(scene->CreateJoint(soBox[1], soBox[0], jd));

	//	��
	bd.boxsize = Vec3f(0.18, 0.2, 0.1);
	shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);

	//	�W���C���g
	jd = PHHingeJointDesc();
	jd.poseSocket.Pos() = Vec3d(0, 0.04, 0);
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.posePlug.Pos() = Vec3d(0, -0.1, 0);
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.damper = 60.0;
	jd.spring = 160.0;
	jntLink.push_back(scene->CreateJoint(soBox[2], soBox[1], jd));

	//	�p��
	soBox.push_back(scene->CreateSolid(descBox));

	//	��Z
	jd = PHHingeJointDesc();
	jd.poseSocket.Pos() = Vec3d(0.13, 0.1, 0);
	jd.posePlug.Pos() = Vec3d(0, 0, 0);
	jd.damper = 6.0;
	jd.spring = 10.0;
	jd.origin = Rad(-30);
	jntLink.push_back(scene->CreateJoint(soBox[3], soBox[2], jd));

	//	�p��
	soBox.push_back(scene->CreateSolid(descBox));

	//	��Y
	jd = PHHingeJointDesc();
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'z');
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'z');
	jd.damper = 6.0;
	jd.spring = 150.0;
	jntLink.push_back(scene->CreateJoint(soBox[4], soBox[3], jd));

	//	�E��r
	bd.boxsize = Vec3f(0.049, 0.16, 0.048);
	shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);

	//	��X
	jd = PHHingeJointDesc();
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.posePlug.Pos() = Vec3d(0, 0.09, 0);
	jd.damper = 6.0;
	jd.spring = 10.0;
	jd.origin = Rad(-30);
	jntLink.push_back(scene->CreateJoint(soBox[5], soBox[4], jd));

	//	�E�O�r
	bd.boxsize = Vec3f(0.046, 0.12, 0.046);
	shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	//	�Ђ�
	jd = PHHingeJointDesc();
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.poseSocket.Pos() = Vec3d(0, -0.09, 0);
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.posePlug.Pos() = Vec3d(0, 0.07, 0);
	jd.damper = 4.0;
	jd.spring = 6.0;
	jd.origin = Rad(-90);
	jntLink.push_back(scene->CreateJoint(soBox[6], soBox[5], jd));

	//	��
	CDSphereDesc sd;
	sd.radius = 0.06;
	shape = phSdk->CreateShape(sd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	//	��
	jd = PHHingeJointDesc();
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.poseSocket.Pos() = Vec3d(0, 0.1, 0);
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.posePlug.Pos() = Vec3d(0, -0.08, 0);
	jd.damper = 3.0;
	jd.spring = 3.0;
	jd.origin = Rad(0);
	jntLink.push_back(scene->CreateJoint(soBox[7], soBox[2], jd));

	//	�ڕW�ʒu
	sd.radius = 0.02;
	shape = phSdk->CreateShape(sd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	soBox.back()->SetDynamical(false);
	soBox.back()->SetFramePosition(Vec3f(0.2, 0.2, -0.2));
	soTarget.push_back(soBox.back());


	
	//	����

	//	�p��
	soBox.push_back(scene->CreateSolid(descBox));

	//	��Z
	jd = PHHingeJointDesc();
	jd.poseSocket.Pos() = Vec3d(-0.13, 0.1, 0);
	jd.posePlug.Pos() = Vec3d(0, 0, 0);
	jd.damper = 6.0;
	jd.spring = 10.0;
	jd.origin = Rad(30);
	jntLink.push_back(scene->CreateJoint(soBox[9], soBox[2], jd));

	//	�p��
	soBox.push_back(scene->CreateSolid(descBox));

	//	��Y
	jd = PHHingeJointDesc();
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'z');
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'z');
	jd.damper = 6.0;
	jd.spring = 150.0;
	jntLink.push_back(scene->CreateJoint(soBox[10], soBox[9], jd));

	//	�E��r
	bd.boxsize = Vec3f(0.049, 0.16, 0.048);
	shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);

	//	��X
	jd = PHHingeJointDesc();
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.posePlug.Pos() = Vec3d(0, 0.09, 0);
	jd.damper = 6.0;
	jd.spring = 10.0;
	jd.origin = Rad(-30);
	jntLink.push_back(scene->CreateJoint(soBox[11], soBox[10], jd));

	//	�E�O�r
	bd.boxsize = Vec3f(0.046, 0.12, 0.046);
	shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	//	�Ђ�
	jd = PHHingeJointDesc();
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.poseSocket.Pos() = Vec3d(0, -0.09, 0);
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.posePlug.Pos() = Vec3d(0, 0.07, 0);
	jd.damper = 4.0;
	jd.spring = 6.0;
	jd.origin = Rad(-90);
	jntLink.push_back(scene->CreateJoint(soBox[12], soBox[11], jd));

	//	�ڕW�ʒu
	shape = phSdk->CreateShape(sd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	soBox.back()->SetDynamical(false);
	soBox.back()->SetFramePosition(Vec3f(-0.2, 0.2, -0.2));
	soTarget.push_back(soBox.back());

	//	�o�l
	PHSpringDesc spd;
	spd.damper = Vec3f(1,1,1) * 1000;
	spd.spring = Vec3f(1,1,1) * 3000;
	spd.poseSocket.Pos() = Vec3d(0, -0.09, 0);
	jntLink.push_back(scene->CreateJoint(soBox[8], soBox[6], spd));
	//	�o�l
	spd.poseSocket.Pos() = Vec3d(0, -0.09, 0);
	jntLink.push_back(scene->CreateJoint(soBox[13], soBox[12], spd));



	// �d�͂�ݒ�
	scene->SetGravity(Vec3f(0, -9.8, 0));
	//	�ڐG��؂�
	scene->SetContactMode(&soTarget[0], PHSceneDesc::MODE_NONE);
	scene->SetContactMode(&soTarget[1], PHSceneDesc::MODE_NONE);
}


void OnKey(char key){
	switch(key){
	case 'z': soTarget[0]->SetFramePosition(Vec3f(0.0, 0.2, 0.0)); break;
	case 'x': soTarget[0]->SetFramePosition(Vec3f(0.1, 0.2, 0.0)); break;
	case 'c': soTarget[0]->SetFramePosition(Vec3f(0.2, 0.2, 0.0)); break;
	case 'v': soTarget[0]->SetFramePosition(Vec3f(0.3, 0.2, 0.0)); break;

	case 'a': soTarget[0]->SetFramePosition(Vec3f(0.0, 0.2, -0.2)); break;
	case 's': soTarget[0]->SetFramePosition(Vec3f(0.1, 0.2, -0.2)); break;
	case 'd': soTarget[0]->SetFramePosition(Vec3f(0.2, 0.2, -0.2)); break;
	case 'f': soTarget[0]->SetFramePosition(Vec3f(0.3, 0.2, -0.2)); break;

	case 'q': soTarget[0]->SetFramePosition(Vec3f(0.0, 0.3, -0.4)); break;
	case 'w': soTarget[0]->SetFramePosition(Vec3f(0.1, 0.3, -0.4)); break;
	case 'e': soTarget[0]->SetFramePosition(Vec3f(0.2, 0.3, -0.4)); break;
	case 'r': soTarget[0]->SetFramePosition(Vec3f(0.3, 0.3, -0.6)); break;


	case '.': soTarget[1]->SetFramePosition(Vec3f(-0.0, 0.2, 0.0)); break;
	case ',': soTarget[1]->SetFramePosition(Vec3f(-0.1, 0.2, 0.0)); break;
	case 'm': soTarget[1]->SetFramePosition(Vec3f(-0.2, 0.2, 0.0)); break;
	case 'n': soTarget[1]->SetFramePosition(Vec3f(-0.3, 0.2, 0.0)); break;

	case 'l': soTarget[1]->SetFramePosition(Vec3f(-0.0, 0.2, -0.2)); break;
	case 'k': soTarget[1]->SetFramePosition(Vec3f(-0.1, 0.2, -0.2)); break;
	case 'j': soTarget[1]->SetFramePosition(Vec3f(-0.2, 0.2, -0.2)); break;
	case 'h': soTarget[1]->SetFramePosition(Vec3f(-0.3, 0.2, -0.2)); break;

	case 'o': soTarget[1]->SetFramePosition(Vec3f(-0.0, 0.3, -0.4)); break;
	case 'i': soTarget[1]->SetFramePosition(Vec3f(-0.1, 0.3, -0.4)); break;
	case 'u': soTarget[1]->SetFramePosition(Vec3f(-0.2, 0.3, -0.4)); break;
	case 'y': soTarget[1]->SetFramePosition(Vec3f(-0.3, 0.3, -0.6)); break;

	case 'p': 
		bTimer = false;
		step();
		break;
	case 'P': 
		bTimer = true;
		break;
	}
}

/**
 brief     	glutDisplayFunc�Ŏw�肵���R�[���o�b�N�֐�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void display(){
	render->ClearBuffer();
	render->DrawScene(scene);
	render->EndScene();
}

/**
 brief     	�����̐ݒ�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void setLight() {
	GRLightDesc light0, light1;
	light0.position = Vec4f(10.0, 20.0, 20.0, 1.0);
	light1.position = Vec4f(-10.0, 10.0, 10.0, 1.0);
	render->PushLight(light0);
	render->PushLight(light1);
}

/**
 brief     	����������
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void initialize(){
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	setLight();
}

/**
 brief		glutReshapeFunc�Ŏw�肵���R�[���o�b�N�֐�
 param		<in/--> w�@�@��
 param		<in/--> h�@�@����
 return		�Ȃ�
 */
void reshape(int w, int h){
	// Viewport�Ǝˉe�s���ݒ�
	render->Reshape(Vec2f(), Vec2f(w,h));
}

/**
 brief 		glutKeyboardFunc�Ŏw�肵���R�[���o�b�N�֐� 
 param		<in/--> key�@�@ ASCII�R�[�h
 param 		<in/--> x�@�@�@ �L�[�������ꂽ���̃}�E�X���W
 param 		<in/--> y�@�@�@ �L�[�������ꂽ���̃}�E�X���W
 return 	�Ȃ�
 */
void keyboard(unsigned char key, int x, int y){
	unsigned int i = 0;
	switch (key) {
		//�I��
		case ESC:		
			exit(0);
			break;
		default:
			OnKey(key);
			break;
	}
}	


/**
 brief		���C���֐�
 param		<in/--> argc�@�@�R�}���h���C�����͂̌�
 param		<in/--> argv�@�@�R�}���h���C������
 return		0 (����I��)
 */
int main(int argc, char* argv[]){
	// SDK�̍쐬�@
	phSdk = PHSdkIf::CreateSdk();
	grSdk = GRSdkIf::CreateSdk();
	// �V�[���I�u�W�F�N�g�̍쐬
	PHSceneDesc dscene;
	dscene.timeStep = dt;
	dscene.numIteration = niter;
	scene = phSdk->CreateScene(dscene);				// �V�[���̍쐬
	BuildScene();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	int window = glutCreateWindow("Joints");

	render = grSdk->CreateDebugRender();
	device = grSdk->CreateDeviceGL(window);

	// �����ݒ�
	device->Init();

	glutTimerFunc(20, timer, 0);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	//glutIdleFunc(idle);
	
	render->SetDevice(device);	// �f�o�C�X�̐ݒ�
	//	�J�����̐ݒ�
	GRCameraDesc cam;
	cam.size = Vec2d(0.05, 0);
	render->SetCamera(cam);	
	initialize();

	// ���_�̐ݒ�
	Affinef view;
	view.Pos() = Vec3f(1.0, 0.8, -2.1);								// �ڂ̈ʒu�i�ώ@�p�j
//	view.Pos() = Vec3f(0.0, 3.0,  0.1);								// �i�m�F�p�j
	view.LookAtGL(Vec3f(0.0, 0.3, 0.0), Vec3f(0.0, 1.0, 0.0));		// center, up 
	view = view.inv();	
	render->SetViewMatrix(view);

	glutMainLoop();
}
