#include "PositionDebug.h"
#include <iostream>
#include <sstream>
#include <Framework/FWInteractScene.h>

#define SPIDAR 1;
using namespace std;



FWLDHapticSample::FWLDHapticSample(){
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
	desc.mode = LOCAL_DYNAMICS;								// humaninterface�̃����_�����O���[�h�̐ݒ�
	desc.hdt = 0.001;										// �}���`���[�g�̏ꍇ�̍X�V[s]
	CreateINScene(desc);									// interactScene�̍쐬

	/// �����V�~�����[�V�������鍄�̂��쐬
	BuildPointer();

	/// �^�C�}�̍쐬�C�ݒ�
	UTMMTimer* mtimer = CreateMMTimerFunc();				// �^�C�}���쐬
	mtimer->Resolution(1);									// ����\[ms]
	mtimer->Interval(1);									// �Ăт������o[ms]
	mtimer->Set(CallBackHapticLoop, NULL);					// �R�[���o�b�N����֐�
	mtimer->Create();										// �R�[���o�b�N�J�n
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
			if(i==0) idesc.defaultPosition = Posed(1,0,0,0,5.0,0.0,0.0); //�|�C���^�̏����ʒu
			if(i==1) idesc.defaultPosition = Posed(1,0,0,0,-5.0,0.0,0.0);
			GetINScene()->CreateINPointer(idesc);	// interactpointer�̍쐬
		}
	}
}

void FWLDHapticSample::IdleFunc(){
	/// �V�~�����[�V������i�߂�(interactscene������ꍇ�͂��������Ă�)
	FWAppHaptic::instance->GetINScene()->Step();
	//Balljoint->GetDefomationMode();//naga
	glutPostRedisplay();
}

void FWLDHapticSample::TwoPointerCalib(){
	TMatrixRow<6,3,float> mainMat ;
	TMatrixRow<6,4,float> subMat ;
	TMatrixRow<4,3,float> transMat ;
	Affined TAffine ;

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
	
	DSTR<<"TAffine"<<std::endl<<TAffine<<std::endl;
	for(int i=0;i<6;i++){
		DSTR<<"mainMat"<<mainMat[i]<<std::endl;
		DSTR<<"subMat"<<subMat[i]<<std::endl;
		Vec3d NewPosition = TAffine*subPosition[i];
		DSTR<<"NewPosition"<<NewPosition<<std::endl;
	}

	Posed tPos;
	tPos.FromAffine(TAffine);
	GetINScene()->GetINPointer(0)->SetDefaultPosition(tPos);
}

void FWLDHapticSample::TwoPointerSet(){
	Vec3d mainPos = GetINScene()->GetINPointer(0)->GetPointerSolid()->GetFramePosition();
	Vec3d subPos = GetINScene()->GetINPointer(1)->GetPointerSolid()->GetFramePosition();

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
	FWAppHaptic::Keyboard(key , x, y);
	PHSceneIf* phscene=GetSdk()->GetPHSdk()->GetScene(0);

	switch (key) {
		case 'r':
			{
				Reset();
			}
			break;
		case ' ':
			{
				DSTR<<cameraInfo.view<<std::endl;
			}
			break;

		case 'v':
			{
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

			break;
		default:
			break;
	}
}
