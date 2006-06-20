/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 Springhead2/src/tests/FileIO/FILoader/main.cpp

�y�T�v�z
  X�t�@�C�������[�h���APhysics�G���W���Ɛڑ����ăV�~�����[�V��������B
  
�y�I����z
  �E�v���O����������I��������0��Ԃ��B  
 
�y�����̗���z
  �EX�t�@�C�������[�h����B
  �E���[�h���������o�͂���B
  �EPhysics�G���W���Ɛڑ����A�V�~�����[�V����������B

�y�e�X�g�p�^�[���z
  �� �{�t�@�C����28�s�� " #define TEST_FILEX " �ɂāA���̓t�@�C�������w�肷��B
�@  test1.x  : �ʌ`��(mesh)�̃e�X�g
    test2.x  : �ʌ`��(mesh)�̃e�X�g�i���݂��Փ˂����Ȃ����̂Ƃ��āA"soBlock2"��"soFloor"��ݒ�j
    test3.x  : �ʌ`��(mesh)�Ƌ�(sphere)�̃e�X�g
	test4.x  : �ʌ`��(mesh)�Ƌ�(sphere)�ƒ�����(box)�̃e�X�g
	test5.x  : timeStep=0.001�ɂ�����e�X�g
	GRTest.x : X�t�@�C������̃O���t�B�b�N�XSDK�̓��̓e�X�g

 */
#include <Springhead.h>
#include <GL/glut.h>
#define	ESC				27				// Esc key
#define EXIT_TIMER	12000			// �����I��������X�e�b�v��
#define TEST_FILEX	"test1.x"		// ���[�h����X�t�@�C��

namespace Spr{
	PHSdkIf* phSdk;
	GRSdkIf* grSdk;
	PHSceneIf* scene;
//	CDShapeIf** shape;
	GRDeviceGLIf* grDevice;
	GRDebugRenderIf* render;
	void PHRegisterTypeDescs();
	void CDRegisterTypeDescs();
	void GRRegisterTypeDescs();
	void FIRegisterTypeDescs();
}
using namespace Spr;

Vec3f *vtx=NULL;


// �ގ��̐ݒ�
GRMaterialDesc mat_red(Vec4f(0.0, 0.0, 0.0, 1.0),		// ambient
						Vec4f(0.7, 0.0, 0.0, 1.0),		// diffuse
						Vec4f(1.0, 1.0, 1.0, 1.0),		// specular
						Vec4f(0.0, 0.0, 0.0, 1.0),		// emissive
						100.0);							// power
GRMaterialDesc mat_green(Vec4f(0.0, 0.0, 0.0, 1.0),		
						Vec4f(0.0, 0.7, 0.0, 1.0),		
						Vec4f(1.0, 1.0, 1.0, 1.0),		
						Vec4f(0.0, 0.0, 0.0, 1.0),		
						20.0);							
GRMaterialDesc mat_blue(Vec4f(0.0, 0.0, 1.0, 1.0),		
						Vec4f(0.0, 0.0, 0.7, 1.0),		
						Vec4f(1.0, 1.0, 1.0, 1.0),		
						Vec4f(0.0, 0.0, 0.0, 1.0),		
						20.0);							
GRMaterialDesc mat_yellow(Vec4f(0.0, 0.0, 1.0, 1.0),		
						  Vec4f(1.0, 0.7, 0.0, 1.0),		
						  Vec4f(1.0, 1.0, 1.0, 1.0),		
						  Vec4f(0.0, 0.0, 0.0, 1.0),		
						  20.0);							
std::vector<GRMaterialDesc> material;

/**
 brief     	glutDisplayFunc�Ŏw�肵���R�[���o�b�N�֐�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void display(){
/*	render->ClearBuffer();
	render->DrawScene(*scene);
	render->EndScene();
	return;
*/
	//	�o�b�t�@�N���A
	render->ClearBuffer();
	//render->SetMaterial(mat_red);	

	PHSceneIf* scene = NULL;
	if (phSdk->NScene()){
		scene = phSdk->GetScene(0);
	}
	if (!scene){
		std::cout << "scene == NULL. File may not found." << std::endl;
		exit(-1);
	}
	PHSolidIf **solids = scene->GetSolids();
	for (int num=0; num < scene->NSolids(); ++num){
		render->SetMaterial(material[num]);			// �ގ��ݒ�

		Affinef af;
		solids[num]->GetPose().ToAffine(af);
		render->PushModelMatrix();
		render->MultModelMatrix(af);
		
		int nShape = solids[num]->NShape();
		for(int s=0; s<nShape; ++s){
			CDShapeIf* shape = solids[num]->GetShape(s);
			Affinef af;
			solids[num]->GetShapePose(s).ToAffine(af);
			render->PushModelMatrix();
			render->MultModelMatrix(af);
			CDConvexMeshIf* mesh = DCAST(CDConvexMeshIf, shape);
			if (mesh){
				Vec3f* base = mesh->GetVertices();
				for (size_t f=0; f<mesh->NFace(); ++f) {	
					CDFaceIf* face = mesh->GetFace(f);
					render->DrawFace(face, base);
				}
			}
			CDSphereIf* sphere = DCAST(CDSphereIf, shape);
			if (sphere){
				float r = sphere->GetRadius();
				GLUquadricObj* quad = gluNewQuadric();
				gluSphere(quad, r, 16, 8);
				gluDeleteQuadric(quad);
			}
			CDBoxIf* box = DCAST(CDBoxIf, shape);
			if (box){
#if 0			// glut�ɂ�钼���̂̕`���			
				Vec3f boxsize = box->GetBoxSize();
				glScalef(boxsize.x, boxsize.y, boxsize.z);	
				glutSolidCube(1.0);		
#else			// �f�o�b�O��
				Vec3f* base =  box->GetVertices();
				for (size_t f=0; f<6; ++f) {	
					CDFaceIf* face = box->GetFace(f);

					for (int v=0; v<4; ++v)
						vtx[v] = base[face->GetIndices()[v]].data;
					Vec3f normal, edge0, edge1;
					edge0 = vtx[1] - vtx[0];
					edge1 = vtx[2] - vtx[0];
					normal = edge0^edge1;
					normal.unitize();
					glNormal3fv(normal);
					render->DrawDirect(GRRenderBaseIf::QUADS, vtx, 4);
				}
#endif 					
			}
			render->PopModelMatrix();
		}
		render->PopModelMatrix();
	}

	render->EndScene();
}
/**
 brief     	�����̐ݒ�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void setLight() {
	GRLightDesc light0;
	light0.ambient			= Vec4f(0.0f, 0.0f, 0.0f, 1.0f);
	light0.diffuse			= Vec4f(0.7f, 0.7f, 0.7f, 1.0f);
	light0.specular			= Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
	light0.position			= Vec4f(0.0f, 10.0f, 0.0f, 1.0f);
	light0.spotDirection	= Vec3f(0.0f, -1.0f, 0.0f);
	light0.spotCutoff		= 145.0f;
	light0.spotFalloff		= 30.0f;
	render->PushLight(light0);

	GRLightDesc light1;
	light1.diffuse			= Vec4f(0.8f, 0.8f, 0.8f, 1.0f);
	light1.specular			= Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
	light1.position			= Vec4f(0.0f, 10.0f, 10.0f, 0.0f);
	light1.spotDirection	= Vec3f(0.0f, -1.0f, 0.0f);
	render->PushLight(light1);
}
/**
 brief     	�ގ��̐ݒ�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void setMaterial() {
	material.push_back(mat_red);
	material.push_back(mat_green);
	material.push_back(mat_blue);
	material.push_back(mat_yellow);
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
		delete[] vtx;
		exit(0);
	}
	if ('0'<= key && key <= '9' && phSdk->NScene()){
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
	if(scene) scene->Step();
	glutPostRedisplay();
	static int count;
	count ++;
	if (count > EXIT_TIMER){
		delete[] vtx;
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
	PHRegisterTypeDescs();
	CDRegisterTypeDescs();
	GRRegisterTypeDescs();

	FISdkIf* fiSdk = CreateFISdk();
	FIFileXIf* fileX = fiSdk->CreateFileX();
	ObjectIfs objs;
	if (argc>=2){
		phSdk = CreatePHSdk();					//	PHSDK��p�ӂ��āC
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
	if (phSdk && phSdk->NScene()){
		ObjectIfs objs;
//		objs.Push(phSdk->GetScenes()[0]);
		objs.Push(phSdk);
		if(grSdk) objs.Push(grSdk);
		fileX->Save(objs, "out.x");
	}
	fiSdk->Clear();	//	�t�@�C�����[�_�̃�����������D
	objs.clear();
	phSdk->Print(DSTR);

	scene = phSdk->GetScene(0);		// Scene�̎擾
//	shape = phSdk->GetShapes();		// Shape�̎擾
	DSTR << "Loaded : " << "NScene=" << phSdk->NScene() << ", NShape=" << phSdk->NShape() << std::endl;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	int window = glutCreateWindow("FILoad");

	// Graphics Sdk
	grSdk = CreateGRSdk();
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

	setLight();			// �����ݒ�
	setMaterial();		// �ގ��ݒ�

	vtx = DBG_NEW Vec3f[4];
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}
