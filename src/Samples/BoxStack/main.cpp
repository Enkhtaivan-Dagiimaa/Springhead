/**
 @file	Springhead2/src/tests/Physics/PHShapeGL/main.cpp

 @brief ���[�U�̃L�[����(�X�y�[�X�L�[)�ɑ΂��ă{�b�N�X�𐶐������R����������B
  
 <PRE>
 <B>�T�v�F</B>
  �E�y�i���e�B�@�ɂ��ʑ��ʑ̓��m�̐ڐG����ƐڐG�͂��m�F����B
  �E���̂����R���������A���̏��2�̃u���b�N��ςݏグ��B
  �E���_���W���f�o�b�O�o�͂����AOpenGL�ŃV�~�����[�V�������s���B
  
 <B>�I����F</B>
�@�E���R�������������̂����̏��5�b�ԐÎ~�����琳��I���Ƃ���B
  �E���R�������������̂����̏�ŐÎ~�����A-500m�n�_�܂ŗ��������ꍇ�A�ُ�I���Ƃ���B
	
 <B>�����̗���F</B>
  �E�V�~�����[�V�����ɕK�v�ȏ��(���̂̌`��E���ʁE�����e���\���Ȃ�)��ݒ肷��B
  �@���̂̌`���OpenGL�Ŏw�肷��̂ł͂Ȃ��ASolid���̂Ŏ�������B  
  �E�^����ꂽ�����ɂ�臙t�b��̈ʒu�̕ω���ϕ����AOpenGL�ŃV�~�����[�V��������B
�@�E�f�o�b�O�o�͂Ƃ��āA���ʑ̖̂�(�O�p�`)�̒��_���W���o�͂���B   
 </PRE>
 
 */

#include <Springhead.h>		//	Springhead�̃C���^�t�F�[�X
#include <ctime>
#include <string>
#include <gl/glut.h>
#pragma hdrstop
using namespace Spr;

#define ESC				27

PHSdkIf* sdk;
PHSolidDesc desc;
PHSceneIf* scene;
PHSolidIf* soFloor;
CDConvexMeshIf* meshFloor=NULL;
CDConvexMeshIf* meshBlock=NULL;
//PHSolidIf* soBlock, *soBlock2;
std::vector<PHSolidIf*> soBlock;


// �����̐ݒ� 
static GLfloat light_position[] = { 25.0, 50.0, 20.0, 1.0 };
static GLfloat light_ambient[]  = { 0.0, 0.0, 0.0, 1.0 };
static GLfloat light_diffuse[]  = { 1.0, 1.0, 1.0, 1.0 }; 
static GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
// �ގ��̐ݒ�
static GLfloat mat_red[]        = { 1.0, 0.0, 0.0, 1.0 };
static GLfloat mat_blue[]       = { 0.0, 0.0, 1.0, 1.0 };
static GLfloat mat_specular[]   = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat mat_shininess[]  = { 120.0 };


/**
 @brief     ���ʑ̖̂�(�O�p�`)�̖@�������߂�
 @param	 	<in/out> normal�@�@ �@��
 @param     <in/-->  base�@�@�@ mesh�̒��_
 @param     <in/-->  face�@�@�@ ���ʑ̖̂�
 @return 	�Ȃ�
 */
void genFaceNormal(float *normal, Vec3f* base, CDFaceIf* face){
		normal[0] = base[face->GetIndices()[0]].y * base[face->GetIndices()[1]].z
						- base[face->GetIndices()[1]].y * base[face->GetIndices()[0]].z
						+ base[face->GetIndices()[1]].y * base[face->GetIndices()[2]].z
						- base[face->GetIndices()[2]].y * base[face->GetIndices()[1]].z
						+ base[face->GetIndices()[2]].y * base[face->GetIndices()[0]].z
						- base[face->GetIndices()[0]].y * base[face->GetIndices()[2]].z;
		normal[1] = base[face->GetIndices()[0]].z * base[face->GetIndices()[1]].x
						- base[face->GetIndices()[1]].z * base[face->GetIndices()[0]].x
						+ base[face->GetIndices()[1]].z * base[face->GetIndices()[2]].x
						- base[face->GetIndices()[2]].z * base[face->GetIndices()[1]].x
						+ base[face->GetIndices()[2]].z * base[face->GetIndices()[0]].x
						- base[face->GetIndices()[0]].z * base[face->GetIndices()[2]].x;
		normal[2] = base[face->GetIndices()[0]].x * base[face->GetIndices()[1]].y
						- base[face->GetIndices()[1]].x * base[face->GetIndices()[0]].y
						+ base[face->GetIndices()[1]].x * base[face->GetIndices()[2]].y
						- base[face->GetIndices()[2]].x * base[face->GetIndices()[1]].y
						+ base[face->GetIndices()[2]].x * base[face->GetIndices()[0]].y
						- base[face->GetIndices()[0]].x * base[face->GetIndices()[2]].y;
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

	Affined ad;
	
	// ���̐Ԃ�����(soFloor)
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_red);
	glPushMatrix();
	Posed pose = soFloor->GetPose();
	pose.ToAffine(ad);
	glMultMatrixd(ad);	

	float normal[3];
	for(int i=0; i<soFloor->GetNShapes(); ++i){
		CDShapeIf** shapes = soFloor->GetShapes();
		CDConvexMeshIf* mesh = ICAST(CDConvexMeshIf, shapes[i]);
		Vec3f* base = mesh->GetVertices();
		for(size_t f=0; f<mesh->GetNFaces();++f){
			CDFaceIf* face = mesh->GetFace(f);
			
			glBegin(GL_POLYGON);
			genFaceNormal(normal, base, face);
			glNormal3fv(normal);
			for(int v=0; v<face->GetNIndices(); ++v){	
				glVertex3fv(base[face->GetIndices()[v]].data);
			}
			glEnd();
		}
	}
	glPopMatrix();
	
	// ��̐�����(soBlock)
	for (unsigned int blockCnt = 0; blockCnt < soBlock.size(); blockCnt++) {	
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_blue);
		glPushMatrix();
		pose = soBlock[blockCnt]->GetPose();
		ad = Affined(pose);
		glMultMatrixd(ad);
			for(int i=0; i<soBlock[blockCnt]->GetNShapes(); ++i){
				CDShapeIf** shapes = soBlock[blockCnt]->GetShapes();
				CDConvexMeshIf* mesh = ICAST(CDConvexMeshIf, shapes[i]);
				Vec3f* base = mesh->GetVertices();
				for(size_t f=0; f<mesh->GetNFaces();++f){
					CDFaceIf* face = mesh->GetFace(f);
				
					glBegin(GL_POLYGON);
					genFaceNormal(normal, base, face);
					glNormal3fv(normal);	
					for(int v=0; v<face->GetNIndices(); ++v){	
						glVertex3fv(base[face->GetIndices()[v]].data);
					}
					glEnd();
				}
			}
		glPopMatrix();
	}

	glutSwapBuffers();
}

/**
 @brief     �����̐ݒ�
 @param	 	�Ȃ�
 @return 	�Ȃ�
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
 @brief     ����������
 @param	 	�Ȃ�
 @return 	�Ȃ�
 */
void initialize(){
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0,20.0,30.0, 
		      0.0, 0.0, 0.0,
		 	  0.0, 1.0, 0.0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);


	setLight();
}

/**
 @brief		glutReshapeFunc�Ŏw�肵���R�[���o�b�N�֐�
 @param		<in/--> w�@�@��
 @param		<in/--> h�@�@����
 @return	�Ȃ�
 */
void reshape(int w, int h){
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)w/(GLfloat)h, 1.0, 500.0);
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
	switch (key) {
		case ESC:		
		case 'q':
			exit(0);
			break;
		case ' ':{
			/*
			soBlock.push_back(scene->CreateSolid(desc));
			size_t index;
			index = soBlock.size() - 1;
			soBlock[index]->AddShape(meshBlock);
			soBlock[index]->SetFramePosition(Vec3f(0.5,10,0));
			soBlock[index]->SetOrientation(Quaternionf::Rot(Rad(30), 'y'));
			*/
			soBlock.push_back(scene->CreateSolid(desc));
			size_t index;
			index = soBlock.size() - 1;



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


			soBlock[index]->AddShape(meshBlock);
			soBlock[index]->SetFramePosition(Vec3f(0.5,3,0));
			soBlock[index]->SetOrientation(Quaternionf::Rot(Rad(30), 'y'));

				/*desc.mass = 2.0;
				desc.inertia *= 2.0;
			soBlock.push_back(scene->CreateSolid(desc));
			index = soBlock.size() - 1;
			soBlock[index]->AddShape(meshBlock);
			soBlock[index]->SetFramePosition(Vec3f(0.5,10,0));
			soBlock[index]->SetOrientation(Quaternionf::Rot(Rad(30), 'y'));
*/
				 }break;
	
		default:
			break;
	}
}	

/**
 @brief  	glutIdleFunc�Ŏw�肵���R�[���o�b�N�֐�
 @param	 	�Ȃ�
 @return 	�Ȃ�
 */
void idle(){

	scene->Step();
	glutPostRedisplay();
}

/**
 @brief		���C���֐�
 @param		<in/--> argc�@�@�R�}���h���C�����͂̌�
 @param		<in/--> argv�@�@�R�}���h���C������
 @return	0 (����I��)
 */
int main(int argc, char* argv[]){
	sdk = CreatePHSdk();					// SDK�̍쐬�@
	scene = sdk->CreateScene();				// �V�[���̍쐬

	// soFloor�p��desc
	desc.mass = 1e20f;
	desc.inertia *= 1e20f;
	soFloor = scene->CreateSolid(desc);		// ���̂�desc�Ɋ�Â��č쐬
	soFloor->SetGravity(false);
	
	// soBlock�p��desc
	desc.mass = 2.0;
	desc.inertia = 2.0*Matrix3d::Unit();

	//	�`��̍쐬
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
			md.vertices[i].x *= 20;
			md.vertices[i].z *= 10;
		}
		meshFloor = ICAST(CDConvexMeshIf, scene->CreateShape(md));
	}

	soFloor->AddShape(meshFloor);
	soFloor->SetFramePosition(Vec3f(0,-2,0));


	scene->SetGravity(Vec3f(0,-9.8f, 0));	// �d�͂�ݒ�

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("PHShapeGL");
	initialize();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	glutMainLoop();

	//	SDK�͊J�����Ȃ��Ă��ǂ��D���Ȃ��Ă�main�𔲂��Ă���J�������D
	delete sdk;
}