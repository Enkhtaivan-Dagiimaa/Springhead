/**
 Springhead2/src/Samples/BoxStack/main.cpp

�y�T�v�z
  �E���[�U�̃L�[(�X�y�[�X�L�[)���͂ɑ΂��ă{�b�N�X�𐶐����A�ςݏグ��B
  
�y�I����z
�@�EEsc�L�[�ŋ����I���B
	
�y�����̗���z
  �E�V�~�����[�V�����ɕK�v�ȏ��(���̂̌`��E���ʁE�����e���\���Ȃ�)��ݒ肷��B
  �@���̂̌`���OpenGL�Ŏw�肷��̂ł͂Ȃ��ASolid���̂Ŏ�������B
  �E�ڐG�G���W�����S���G���W���ɐݒ肷��B
  �E�^����ꂽ�����ɂ�臙t(=0.1)�b��̈ʒu�̕ω���ϕ����AOpenGL�ŃV�~�����[�V��������B  
�@�E���[�U�̃L�[���͂ɑ΂�Solid�𔭐�������B
 
*/

#include <Springhead.h>		//	Springhead�̃C���^�t�F�[�X
#include <ctime>
#ifdef __APPLE__
# include <GLUT/glut.h>
#else
# include <GL/glut.h>
#endif
#include <sstream>
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif
using namespace Spr;

#define ESC		27

PHSdkIf* sdk;
PHSolidDesc desc;
PHSceneIf* scene;
CDConvexMeshIf* meshFloor=NULL;
CDConvexMeshIf* meshBox=NULL;
PHSolidIf* soFloor;
std::vector<PHSolidIf*> soBox;

// �����̐ݒ� 
static GLfloat light_position[] = { 25.0, 50.0, 20.0, 1.0 };
static GLfloat light_ambient[]  = { 0.0, 0.0, 0.0, 1.0 };
static GLfloat light_diffuse[]  = { 1.0, 1.0, 1.0, 1.0 }; 
static GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
// �ގ��̐ݒ�
static GLfloat mat_floor[]      = { 1.0, 0.0, 0.0, 1.0 };
static GLfloat mat_box[]        = { 0.8, 0.8, 1.0, 1.0 };
static GLfloat mat_specular[]   = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat mat_shininess[]  = { 120.0 };


int frame=0,timepassed=0,timebase=0;


int boxStep = 0;
void fps(){
	frame++; 
	timepassed = glutGet(GLUT_ELAPSED_TIME); 
	if (1000 < timepassed - timebase) {
		std::cout << "   FPS:" << frame*1000.0/(timepassed - timebase);
		timebase = timepassed;		
		frame = 0;
	}
}

/**
 brief     	���ʑ̖̂�(�O�p�`)�̖@�������߂�
 param	 	<in/out> normal�@�@ �@��
 param     	<in/-->  base�@�@�@ mesh�̒��_
 param     	<in/-->  face�@�@�@ ���ʑ̖̂�
 return 	�Ȃ�
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
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void display(){
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	Affined ad;
	
	// ���̏�(soFloor)
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_floor);
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
	
	// �{�b�N�X(soBox)
	for (unsigned int boxCnt=0; boxCnt<soBox.size(); ++boxCnt) {	
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_box);
		glPushMatrix();
		pose = soBox[boxCnt]->GetPose();
		ad = Affined(pose);
		glMultMatrixd(ad);
			for(int i=0; i<soBox[boxCnt]->NShape(); ++i){
				CDShapeIf** shapes = soBox[boxCnt]->GetShapes();
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
		std::cout << "\rBox count : " << static_cast<unsigned int>(soBox.size());
		fps();
	}



	glutSwapBuffers();
}

/**
 brief     	�����̐ݒ�
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

	//gluLookAt(0.0,10.0,25.0, 
		      
	gluLookAt(0.0,10.0,100.0, 
		0.0,10.0, 0.0,
		 	  0.0, 1.0, 0.0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	setLight();
}

/**
 brief		glutReshapeFunc�Ŏw�肵���R�[���o�b�N�֐�
 param		<in/--> w�@�@��
 param		<in/--> h�@�@����
 return		�Ȃ�
 */
void reshape(int w, int h){
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
void keyboard(unsigned char key, int x, int y){
	switch (key) {
		case ESC:		
		case 'q':
			exit(0);
			break;
		case ' ':
			{
				/*
				soBox.push_back(scene->CreateSolid(desc));
				soBox.back()->AddShape(meshBox);
				if (soBox.size() < 30) {
					soBox.back()->SetFramePosition(Vec3f(0.5, 10+3*soBox.size(),0));
				} else {
					soBox.back()->SetFramePosition(Vec3f(0.5,100,0));
				}
				soBox.back()->SetOrientation(Quaternionf::Rot(Rad(30), 'y'));  
				std::ostringstream os;
				os << "box" << soBox.size();
				soBox.back()->SetName(os.str().c_str());
				*/
				
			} 
		default:
			break;
	}
}	

/**
 brief  	glutTimerFunc�Ŏw�肵���R�[���o�b�N�֐�
 param	 	<in/--> id�@�@ �^�C�}�[�̋�ʂ����邽�߂̏��
 return 	�Ȃ�
 */
void timer(int id){
	/// �����̃`�F�b�N�Ɖ�ʂ̍X�V���s��
	float DT = 0.05f;
	int times = (int)(DT / scene->GetTimeStep());
	for(int i=0; i<times; ++i) scene->Step();
	glutPostRedisplay();
	glutTimerFunc((unsigned int)1000*DT, timer, 0);
	//boxStep++;
	
	//if (boxStep > 5) {
						soBox.push_back(scene->CreateSolid(desc));
				soBox.back()->AddShape(meshBox);
				if (soBox.size() < 30) {
					soBox.back()->SetFramePosition(Vec3f(0.5, 10+3*soBox.size(),0));
				} else {
					soBox.back()->SetFramePosition(Vec3f(0.5,100,0));
				}
				soBox.back()->SetOrientation(Quaternionf::Rot(Rad(30), 'y'));  
				std::ostringstream os;
				os << "box" << soBox.size();
				soBox.back()->SetName(os.str().c_str());

	//			boxStep = 0;
	//}

}

/**
 brief		���C���֐�
 param		<in/--> argc�@�@�R�}���h���C�����͂̌�
 param		<in/--> argv�@�@�R�}���h���C������
 return		0 (����I��)
 */
int main(int argc, char* argv[]){
	sdk = CreatePHSdk();					// SDK�̍쐬�@
	PHSceneDesc dscene;
	dscene.contactSolver = PHSceneDesc::SOLVER_CONSTRAINT;	// �ڐG�G���W����I��
	//dscene.timeStep = 0.01;
	dscene.timeStep = 0.033;
	scene = sdk->CreateScene(dscene);				// �V�[���̍쐬

	// soFloor�p��desc
	desc.mass = 1e20f;
	desc.inertia *= 1e20f;
	soFloor = scene->CreateSolid(desc);		// ���̂�desc�Ɋ�Â��č쐬
	soFloor->SetGravity(false);
	
	// soBox�p��desc
	desc.mass = 2.0;
	desc.inertia = 2.0 * Matrix3d::Unit();

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
		meshBox = ICAST(CDConvexMeshIf, sdk->CreateShape(md));

		// soFloor(meshFloor)�ɑ΂��ăX�P�[�����O
		for(unsigned i=0; i<md.vertices.size(); ++i){
			md.vertices[i].x *= 300;
			md.vertices[i].z *= 200;
		}
		meshFloor = ICAST(CDConvexMeshIf, sdk->CreateShape(md));
	}
	soFloor->AddShape(meshFloor);
	soFloor->SetFramePosition(Vec3f(0,-2,0));


	scene->SetGravity(Vec3f(0,-9.8f, 0));	// �d�͂�ݒ�

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("BoxStack");
	initialize();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(0, timer, 0);

	glutMainLoop();

	//	SDK�͊J�����Ȃ��Ă��ǂ��D���Ȃ��Ă�main�𔲂��Ă���J�������D
	delete sdk;
}
