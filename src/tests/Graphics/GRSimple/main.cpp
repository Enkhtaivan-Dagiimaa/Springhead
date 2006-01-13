/** 
 @file  Springhead2/src/tests/Graphics/GRSimple/main.cpp

@brief �`��̃e�X�g

 <PRE>
 <B>�T�v�F</B>
  �E���̂̉^�����m�F����B
  �E�ʒu���o�͂��A�����_�����O�͍s��Ȃ��B
  
 <B>�I����F</B>
  �E�v���O����������I��������0��Ԃ��B
 
 <B>�����̗���F</B>
  �E�V�~�����[�V�����ɕK�v�ȏ��(���ʁE�d�S�E�����e���\���Ȃ�)��ݒ肷��B
  �E2�̍��̂�p�ӂ��A�e���̂ɑ΂��ďd�S��2m����E��10N�̗͂�������B
  �E�^����ꂽ�����ɂ�臙t�b��̈ʒu�̕ω���10�X�e�b�v�ϕ����A���̓s�x�ʒu���o�͂���B
  �E10�X�e�b�v���̌v�Z���I������A�v���O�������I��������B
 </PRE>
    
 */
#include <Springhead.h>		//	Springhead�̃C���^�t�F�[�X
#pragma hdrstop
#include <gl/glut.h>
using namespace Spr;

GRSdkIf* grSdk;
GRDebugRenderIf* render;
GRDeviceGLIf* grDevice;

PHSdkIf* phSdk;
PHSceneIf* scene;
PHSolidIf* soFloor, *soBlock;
/**
 @brief     glutDisplayFunc�Ŏw�肵���R�[���o�b�N�֐�
 @param	 	�Ȃ�
 @return 	�Ȃ�
 */
void display(){
}

/**
 @brief		glutReshapeFunc�Ŏw�肷��R�[���o�b�N�֐�
 @param		<in/--> w�@�@��
 @param		<in/--> h�@�@����
 @return	�Ȃ�
 */
void reshape(int w, int h){
	render->Resize(Vec2f(w,h));
}

/**
 @brief 	glutKeyboardFunc�Ŏw�肵���R�[���o�b�N�֐� 
 @param		<in/--> key�@�@ ASCII�R�[�h
 @param 	<in/--> x�@�@�@ �L�[�������ꂽ���̃}�E�X���W
 @param 	<in/--> y�@�@�@ �L�[�������ꂽ���̃}�E�X���W
 @return 	�Ȃ�
 */
void keyboard(unsigned char key, int x, int y){
	if (key == '\x1b') exit(0);
}	

/**
 @brief  	glutIdleFunc�Ŏw�肵���R�[���o�b�N�֐�
 @param	 	�Ȃ�
 @return 	�Ȃ�
 */
void idle(){
	glutPostRedisplay();
	static int count;
	count ++;
	if (count > 200) exit(0);
}

int main(int argc, char* argv[]){
	phSdk = CreatePHSdk();						// SDK�̍쐬�@
	scene = phSdk->CreateScene();				// �V�[���̍쐬
	PHSolidDesc desc;
	desc.mass = 2.0;
	desc.inertia *= 2.0;
	soBlock = scene->CreateSolid(desc);			// ���̂�desc�Ɋ�Â��č쐬

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
		md.vertices.push_back(Vec3f(-1,-1,-1)); md.vertices.push_back(Vec3f(-1,-1, 1));	
		md.vertices.push_back(Vec3f(-1, 1,-1));	md.vertices.push_back(Vec3f(-1, 1, 1));
		md.vertices.push_back(Vec3f( 1,-1,-1));	md.vertices.push_back(Vec3f( 1,-1, 1));
		md.vertices.push_back(Vec3f( 1, 1,-1));	md.vertices.push_back(Vec3f( 1, 1, 1));
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
	int window = glutCreateWindow("GRSimple");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);

	grSdk = CreateGRSdk();
	render = grSdk->CreateDebugRender();
	grDevice = grSdk->CreateDeviceGL(window);
	render->SetDevice(grDevice);

	glutMainLoop();

	//	SDK�͊J�����Ȃ��ėǂ��D���Ȃ��Ă�main�𔲂��Ă���J�������D
}
