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


//#include <windows.h>
//#include <ctime>
//#include <GL/glut.h>
//#include <sstream>

#include "../../SampleApp.h"

#pragma hdrstop
using namespace Spr;
using namespace std;

/*int frame=0,timepassed=0,timebase=0;
void fps(){
	frame++; 
	timepassed = glutGet(GLUT_ELAPSED_TIME); 
	if (1000 < timepassed - timebase) {
		std::cout << "   FPS:" << frame*1000.0/(timepassed - timebase);
		timebase = timepassed;		
		frame = 0;
	}
}*/

class MyApp : public SampleApp{
public:
	enum {
		ID_BOX = ID_LAST,
		ID_CAPSULE,
		ID_ROUNDCONE,
		ID_SPHERE,
		ID_ROCK,
		ID_BLOCK,
		ID_TOWER,
		ID_SHAKE,
	};

	bool					createFloor;

	CDConvexMeshIf*			meshFloor;
	CDConvexMeshIf*			meshWall;
	CDConvexMeshIf*			meshConvex;
	CDBoxIf*				meshBox;
	CDSphereIf*				meshSphere;
	CDCapsuleIf*			meshCapsule;
	CDRoundConeIf*			meshRoundCone;

	PHSolidIf*				soFloor;
	std::vector<PHSolidIf*> soBox;

	double					floorShakeAmplitude;

public:
	MyApp(){
		appName = "BoxStack";
		createFloor = true;
		floorShakeAmplitude = 0.0;

		AddAction(ID_BOX, "drop box");
		AddHotKey(ID_BOX, 'b');
		AddAction(ID_CAPSULE, "drop capsule");
		AddHotKey(ID_CAPSULE, 'c');
		AddAction(ID_ROUNDCONE, "drop round cone");
		AddHotKey(ID_ROUNDCONE, 'r');
		AddAction(ID_SPHERE, "drop sphere");
		AddHotKey(ID_SPHERE, 's');
		AddAction(ID_ROCK, "drop rock");
		AddHotKey(ID_ROCK, 'd');
		AddAction(ID_BLOCK, "drop block");
		AddHotKey(ID_BLOCK, 'e');
		AddAction(ID_TOWER, "drop tower");
		AddHotKey(ID_TOWER, 't');
		AddAction(ID_SHAKE, "shake floor");
		AddHotKey(ID_SHAKE, 'f');
	}
	~MyApp(){}

	virtual void BuildScene(){
		/// �`��ݒ�
		//fwScene->SetSolidMaterial(GRRenderIf::WHITE);
		fwScene->SetWireMaterial(GRRenderIf::WHITE);
		fwScene->SetRenderMode(true, true);				///< �\���b�h�`��C���C���t���[���`��
		fwScene->EnableRenderAxis(true, true, true);		///< ���W��
		fwScene->SetAxisStyle(FWSceneIf::AXIS_ARROWS);	///< ���W���̃X�^�C��
		fwScene->EnableRenderForce(false, true);			///< ��
		fwScene->EnableRenderContact(true);				///< �ڐG�f��

		phScene = fwScene->GetPHScene();
		phScene->SetGravity(Vec3f(0.0f, -9.8f, 0.0f));	// �d�͂�ݒ�
		phScene->SetTimeStep(0.05);
		phScene->SetNumIteration(15);
		//phScene->SetNumIteration(10, 1);	// correction iteration
		//phScene->SetNumIteration(10, 2);	// contact iteration
		
		phScene->SetStateMode(true);
		states = ObjectStatesIf::Create();
		//scene->GetConstraintEngine()->SetUseContactSurface(true); //�ʐڐG�ł̗͌v�Z��L����

		//	�`��̍쐬
		CDBoxDesc bd;
		bd.boxsize = Vec3f(2,2,2);
		meshBox = XCAST(GetSdk()->GetPHSdk()->CreateShape(bd));
		meshBox->SetName("meshBox");

		CDSphereDesc sd;
		sd.radius = 1;
		meshSphere = XCAST(GetSdk()->GetPHSdk()->CreateShape(sd));
		meshSphere->SetName("meshSphere");

		CDCapsuleDesc cd;
		cd.radius = 1;
		cd.length = 1;
		meshCapsule = XCAST(GetSdk()->GetPHSdk()->CreateShape(cd));
		meshCapsule->SetName("meshCapsule");

		CDRoundConeDesc rcd;
		rcd.length = 3;
		meshRoundCone= XCAST(GetSdk()->GetPHSdk()->CreateShape(rcd));
		meshRoundCone->SetName("meshRoundCone");
		
		CDConvexMeshDesc md;
		md.vertices.push_back(Vec3f(-1,-1,-1));
		md.vertices.push_back(Vec3f(-1,-1, 1));	
		md.vertices.push_back(Vec3f(-1, 1,-1));	
		md.vertices.push_back(Vec3f(-1, 1, 1));
		md.vertices.push_back(Vec3f( 1,-1,-1));	
		md.vertices.push_back(Vec3f( 1,-1, 1));
		md.vertices.push_back(Vec3f( 1, 1,-1));
		md.vertices.push_back(Vec3f( 1, 1, 1));
		meshConvex = XCAST(GetSdk()->GetPHSdk()->CreateShape(md));
		meshConvex->SetName("meshConvex");

		// soFloor(meshFloor)�ɑ΂��ăX�P�[�����O
		for(unsigned i=0; i<md.vertices.size(); ++i){
			md.vertices[i].x *= 30;
			md.vertices[i].z *= 20;
		}
		CDConvexMeshDesc cmd;
		cmd = md;
		meshFloor = XCAST(GetSdk()->GetPHSdk()->CreateShape(cmd));
		meshFloor->SetName("meshFloor");
		
		for(unsigned i=0; i<md.vertices.size(); ++i){
			md.vertices[i].y *= 6;
		}
		meshWall = XCAST(GetSdk()->GetPHSdk()->CreateShape(md));
		meshWall->SetName("meshWall");

		if(createFloor){
			// ���̍���
			soFloor = phScene->CreateSolid();
			soFloor->SetMass(10.0);
			soFloor->SetInertia(100.0 * Matrix3d::Unit());
			soFloor->SetDynamical(false);
			soFloor->SetFramePosition(Vec3f(0,-1,0));
			// ���̌`��
			soFloor->AddShape(meshFloor);
			soFloor->AddShape(meshWall);
			soFloor->AddShape(meshWall);
			soFloor->AddShape(meshWall);
			soFloor->AddShape(meshWall);
			soFloor->SetShapePose(1, Posed::Trn(-60, 0,   0));
			soFloor->SetShapePose(2, Posed::Trn(  0, 0, -40));
			soFloor->SetShapePose(3, Posed::Trn( 60, 0,   0));
			soFloor->SetShapePose(4, Posed::Trn(  0, 0,  40));

			fwScene->SetSolidMaterial(GRRenderIf::GRAY, soFloor);
		}
		// �^�C�}
		timer = CreateTimer(UTTimerIf::FRAMEWORK);
		timer->SetInterval(10);
	}

	// �^�C�}�R�[���o�b�N�֐��D�^�C�}�����ŌĂ΂��
	virtual void OnStep() {
		SampleApp::OnStep();
		// ����h�炷
		if (soFloor){
			double time = phScene->GetCount() * phScene->GetTimeStep();
			double omega = 2.0 * M_PI;
			soFloor->SetFramePosition(Vec3d(floorShakeAmplitude*sin(time*omega),0,0));			
			soFloor->SetVelocity(Vec3d(floorShakeAmplitude*omega*cos(time*omega),0,0));
		}
	}

	// �`��֐��D�`��v���������Ƃ��ɌĂ΂��
	virtual void OnDraw(GRRenderIf* render) {
		SampleApp::OnDraw(render);

		std::ostringstream sstr;
		sstr << "NObj = " << phScene->NSolids();
		render->DrawFont(Vec2f(-21, 23), sstr.str());
	}

	void Drop(int shape, Vec3d v, Vec3d w, Vec3d p, Quaterniond q){
		UTString basename;
		PHSolidDesc desc;
		CDConvexMeshDesc md;
		Posed pose;
			
		states->ReleaseState(phScene);
		PHSolidIf* solid = phScene->CreateSolid(desc);
		switch(shape){
		// ��
		case 0:
			solid->AddShape(meshBox);
			fwScene->SetSolidMaterial(GRRenderIf::BLUE, solid);
			basename = "box";
			break;
		// �J�v�Z��
		case 1:
			solid->AddShape(meshCapsule);
			fwScene->SetSolidMaterial(GRRenderIf::GREEN, solid);
			basename = "capsule";
			break;
		// �ۃR�[��
		case 2:
			solid->AddShape(meshRoundCone);
			fwScene->SetSolidMaterial(GRRenderIf::RED, solid);
			basename = "roundcone";
			break;
		// ��
		case 3:
			solid->AddShape(meshSphere);
			fwScene->SetSolidMaterial(GRRenderIf::YELLOW, solid);
			basename = "sphere";
			break;
		// �����_���ʑ��ʑ�
		case 4:{
			int nv = rand()%100 + 50;
			for(int i=0; i < nv; ++i){
				Vec3d v;
				for(int c=0; c<3; ++c){
					v[c] = (rand() % 100 / 100.0 - 0.5) * 5 * 1.3;
				}
				md.vertices.push_back(v);
			}
			solid->AddShape(GetSdk()->GetPHSdk()->CreateShape(md));
			fwScene->SetSolidMaterial(GRRenderIf::ORANGE, solid);
			basename = "cmesh";
			}break;
		// ���̉�
		case 5:
			for(int i = 0; i < 7; i++)
				solid->AddShape(meshBox);			
			pose.Pos() = Vec3d( 3,  0,  0); solid->SetShapePose(1, pose);
			pose.Pos() = Vec3d(-3,  0,  0); solid->SetShapePose(2, pose);
			pose.Pos() = Vec3d( 0,  3,  0); solid->SetShapePose(3, pose);
			pose.Pos() = Vec3d( 0, -3,  0); solid->SetShapePose(4, pose);
			pose.Pos() = Vec3d( 0,  0,  3); solid->SetShapePose(5, pose);
			pose.Pos() = Vec3d( 0,  0, -3); solid->SetShapePose(6, pose);
			basename = "boxes";
			fwScene->SetSolidMaterial(GRRenderIf::CYAN, solid);
			break;
		}

		solid->SetVelocity(v);
		solid->SetAngularVelocity(w);
		solid->SetFramePosition(p);
		solid->SetOrientation(q);
		
		soBox.push_back(solid);
		std::ostringstream os;
		os << basename << (unsigned int)soBox.size();
		soBox.back()->SetName(os.str().c_str());
	}

	virtual void OnAction(int id){
		if(id == ID_BOX)
			Drop(0, Vec3d(), Vec3d(0, 0, 0.2), Vec3d(0.5, 20, 0), Quaterniond::Rot(Rad(30), 'y'));
		if(id == ID_CAPSULE)
			Drop(1, Vec3d(), Vec3d(0, 0, 0.2), Vec3d(0.5, 20, 0), Quaterniond::Rot(Rad(30), 'y'));
		if(id == ID_ROUNDCONE)	
			Drop(2, Vec3d(), Vec3d(0, 0, 0.2), Vec3d(0.5, 20, 0), Quaterniond::Rot(Rad(30), 'y'));
		if(id == ID_SPHERE)
			Drop(3, Vec3d(), Vec3d(0, 0, 0.2), Vec3d(0.5, 20, 0), Quaterniond::Rot(Rad(30), 'y'));
		if(id == ID_ROCK)
			Drop(4, Vec3d(), Vec3d(0, 0, 0.2), Vec3d(0.5, 20, 0), Quaterniond::Rot(Rad(30), 'y'));
		if(id == ID_BLOCK)
			Drop(5, Vec3d(), Vec3d(0, 0, 0.2), Vec3d(0.5, 20, 0), Quaterniond::Rot(Rad(30), 'y'));
		if(id == ID_TOWER){
			const double tower_radius = 10;
			const int tower_height = 5;
			const int numbox = 20;
			double theta;
			for(int i = 0; i < tower_height; i++){
				for(int j = 0; j < numbox; j++){
					theta = ((double)j + (i % 2 ? 0.0 : 0.5)) * Rad(360) / (double)numbox;
					Drop(0, Vec3d(), Vec3d(), Vec3d(0.5, 20, 0), Quaterniond::Rot(-theta, 'y'));
				}
			}
		}
		if(id == ID_SHAKE){
			std::cout << "F: shake floor." << std::endl;
			if (!floorShakeAmplitude) floorShakeAmplitude = 2;
			else floorShakeAmplitude = 0;
		}
		SampleApp::OnAction(id);
	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////

MyApp app;

/**
 brief		���C���֐�
 param		<in/--> argc�@�@�R�}���h���C�����͂̌�
 param		<in/--> argv�@�@�R�}���h���C������
 return		0 (����I��)
 */
int main(int argc, char *argv[]) {
	app.Init(argc, argv);
	app.StartMainLoop();
	return 0;
}

