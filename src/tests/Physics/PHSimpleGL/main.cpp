/** 
 @file  Springhead2/src/tests/Physics/PHSimpleGL/main.cpp
 
@brief 2�̍��̂̈ʒu�̕ω����m�F����e�X�g�v���O�����i�ʒu���o�́AGL�\���j
   
 <PRE>  
 <B>�T�v�F</B>
  �E���̂̉^�����m�F����B
  �E�ʒu���o�͂��AOpenGL�ŃV�~�����[�V�������s���B
 
 <B>�I����F</B>
  �E�v���O����������I��������0��Ԃ��B
  
 <B>�����̗���F</B>
  �E�V�~�����[�V�����ɕK�v�ȏ��(���ʁE�d�S�E�����e���\���Ȃ�)��ݒ肷��B
    ���̂̌`���OpenGL�Ńe�B�[�|�b�g���w�肷��B
  �E�V�~�����[�V�����������n�߂���A���L�����ō��̂ɗ͂�������B
      ���̐Ԃ��e�B�[�|�b�g�F�I�u�W�F�N�g�̃��[�J�����W�n�����_�Ƃ��A�d�S��1m����E��1N�̗͂�������B
      �E�̐��e�B�[�|�b�g�F�I�u�W�F�N�g�̃��[�J�����W�n�̌��_����1m�E�ɏd�S�����炵�A�d�S��1m����E��1N�̗͂�������B
  �E�^����ꂽ�����ɂ�臙t�b��̈ʒu�̕ω���ϕ����A���̂̈ʒu�����o�͂ƁAOpenGL�����_�����O���s���B
 </PRE>
   
 */
#include <Springhead.h>		//	Springhead�̃C���^�t�F�[�X
#include <ctime>
#include <gl/glut.h>
#pragma hdrstop
using namespace Spr;

#define ESC		27
#define RATIO	1.001

PHSdkIf* sdk;
PHSceneIf* scene;
PHSolidIf* redTeapot, *blueTeapot;	// Solid�^�C�v
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

static double stepCnt = 0.0;
namespace {
	Vec3d redForce = Vec3d(1.0, 0.0, 0.0);
	Vec3d redVel = Vec3d(0.0, 0.0, 0.0);				// ���x
	Vec3d redPos = Vec3d(0.0, 0.0, 0.0);				// �ʒu
	
	Vec3d blueForce = Vec3d(1.0, 0.0, 0.0);
	Vec3d blueVel = Vec3d(0.0, 0.0, 0.0);				
	Vec3d bluePos = Vec3d(0.0, 0.0, 0.0);				// �ʒu
}

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
	Posed pose = redTeapot->GetPose();
	pose.ToAffine(ad);
	//	redTeapot->GetOrientation().to_matrix(af);
	//	af.Pos() = redTeapot->GetFramePosition();
	// pose�ł͐��x��double�Ȃ̂�matrixd�ɂ���
	glMultMatrixd(ad);
	glutSolidTeapot(1.0);
	glPopMatrix();

	// �E�̐��e�B�[�|�b�g
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_blue);

	glPushMatrix();
	pose = blueTeapot->GetPose();
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
 @brief  	glutReshapeFunc�Ŏw�肵���R�[���o�b�N�֐�
 @param	 	<in/--> w�@�@��
 @param  	<in/--> h�@�@����
 @return 	�Ȃ�
 */
void reshape(int w, int h){
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (double)(w/h), 1.0, 500);
	glMatrixMode(GL_MODELVIEW);
}

/**
 @brief 	glutKeyboardFunc�Ŏw�肵���R�[���o�b�N�֐� 
 @param		<in/--> key�@�@ ASCII�R�[�h
 @param 	<in/--> x�@�@�@ �L�[�������ꂽ���̃}�E�X���W
 @param 	<in/--> y�@�@�@ �L�[�������ꂽ���̃}�E�X���W
 @return 	�Ȃ�
 */
void keyboard(unsigned char key, int x, int y){
	if (key == ESC) exit(0);
}

/**
 @brief  	glutIdleFunc�Ŏw�肵���R�[���o�b�N�֐�
 @param	 	�Ȃ�
 @return 	�Ȃ�
 */
#define absMax(x,y) abs(x)>abs(y)? x:y
Vec3d maxdiff = Vec3d(0.0, 0.0, 0.0);
#define BLUE_1
void idle(){
	//	���̂̏d�S��1m����E�ɉ����D
	Vec3d force = Vec3d(1, 0, 0);
	redTeapot->AddForce( force, Vec3f(0,1,0)+redTeapot->GetCenterPosition());
	blueTeapot->AddForce( force, Vec3f(0,1,0)+blueTeapot->GetCenterPosition());

	//DSTR << "==============" << blueTeapot->GetCenterPosition() << std::endl;
	scene->Step();

	stepCnt += scene->GetTimeStep();
	double dt = scene->GetTimeStep();



	// ���x�𓱂��A�������Ԍo�ߌ�̈ʒu���v�Z 

	redVel		+= force * redTeapot->GetMassInv() * dt;	// (force/m)*dt
	redPos		+= redVel * dt;

#ifdef BLUE_1
	blueVel		+= force * blueTeapot->GetMassInv() * dt;	// (force/m)*dt
	bluePos		+= blueVel * dt;
	// MAXDIFF : x=-1.99999 y=-1.00015
	// myCalcFrame:(28.5125      0      3) getFrame:(28.5135 -0.0450767      3) getCenter:(29.5125 0.00015      3)
#else	

bluePos		= Vec3d(3.5, 0, 3) + 0.5 * force * blueTeapot->GetMassInv() * stepCnt * stepCnt;
	// maxdiff x=-2.01168 y=1.00015 
    // (  28.5      0      3)(28.5135 -0.0450767      3)		diff:(-0.0135233 0.0450767      0)
#endif


	std::cout << redTeapot->GetFramePosition();
	std::cout << blueTeapot->GetFramePosition() << std::endl;
	
	//DSTR << stepCnt << std::endl;		// 0.005 -> 0.01	// �������� [s] 5msec����



Vec3d diff;
#if 1
	diff = bluePos-blueTeapot->GetFramePosition();
	maxdiff.x = absMax(diff.x, maxdiff.x);
	maxdiff.y = absMax(diff.y, maxdiff.y);
	DSTR << bluePos << blueTeapot->GetFramePosition() << "\t\tdiff:" << diff;// << blueTeapot->GetCenterPosition();
#else
	diff = redPos-redTeapot->GetFramePosition();
	DSTR << redPos << redTeapot->GetFramePosition() << "\t\tdiff:" << diff;
#endif
DSTR <<  std::endl;


	//if (stepCnt >= 10.0) {	// 10�b
	if (stepCnt >= 10.0) {	// 10�b
		DSTR << "myCalcFrame:" << bluePos << " getFrame:" << blueTeapot->GetFramePosition() << " getCenter:" << blueTeapot->GetCenterPosition() << std::endl;
		//DSTR << blueTeapot->GetCenterPosition() << redTeapot->GetCenterPosition() << std::endl;
		DSTR << "MAXDIFF : " << "x=" << maxdiff.x << " y=" << maxdiff.y << std::endl;
		DSTR << "\n����I��." << std::endl;
		exit(EXIT_SUCCESS);
	} 
	

	glutPostRedisplay();
}

/**
 @brief		���C���֐�
 @param		<in/--> argc�@�@�R�}���h���C�����͂̌�
 @param		<in/--> argv�@�@�R�}���h���C������
 @return	0 (����I��)
 */
int main(int argc, char* argv[]){
	sdk = CreatePHSdk();				//	SDK�̍쐬
	scene = sdk->CreateScene();			//	�V�[���̍쐬
	
	PHSolidDesc desc;					// ����teapot
	desc.mass = 2.0;					// ����	
	desc.inertia *=2.0;					// �����e���\��
	desc.center = Vec3f(0,0,0);			// ���ʒ��S�̈ʒu
	redTeapot = scene->CreateSolid(desc);	// ���̂�desc�Ɋ�Â��č쐬
	redPos = redTeapot->GetFramePosition();


	desc.center = Vec3f(1,0,0);			//	�d�S�̈ʒu��Solid�̌��_����1m�E�ɂ��炷�D
	desc.pose.pos = Vec3f(3.5, 0.0, 0.0);
	/// ��O�ɕ��s�ړ�
	desc.pose = desc.pose * Posed::Trn(0.0, 0.0, 3.0);
	blueTeapot = scene->CreateSolid(desc);	
	bluePos = blueTeapot->GetFramePosition();
	//bluePos = blueTeapot->GetCenterPosition();
	DSTR << "-------------------"<< blueTeapot->GetFramePosition() << blueTeapot->GetCenterPosition() << std::endl;	// Frame��OK.(3.5,0,3) center(7.5,0,3)

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

