// PHConstraintTest.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

/**
 Springhead2/src/tests/Physics/PHConstraintTest.cpp

�y�T�v�z
  
�y�I����z
 
�y�����̗���z
 
 */

#include <Springhead.h>		//	Springhead�̃C���^�t�F�[�X
#include <../src/WinBasis/WBPreciseTimer.h>
#include <ctime>
#include <string>
#include <gl/glut.h>
#include <windows.h>
#pragma hdrstop
using namespace Spr;

GetObject();

#define ESC				27
#define FALL_DISTANCE	-500		// ��������(�ُ�I����)

PHSdkIf* sdk;
PHSceneIf* scene;
PHSolidIf* soFloor, *soBlock[2];

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

static int elapse = 100;		//timer����[ms]
static double dt = 0.1;	//�ϕ��X�e�b�v[s]

/**
 brief     ���ʑ̖̂�(�O�p�`)�̖@�������߂�
 param	   <in/out> normal�@�@  �@��
 param     <in/-->   base�@�@�@ mesh�̒��_
 param     <in/-->   face�@�@�@ ���ʑ̖̂�
 return     �Ȃ�
 */
void genFaceNormal(Vec3f& normal, Vec3f* base, CDFaceIf* face){
	Vec3f edge0, edge1;
	edge0 = base[face->GetIndices()[1]] - base[face->GetIndices()[0]];
	edge1 = base[face->GetIndices()[2]] - base[face->GetIndices()[0]];
	normal = edge0^edge1;
	normal.unitize();	
}

/**
 brief     	glutDisplayFunc�Ŏw�肵���R�[���o�b�N�֐�
 param		�Ȃ�
 return 	�Ȃ�
 */
void _cdecl display(){
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

	Vec3f normal;
	for(int i=0; i<soFloor->NShape(); ++i){
		CDShapeIf** shapes = soFloor->GetShapes();
		CDConvexMeshIf* mesh = ICAST(CDConvexMeshIf, shapes[i]);
		Vec3f* base = mesh->GetVertices();
		for(size_t f=0; f<mesh->NFace();++f){
			CDFaceIf* face = mesh->GetFace(f);
			
			glBegin(GL_POLYGON);
			genFaceNormal(normal, base, face);
			glNormal3fv(normal.data);
			for(int v=0; v<face->NIndex(); ++v){	
				glVertex3fv(base[face->GetIndices()[v]].data);
			}
			glEnd();
		}
	}
	glPopMatrix();

	
	for(int n = 0; n < 2; n++){
		// ��̐�����(soBlock)
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_blue);
		glPushMatrix();
		pose = soBlock[n]->GetPose();
		ad = Affined(pose);
		glMultMatrixd(ad);
			for(int i=0; i<soBlock[n]->NShape(); ++i){
			CDShapeIf** shapes = soBlock[i]->GetShapes();
			CDConvexMeshIf* mesh = ICAST(CDConvexMeshIf, shapes[i]);
			Vec3f* base = mesh->GetVertices();
			for(size_t f=0; f<mesh->NFace();++f){
				CDFaceIf* face = mesh->GetFace(f);
				
				glBegin(GL_POLYGON);
				genFaceNormal(normal, base, face);
				glNormal3fv(normal.data);	
				for(int v=0; v<face->NIndex(); ++v){	
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
void initialize(){
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0, 3.0, 9.0, 
		      0.0, 0.0, 0.0,
		 	  0.0, 1.0, 0.0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	setLight();
}

/**
 brief		glutReshapeFunc�Ŏw�肵���R�[���o�b�N�֐�
 param		<in/--> w�@�@��
 param		<in/--> h�@�@����
 return		 �Ȃ�
 */
void _cdecl reshape(int w, int h){
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)w/(GLfloat)h, 1.0, 500.0);
	glMatrixMode(GL_MODELVIEW);
}

/**
 brief 		glutKeyboardFunc�Ŏw�肵���R�[���o�b�N�֐� 
 param		<in/--> key�@�@ ASCII�R�[�h
 param 		<in/--> x�@�@�@ �L�[�������ꂽ���̃}�E�X���W
 param 		<in/--> y�@�@�@ �L�[�������ꂽ���̃}�E�X���W
 return 	�Ȃ�
 */
void _cdecl keyboard(unsigned char key, int x, int y){
	if (key == ESC) exit(0);
}	

/**
 brief  	glutIdleFunc�Ŏw�肵���R�[���o�b�N�֐�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void _cdecl idle(){
	//Vec3d prepos, curpos;	// position
	//prepos = soBlock->GetFramePosition();

	scene->Step();

	// �ʒu�����o��
	//std::cout << soFloor->GetFramePosition();
	//std::cout << soBlock->GetFramePosition() << std::endl;

	//curpos = soBlock->GetFramePosition();

	// ���̏��5�b�Î~�����琳��I���Ƃ���B
	/*if (approx(prepos, curpos)) {
		if (timeflag == false){
			starttime = clock();
			timeflag = true;
		} else {
			endtime = clock();
			count = (endtime - starttime) / (float)CLOCKS_PER_SEC;
			if (count > 5) {
				DSTR << "\n����I��." << std::endl;
				exit(EXIT_SUCCESS);
			} 
		}		
	}
	else {
		// ���R�������������̂����̏�ŐÎ~�����A-500m�n�_�܂ŗ��������ꍇ�A�ُ�I���Ƃ���B
		timeflag = false;
		if (curpos.y < FALL_DISTANCE) {	
			DSTR << "\n�ُ�I��." << std::endl;
			exit(EXIT_FAILURE);
		}
	}*/	

	glutPostRedisplay();
}
void _cdecl timer(int id){
//	for(int i=0; i<10;++i) 
	static WBPreciseTimer pt;
	pt.CountUS();

	int n = (double)(elapse) / 1000.0 / dt;
	for(int i = 0; i < n; i++)
		idle();
	//DSTR << pt.CountUS() << std::endl;
	glutTimerFunc(elapse, timer, 0);
}

/**
 brief 		���ʑ̖̂�(�O�p�`)�̒��_���W���f�o�b�O�o�͂�����B
 param 		<in/--> solidID�@�@ solid��ID
 return 	�Ȃ�
 */
/*void dstrSolid(std::string& solidName) {
	PHSolidIf* solid;
	if (solidName == "soFloor")			solid = soFloor;
	else if (solidName == "soBlock")	solid = soBlock;
	//DSTR << "***  " << solidName << "   ***\n";

	for(int i=0; i<solid->NShape(); ++i){
		CDShapeIf** shapes = solid->GetShapes();
		CDConvexMeshIf* mesh = ICAST(CDConvexMeshIf, shapes[i]);
		Vec3f* base = mesh->GetVertices();
		for(size_t f=0; f<mesh->NFace();++f){
			CDFaceIf* face = mesh->GetFace(f);
			for(int v=0; v<face->NIndex(); ++v){
				//DSTR << base[face->GetIndices()[v]];
			}
			//DSTR << std::endl;
		}
	}
}*/

/**
 brief		���C���֐�
 param		<in/--> argc�@�@�R�}���h���C�����͂̌�
 param		<in/--> argv�@�@�R�}���h���C������
 return		0 (����I��)
 */
int _cdecl main(int argc, char* argv[]){
	sdk = CreatePHSdk();						// SDK�̍쐬�@
	PHSceneDesc dscene;
	dscene.contactSolver = PHSceneDesc::SOLVER_CONSTRAINT;	// �ڐG�G���W����I��
	dscene.timeStep = dt;
	scene = sdk->CreateScene(dscene);				// �V�[���̍쐬

	PHSolidDesc dsolid;
	dsolid.mass = 2.0;
	dsolid.inertia *= 2.0;
	soBlock[0] = scene->CreateSolid(dsolid);			// ���̂�desc�Ɋ�Â��č쐬
	soBlock[1] = scene->CreateSolid(dsolid);

	dsolid.mass = 1e20f;
	dsolid.inertia *= 1e20f;
	soFloor = scene->CreateSolid(dsolid);			// ���̂�desc�Ɋ�Â��č쐬
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
		meshBlock = ICAST(CDConvexMeshIf, sdk->CreateShape(md));

		// soFloor(meshFloor)�ɑ΂��ăX�P�[�����O
		for(unsigned i=0; i<md.vertices.size(); ++i){
			md.vertices[i].x *= 5;
			md.vertices[i].z *= 5;
		}
		meshFloor = ICAST(CDConvexMeshIf, sdk->CreateShape(md));
	}

	soFloor->AddShape(meshFloor);
	soBlock[0]->AddShape(meshBlock);
	soBlock[1]->AddShape(meshBlock);
	soFloor->SetFramePosition(Vec3f(0,-1,0));
	soBlock[0]->SetFramePosition(Vec3f(0,1,0));
	soBlock[1]->SetFramePosition(Vec3f(0,3,0));
	soBlock[0]->SetOrientation(Quaternionf::Rot(Rad(0), 'z'));
	soBlock[1]->SetOrientation(Quaternionf::Rot(Rad(0), 'z'));

	scene->SetGravity(Vec3f(0,-9.8f, 0));	// �d�͂�ݒ�

	// �f�o�b�O�o��
	//dstrSolid(std::string("soFloor"));
	//dstrSolid(std::string("soBlock"));

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("PHConstraintTest");
	initialize();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(0, timer, 0);

	glutMainLoop();
}