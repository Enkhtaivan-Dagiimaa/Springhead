/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 Springhead2/src/tests/FileIO/FILoadGRScene/main.cpp

�y�T�v�z
  �EX�t�@�C�������[�h���AGraphics�V�[�������[�h�E�`�悷��B
  
�y�I����z
  �E�v���O����������I��������0��Ԃ��B  
 
�y�����̗���z
  �EX�t�@�C�������[�h����B
  �E���[�h���������o�͂���B
  �EGraphics�G���W���Ɛڑ����A�`�悷��B

�yNote�z
  �E�{�t�@�C����30�s�� " #define TEST_FILEX " �ɂāA���̓t�@�C�������w�肷��B
  
*/
#include <Springhead.h>
#include <GL/glut.h>
#define	ESC				27				// Esc key
#define EXIT_TIMER	12000			// �����I��������X�e�b�v��
#define TEST_FILEX	"GRTest.x"		// ���[�h����X�t�@�C��

namespace Spr{
	PHSdkIf* phSdk;
	GRSdkIf* grSdk;
	GRSceneIf* scene;
	GRDeviceGLIf* grDevice;
	GRDebugRenderIf* render;
	void PHRegisterTypeDescs();
	void CDRegisterTypeDescs();
	void GRRegisterTypeDescs();
	void FIRegisterTypeDescs();
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
		std::cout << "scene == NULL. File may not found." << std::endl;
		exit(-1);
	}
	render->EndScene();
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
	if ('0'<= key && key <= '9' && phSdk && phSdk->NScene()){
		int i = key-'0';
		static UTRef<Spr::ObjectStatesIf> states[10];
		if (states[i]){
			states[i]->LoadState(scene);
		}else{
			states[i] = CreateObjectStates();
			states[i]->SaveState(scene);
		}
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
	static int count;
	count ++;
	if (count > EXIT_TIMER){
		std::cout << EXIT_TIMER << " count passed." << std::endl;
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
	PHSdkIf::RegisterSdk();
	GRSdkIf::RegisterSdk();
	FWSdkIf::RegisterSdk();

	FISdkIf* fiSdk = FISdkIf::CreateSdk();
	FIFileXIf* fileX = fiSdk->CreateFileX();
	ObjectIfs objs;
	if (argc>=2){
		phSdk = PHSdkIf::CreateSdk();			//	PHSDK��p�ӂ��āC
		objs.push_back(phSdk);		
		fileX->Load(objs, argv[1]);				//	�t�@�C�����[�_�ɓn������
	}else{
		if (! fileX->Load(objs, TEST_FILEX) ) {	//	PHSDK���ƃ��[�h���āC
			DSTR << "Error: Cannot open load file. " << std::endl;
			exit(EXIT_FAILURE);
		}
		phSdk = NULL;
		grSdk = NULL;
		for(unsigned  i=0; i<objs.size(); ++i){	
			if(!phSdk) phSdk = DCAST(PHSdkIf, objs[i]);	//	PHSDK���󂯎�����
			if(!grSdk) grSdk = DCAST(GRSdkIf, objs[i]);	//	GRSdk���󂯎��
		}
	}
	if (!grSdk) return -1;
	objs.clear();
	objs.Push(grSdk);
	fileX->Save(objs, "out.x");
	
	fiSdk->Clear();	//	�t�@�C�����[�_�̃�����������D
	objs.clear();
	grSdk->Print(DSTR);
	scene = grSdk->GetScene(0);		// Scene�̎擾
	DSTR << "Loaded : " << "NScene=" << (int)grSdk->NScene() << std::endl;
	scene->Print(DSTR);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	int window = glutCreateWindow("FILoadGRScene");

	render = grSdk->CreateDebugRender();
	grDevice = grSdk->CreateDeviceGL(window);
	grDevice->Init();
	render->SetDevice(grDevice);

	// ���_�ݒ�
	Affinef view;
	view.Pos() = Vec3f(0.0, 3.0, 3.0);									// eye
		view.LookAtGL(Vec3f(0.0, 0.0, 0.0), Vec3f(0.0, 1.0, 0.0));		// center, up 
	view = view.inv();	
	render->SetViewMatrix(view);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}
