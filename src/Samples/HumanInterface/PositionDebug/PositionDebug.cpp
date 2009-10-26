#include "PositionDebug.h"
#include <iostream>
#include <sstream>
#include <Framework/FWInteractScene.h>

#define SPIDAR 0;
using namespace std;



FWLDHapticSample::FWLDHapticSample(){
	bStep = true;
	bDrawInfo = false;
}
void FWLDHapticSample::Display(){
	/// �`�惂�[�h�̐ݒ�
	GetSdk()->SetDebugMode(true);
	GRDebugRenderIf* render = GetCurrentWin()->render->Cast();
	render->SetRenderMode(true, false);
	render->EnableRenderAxis(bDrawInfo);
	render->EnableRenderForce(bDrawInfo);
	render->EnableRenderContact(bDrawInfo);
	render->EnableRenderWorldAxis(true);
	render->EnableGrid(true,0.0,1.0);

	/// �J�������W�̎w��
	GRCameraIf* cam = GetCurrentWin()->scene->GetGRScene()->GetCamera();
	if (cam && cam->GetFrame()){
		cam->GetFrame()->SetTransform(cameraInfo.view);
	}else{
		GetCurrentWin()->render->SetViewMatrix(cameraInfo.view.inv());
	}

	/// �`��̎��s
	if(!GetCurrentWin()) return;
	GetSdk()->SwitchScene(GetCurrentWin()->GetScene());
	GetSdk()->SwitchRender(GetCurrentWin()->GetRender());
	GetSdk()->Draw();
	
	DisplayContactPlane();
	DisplayLineToNearestPoint();

	glutSwapBuffers();
}
void FWLDHapticSample::Init(int argc, char* argv[]){
	/// Sdk�̏������C�V�[���̍쐬
	CreateSdk();
	GetSdk()->Clear();										// SDK�̏�����
	GetSdk()->CreateScene(PHSceneDesc(), GRSceneDesc());	// Scene�̍쐬
	GetSdk()->GetScene()->GetPHScene()->SetTimeStep(0.02);	// ���݂̐ݒ�
	//GetSdk()->GetScene()->GetPHScene()->SetGravity(Vec3d());
	/// �`�惂�[�h�̐ݒ�
	SetGRAdaptee(TypeGLUT);									// GLUT���[�h�ɐݒ�
	GetGRAdaptee()->Init(argc, argv);						// Sdk�̍쐬

	/// �`��Window�̍쐬�C������
	FWWinDesc windowDesc;									// GL�̃E�B���h�E�f�B�X�N���v�^
	windowDesc.title = "Springhead2";						// �E�B���h�E�̃^�C�g��
	CreateWin(windowDesc);									// �E�B���h�E�̍쐬
	InitWindow();											// �E�B���h�E�̏�����
	InitCameraView();										// �J�����r���[�̏�����

	/// HumanInterface�̏�����
	InitHumanInterface();

	/// InteractScene�̍쐬
	FWInteractSceneDesc desc;
	desc.fwScene = GetSdk()->GetScene();					// fwScene�ɑ΂���interactscene�����
	desc.iaMode = LOCAL_DYNAMICS;								// humaninterface�̃����_�����O���[�h�̐ݒ�
	desc.hdt = 0.001;										// �}���`���[�g�̏ꍇ�̍X�V[s]
	CreateIAScene(desc);									// interactScene�̍쐬

	/// �����V�~�����[�V�������鍄�̂��쐬
	BuildPointer();

	/// �^�C�}�̍쐬�C�ݒ�
	int mtimer = CreateTimer(FWTimer::MM);				// �^�C�}���쐬
	SetResolution(mtimer,1);									// ����\[ms]
	SetInterval(mtimer,1);									// �Ăт������o[ms]
}
void FWLDHapticSample::InitCameraView(){
	//std::istringstream issView(
	//	"((0.9996 0.0107463 -0.0261432 -0.389004)"
	//	"(-6.55577e-010 0.924909 0.380188 5.65711)"
	//	"(0.0282657 -0.380037 0.92454 13.7569)"
	//	"(     0      0      0      1))"
	//);
	//�^�ォ��̎��_
	std::istringstream issView(
		"((1.0 0.0 0.0 0.0)"
		"(0.0 0.0 1.0 20.0)"
		"(0.0 -1.0 0.0 0)"
		"(     0      0      0      1))"
	);
	issView >> cameraInfo.view;
}

void FWLDHapticSample::InitHumanInterface(){
	/// HISdk�̍쐬
	CreateHISdk();

	DRUsb20SimpleDesc usbSimpleDesc;
	GetHISdk()->AddRealDevice(DRUsb20SimpleIf::GetIfInfoStatic(), &usbSimpleDesc);
	DRUsb20Sh4Desc usb20Sh4Desc;
	for(int i=0; i<10; ++i){
		usb20Sh4Desc.number = i;
		GetHISdk()->AddRealDevice(DRUsb20Sh4If::GetIfInfoStatic(), &usb20Sh4Desc);
	}
	GetHISdk()->AddRealDevice(DRKeyMouseWin32If::GetIfInfoStatic());
	GetHISdk()->Init();
	GetHISdk()->Print(DSTR);

#if SPIDAR

	/// SPIDARG6��2��g���ꍇ
	UTRef<HISpidarGIf> spg[2];
	for(size_t i = 0; i < 2; i++){
		spg[i] = GetHISdk()->CreateHumanInterface(HISpidarGIf::GetIfInfoStatic())->Cast();
		if(i == 0) spg[i]->Init(&HISpidarGDesc("SpidarG6X3R"));
		if(i == 1) spg[i]->Init(&HISpidarGDesc("SpidarG6X3L"));
		AddHI(spg[i]);
	}
#else
	/// SPIDAR4D���g���ꍇ
	UTRef<HISpidar4If> spg[2];
	for(size_t i = 0; i < 2; i++){
		spg[i] = GetHISdk()->CreateHumanInterface(HISpidar4If::GetIfInfoStatic())->Cast();
		if(i == 0) spg[i]->Init(&HISpidar4Desc("SpidarR",Vec4i(1,2,3,4)));
		if(i == 1) spg[i]->Init(&HISpidar4Desc("SpidarL",Vec4i(5,6,7,8)));
		AddHI(spg[i]);
	}
#endif
}
void FWLDHapticSample::BuildPointer(){
	PHSceneIf* phscene = GetSdk()->GetScene()->GetPHScene();
	PHSolidDesc desc;

	/// �|�C���^
	{	
		for(int i= 0; i < 2; i++){
			PHSolidIf* soPointer = phscene->CreateSolid(desc);
			CDSphereDesc sd;
			sd.radius = 0.5;//1.0;
			CDSphereIf* shapePointer = DCAST(CDSphereIf,  GetSdk()->GetPHSdk()->CreateShape(sd));
			soPointer->AddShape(shapePointer);
			soPointer->SetDynamical(false);
			soPointer->GetShape(0)->SetStaticFriction(1.0);
			soPointer->GetShape(0)->SetDynamicFriction(1.0);
			GetSdk()->GetScene()->GetPHScene()->SetContactMode(soPointer, PHSceneDesc::MODE_NONE);
			FWInteractPointerDesc idesc;			// interactpointer�̃f�B�X�N���v�^
			idesc.pointerSolid = soPointer;			// soPointer��ݒ�
			idesc.humanInterface = GetHI(i);		// humaninterface��ݒ�
			idesc.springK = 10;						// haptic rendering�̃o�l�W��
			idesc.damperD = 0.05;					// haptic rendering�̃_���p�W��
		#if SPIDAR
			idesc.posScale = 300;					// soPointer�̉���̐ݒ�(�`�{)
			idesc.forceScale = 1.0;
		#else
			idesc.posScale = 60;					// soPointer�̉���̐ݒ�(�`�{)
		#endif
			idesc.localRange = 1.0;					// LocalDynamics���g���ꍇ�̋ߖT�͈�
			//idesc.defaultPosition =Posed(1,0,0,0,456,0,0);

			GetIAScene()->CreateIAPointer(idesc);	// interactpointer�̍쐬
		}
		//GetIAScene()->GetIAPointer(0)->SetDefaultPosition(Posed(1,0,0,0,10,0,0));
	}
	//
}

void FWLDHapticSample::IdleFunc(){
	/// �V�~�����[�V������i�߂�(interactscene������ꍇ�͂��������Ă�)
	if(bStep) FWLDHapticSample::instance->GetIAScene()->Step();
	else if(bOneStep){
		FWLDHapticSample::instance->GetIAScene()->Step();
		bOneStep = false;
	}
	PostRedisplay();
}

void FWLDHapticSample::TimerFunc(int id){	
	/// HapticLoop���R�[���o�b�N����
	((FWLDHapticSample*)instance)->GetIAScene()->CallBackHapticLoop();
	PostRedisplay();
}


void FWLDHapticSample::TwoPointerCalib(){
	ReleaseAllTimer();
	TMatrixRow<6,3,float> mainMat ;
	TMatrixRow<6,4,float> subMat ;
	TMatrixRow<4,3,float> transMat ;
	Affined TAffine ;

	if(mainPosition.size()>5){

	//mainMat�F(x,y,z)�����Ƃ���n�s�̍s��
	for(int i=0; i<6 ; i++){
		for(int j=0; j<3; j++){
			mainMat[i][j] =mainPosition[i][j];
		}
	}
	//subMat�F(x,y,z,1)�����Ƃ���n�s�̍s��
	for(int i=0; i<6 ; i++){
		for(int j=0; j<4; j++){
			if(j==3) subMat[i][j] = 1;
			else subMat[i][j] =subPosition[i][j];	
		}
	}
	//DSTR<<"mainMat"<<std::endl<<mainMat<<std::endl;
	//DSTR<<"subMat"<<std::endl<<subMat<<std::endl;

	//�^���t�s�񂩂�sub��main�ɕϊ�����affin�s��̐��l���Z�o
	TMatrixRow<4,4,float> subMatb = subMat.trans()* subMat;
	transMat = subMatb.inv()* subMat.trans() * mainMat;

	//Affin�s��̌`�ɐ��`
	for(int i=0; i<3; i++){
		for(int j=0; j<4; j++){
			TAffine[i][j]=transMat[j][i];
		}
	}
	TAffine[3][0]=0;
	TAffine[3][1]=0;
	TAffine[3][2]=0;
	TAffine[3][3]=1;
	
	//DSTR<<"TAffine"<<std::endl<<TAffine<<std::endl;
	for(int i=0;i<6;i++){
		DSTR<<"mainMat"<<mainMat[i]<<std::endl;
		DSTR<<"subMat"<<subMat[i]<<std::endl;
		Vec3d NewPosition = TAffine*subPosition[i];
		DSTR<<"NewPosition"<<NewPosition<<std::endl;
	}

	tPos.FromAffine(TAffine);
	GetIAScene()->GetIAPointer(1)->SetPointersCalibPosition(tPos);
	//DSTR<<tPos<<std::endl;
	//GetIAScene()->GetIAPointer(0)->SetDefaultPosition(Posed(1,0,0,0,10,0,0));
	//DSTR<<GetIAScene()->GetIAPointer(0)->GetDefaultPosition()<<std::endl;
	//DSTR<<GetIAScene()->GetIAPointer(1)->GetDefaultPosition()<<std::endl;
	//DSTR<<"After"<<GetIAScene()->GetIAPointer(0)->GetDefaultPosition()<<std::endl;
	//DSTR<<"After"<<GetIAScene()->GetIAPointer(1)->GetDefaultPosition()<<std::endl;

	FWLDHapticSample* adaptee = (FWLDHapticSample*)GetIAScene(0)->GetIAAdaptee();
	//adaptee->SyncPointer();
	
	}
	CreateAllTimer();

}

void FWLDHapticSample::TwoPointerSet(){
	Vec3d mainPos = GetIAScene()->GetIAPointer(0)->GetPointerSolid()->GetFramePosition();
	Vec3d subPos = GetIAScene()->GetIAPointer(1)->GetPointerSolid()->GetFramePosition();

	mainPosition.push_back(mainPos);
	subPosition.push_back(subPos);

	DSTR<<"mainPos"<<mainPosition.size()<<":"<<mainPos<<std::endl;
	DSTR<<"subPos"<<subPosition.size()<<":"<<subPos<<std::endl;

	//for(int i=0;i<4;i++){
	//	mainPosition.push_back(Vec3d(0,0,0));
	//	subPosition.push_back(Vec3d(0,0,0));
	//}

	//mainPosition.push_back(Vec3d(1,0,0));
	//subPosition.push_back(Vec3d(3,0,0));
	//mainPosition.push_back(Vec3d(0,1,0));
	//subPosition.push_back(Vec3d(0,3,0));
	//mainPosition.push_back(Vec3d(0,0,1));
	//subPosition.push_back(Vec3d(0,0,3));
	//mainPosition.push_back(Vec3d(0,2,1));
	//subPosition.push_back(Vec3d(0,6,3));

}

void FWLDHapticSample::Keyboard(int key, int x, int y){
	PHSceneIf* phscene=GetSdk()->GetPHSdk()->GetScene(0);

	switch (key) {
		case  27: //ESC
		case 'q':
		exit(0);
		break;
		case 'r':
			{
				Reset();
			}
			break;
		case 's':
			bStep = false;
			bOneStep = true;
			DSTR << "Stepwise Execution" << endl;
		break;
		case ' ':
			{
				DSTR<<cameraInfo.view<<std::endl;
			}
			break;

		case 'v':
			{
				GetIAScene()->GetIAPointer(1)->SetDefaultPosition(Posed(1,0,0,0,10,0,0));
				DSTR<<GetIAScene()->GetIAPointer(1)->GetDefaultPosition()<<std::endl;
				//�^�ォ��̎��_
				std::istringstream issView(
					"((1.0 0.0 0.0 0.0)"
					"(0.0 0.0 1.0 20.0)"
					"(0.0 -1.0 0.0 0)"
					"(     0      0      0      1))"
				);
				issView >> cameraInfo.view;
			}
			break;

			
		case 'b':
			{
				DSTR<<GetIAScene()->GetIAPointer(1)->GetDefaultPosition()<<std::endl;
				//�^�O����̎��_
				std::istringstream issView(
					"((1.0 0.0 0.0 0.0)"
					"(0.0 1.0 0.0 0.0)"
					"(0.0 0.0 1.0 20.0)"
					"(     0      0      0      1))"
				);
				issView >> cameraInfo.view;
			}
			break;

		case 'z':
			{
				TwoPointerSet();
			}

			break;
			
		case 'x':
			{
				TwoPointerCalib();
			}
		case 'c':
		{
			ReleaseAllTimer();
			for(int i = 0; i < GetIAScene()->NIAPointers(); i++){
				GetIAScene()->GetIAPointer(i)->Calibration();
			}
			CreateAllTimer();
		}
		break;
		case 'f':
		{
			static bool bf = false;
			bf = !bf;
			for(int i = 0; i < GetIAScene()->NIAPointers(); i++){
				GetIAScene()->GetIAPointer(i)->EnableForce(bf);
			}
			if(bf){
				DSTR << "Enable Force Feedback" << std::endl;
			}else{
				DSTR << "Disable Force Feedback" << std::endl;
			}
		}
		break;

			break;
		default:
			break;
	}
}
void FWLDHapticSample::DisplayContactPlane(){
	FWInteractScene* inScene = GetIAScene()->Cast();
	int N = inScene->NIASolids();
	for(int i = 0; i <  N; i++){
		FWInteractSolid* inSolid = inScene->GetIASolid(i);
		if(!inSolid->bSim) continue;
		for(int j = 0; j < inScene->NIAPointers(); j++){
			FWInteractPointer* inPointer = inScene->GetIAPointer(j)->Cast();
			FWInteractInfo* inInfo = &inPointer->interactInfo[i];
			if(!inInfo->flag.blocal) continue;
			Vec3d pPoint = inPointer->pointerSolid->GetPose() * inInfo->neighborInfo.pointer_point;
			Vec3d cPoint = inSolid->sceneSolid->GetPose() * inInfo->neighborInfo.closest_point;
			Vec3d normal = inInfo->neighborInfo.face_normal;
			Vec3d v1(0,1,0);

			v1 +=  Vec3d(0, 0, 0.5) - Vec3d(0, 0, 0.5)*normal*normal;
			v1 -= v1*normal * normal;
			v1.unitize();
			Vec3d v2 = normal ^ v1;

			Vec4f moon(1.0, 1.0, 0.8, 0.3);
			GRDebugRenderIf* render = GetCurrentWin()->render->Cast();
			render->SetMaterial( GRMaterialDesc(moon) );
			render->PushModelMatrix();
			Vec3d offset = 0.02 * normal;
			render->SetLighting( false );
			render->SetAlphaTest(true);
			render->SetAlphaMode(render->BF_SRCALPHA, render->BF_ONE);
			cPoint += offset/2;
			glBegin(GL_QUADS);
				/// �ڐG�ʒ��	
				glVertex3d(cPoint[0] + v1[0] + v2[0], cPoint[1] + v1[1] + v2[1], cPoint[2] + v1[2] + v2[2]);
				glVertex3d(cPoint[0] - v1[0] + v2[0], cPoint[1] - v1[1] + v2[1], cPoint[2] - v1[2] + v2[2]);
				glVertex3d(cPoint[0] - v1[0] - v2[0], cPoint[1] - v1[1] - v2[1], cPoint[2] - v1[2] - v2[2]);
				glVertex3d(cPoint[0] + v1[0] - v2[0], cPoint[1] + v1[1] - v2[1], cPoint[2] + v1[2] - v2[2]);
				/// ����1
				glVertex3d(cPoint[0] + v1[0] + v2[0] + offset[0], 
								cPoint[1] + v1[1] + v2[1] + offset[1], 
								cPoint[2] + v1[2] + v2[2] + offset[2]);
				glVertex3d(cPoint[0] + v1[0] + v2[0], 
								cPoint[1] + v1[1] + v2[1], 
								cPoint[2] + v1[2] + v2[2]);
				glVertex3d(cPoint[0] - v1[0] + v2[0], 
								cPoint[1] - v1[1] + v2[1], 
								cPoint[2] - v1[2] + v2[2]);
				glVertex3d(cPoint[0] - v1[0] + v2[0] + offset[0], 
								cPoint[1] - v1[1] + v2[1] + offset[1], 
								cPoint[2] - v1[2] + v2[2] + offset[2]);
				/// ����2
				glVertex3d(cPoint[0] - v1[0] + v2[0] + offset[0], 
								cPoint[1] - v1[1] + v2[1] + offset[1], 
								cPoint[2] - v1[2] + v2[2] + offset[2]);
				glVertex3d(cPoint[0] - v1[0] + v2[0], 
								cPoint[1] - v1[1] + v2[1], 
								cPoint[2] - v1[2] + v2[2]);
				glVertex3d(cPoint[0] - v1[0] - v2[0], 
								cPoint[1] - v1[1] - v2[1], 
								cPoint[2] - v1[2] - v2[2]);
				glVertex3d(cPoint[0] - v1[0] - v2[0] + offset[0], 
								cPoint[1] - v1[1] - v2[1] + offset[1], 
								cPoint[2] - v1[2] - v2[2] + offset[2]);
				/// ����3
				glVertex3d(cPoint[0] - v1[0] - v2[0] + offset[0], 
								cPoint[1] - v1[1] - v2[1] + offset[1], 
								cPoint[2] - v1[2] - v2[2] + offset[2]);
				glVertex3d(cPoint[0] - v1[0] - v2[0], 
								cPoint[1] - v1[1] - v2[1], 
								cPoint[2] - v1[2] - v2[2]);
				glVertex3d(cPoint[0] + v1[0] - v2[0], 
								cPoint[1] + v1[1] - v2[1], 
								cPoint[2] + v1[2] - v2[2]);
				glVertex3d(cPoint[0] + v1[0] - v2[0] + offset[0], 
								cPoint[1] + v1[1] - v2[1] + offset[1], 
								cPoint[2] + v1[2] - v2[2] + offset[2]);
				/// ����4
				glVertex3d(cPoint[0] + v1[0] - v2[0] + offset[0], 
								cPoint[1] + v1[1] - v2[1] + offset[1], 
								cPoint[2] + v1[2] - v2[2] + offset[2]);
				glVertex3d(cPoint[0] + v1[0] - v2[0], 
								cPoint[1] + v1[1] - v2[1], 
								cPoint[2] + v1[2] - v2[2]);
				glVertex3d(cPoint[0] + v1[0] + v2[0], 
								cPoint[1] + v1[1] + v2[1],
								cPoint[2] + v1[2] + v2[2]);
				glVertex3d(cPoint[0] + v1[0] + v2[0] + offset[0], 
								cPoint[1] + v1[1] + v2[1] + offset[1], 
								cPoint[2] + v1[2] + v2[2] + offset[2]);
				/// �ڐG�ʏ��
				glVertex3d(cPoint[0] - v1[0] + v2[0] + offset[0], 
								cPoint[1] - v1[1] + v2[1] + offset[1], 
								cPoint[2] - v1[2] + v2[2] + offset[2]);
				glVertex3d(cPoint[0] + v1[0] + v2[0] + offset[0], 
								cPoint[1] + v1[1] + v2[1] + offset[1], 
								cPoint[2] + v1[2] + v2[2] + offset[2]);
				glVertex3d(cPoint[0] + v1[0] - v2[0] + offset[0], 
								cPoint[1] + v1[1] - v2[1] + offset[1], 
								cPoint[2] + v1[2] - v2[2] + offset[2]);
				glVertex3d(cPoint[0] - v1[0] - v2[0] + offset[0], 
								cPoint[1] - v1[1] - v2[1] + offset[1], 
								cPoint[2] - v1[2] - v2[2] + offset[2]);
			glEnd();
			render->SetLighting( true);
			render->SetAlphaTest(false);
			render->PopModelMatrix();
			glEnable(GL_DEPTH_TEST);
		}
	}
}

void FWLDHapticSample::DisplayLineToNearestPoint(){
	FWInteractScene* inScene = DCAST(FWInteractScene, GetIAScene());
	int N = inScene->NIASolids();
	GLfloat moon[]={0.8,0.8,0.8};
	for(int i = 0; i <  N; i++){
		FWInteractSolid* inSolid = inScene->GetIASolid(i);
		if(!inSolid->bSim) continue;
		for(int j = 0; j < inScene->NIAPointers(); j++){
			FWInteractPointer* inPointer = inScene->GetIAPointer(j)->Cast();
			FWInteractInfo* inInfo = &inPointer->interactInfo[i];
			if(!inInfo->flag.blocal) continue;
			Vec3d pPoint = inPointer->pointerSolid->GetPose() * inInfo->neighborInfo.pointer_point;
			Vec3d cPoint = inSolid->sceneSolid->GetPose() * inInfo->neighborInfo.closest_point;
			Vec3d normal = inInfo->neighborInfo.face_normal;
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, moon);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, moon);
			glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, moon);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, moon);
			glDisable(GL_DEPTH_TEST);
			glBegin(GL_LINES);
			glVertex3f(pPoint.X() + normal[0], pPoint.Y() + normal[1], pPoint.Z() + normal[2]);
			glVertex3f(cPoint.X(), cPoint.Y(), cPoint.Z());
			glEnd();
			glEnable(GL_DEPTH_TEST);
		}
	}
}