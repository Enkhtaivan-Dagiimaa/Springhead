/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 Springhead2/src/tests/FileIO/FILoadMesh/main.cpp

�y�T�v�z
  �t�@�C�����o��SDK�ŁAX�t�@�C���̏ڍׂ���͂�����B
  �i���_�ʒu�A���_�C���f�b�N�X�A�@���A�}�e���A���A�e�N�X�`���j

�y�I����z
  �E�v���O����������I��������0��Ԃ��B  

�yNote�z
  �{�t�@�C����26�s��" #define TEST_CASE " �ŁA���[�h����X�t�@�C���̓��͐؂�ւ����\�B

*/
#include <Springhead.h>
#include <GL/glut.h>
#define	ESC				27				// Esc key
#define EXIT_TIMER	20000
#define TEST_FILECOLLADA	"test1.dae"


namespace Spr{
	UTRef<GRSdkIf> grSdk;
	UTRef<GRSceneIf> scene;
	UTRef<FISdkIf> fiSdk; 
	GRDeviceGLIf* grDevice;
	GRDebugRenderIf* render;
}
using namespace Spr;


/**
 brief     	glutDisplayFunc�Ŏw�肵���R�[���o�b�N�֐�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void display(){
	//	�o�b�t�@�N���A
	render->ClearBuffer();
	scene->Render(render);
	if (!scene){
		DSTR << "scene == NULL. File may not found." << std::endl;
		exit(-1);
	}
	render->EndScene();
	glutSwapBuffers();
}

/**
 brief		�����̐ݒ�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void setLight() {
	GRLightDesc light0;
	light0.ambient	= Vec4f(0.2, 0.2, 0.2, 1.0);
	light0.diffuse	= Vec4f(0.8, 0.8, 0.8, 1.0);
	light0.specular	= Vec4f(1.0, 1.0, 1.0, 1.0);
	light0.position = Vec4f(-10.81733, 100.99039, -21.19059, 0.0);
	light0.attenuation0  = 1.0;
	light0.attenuation1  = 0.0;
	light0.attenuation2  = 0.0;
	light0.spotDirection = Vec3f(0.0, -1.0, 0.0);
	light0.spotFalloff   = 10.0;
	light0.spotInner	 = 20;
	light0.spotCutoff	 = 150.0;
	render->PushLight(light0);

	GRLightDesc light1;
	light1.ambient	= Vec4f(0.2, 0.2, 0.2, 1.0);
	light1.diffuse	= Vec4f(0.8, 0.8, 0.8, 1.0);
	light1.specular	= Vec4f(1.0, 1.0, 1.0, 1.0);
	light1.position = Vec4f(-1.85930, 100.10970, -49.58080, 0.0);
	light1.attenuation0  = 1.0;
	light1.attenuation1  = 0.0;
	light1.attenuation2  = 0.0;
	light1.spotDirection = Vec3f(0.0, -1.0, 0.0);
	light1.spotFalloff   = 0.0;
	light1.spotInner	 = 10;
	light1.spotCutoff	 = 150.0;
	render->PushLight(light1);
}

/**
 brief  	glutReshapeFunc�Ŏw�肵���R�[���o�b�N�֐�
 param	 	<in/--> w�@�@��
 param  	<in/--> h�@�@����
 return 	�Ȃ�
 */
void reshape(int w, int h){
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
	if (key == ESC) {
		exit(0);
	}
}

/**
 brief  	glutIdleFunc�Ŏw�肵���R�[���o�b�N�֐�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void idle(){
//	if(scene && *scene) (*(scene))->Step();
	glutPostRedisplay();
	static int count=0;
	count++;
	if (count > EXIT_TIMER){
		DSTR << EXIT_TIMER << " count passed." << std::endl;
		exit(0);
	}
}

/**
 brief		���C���֐�
 param		<in/--> argc�@�@�R�}���h���C�����͂̌�
 param		<in/--> argv�@�@�R�}���h���C������
 return		0 (����I��)
 */
int main(int argc, char* argv[]){
	FWSdkIf::RegisterSdk();

	fiSdk = FISdkIf::CreateSdk();
	FIFileCOLLADAIf* fileCOLLADA = fiSdk->CreateFileCOLLADA();
	ObjectIfs objs;

	grSdk = GRSdkIf::CreateSdk();	
	objs.push_back(grSdk);
	scene = grSdk->CreateScene();
	objs.push_back(scene);
	fileCOLLADA->Load(objs, TEST_FILECOLLADA);

	if (!grSdk) return -1;
	objs.clear();
	objs.Push(grSdk);
	fileCOLLADA->Save(objs, "out.dae");
	
	fiSdk->Clear();	//	�t�@�C�����[�_�̃�����������D
	objs.clear();
	grSdk->Print(DSTR);
	//scene = grSdk->GetScene(0);		// Scene�̎擾
	
	grSdk->Print(DSTR);

	DSTR << "Loaded : " << "NScene=" << (int)grSdk->NScene() << std::endl;
	scene->Print(DSTR);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	int window = glutCreateWindow("FILoadMesh");

	render = grSdk->CreateDebugRender();
	grDevice = grSdk->CreateDeviceGL();
	grDevice->Init();
	render->SetDevice(grDevice);

	// ���_�ݒ�
	Affinef view;
	view.Pos() = Vec3f(0.0, 3.0,-80.0);									// eye
		view.LookAtGL(Vec3f(0.0, 0.0, 0.0), Vec3f(0.0, 1.0, 0.0));		// center, up 
	view = view.inv();	
	render->SetViewMatrix(view);

	// �����̐ݒ�
	setLight();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}
