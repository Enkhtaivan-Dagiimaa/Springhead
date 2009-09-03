#include "HapticDemo.h"
#include "Samplemodel.h"
#include <iostream>
#include <sstream>
#include <Framework/FWInteractScene.h>

using namespace std;

#define SPIDAR 1;

FWLDHapticSample::FWLDHapticSample(){
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
	BuildScene();
	BuildPointer();

	/// �^�C�}�̍쐬�C�ݒ�
	UTMMTimer* mtimer = CreateMMTimerFunc();				// �^�C�}���쐬
	mtimer->Resolution(1);									// ����\[ms]
	mtimer->Interval(1);									// �Ăт������o[ms]
	mtimer->Set(CallBackHapticLoop, NULL);					// �R�[���o�b�N����֐�
	mtimer->Create();										// �R�[���o�b�N�J�n
}
void FWLDHapticSample::InitCameraView(){
	std::istringstream issView(
		"((0.9996 0.0107463 -0.0261432 -0.389004)"
		"(-6.55577e-010 0.924909 0.380188 5.65711)"
		"(0.0282657 -0.380037 0.92454 13.7569)"
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
			idesc.springK = 5;						// haptic rendering�̃o�l�W��
			idesc.damperD = 0.0;					// haptic rendering�̃_���p�W��
		#if SPIDAR
			idesc.posScale = 300;					// soPointer�̉���̐ݒ�(�`�{)
		#else
			idesc.posScale = 60;					// soPointer�̉���̐ݒ�(�`�{)
		#endif
			idesc.localRange = 1.0;					// LocalDynamics���g���ꍇ�̋ߖT�͈�
			if(i==0) idesc.position = Posed(1,0,0,0,5,0,0); //�|�C���^�̏����ʒu
			if(i==1) idesc.position = Posed(1,0,0,0,5,0,0);
			GetINScene()->CreateINPointer(idesc);	// interactpointer�̍쐬
		}
	}
}

void FWLDHapticSample::BuildScene(){
	PHSceneIf* phscene = GetSdk()->GetScene()->GetPHScene();
	PHSolidDesc desc;
	CDBoxDesc bd;

	/// ��(�����@���ɏ]��Ȃ��C�^�����ω����Ȃ�)
	{
		/// ����(soFloor)�̍쐬
		desc.mass = 1e20f;
		desc.inertia *= 1e30f;
		PHSolidIf* soFloor = phscene->CreateSolid(desc);		// ���̂�desc�Ɋ�Â��č쐬
		soFloor->SetDynamical(false);
		soFloor->SetGravity(false);
		/// �`��(shapeFloor)�̍쐬
		bd.boxsize = Vec3f(50, 10, 50);
		CDShapeIf* shapeFloor = GetSdk()->GetPHSdk()->CreateShape(bd);
		/// ���̂Ɍ`���t������
		soFloor->AddShape(shapeFloor);
		soFloor->GetShape(0)->SetVibration(5,80,100);
		soFloor->SetFramePosition(Vec3d(0, -10, 0));
	}

	{	
		PH3ElementBallJointDesc desc;
		//PHBallJointDesc desc;
		{
			desc.poseSocket.Pos()	= Vec3f(0.0f,0.0f , -1.2f);
			desc.posePlug.Pos()	= Vec3f(0.0f,0.0f , 1.2f);
			desc.spring			= 10;//10.0;
			desc.damper		= 2;//2.0;
			desc.secondDamper = 1;
			desc.hardnessRate = 1;
			desc.yieldStress =0;
			desc.type		=PH3ElementBallJointDesc::deformationType::Plastic;

		}
		PHSolidIf* rootSolid = CreateCapsule(GetSdk());
		rootSolid->SetMass(0.001);
		rootSolid->SetDynamical(false);
		double posy = 15;
		Vec3d pos = Vec3d(0, posy, 0);
		//rootSolid->SetFramePosition(pos);
		//rootSolid->SetOrientation(Quaterniond().Rot(Rad(90),Vec3d(0,1,0)));

		//PHTreeNodeIf* root=GetSdk()->GetScene()->GetPHScene()->CreateRootNode(rootSolid,PHRootNodeDesc());
	
		for(int i = 1; i < 1; i++){
			PHSolidIf* nodeSolid = CreateCapsule(GetSdk());
			nodeSolid->SetMass(0.001);
			PHJointIf* joint=GetSdk()->GetScene()->GetPHScene()->CreateJoint(rootSolid, nodeSolid, desc);
			if(i==6){
			//	Balljoint=DCAST(PH3ElementBallJointIf,joint);//naga
				//nodeSolid->SetDynamical(false);
			}
			nodeSolid->SetFramePosition(Vec3d(0, posy - 2.4 * i, 0));
			//nodeSolid->SetOrientation(Quaterniond().Rot(Rad(90),Vec3d(0,1,0)));
			GetSdk()->GetScene()->GetPHScene()->SetContactMode(rootSolid, nodeSolid, PHSceneDesc::MODE_NONE);
			//root=GetSdk()->GetScene()->GetPHScene()->CreateTreeNode(root,nodeSolid);
			rootSolid = nodeSolid;
		}
	}
}

void FWLDHapticSample::IdleFunc(){
	/// �V�~�����[�V������i�߂�(interactscene������ꍇ�͂��������Ă�)
	FWAppHaptic::instance->GetINScene()->Step();
	//Balljoint->GetDefomationMode();//naga
	glutPostRedisplay();
}						

void FWLDHapticSample::Reset(){

	//MTimerRelease();

	///// Sdk�̏������C�V�[���̍쐬
	//CreateSdk();
	//GetSdk()->Clear();										// SDK�̏�����
	//GetSdk()->CreateScene(PHSceneDesc(), GRSceneDesc());	// Scene�̍쐬
	//GetSdk()->GetScene()->GetPHScene()->SetTimeStep(0.02);	// ���݂̐ݒ�
	///// HumanInterface�̏�����
	//InitHumanInterface();
	///// InteractScene�̍쐬
	//FWInteractSceneDesc desc;
	//desc.fwScene = GetSdk()->GetScene();					// fwScene�ɑ΂���interactscene�����
	//desc.mode = LOCAL_DYNAMICS;								// humaninterface�̃����_�����O���[�h�̐ݒ�
	//desc.hdt = 0.001;										// �}���`���[�g�̏ꍇ�̍X�V[s]
	//CreateINScene(desc);									// interactScene�̍쐬
	///// �����V�~�����[�V�������鍄�̂��쐬
	//BuildScene();
	//BuildPointer();
	////�`��Window�̏�����
	//GetCurrentWin()->SetScene(GetSdk()->GetScene());
	//InitCameraView();

	//MTimerStart();


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
				CreateBox(GetSdk());
				for(int i=0;i<GetINScene()->NINPointers();i++){
					phscene->SetContactMode(GetINScene()->GetINPointer(i)->GetPointerSolid(), PHSceneDesc::MODE_NONE);
				}
				DSTR << "Nobj:" << phscene->NSolids() << endl;
			}
			break;
		case 'v':
			{
				CreateCapsule(GetSdk());
				for(int i=0;i<GetINScene()->NINPointers();i++){
					phscene->SetContactMode(GetINScene()->GetINPointer(i)->GetPointerSolid(), PHSceneDesc::MODE_NONE);
				}
				DSTR << "Nobj:" << phscene->NSolids() << endl;
			}
			break;
		case 'b':
			{
				CreateSphere(GetSdk());
				for(int i=0;i<GetINScene()->NINPointers();i++){
					phscene->SetContactMode(GetINScene()->GetINPointer(i)->GetPointerSolid(), PHSceneDesc::MODE_NONE);
				}
				DSTR << "Nobj:" << phscene->NSolids() << endl;
			}
			break;
		case 'n':
			{
				CreateRoundCone(GetSdk());
				for(int i=0;i<GetINScene()->NINPointers();i++){
					phscene->SetContactMode(GetINScene()->GetINPointer(i)->GetPointerSolid(), PHSceneDesc::MODE_NONE);
				}
				DSTR << "Nobj:" << phscene->NSolids() << endl;
			}
			break;
		case 'm':
			{
				CreatePolyhedron(GetSdk());
				for(int i=0;i<GetINScene()->NINPointers();i++){
					phscene->SetContactMode(GetINScene()->GetINPointer(i)->GetPointerSolid(), PHSceneDesc::MODE_NONE);
				}
				DSTR << "Nobj:" << phscene->NSolids() << endl;
			}
			break;
		case 'x':
			{
				CreateWall(GetSdk());
				for(int i=0;i<GetINScene()->NINPointers();i++){
					phscene->SetContactMode(GetINScene()->GetINPointer(i)->GetPointerSolid(), PHSceneDesc::MODE_NONE);
				}
				DSTR << "Nobj:" << phscene->NSolids() << endl;
			}
			break;
		case 'z':
			{
				CreateTower(GetSdk());
				for(int i=0;i<GetINScene()->NINPointers();i++){
					phscene->SetContactMode(GetINScene()->GetINPointer(i)->GetPointerSolid(), PHSceneDesc::MODE_NONE);
				}
				DSTR << "Nobj:" << phscene->NSolids() << endl;
			}
			break;
		default:
			break;
	}
}
