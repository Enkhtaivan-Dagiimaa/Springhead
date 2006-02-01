/**
 Springhead2/src/tests/Physics/PHShapeGL/main.cpp

�y�T�v�z
  ����Solid�Ɍ`������������e�X�g�v���O�����i�ʒu���o�́A�ʂ̒��_���W���o�́AGL�\���j
�@�E�y�i���e�B�@�ɂ��ʑ��ʑ̓��m�̐ڐG����ƐڐG�͂��m�F����B
  �E���̂����R���������A���̏��2�̃u���b�N��ςݏグ��B
  �E���_���W���f�o�b�O�o�͂����AOpenGL�ŃV�~�����[�V�������s���B
  
�y�I����z
  �E���R�������������̂����̏��5�b�ԐÎ~�����琳��I���Ƃ���B
  �E���R�������������̂����̏�ŐÎ~�����A-500m�n�_�܂ŗ��������ꍇ�A�ُ�I���Ƃ���B
 
�y�����̗���z
  �E�V�~�����[�V�����ɕK�v�ȏ��(���̂̌`��E���ʁE�����e���\���Ȃ�)��ݒ肷��B
  �@���̂̌`���OpenGL�Ŏw�肷��̂ł͂Ȃ��ASolid���̂Ŏ�������B  
  �E�^����ꂽ�����ɂ�臙t�b��̈ʒu�̕ω���ϕ����AOpenGL�ŃV�~�����[�V��������B
�@�E�f�o�b�O�o�͂Ƃ��āA���ʑ̖̂�(�O�p�`)�̒��_���W���o�͂���B   
 
 */

#include <Springhead.h>		//	Springhead�̃C���^�t�F�[�X
#include <ctime>
#include <string>
#include <gl/glut.h>
#pragma hdrstop
using namespace Spr;

#define ESC				27
#define FALL_DISTANCE	-500		// ��������(�ُ�I����)

GRSdkIf* grSdk;
GRDebugRenderIf* render;
GRDeviceGLIf* grDevice;

PHSdkIf* phSdk;
PHSceneIf* scene;
PHSolidIf* soFloor, *soBlock;

// �����̐ݒ� 
static GLfloat light_position[] = { 15.0, 30.0, 20.0, 1.0 };
static GLfloat light_ambient[]  = { 0.0, 0.0, 0.0, 1.0 };
static GLfloat light_diffuse[]  = { 1.0, 1.0, 1.0, 1.0 }; 
static GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
// �ގ��̐ݒ�
static GLfloat mat_red[]        = { 1.0, 0.0, 0.0, 1.0 };
static GLfloat mat_blue[]       = { 0.0, 0.0, 1.0, 1.0 };
static GLfloat mat_specular[]   = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat mat_shininess[]  = { 120.0 };

static clock_t starttime, endtime, count;
static bool timeflag = false;

/**
 brief     	glutDisplayFunc�Ŏw�肵���R�[���o�b�N�֐�
 param		�Ȃ�
 return 	�Ȃ�
 */
void display(){
	/*	�o�b�t�@�N���A
	 *	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);�@������ŃR�[��. */
	grDevice->ClearBuffer();


	glMaterialf(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, (1.f,1.f,1.f,1.f));
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	Affined ad;
#if 0	
	Affinef view;
	view.Pos() = Vec3f(5.0, 5.0, 10.0);	                        // eye
	view.LookAtGL(Vec3f(0.0, 0.0, 0.0), Vec3f(0.0, 1.0, 0.0));		// center, up 
	// �r���[�s��̋t�s������[���h�s��Ƃ���
	view = view.inv();	
	render->SetViewMatrix(view);
#endif
	/********************************
	 *     ���̐Ԃ�����(soFloor)    *
	 ********************************/
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_red);
	render->PushModelMatrix();		// �s��X�^�b�N���v�b�V��
	Posed pose = soFloor->GetPose();
	pose.ToAffine(ad);

	// MultModelMatrix(ad); �́AglMultMatrixd(ad); �Ɠ���
	//grDevice->MultModelMatrix(ad);		// ���݂�modelView * model
	grDevice->SetModelMatrix(ad);

	render->DrawSolid(soFloor);
	render->PopModelMatrix();		// �s��X�^�b�N���|�b�v

	/********************************
	 *     ��̐�����(soBlock)    *
	 ********************************/
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_blue);
	render->PushModelMatrix();
	pose = soBlock->GetPose();
	ad = Affined(pose);

	// ������glMultMatrixd(ad); ���R�[��.
	//grDevice->MultModelMatrix(ad); �Ƃ��Ă����l.
	grDevice->SetModelMatrix(ad);		// view * model

	render->DrawSolid(soBlock);
	render->PopModelMatrix();

	/* �_�u���o�b�t�@���[�h���A�J�����g�E�B���h�E�̃o�b�t�@�������s���A
	   scene�̃����_�����O���I��. glutSwapBuffers();������ŃR�[��. */
	render->EndScene();
}

/**
 brief		�����̐ݒ�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void setLight() {
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

/**
 brief     	����������
 param	 	�Ȃ�
 return 	�Ȃ�
 */
#if 1
void initialize(){
/*	Vec3f eye(0.0, 0.0, 3.0);
	Vec3f center(0.0, 0.0, 0.0);
	Vec3f up(0.0, 1.0, 0.0);
*/	glClearColor(0.0, 0.0, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0, 3.0, 9.0, 
		      0.0, 0.0, 0.0,
		 	  0.0, 1.0, 0.0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	setLight();
}
#endif

/**
 brief		glutReshapeFunc�Ŏw�肵���R�[���o�b�N�֐�
 param		<in/--> w�@�@��
 param		<in/--> h�@�@����
 return		 �Ȃ�
 */
void reshape(int w, int h){
	// Viewport�Ǝˉe�s���ݒ�
	grDevice->Resize(Vec2f(w,h));

	/*glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)w/(GLfloat)h, 1.0, 500.0);
	glMatrixMode(GL_MODELVIEW);*/
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
	soBlock = scene->CreateSolid(desc);		// ���̂�desc�Ɋ�Â��č쐬

	Posed p = Posed::Rot(Rad(0.0), 'z');
	soBlock->SetPose(p);

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
		meshBlock = ICAST(CDConvexMeshIf, scene->CreateShape(md));

		// soFloor(meshFloor)�ɑ΂��ăX�P�[�����O
		for(unsigned i=0; i<md.vertices.size(); ++i){
			md.vertices[i].x *= 3;
			md.vertices[i].z *= 3;
		}
		meshFloor = ICAST(CDConvexMeshIf, scene->CreateShape(md));
	}

	soFloor->AddShape(meshFloor);
	soBlock->AddShape(meshBlock);
	soFloor->SetFramePosition(Vec3f(0,-1,0));
	soBlock->SetFramePosition(Vec3f(-0.5,5,0));
	soBlock->SetOrientation(Quaternionf::Rot(Rad(30), 'z'));

	scene->SetGravity(Vec3f(0,-9.8f, 0));	// �d�͂�ݒ�

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(480, 360);
	int window = glutCreateWindow("PHShapeGL");
	grSdk = CreateGRSdk();
	render = grSdk->CreateDebugRender();
	grDevice = grSdk->CreateDeviceGL(window);

	// �����ݒ�
	// initialize();
	grDevice->Init();

	setLight();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);


	render->SetDevice(grDevice);	

	glutMainLoop();

	//	SDK�͊J�����Ȃ��Ă��ǂ��D���Ȃ��Ă�main�𔲂��Ă���J�������D
	delete phSdk;
}
