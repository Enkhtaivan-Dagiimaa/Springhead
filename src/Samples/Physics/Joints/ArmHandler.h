#include "Handler.h"

class ArmHandler : public Handler{
public:
	enum{
		ID_MOVE_BALL_0,
		ID_MOVE_BALL_1,
		ID_MOVE_BALL_2,
		ID_MOVE_BALL_3,
		ID_MOVE_BALL_4,
		ID_MOVE_BALL_5,
		ID_MOVE_BALL_6,
	};

	CDShapeIf*	shapeBox;
	CDShapeIf*	shapeSphere;

	vector<PHSolidIf*>			links;
	vector<PHHingeJointIf*>		joints;
	PHSolidIf*					ball;
	PHSpringIf*					spring;

	int							nLinks;			///< �����N�̐�
	double						linkSize;		///< �����N�̒���
	double						jointSpring;	///< �֐߂̃o�l�W��
	double						jointDamper;	///< �֐߂̃_���p�W��
	double						handSpring;		///< ���̃o�l�W��
	double						handDamper;		///< ���̃_���p�W��
	bool						useExternal;	///< �O���R���g���[���Ŏ�搧����s�����ǂ���
	bool						useABA;			///< ABA���g�����ǂ���

public:
	ArmHandler(SampleApp* a):Handler(a){
		nLinks		= 5;
		linkSize	= 5.0;
		// jointSpring = 2000;
		// jointDamper = 100;
		// handSpring	= 2000;
		// handDamper  = 100;

		jointSpring = 2000;
		jointDamper =  100;
		handSpring	= 2000;
		handDamper  =  300;

		useExternal	= false;
		useABA		= false;

		app->AddAction(MENU_ARM, ID_MOVE_BALL_0, "move ball");
		app->AddHotKey(MENU_ARM, ID_MOVE_BALL_0, 's');
		app->AddAction(MENU_ARM, ID_MOVE_BALL_1, "move ball");
		app->AddHotKey(MENU_ARM, ID_MOVE_BALL_1, 'd');
		app->AddAction(MENU_ARM, ID_MOVE_BALL_2, "move ball");
		app->AddHotKey(MENU_ARM, ID_MOVE_BALL_2, 'f');
		app->AddAction(MENU_ARM, ID_MOVE_BALL_3, "move ball");
		app->AddHotKey(MENU_ARM, ID_MOVE_BALL_3, 'g');
		app->AddAction(MENU_ARM, ID_MOVE_BALL_4, "move ball");
		app->AddHotKey(MENU_ARM, ID_MOVE_BALL_4, 'h');
		app->AddAction(MENU_ARM, ID_MOVE_BALL_5, "move ball");
		app->AddHotKey(MENU_ARM, ID_MOVE_BALL_5, 'j');
	}

	virtual void BuildScene(){
		soFloor = app->CreateFloor();
		soFloor->SetFramePosition(Vec3d(0,-30,0));
		
		PHSdkIf* phSdk = GetPHSdk();
		PHSceneIf* phScene = GetPHScene();
		
		// �`��쐬
		CDBoxDesc bd;
		bd.boxsize = Vec3d(2.0, 6.0, 2.0);
		shapeBox = phSdk->CreateShape(bd);

		CDSphereDesc descSphere;
		descSphere.radius = 1.0;
		shapeSphere = phSdk->CreateShape(descSphere);
		
		// ���{�b�g�A�[�����\�����鍄��
		links.resize(nLinks);
		for(int i = 0; i < nLinks; i++){
			links[i] = phScene->CreateSolid();
			links[i]->AddShape(shapeBox);
		}
		
		// �����N���֐߂łȂ�
		joints.resize(nLinks);
		PHHingeJointDesc descHinge;
		descHinge.poseSocket.Pos()	= Vec3d(0.0,  0.5 * linkSize, 0.0);
		descHinge.posePlug.Pos()	= Vec3d(0.0, -0.5 * linkSize, 0.0);
		joints[0] = DCAST(PHHingeJointIf, phScene->CreateJoint(soFloor, links[0], descHinge));

		for(int i = 1; i < nLinks; i++){
			joints[i] = DCAST(PHHingeJointIf, phScene->CreateJoint(links[i-1], links[i], descHinge));
			joints[i]->SetSpring(jointSpring);
			joints[i]->SetDamper(jointDamper);
		}
		if(useABA){
			PHTreeNodeIf* node = phScene->CreateRootNode(soFloor, PHRootNodeDesc());
			for(int i = 0; i < nLinks; i++)
				node = phScene->CreateTreeNode(node, links[0], PHTreeNodeDesc());
		}

		// �{�[���i�ڕW����)
		ball = phScene->CreateSolid();
		ball->AddShape(shapeSphere);
		ball->SetFramePosition(Vec3d(10.0, 5.0, 1.0));
		ball->SetDynamical(false);		
	
		// ���ƃ{�[�����Ȃ��o�l
		if(!useExternal){
			PHSpringDesc descSpring;
			descSpring.spring = handSpring * Vec3d(1,1,1);
			descSpring.damper = handDamper * Vec3d(1,1,1);
			spring = DCAST(PHSpringIf, phScene->CreateJoint(links.back(), ball, descSpring));
		}
		links.front()->SetDynamical(false);

		phScene->SetContactMode(PHSceneDesc::MODE_NONE);		// �ڐG��؂�
	}

	virtual void OnAction(int id){
		if(id == ID_MOVE_BALL_0)
			ball->SetFramePosition(Vec3d(-20.0, 30.0, 0.0));
		if(id == ID_MOVE_BALL_1)
			ball->SetFramePosition(Vec3d(-10.0, 20.0, 0.0));
		if(id == ID_MOVE_BALL_2)
			ball->SetFramePosition(Vec3d( -5.0, 10.0, 0.0));
		if(id == ID_MOVE_BALL_3)
			ball->SetFramePosition(Vec3d(  0.0, 10.0, 0.0));
		if(id == ID_MOVE_BALL_4)
			ball->SetFramePosition(Vec3d(  5.0, 10.0, 0.0));
		if(id == ID_MOVE_BALL_5)
			ball->SetFramePosition(Vec3d( 10.0, 20.0, 0.0));
		if(id == ID_MOVE_BALL_6)
			ball->SetFramePosition(Vec3d( 20.0, 30.0, 0.0));
	}

	virtual void OnTimer(){
		// ���R���g���[���iPD����j
		if(useExternal){
			// ���Α��x
			Vec3d dVel = Vec3d() - links.back()->GetVelocity();
			// �ʒu�΍�
			Vec3d dPos = ball->GetPose().Pos() - links.back()->GetPose().Pos();
			Vec3d force = handSpring*dPos + handDamper*dVel;
			links.back()->AddForce(force, soBox[4]->GetPose()*Vec3d(0,3,0));
		}
	}
};
