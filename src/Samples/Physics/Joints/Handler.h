#ifndef JOINTS_HANDLER_H
#define JOINTS_HANDLER_H

#include <Springhead.h>
#include <Framework/SprFWApp.h>
#include <Framework/SprFWEditor.h>

using namespace Spr;

/// �V�[���̍\�z�ƃC���^���N�V�����̊�{�N���X
class Handler : public UTRefCount{
public:
	bool		bReady;
	PHSolidDesc sdFloor;					///< �����̂̃f�B�X�N���v�^
	PHSolidDesc sdBox;						///< �����̂̃f�B�X�N���v�^
	CDShapeIf* shapeBox;					///< ���`��
	CDShapeIf* shapeSphere;					///< ���`��

	PHSolidIf* soFloor;						///< �����̂̃C���^�t�F�[�X
	std::vector<PHSolidIf*> soBox;			///< �����̂̃C���^�t�F�[�X
	std::vector<PHJointIf*> jntLink;		///< �֐߂̃C���^�t�F�[�X
	std::vector<PHTreeNodeIf*> nodeTree;	///< ABA�i�֐ߍ��W�V�~�����[�V�����j�̂��߂̃c���[�m�[�h

	PHSdkIf* GetPHSdk(){ return FWApp::instance->GetSdk()->GetPHSdk(); }
	PHSceneIf* GetPHScene(){ return FWApp::instance->GetSdk()->GetScene()->GetPHScene(); }

public:
	Handler(){ bReady = false; }
	/// ���p�̃V�[���Z�b�g�A�b�v����
	void BuildCommon(bool floor = true, bool rootBox = true, bool gravity = true){
		PHSdkIf* phSdk = GetPHSdk();
		PHSceneIf* phScene = GetPHScene();

		// ��
		if(floor){
			CDBoxDesc desc(Vec3d(50, 1, 50));
			soFloor = phScene->CreateSolid(sdFloor);
			soFloor->AddShape(phSdk->CreateShape(desc));
			soFloor->SetFramePosition(Vec3d(0,-2,0));
			soFloor->SetDynamical(false);			// �O�͂ɂ���ē����Ȃ��悤�ɂ���
		}

		//���̍��ɂȂ锠
		if(rootBox){
			CDBoxDesc bd(Vec3d(2.0, 2.0, 2.0));
			shapeBox = phSdk->CreateShape(bd);
			shapeBox->SetName("shapeBox");
			PHSolidIf* s = phScene->CreateSolid(sdBox);
			soBox.push_back(s);
			Posed pose;
			s->AddShape(shapeBox);

			s->AddShape(shapeBox);
			pose.Pos() = Vec3d(2.1, 0.0, 0.0);
			s->SetShapePose(1, pose);

			s->AddShape(shapeBox);
			pose.Pos() = Vec3d(0.0, 2.1, 0.0);
			s->SetShapePose(2, pose);

			s->AddShape(shapeBox);
			pose.Pos() = Vec3d(0.0, 0.0, 2.1);
			s->SetShapePose(3, pose);
			
			//�󒆂ɌŒ肷��
			soBox.back()->SetFramePosition(Vec3d(0.0, 20.0, 0.0));
			soBox.back()->SetDynamical(false);	
			nodeTree.push_back(phScene->CreateRootNode(soBox.back(), PHRootNodeDesc()));
		}

		// �d�͂�ݒ�
		if(gravity){
			phScene->SetGravity(Vec3d(0, -9.8, 0));
		}
	}

	/// �N���A
	virtual void Clear(){
		soFloor = NULL;
		soBox.clear();
		jntLink.clear();
		nodeTree.clear();
		bReady = false;
	}
	virtual void Build(){ bReady = true; }
	virtual double GetSceneRadius(){ return 20.0; }	///< �V�[���̑�̂̑傫���i�J�����������p�j
	virtual void OnTimer(){}
	
	/// ���ʂ̃L�[���͏���
	virtual void OnKey(int key){
		if(key == 'x'){
			static bool bEnable = true;
			bEnable = !bEnable;
			for(int i = 0; i < (int)jntLink.size(); i++)
				jntLink[i]->Enable(bEnable);
		}
		/// ���̂𓊂���:
		/// ��
		else if(key == 'z'){
			soBox.push_back(GetPHScene()->CreateSolid(sdBox));
			soBox.back()->AddShape(shapeBox);
			soBox.back()->SetFramePosition(Vec3d(15.0, 15.0, 0.0));
			soBox.back()->SetOrientation(Quaterniond::Rot(Rad(1), 'z'));
			soBox.back()->SetVelocity(Vec3d(-5.0, 0.0, 0.0));
			soBox.back()->SetMass(2.0);
		}
		else if(key == 'Z'){
			soBox.push_back(GetPHScene()->CreateSolid(sdBox));
			soBox.back()->AddShape(shapeBox);
			soBox.back()->SetFramePosition(Vec3d(5.0, 13.0, 5.0));
			soBox.back()->SetVelocity(Vec3d(0.0, 0.0, -20.0));
			soBox.back()->SetMass(2.0);
		}
		/// �J�v�Z��
		else if(key == 'C'){
			CDCapsuleDesc cd;
			cd.radius = 1.0;
			cd.length = 4.0;
			CDShapeIf* shape = GetPHSdk()->CreateShape(cd);

			soBox.push_back(GetPHScene()->CreateSolid(sdBox));
			soBox.back()->AddShape(shape);
			soBox.back()->SetOrientation(Quaterniond::Rot(Rad(90), 'y'));
			soBox.back()->SetFramePosition(Vec3d(15.0, 15.0, 0.0));
			soBox.back()->SetVelocity(Vec3d(-3.0, 0.0, 0.0));
			soBox.back()->SetAngularVelocity(Vec3d(0.0, 0.0, 2.0));
			soBox.back()->SetMass(2.0);
		}
		/// ��
		else if(key == 'S'){
			CDSphereDesc sd;
			sd.radius = 1.0;
			CDShapeIf* shape = GetPHSdk()->CreateShape(sd);

			soBox.push_back(GetPHScene()->CreateSolid(sdBox));
			soBox.back()->AddShape(shape);
			soBox.back()->SetOrientation(Quaterniond::Rot(Rad(90), 'y'));
			soBox.back()->SetFramePosition(Vec3d(15.0, 15.0, 0.0));
			soBox.back()->SetVelocity(Vec3d(-3.0, 0.0, 0.0));
			soBox.back()->SetMass(2.0);
		}
		// dt�𔼕��ɂ���
		else if(key == '.'){
			double dt = GetPHScene()->GetTimeStep();
			dt /= 2.0;
			dt = std::max(0.001, dt);
			GetPHScene()->SetTimeStep(dt);
		}
		// dt��{�ɂ���
		else if(key == ','){
			double dt = GetPHScene()->GetTimeStep();
			dt *= 2.0;
			GetPHScene()->SetTimeStep(dt);
		}
	}
};

#endif