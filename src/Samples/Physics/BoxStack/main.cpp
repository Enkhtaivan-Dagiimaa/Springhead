/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
\page pageBoxStack �L�[���͂Ŕ��𐶐����Đςݏグ��T���v��
Springhead2/src/Samples/BoxStack

\section secQuitBoxStack �I���
- Esc��'q'�ŋ����I���B
- ' ', 'm', 'd', 'c', 'v', 'z', 'x'�Ŕ��������Ă���
	
\section secFlowBoxStack �����̗���
- �V�~�����[�V�����ɕK�v�ȏ��(���̂̌`��E���ʁE�����e���\���Ȃ�)��ݒ肷��B
  ���̂̌`���OpenGL�Ŏw�肷��̂ł͂Ȃ��ASolid���̂Ŏ�������B
- �ڐG�G���W�����S���G���W���ɐݒ肷��B
- �^����ꂽ�����ɂ�臙t(=0.1)�b��̈ʒu�̕ω���ϕ����AOpenGL�ŃV�~�����[�V��������B  
- ���[�U�̃L�[���͂ɑ΂�Solid�𔭐�������B
*/


#include <windows.h>

#include <Springhead.h>		//	Springhead�̃C���^�t�F�[�X
#include <Physics/PHConstraintEngine.h>
#include <ctime>
#include <GL/glut.h>
#include <sstream>

#pragma hdrstop
using namespace Spr;
using namespace std;

#define ESC		27

//	���ƕ��̂̏��Ԃ����ւ��ăf�o�b�O���邽�߂̃t���O�B�����蔻��̃f�o�b�O�Ŏg�����B
#define CREATE_FLOOR


bool bStep = true;
UTRef<PHSdkIf> sdk;
PHSolidDesc desc;
PHSceneIf* scene;
CDConvexMeshIf* meshFloor=NULL;
CDConvexMeshIf* meshWall=NULL;
CDConvexMeshIf* meshConvex=NULL;
CDBoxIf* meshBox=NULL;
CDSphereIf* meshSphere=NULL;
CDCapsuleIf* meshCapsule=NULL;
CDRoundConeIf* meshRoundCone=NULL;
PHSolidIf* soFloor;
std::vector<PHSolidIf*> soBox;

UTRef<GRSdkIf> grSdk;
GRDebugRenderIf* render;
UTRef<GRDeviceGLIf> device;

double	CameraRotX = 0.0, CameraRotY = Rad(80.0), CameraZoom = 30.0;
bool bLeftButton = false, bRightButton = false;


// �����̐ݒ� 
static GLfloat light_position[] = { 25.0, 50.0, 20.0, 1.0 };
static GLfloat light_ambient[]  = { 0.0, 0.0, 0.0, 1.0 };
static GLfloat light_diffuse[]  = { 1.0, 1.0, 1.0, 1.0 }; 
static GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
// �ގ��̐ݒ�
static GLfloat mat_floor[]      = { 1.0, 0.0, 0.0, 1.0 };
static GLfloat mat_box[2][4]     = {
	{ 1.0, 0.6, 0.6, 1.0 },
	{ 0.6, 0.6, 1.0, 1.0 }
};
static GLfloat mat_specular[]   = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat mat_shininess[]  = { 120.0 };

int frame=0,timepassed=0,timebase=0;

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
void __cdecl display(){
	//	�o�b�t�@�N���A
	render->ClearBuffer();
	Affinef view;
	double yoffset = 10.0;
	view.Pos() = CameraZoom * Vec3f(
		cos(CameraRotX) * cos(CameraRotY),
		sin(CameraRotX),
		cos(CameraRotX) * sin(CameraRotY));
	view.PosY() += yoffset;
	view.LookAtGL(Vec3f(0.0, yoffset, 0.0), Vec3f(0.0f, 100.0f, 0.0f));
	render->SetViewMatrix(view.inv());

	render->DrawScene(scene);
/*
	render->SetMaterialSample((GRDebugRenderIf::TMaterialSample)0);
	render->DrawSolid(soFloor);
	for(unsigned int i=0; i<soBox.size(); ++i){
		render->SetMaterialSample((GRDebugRenderIf::TMaterialSample)(i+1));
		render->DrawSolid(soBox[i]);
	}
	if (scene && scene->GetConstraintEngine()){
		grscene->GetConstraintEngine()
	}
*/
	std::ostringstream sstr;
	sstr << "NObj = " << scene->NSolids();
	render->DrawFont(Vec2f(-21, 23), sstr.str());
	glutSwapBuffers();

/*
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
		CDShapeIf* shape = soFloor->GetShape(i);
		CDConvexMeshIf* mesh = DCAST(CDConvexMeshIf, shape);
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
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_box[boxCnt%2]);
		glPushMatrix();
		pose = soBox[boxCnt]->GetPose();
		ad = Affined(pose);
		glMultMatrixd(ad);
			for(int i=0; i<soBox[boxCnt]->NShape(); ++i){
				glPushMatrix();
				pose = soBox[boxCnt]->GetShapePose(i);
				ad = Affined(pose);
				glMultMatrixd(ad);

				CDShapeIf* shape = soBox[boxCnt]->GetShape(i);
				CDConvexMeshIf* mesh = DCAST(CDConvexMeshIf, shape);
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

				glPopMatrix();
			}
		glPopMatrix();
		std::cout << "\rBox count : " << static_cast<unsigned int>(soBox.size());
		fps();
	}

	glutSwapBuffers();
*/
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

	gluLookAt(0.0,10.0,30.0, 
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
void __cdecl reshape(int w, int h){
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
UTRef<ObjectStatesIf> states;

void __cdecl keyboard(unsigned char key, int x, int y){
	switch (key) {
		case ESC:		
		case 'q':
			exit(0);
			break;
		case 'r':
			bStep = true;
			break;
		case 'p':
			bStep = false;
			scene->Step();
			break;
		case 'P':
			scene->SetContactMode(PHSceneDesc::MODE_PENALTY);
			break;
		case 'C':
			scene->SetContactMode(PHSceneDesc::MODE_LCP);
			break;
		case ' ':
			states->ReleaseState(scene);
			{
				soBox.push_back(scene->CreateSolid(desc));
				soBox.back()->AddShape(meshConvex);
//				soBox.back()->SetFramePosition(Vec3f(0.5, 10+3*soBox.size(),0));
				soBox.back()->SetFramePosition(Vec3f(0, 20,0));
				soBox.back()->SetOrientation(
					Quaternionf::Rot(Rad(30), 'y') * 
					Quaternionf::Rot(Rad(10), 'x'));  
				std::ostringstream os;
				os << "box" << (unsigned int)soBox.size();
				soBox.back()->SetName(os.str().c_str());
			}break;
		case 'c':
			states->ReleaseState(scene);
			{
				soBox.push_back(scene->CreateSolid(desc));
				soBox.back()->SetAngularVelocity(Vec3f(0,0,0.2));
				soBox.back()->AddShape(meshCapsule);
				soBox.back()->SetFramePosition(Vec3f(0.5, 20,0));
//				soBox.back()->SetFramePosition(Vec3f(0.5, 10+3*soBox.size(),0));
				soBox.back()->SetOrientation(Quaternionf::Rot(Rad(30), 'y'));  
				std::ostringstream os;
				os << "capsule" << (unsigned int)soBox.size();
				soBox.back()->SetName(os.str().c_str());

			}break;
		case 'o':
			states->ReleaseState(scene);
			{
//				desc.dynamical = false;
//				desc.velocity.y = -1.0;
				soBox.push_back(scene->CreateSolid(desc));
				desc.velocity.y = 0.0;
				desc.dynamical = true;
				soBox.back()->SetAngularVelocity(Vec3f(0,0,0.2));
				soBox.back()->AddShape(meshRoundCone);
				soBox.back()->SetFramePosition(Vec3f(0.5, 20,0));
//				soBox.back()->SetFramePosition(Vec3f(0.5, 10+3*soBox.size(),0));
				soBox.back()->SetOrientation(Quaternionf::Rot(Rad(30), 'y'));  
				std::ostringstream os;
				os << "roundCone" << (unsigned int)soBox.size();
				soBox.back()->SetName(os.str().c_str());
#ifndef CREATE_FLOOR	
	// soFloor�p��desc
	desc.mass = 2.0;
	desc.inertia = 2.0 * Matrix3d::Unit();
	desc.mass = 1e20f;
	desc.inertia *= 1e20f;
	soFloor = scene->CreateSolid(desc);		// ���̂�desc�Ɋ�Â��č쐬
	soFloor->SetDynamical(false);
	soFloor->AddShape(meshFloor);
	soFloor->SetFramePosition(Vec3f(0,-1,0));

	soFloor->AddShape(meshWall);
	soFloor->AddShape(meshWall);
	soFloor->AddShape(meshWall);
	soFloor->AddShape(meshWall);
	soFloor->SetShapePose(1, Posed::Trn(-60, 0,   0));
	soFloor->SetShapePose(2, Posed::Trn(  0, 0, -40));
	soFloor->SetShapePose(3, Posed::Trn( 60, 0,   0));
	soFloor->SetShapePose(4, Posed::Trn(  0, 0,  40));
#endif
			}break;
		case 'd':
			states->ReleaseState(scene);
			{
				soBox.push_back(scene->CreateSolid(desc));
				soBox.back()->AddShape(meshSphere);
//				soBox.back()->SetFramePosition(Vec3f(0.5, 10+3*soBox.size(),0));
				soBox.back()->SetFramePosition(Vec3f(0.5, 20,0));
				soBox.back()->SetOrientation(Quaternionf::Rot(Rad(30), 'y'));  
				std::ostringstream os;
				os << "sphere" << (unsigned int)soBox.size();
				soBox.back()->SetName(os.str().c_str());
			}break;
		case 'm':
			states->ReleaseState(scene);
			{
				soBox.push_back(scene->CreateSolid(desc));
				CDConvexMeshDesc md;
				int nv = rand()%100 + 50;
				for(int i=0; i < nv; ++i){
					Vec3d v;
					for(int c=0; c<3; ++c){
						v[c] = (rand() % 100 / 100.0 - 0.5) * 5 * 1.3;
					}
					md.vertices.push_back(v);
				}
				CDShapeIf* s = sdk->CreateShape(md);
				soBox.back()->AddShape(s);
				soBox.back()->SetFramePosition(Vec3f(0.5, 20,0));
//				soBox.back()->SetFramePosition(Vec3f(0.5, 10+3*soBox.size(),0));
				soBox.back()->SetOrientation(Quaternionf::Rot(Rad(30), 'y'));  
				std::ostringstream os;
				os << "cmesh" << (unsigned int)soBox.size();
				soBox.back()->SetName(os.str().c_str());
			}break;
		case 'v':
			states->ReleaseState(scene);
			{
				soBox.push_back(scene->CreateSolid(desc));
				soBox.back()->AddShape(meshBox);
				soBox.back()->AddShape(meshBox);
				soBox.back()->AddShape(meshBox);
				soBox.back()->AddShape(meshBox);
				soBox.back()->AddShape(meshBox);
				soBox.back()->AddShape(meshBox);
				soBox.back()->AddShape(meshBox);
				Posed pose;
				pose.Pos() = Vec3d(3, 0, 0);
				soBox.back()->SetShapePose(1, pose);
				pose.Pos() = Vec3d(-3, 0, 0);
				soBox.back()->SetShapePose(2, pose);
				pose.Pos() = Vec3d(0, 3, 0);
				soBox.back()->SetShapePose(3, pose);
				pose.Pos() = Vec3d(0, -3, 0);
				soBox.back()->SetShapePose(4, pose);
				pose.Pos() = Vec3d(0, 0, 3);
				soBox.back()->SetShapePose(5, pose);
				pose.Pos() = Vec3d(0, 0, -3);
				soBox.back()->SetShapePose(6, pose);
				
				soBox.back()->SetFramePosition(Vec3f(0.5, 20,0));
//				soBox.back()->SetFramePosition(Vec3f(0.5, 10+3*soBox.size(),0));
				soBox.back()->SetOrientation(Quaternionf::Rot(Rad(30), 'y'));  
				std::ostringstream os;
				os << "box" << (unsigned int)soBox.size();
				soBox.back()->SetName(os.str().c_str());
			}break;
		case 'z':
			states->ReleaseState(scene);
			{
				PHSolidDesc soliddesc;
				double tower_radius = 10;
				int tower_height = 5;
				int numbox = 20;
				double theta;
				for(int i = 0; i < tower_height; i++){
					for(int j = 0; j < numbox; j++){
						soBox.push_back(scene->CreateSolid(soliddesc));
						soBox.back()->AddShape(meshBox);
						theta = ((double)j + (i % 2 ? 0.0 : 0.5)) * Rad(360) / (double)numbox;
						soBox.back()->SetFramePosition(Vec3f(0.5, 20,0));
//						soBox.back()->SetFramePosition(Vec3d(tower_radius * cos(theta), 2.0 * ((double)i + 0.5), tower_radius * sin(theta)));
						soBox.back()->SetOrientation(Quaterniond::Rot(-theta, 'y'));  
					}
				}
			}break;
		case 'x':
			states->ReleaseState(scene);
			{
				soBox.push_back(scene->CreateSolid(desc));
				soBox.back()->AddShape(meshBox);
				soBox.back()->SetFramePosition(Vec3f(0.0, 15.0, 10.0));
				soBox.back()->SetVelocity(Vec3d(0.0, 0.0, -5.0));
			}break;
		case 's':
			{
				states->SaveState(scene);
			}break;
		case 'k':
			{
				states->ReleaseState(scene);
			}break;
		case 'l':
			{
				states->LoadState(scene);
			}break;
		case 'S':
			{
				scene->WriteState("state.bin");
			}break;
		case 'L':
			{
				scene->ReadState("state.bin");
				scene->Step();
				std::ofstream f("dump_after_load.bin", std::ios::binary|std::ios::out);
				scene->DumpObjectR(f);
			}break;
		case 'D':
			{
				std::ofstream f("dump.bin", std::ios::binary|std::ios::out);
				scene->DumpObjectR(f);
			}break;
		default:
			break;
	}
}	

int xlast, ylast;
void __cdecl mouse(int button, int state, int x, int y){
	xlast = x, ylast = y;
	if(button == GLUT_LEFT_BUTTON)
		bLeftButton = (state == GLUT_DOWN);
	if(button == GLUT_RIGHT_BUTTON)
		bRightButton = (state == GLUT_DOWN);
}

void __cdecl motion(int x, int y){
	static bool bFirst = true;
	int xrel = x - xlast, yrel = y - ylast;
	xlast = x;
	ylast = y;
	if(bFirst){
		bFirst = false;
		return;
	}
	// ���{�^��
	if(bLeftButton){
		CameraRotY += xrel * 0.01;
		CameraRotY = Spr::max(Rad(-180.0), Spr::min(CameraRotY, Rad(180.0)));
		CameraRotX += yrel * 0.01;
		CameraRotX = Spr::max(Rad(-80.0), Spr::min(CameraRotX, Rad(80.0)));
	}
	// �E�{�^��
	if(bRightButton){
		CameraZoom *= exp(yrel/10.0);
		CameraZoom = Spr::max(0.1, Spr::min(CameraZoom, 100.0));
	}
}

//	#include <WinBasis/WBPreciseTimer.h>
//	WBPreciseTimer ptimer;

/**
 brief  	glutTimerFunc�Ŏw�肵���R�[���o�b�N�֐�
 param	 	<in/--> id�@�@ �^�C�}�[�̋�ʂ����邽�߂̏��
 return 	�Ȃ�
 */
void __cdecl timer(int id){
	/// �����̃`�F�b�N�Ɖ�ʂ̍X�V���s��
	float DT = 0.05f;
	if (bStep) scene->Step();
/*
	//	dv �ɂ͍S���ɂ�鑬�x�ω�������̂ŁA���̂ɉ�������S���͂����o�����Ƃ��ł���B
	if (soBox.size()) DSTR << ((PHSolid*)soBox.back())->dv << std::endl;
*/	
/*
	{
		// SaveState, LoadState�𗘗p����1�X�e�b�v��̌��ʂ�����
		states->SaveState(scene);
		int NSolids = scene->NSolids();
		PHSolidIf** solids = scene->GetSolids();
		std::vector<Vec3d> vel, nextvel;
		std::cout << "----------" << std::endl;
		for(int i = 0; i < NSolids; i++){
			vel.push_back(solids[i]->GetVelocity());
			std::cout << "vel" << vel[i] << std::endl;
 		}
		scene->Step();
		for(int i = 0; i < NSolids; i++){
			nextvel.push_back(solids[i]->GetVelocity());
			std::cout << "nextvel" << nextvel[i] << std::endl;
		}	
		states->LoadState(scene);
		states->ReleaseState(scene);
	}
*/

	glutPostRedisplay();
	unsigned int msecs = static_cast<unsigned int>(1000*DT);
	glutTimerFunc(msecs, timer, 0);
}

/**
 brief		���C���֐�
 param		<in/--> argc�@�@�R�}���h���C�����͂̌�
 param		<in/--> argv�@�@�R�}���h���C������
 return		0 (����I��)
 */
int main(int argc, char* argv[]){
	sdk = PHSdkIf::CreateSdk();					// SDK�̍쐬�@
	PHSceneDesc dscene;
	dscene.timeStep = 0.02;
	dscene.numIteration = 15;
	scene = sdk->CreateScene(dscene);				// �V�[���̍쐬
	scene->SetStateMode(true);
	states = ObjectStatesIf::Create();
	
#ifdef CREATE_FLOOR
	// soFloor�p��desc
	desc.mass = 10.0f;
	desc.inertia *= 100.0f;
	soFloor = scene->CreateSolid(desc);		// ���̂�desc�Ɋ�Â��č쐬
	soFloor->SetDynamical(false);
#endif

	// soBox�p��desc
	desc.mass = 2.0;
	desc.inertia = 2.0 * Matrix3d::Unit();

	//	�`��̍쐬
	{
		CDBoxDesc bd;
		bd.boxsize = Vec3f(2,2,2);
		meshBox = XCAST(sdk->CreateShape(bd));
		meshBox->SetName("meshBox");
		CDSphereDesc sd;
		sd.radius = 1;
		meshSphere = XCAST(sdk->CreateShape(sd));
		meshSphere->SetName("meshSphere");
		CDCapsuleDesc cd;
		cd.radius = 1;
		cd.length = 1;
		meshCapsule = XCAST(sdk->CreateShape(cd));
		meshCapsule->SetName("meshCapsule");

		CDRoundConeDesc rcd;
		rcd.length = 3;
		meshRoundCone= XCAST(sdk->CreateShape(rcd));
		meshRoundCone->SetName("meshRoundCone");
	}
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
		meshConvex = DCAST(CDConvexMeshIf, sdk->CreateShape(md));
		meshConvex->SetName("meshConvex");

		// soFloor(meshFloor)�ɑ΂��ăX�P�[�����O
		for(unsigned i=0; i<md.vertices.size(); ++i){
			md.vertices[i].x *= 30;
			md.vertices[i].z *= 20;
		}
		CDConvexMeshDesc cmd;
		cmd = md;
/*		for(int i=0; i < 30; ++i){
			Vec3d v;
			v.x = (rand() % 100 / 100.0 - 0.5) * 50;
			v.y = (rand() % 100 / 100.0 - 0.5) * 5;
			v.z = (rand() % 100 / 100.0 - 0.5) * 30;
			cmd.vertices.push_back(v);
		}
*/		meshFloor = DCAST(CDConvexMeshIf, sdk->CreateShape(cmd));
		meshFloor->SetName("meshFloor");
		
		for(unsigned i=0; i<md.vertices.size(); ++i){
			md.vertices[i].y *= 6;
		}
		meshWall = DCAST(CDConvexMeshIf, sdk->CreateShape(md));
		meshWall->SetName("meshWall");
	}


#ifdef CREATE_FLOOR
	soFloor->AddShape(meshFloor);
	soFloor->SetFramePosition(Vec3f(0,-1,0));

	soFloor->AddShape(meshWall);
	soFloor->AddShape(meshWall);
	soFloor->AddShape(meshWall);
	soFloor->AddShape(meshWall);
	soFloor->SetShapePose(1, Posed::Trn(-60, 0,   0));
	soFloor->SetShapePose(2, Posed::Trn(  0, 0, -40));
	soFloor->SetShapePose(3, Posed::Trn( 60, 0,   0));
	soFloor->SetShapePose(4, Posed::Trn(  0, 0,  40));
#endif

	scene->SetGravity(Vec3f(0,-30.0f, 0));	// �d�͂�ݒ�
	scene->SetTimeStep(0.05);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("BoxStack");

	grSdk = GRSdkIf::CreateSdk();
	render = grSdk->CreateDebugRender();
	render->SetRenderMode(true, false);
//	render->EnableRenderAxis();
	render->EnableRenderForce();
	render->EnableRenderContact();

	device = grSdk->CreateDeviceGL();
	device->Init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(0, timer, 0);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	glutReshapeWindow(800, 600);

	render->SetDevice(device);
	initialize();

	glutMainLoop();

	//	SDK�͊J�����Ȃ��Ă��ǂ��D���Ȃ��Ă�main�𔲂��Ă���J�������D
	delete sdk;
}
