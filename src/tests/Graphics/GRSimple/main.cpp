/**
 Springhead2/src/tests/Physics/PHShapeGL/main.cpp

�y�T�v�z
  �O���t�B�b�N�X�����_���[�N���X��API���g���AGL�f�o�C�X�Ń����_�����O���s���B�@
  
�y�I����z
  �E5�b��ɋ����I���B

�y�����̗���z
  �E�V�~�����[�V�����ɕK�v�ȏ��(���̂̌`��E���ʁE�����e���\���Ȃ�)��ݒ肷��B  
  �E�^����ꂽ�����ɂ�臙t�b��̈ʒu�̕ω���ϕ����AOpenGL�ŃV�~�����[�V��������B

 */
#include <Springhead.h>		//	Springhead�̃C���^�t�F�[�X
#include <ctime>
#include <string>
#include <WinBasis/WinBasis.h>
#include <windows.h>
#include <gl/glut.h>
#pragma hdrstop
using namespace Spr;
#define ESC				27
#define EXIT_TIMER		3000
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
GRMaterial mat_blue(Vec4f(0.5, 0.5, 0.7, 0.85),		// ambient	
					Vec4f(0.1, 0.5, 0.8, 0.85),		// diffuse			
					Vec4f(1.0, 1.0, 1.0, 0.85),		// specular		
					Vec4f(0.0, 0.0, 0.0, 1.0),		// emissive		
					100.0);							// power	

GRMaterial mat_white(Vec4f(0.5, 0.5, 0.5, 0.85),	
					 Vec4f(1.0, 1.0, 1.0, 1.0),			
					 Vec4f(0.8, 0.8, 0.8, 0.85),		
					 Vec4f(0.0, 0.0, 0.0, 0.85),		
					 20.0);

// �J�����̐ݒ�
GRCamera camera2(Vec2f(WINSIZE_WIDTH, WINSIZE_HEIGHT), Vec2f(0.0, 0.0), 1.0, 5000.0);


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

	Affined ad;
	//-----------------------------------
	//		��(soFloor) 
	//-----------------------------------
	render->SetAlphaTest(true);
	render->SetAlphaMode(render->BF_ONE, render->BF_ZERO);

	render->SetMaterial(mat_white);		// �}�e���A���ݒ�
	render->PushModelMatrix();			// �s��X�^�b�N���v�b�V��
	Posed pose = soFloor->GetPose();
	pose.ToAffine(ad);
	render->MultModelMatrix(ad);		// ���݂�modelView * model
	render->DrawSolid(soFloor);
	render->PopModelMatrix();			// �s��X�^�b�N���|�b�v

	//-----------------------------------
	//		�u���b�N(soBlock)
	//-----------------------------------
	render->SetDepthWrite(false);
	render->SetAlphaMode(render->BF_SRCALPHA, render->BF_INVSRCALPHA);

	for(unsigned int blockCnt=0; blockCnt<NUM_BLOCKS; ++blockCnt){
		render->SetMaterial(mat_blue);
		render->PushModelMatrix();
		pose = soBlock[blockCnt]->GetPose();
		ad = Affined(pose);
		render->SetModelMatrix(ad);			// view * model�i���̏ꍇ�Arender->MultModelMatrix(ad); �Ƃ��Ă����l�j
		render->DrawSolid(soBlock[blockCnt]);
		render->PopModelMatrix();
	}

	render->SetDepthWrite(true);
	render->SetAlphaTest(true);

	//-----------------------------------
	//				��
	//-----------------------------------
	render->SetMaterial(mat_white);
	Vec3f vtx[4] = {Vec3f(0,0,0), Vec3f(10,0,0), Vec3f(0,10,0), Vec3f(0,0,10)};
	size_t vtxIndex[6] = {0, 1, 0, 2, 0, 3};
	render->SetLineWidth(2.0);
	render->DrawIndexed(render->LINES, vtxIndex, vtxIndex + 6, vtx);

	//-----------------------------------
	//		�e�L�X�g�`��/�t�H���g
	//-----------------------------------
	GRFont font1;
	font1.height = 30.0;
	font1.width	= 0.0;
	font1.weight	= FW_NORMAL;
	font1.color  = 0xFFFFFF;
	font1.bItalic = TRUE;
	font1.face   = "ARIAL";
	std::string str = "X";
	render->DrawFont(Vec3f(10.0, 1.0, -1.0), str, font1);	
	font1.face = "�l�r ����";
	font1.color = 0xFFFF00;
	str = "Y";
	render->DrawFont(Vec3f(1.0, 10.0, 0.0), str, font1);		
	GRFont font2;
	font2 = font1;
	font2.color = 0x00FFFF;
	str = "Z";
	render->DrawFont(Vec3f(-2.0, 1.0, 10.0), str, font2);	



	render->EndScene();
}

/**
 brief		�����̐ݒ�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void setLight() {
	GRLight light0;
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

	GRLight light1;
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
	render->Reshape(Vec2f(w,h));
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
	scene = phSdk->CreateScene();				// �V�[���̍쐬
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
		meshBlock = ICAST(CDConvexMeshIf, phSdk->CreateShape(md));

		// soFloor(meshFloor)�ɑ΂��ăX�P�[�����O
		for(unsigned i=0; i<md.vertices.size(); ++i){
			md.vertices[i].x *= 30;
			md.vertices[i].z *= 30;
		}
		meshFloor = ICAST(CDConvexMeshIf, phSdk->CreateShape(md));
	}
	
	soFloor->AddShape(meshFloor);
	soFloor->SetFramePosition(Vec3f(0,-1,0));
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
