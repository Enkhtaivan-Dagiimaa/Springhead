/** 
 @file  Springhead2/src/tests/Physics/PHSimpleGL/main.cpp
 
@brief 2�̍��̂̈ʒu�̕ω����m�F����e�X�g�v���O�����i�ʒu���o��,GL�\���j
   
 <PRE>  
 <B>�e�X�g���e</B>
  �E�v���O����������I��������0��Ԃ��B
  
 <B>�v���O�����T�v</B>
  �E�V�~�����[�V�����ɕK�v�ȏ��(���ʁE�d�S�E�����e���\���Ȃ�)��ݒ肷��B
    ���̂̌`���OpenGL�Ńe�B�[�|�b�g���w�肷��B
  �E�V�~�����[�V�����������n�߂���A���L�����ō��̂ɗ͂�������B
      ���̐Ԃ��e�B�[�|�b�g�F�I�u�W�F�N�g�̃��[�J�����W�n�����_�Ƃ��A�d�S��1m����E��1N�̗͂�������B
      �E�̐��e�B�[�|�b�g�F�I�u�W�F�N�g�̃��[�J�����W�n�̌��_����1m�E�ɏd�S�����炵�A�d�S��1m����E��1N�̗͂�������B
  �E�^����ꂽ�����ɂ�臙t�b��̈ʒu�̕ω���ϕ����A���̂̈ʒu�����o�͂ƁAOpenGL�����_�����O���s���B
 </PRE>
   
 */
#include <Springhead.h>		//	Springhead�̃C���^�t�F�[�X
#include <gl/glut.h>
#pragma hdrstop
using namespace Spr;

#define ESC 27

PHSdkIf* sdk;
PHSceneIf* scene;
PHSolidIf* solid1, *solid2;
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

/**
 @brief     glutDisplayFunc�Ŏw�肵���R�[���o�b�N�֐�
 @param	 	�Ȃ�
 @return 	�Ȃ�
 */
void display(){

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glMaterialf(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, (1.f,1.f,1.f,1.f));
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	// ���̐Ԃ��e�B�[�|�b�g
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_red);

	Affined ad;
	glPushMatrix();
	Posed pose = solid1->GetPose();
	pose.ToAffine(ad);
	//	solid1->GetOrientation().to_matrix(af);
	//	af.Pos() = solid1->GetFramePosition();
	// pose�ł͐��x��double�Ȃ̂�matrixd�ɂ���
	glMultMatrixd(ad);
	glutSolidTeapot(1.0);
	glPopMatrix();

	// �E�̐��e�B�[�|�b�g
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_blue);

	glPushMatrix();
	pose = solid2->GetPose();
	ad = Affined(pose);
	glMultMatrixd(ad);
	glutSolidTeapot(1.0);
	glPopMatrix();

	glutSwapBuffers();
}

/**
 @brief     �����̐ݒ�
 @param	 	�Ȃ�
 @return 	�Ȃ�
 */
void setLight() {
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glEnable(GL_LIGHT0);
}

/**
 @brief     ����������
 @param	 	�Ȃ�
 @return 	�Ȃ�
 */
void initialize(){
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(4.0, 3.0, 10.0, 
		      4.0, 0.0, 0.0,
		 	  0.0, 1.0, 0.0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	setLight();
}

/**
 @brief  glutReshapeFunc�Ŏw�肵���R�[���o�b�N�֐�
 @param	 <in/--> w   ��
 @param  <in/--> h   ����
 @return �Ȃ�
 */
void reshape(int w, int h){
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (double)(w/h), 1.0, 500);
	glMatrixMode(GL_MODELVIEW);
}

/**
 @brief glutKeyboardFunc�Ŏw�肵���R�[���o�b�N�֐� 
 @param	<in/--> key     ASCII�R�[�h
 @param <in/--> x       �L�[�������ꂽ���̃}�E�X���W
 @param <in/--> y       �L�[�������ꂽ���̃}�E�X���W
 @return �Ȃ�
 */
void keyboard(unsigned char key, int x, int y){
	if (key == ESC) exit(0);
}

/**
 @brief  glutIdleFunc�Ŏw�肵���R�[���o�b�N�֐�
 @param	 �Ȃ�
 @return �Ȃ�
 */
void idle(){
	//	���̂̏d�S��1m����E�ɉ����D
	solid1->AddForce( Vec3f(1,0,0), Vec3f(0,1,0)+solid1->GetCenterPosition());
	solid2->AddForce( Vec3f(1,0,0), Vec3f(0,1,0)+solid2->GetCenterPosition());
	scene->Step();
	glutPostRedisplay();
	std::cout << solid1->GetFramePosition();
	std::cout << solid2->GetFramePosition() << std::endl;
}

/**
 @brief  ���C���֐�
 @param	 <in/--> argc    �R�}���h���C�����͂̌�
 @param  <in/--> argv    �R�}���h���C������
 @return  0 (����I��)
 */
int main(int argc, char* argv[]){
	sdk = CreatePHSdk();				//	SDK�̍쐬
	scene = sdk->CreateScene();			//	�V�[���̍쐬
	
	PHSolidDesc desc;					// ����teapot
	desc.mass = 2.0;					// ����	
	desc.inertia *=2.0;					// �����e���\��
	desc.center = Vec3f(0,0,0);			// ���ʒ��S�̈ʒu
	solid1 = scene->CreateSolid(desc);	// ���̂�desc�Ɋ�Â��č쐬
	
	desc.center = Vec3f(1,0,0);			//	�d�S�̈ʒu��Solid�̌��_����1m�E�ɂ��炷�D
	desc.pose.pos = Vec3f(3.5, 0.0, 0.0);
	solid2 = scene->CreateSolid(desc);	

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("PHSimpleGL");

	initialize();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	glutMainLoop();
	
	return 0;
}

