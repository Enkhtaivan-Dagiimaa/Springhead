/**
 Springhead2/src/tests/Physics/PHConstraintTest.cpp

�y�T�v�z
  �E�ڐG�G���W�����S���@�iSOLVER_CONSTRAINT�j�ɐݒ肵�A�V�~�����[�V��������B
  
�y�I����z
�@�E���X�e�b�v�ԁA���̏�ɐÎ~�����琳��I���B�@
   
 */
#include <Springhead.h>		//	Springhead�̃C���^�t�F�[�X
#include <string>
#ifdef _MSC_VER
# include <../src/WinBasis/WBPreciseTimer.h>
# include <windows.h>
#endif
#include <GL/glut.h>
#pragma hdrstop
using namespace Spr;

#define ESC				27
#define EXIT_TIMER		200		
#define NUM_BLOCK		6
//#define NUM_BLOCK		1

float boxpos[10][3] = {
	{-3, 1, 0}, {0, 1, 0}, {3, 1, 0}, {-1.5, 4, 0}, {1.5, 4, 0},
	{0, 7, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}
};

PHSdkIf* sdk;
PHSceneIf* scene;
PHSolidIf* soFloor, *soBlock[NUM_BLOCK];
static int	 stillStepCnt[NUM_BLOCK];			// �Î~�����X�e�b�v�J�E���g
static Vec3d prepos[NUM_BLOCK];				// previous position
static Vec3d curpos[NUM_BLOCK];				// current position

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

static int elapse = 100;	//timer����[ms]
static double dt = 0.05;	//�ϕ��X�e�b�v[s]

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
void SPR_CDECL display(){
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

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

	
	for(int n = 0; n < NUM_BLOCK; n++){
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

	gluLookAt(0.0, 3.0, 15.0, 
		      0.0, 3.0, 0.0,
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
void SPR_CDECL reshape(int w, int h){
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
void SPR_CDECL keyboard(unsigned char key, int x, int y){
	if (key == ESC) exit(0);
}	

/**
 brief  	glutIdleFunc�Ŏw�肵���R�[���o�b�N�֐�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void SPR_CDECL idle(){
#if 0	
	int iCnt = 0;
	for (iCnt = 0; iCnt < NUM_BLOCK; iCnt++){
		prepos[iCnt] = soBlock[iCnt]->GetFramePosition();
	}
	scene->Step();
	for (iCnt = 0; iCnt < NUM_BLOCK; iCnt++){
		curpos[iCnt] = soBlock[iCnt]->GetFramePosition();
	}

	for (iCnt = 0; iCnt < NUM_BLOCK; iCnt++){
		if (approx(prepos[iCnt], curpos[iCnt])){			// ��v�����ꍇ�̓C���N�������g
			stillStepCnt[iCnt]++;
		} else {											// ��v���Ȃ������ꍇ�̓J�E���g��0�ɏ�����
			stillStepCnt[iCnt] = 0;
		}
	}

	iCnt = 0;
	while(stillStepCnt[iCnt] > EXIT_TIMER){
		if (iCnt == NUM_BLOCK-1){
			DSTR << "\nPHConstraintTest success." << std::endl;
			exit(EXIT_SUCCESS);
		}
	}

	std::cout << prepos[NUM_BLOCK-1] << ' ' << curpos[NUM_BLOCK-1] << ' ' << prepos[NUM_BLOCK-1]-curpos[NUM_BLOCK-1] << std::endl;
	glutPostRedisplay();
#else
	scene->Step();
	glutPostRedisplay();
	static int count;
	count++;
	if (++count > 200) exit(0);
#endif	
}

/**
 brief  	glutTimerFunc�Ŏw�肵���R�[���o�b�N�֐�
 param	 	<in/--> id�@�@ �^�C�}�[�̋�ʂ����邽�߂̏��
 return 	�Ȃ�
 */
void SPR_CDECL timer(int id){
	int n = (int)((double)(elapse) / 1000.0 / dt);
	for(int i = 0; i < n; i++)
		idle();
#ifdef _MSC_VER	
	static WBPreciseTimer pt;
	pt.CountUS();
	DSTR << pt.CountUS() << std::endl;
#endif
	glutTimerFunc(elapse, timer, 0);
}

/**
 brief		���C���֐�
 param		<in/--> argc�@�@�R�}���h���C�����͂̌�
 param		<in/--> argv�@�@�R�}���h���C������
 return		0 (����I��)
 */
int SPR_CDECL main(int argc, char* argv[]){
	sdk = CreatePHSdk();							// SDK�̍쐬�@
	PHSceneDesc dscene;
	dscene.contactSolver = PHSceneDesc::SOLVER_CONSTRAINT;	// �ڐG�G���W����I��
	dscene.timeStep = dt;
	scene = sdk->CreateScene(dscene);				// �V�[���̍쐬

	PHSolidDesc dsolid;
	dsolid.mass = 2.0;
	dsolid.inertia *= 2.0;
	for(int i = 0; i < NUM_BLOCK; i++)
		soBlock[i] = scene->CreateSolid(dsolid);	// ���̂�desc�Ɋ�Â��č쐬
	
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
			md.vertices[i].x *= 10;		
			md.vertices[i].z *= 5;
		}
		meshFloor = ICAST(CDConvexMeshIf, sdk->CreateShape(md));
	}

	soFloor->AddShape(meshFloor);
	for(int i = 0; i < NUM_BLOCK; i++)
		soBlock[i]->AddShape(meshBlock);
	
	soFloor->SetFramePosition(Vec3f(0,-1,0));
	for(int i = 0; i < NUM_BLOCK; i++)
		soBlock[i]->SetFramePosition(*(Vec3f*)&boxpos[i]);
	//soBlock[5]->SetOrientation(Quaternionf::Rot(Rad(45), 'z'));
	soBlock[NUM_BLOCK-1]->SetOrientation(Quaternionf::Rot(Rad(45), 'z'));

	scene->SetGravity(Vec3f(0,-9.8f, 0));	// �d�͂�ݒ�

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
