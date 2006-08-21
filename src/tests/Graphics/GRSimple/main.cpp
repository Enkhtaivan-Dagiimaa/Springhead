/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 Springhead2/src/tests/Graphics/GRSimple/main.cpp

�y�T�v�z
  �O���t�B�b�N�X�����_���[�N���X��API���g���AGL�f�o�C�X�Ń����_�����O���s���B�@
  
�y�I����z
  �E5000�X�e�b�v��ɋ����I���B

�y�����̗���z
  �E�V�~�����[�V�����ɕK�v�ȏ��(���̂̌`��E���ʁE�����e���\���Ȃ�)��ݒ肷��B  
  �E�^����ꂽ�����ɂ�臙t�b��̈ʒu�̕ω���ϕ����AOpenGL�ŃV�~�����[�V��������B

 */
#include <Springhead.h>		//	Springhead�̃C���^�t�F�[�X
#include <ctime>
#include <string>
#include <GL/glut.h>
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif
using namespace Spr;
#define ESC				27
#define EXIT_TIMER		5000
#define WINSIZE_WIDTH	480
#define WINSIZE_HEIGHT	360
#define NUM_BLOCKS		5

GRSdkIf* grSdk;
GRDebugRenderIf* render;
GRDeviceGLIf* grDevice;

PHSdkIf* phSdk;
PHSceneIf* scene;
PHSolidIf* soFloor;
std::vector<PHSolidIf*> soBlock;

// �ގ��̐ݒ�
GRMaterialDesc matFloor(Vec4f(0.2, 0.2, 0.2, 1.0),		// ambient
					Vec4f(0.6, 0.6, 0.6, 1.0),		// diffuse
					Vec4f(0.2, 0.2, 0.2, 1.0),		// specular
					Vec4f(0.0, 0.0, 0.0, 1.0),		// emissive
					10.0);							// power
GRMaterialDesc matBlock(Vec4f(0.5, 0.5, 0.7, 0.9),		
					Vec4f(0.1, 0.5, 0.8, 0.9),		
					Vec4f(1.0, 1.0, 1.0, 0.9),			
					Vec4f(0.0, 0.0, 0.0, 1.0),		
					10.0);
GRMaterialDesc matLine(Vec4f(1.0, 1.0, 1.0, 1.0),		
					Vec4f(1.0, 1.0, 1.0, 1.0),		
					Vec4f(1.0, 1.0, 1.0, 1.0),		
					Vec4f(0.0, 0.0, 0.0, 1.0),		
					100.0);	


// �J�����̐ݒ�
GRCameraDesc camera2(Vec2f(1, 0), Vec2f(0.0, 0.0), 1.0, 5000.0);


/**
 brief     	glutDisplayFunc�Ŏw�肵���R�[���o�b�N�֐�
 param		�Ȃ�
 return 	�Ȃ�
 */
void display(){
	//	�o�b�t�@�N���A
	render->ClearBuffer();

	// ���_���Đݒ肷��
	Affinef view;
	view.Pos() = Vec3f(5.0, 15.0, 15.0);								// eye
	view.LookAtGL(Vec3f(0.0, 0.0, 0.0), Vec3f(0.0, 1.0, 0.0));			// center, up 
	view = view.inv();	
	render->SetViewMatrix(view);

	//-----------------------------------
	//		��(soFloor) 
	//-----------------------------------
	render->SetAlphaTest(true);
	render->SetAlphaMode(render->BF_ONE, render->BF_ZERO);

	render->SetMaterial(matFloor);		// �}�e���A���ݒ�
	render->DrawSolid(soFloor);

	//-----------------------------------
	//		�u���b�N(soBlock)
	//-----------------------------------
	render->SetDepthWrite(false); 
	render->SetAlphaMode(render->BF_SRCALPHA, render->BF_ONE);
	for(unsigned int blockCnt=0; blockCnt<NUM_BLOCKS; ++blockCnt){
		render->SetMaterial(matBlock);
		render->DrawSolid(soBlock[blockCnt]);
	}

	render->SetDepthWrite(true);
	render->SetAlphaTest(false);

	//-----------------------------------
	//				��
	//-----------------------------------
	render->SetMaterial(matLine);
	Vec3f vtx[4] = {Vec3f(0,0,0), Vec3f(10,0,0), Vec3f(0,10,0), Vec3f(0,0,10)};
	size_t vtxIndex[6] = {0, 1, 0, 2, 0, 3};
	render->SetLineWidth(2.0);
	render->SetVertexFormat(GRVertexElement::vfP3f);
	render->DrawIndexed(render->LINES, vtxIndex, vtx, 6);


	//-----------------------------------
	//		�e�L�X�g�`��/�t�H���g
	//-----------------------------------
	GRFont font1;
	font1.height = 30;
	font1.width	= 0;
	font1.weight = 400;
	font1.color  = 0xFFFFFF;
	font1.bItalic = true;
	font1.face   = "ARIAL";
	std::string str = "X";
	render->DrawFont(Vec3f(8.0, 1.0, -1.0), str, font1);	
	font1.face = "�l�r ����";
	font1.color = 0xFFFF00;
	str = "Y";
	render->DrawFont(Vec3f(1.0, 7.0, 0.0), str, font1);		
	GRFont font2;
	font2 = font1;
	font2.color = 0x00FFFF;
	str = "Z";
	render->DrawFont(Vec3f(-1.0, 1.0, 9.0), str, font2);	

	render->EndScene();
}

/**
 brief		�����̐ݒ�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void setLight() {
	GRLightDesc light0;
	light0.ambient	= Vec4f(0.0, 0.0, 0.0, 1.0);
	light0.diffuse	= Vec4f(0.7, 1.0, 0.7, 1.0);
	light0.specular	= Vec4f(1.0, 1.0, 1.0, 1.0);
	light0.position = Vec4f(10.0, 20.0, 20.0, 1.0);
	light0.attenuation0  = 1.0;
	light0.attenuation1  = 0.0;
	light0.attenuation2  = 0.0;
	light0.spotDirection = Vec3f(-2.0, -3.0, -5.0);
	light0.spotFalloff   = 0.0;
	light0.spotCutoff	 = 70.0;
	render->PushLight(light0);

	GRLightDesc light1;
	light1.ambient	= Vec4f(0.5, 0.0, 1.0, 1.0);
	light1.diffuse	= Vec4f(1.0, 0.0, 1.0, 1.0);
	light1.specular	= Vec4f(0.0, 1.0, 0.0, 1.0);
	light1.position = Vec4f(-10.0, 10.0, 10.0, 1.0);
	render->PushLight(light1);
}
/**
 brief		glutReshapeFunc�Ŏw�肵���R�[���o�b�N�֐�
 param		<in/--> w�@�@��
 param		<in/--> h�@�@����
 return		 �Ȃ�
 */
void reshape(int w, int h){
	// Viewport�Ǝˉe�s���ݒ�
	render->Reshape(Vec2f(0,0), Vec2f(w,h));
}
/**
 brief 		glutKeyboardFunc�Ŏw�肵���R�[���o�b�N�֐� 
 param		<in/--> key�@�@ ASCII�R�[�h
 param 		<in/--> x�@�@�@ �L�[�������ꂽ���̃}�E�X���W
 param 		<in/--> y�@�@�@ �L�[�������ꂽ���̃}�E�X���W
 return 	�Ȃ�
 */
void keyboard(unsigned char key, int x, int y){
	if (key == ESC) exit(0);

}	
/**
 brief  	glutIdleFunc�Ŏw�肵���R�[���o�b�N�֐�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void idle(){
	scene->Step();
	glutPostRedisplay();
	static int count;
	count++;
	if (++count > EXIT_TIMER) exit(0);
}
/**
 brief		���C���֐�
 param		<in/--> argc�@�@�R�}���h���C�����͂̌�
 param		<in/--> argv�@�@�R�}���h���C������
 return		0 (����I��)
 */
int main(int argc, char* argv[]){
	phSdk = CreatePHSdk();					// SDK�̍쐬�@
	PHSceneDesc sd;
	sd.timeStep = 0.01;
	scene = phSdk->CreateScene(sd);				// �V�[���̍쐬
	PHSolidDesc desc;
	desc.mass = 2.0;
	desc.inertia *= 2.0;

	unsigned int blockCnt;
	for (blockCnt=0; blockCnt<NUM_BLOCKS; ++blockCnt){
		soBlock.push_back(scene->CreateSolid(desc));		// ���̂�desc�Ɋ�Â��č쐬
	}

	desc.mass = 1e20f;
	desc.inertia *= 1e20f;
	soFloor = scene->CreateSolid(desc);		// ���̂�desc�Ɋ�Â��č쐬
	soFloor->SetGravity(false);
	
	//	�`��̍쐬
	CDConvexMeshIf* meshFloor=NULL;
	CDConvexMeshIf* meshBlock=NULL;
	{
		CDConvexMeshDesc md;
		md.vertices.push_back(Vec3f(-1,-1,-1));
		md.vertices.push_back(Vec3f(-1,-1, 1));	
		md.vertices.push_back(Vec3f(-1, 1,-1));	
		md.vertices.push_back(Vec3f(-1, 1, 1));
		md.vertices.push_back(Vec3f( 1,-1,-1));	
		md.vertices.push_back(Vec3f( 1,-1, 1));
		md.vertices.push_back(Vec3f( 1, 1,-1));
		md.vertices.push_back(Vec3f( 1, 1, 1));
		meshBlock = DCAST(CDConvexMeshIf, phSdk->CreateShape(md));

		// soFloor(meshFloor)�ɑ΂��ăX�P�[�����O
		for(unsigned i=0; i<md.vertices.size(); ++i){
			md.vertices[i].x *= 30;
			md.vertices[i].z *= 30;
		}
		meshFloor = DCAST(CDConvexMeshIf, phSdk->CreateShape(md));
	}
	
	soFloor->AddShape(meshFloor);
	soFloor->SetFramePosition(Vec3f(0,-3,0));
	for (blockCnt=0; blockCnt<NUM_BLOCKS; ++blockCnt){
		soBlock[blockCnt]->AddShape(meshBlock);
		soBlock[blockCnt]->SetFramePosition(Vec3f(3, 15*(blockCnt+1), 3));
	}

	scene->SetGravity(Vec3f(0,-9.8f, 0));	// �d�͂�ݒ�

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(WINSIZE_WIDTH, WINSIZE_HEIGHT);
	int window = glutCreateWindow("GRSimple");
	grSdk = CreateGRSdk();
	render = grSdk->CreateDebugRender();
	grDevice = grSdk->CreateDeviceGL(window);

	// �����ݒ�
	grDevice->Init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	
	render->SetDevice(grDevice);	
	render->SetCamera(camera2);	

	setLight();

	glutMainLoop();

	//	SDK�͊J�����Ȃ��Ă��ǂ��D���Ȃ��Ă�main�𔲂��Ă���J�������D
	delete phSdk;
}
