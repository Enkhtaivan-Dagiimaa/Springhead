#ifndef JOINTS_HANDLER_H
#define JOINTS_HANDLER_H

#include "../../SampleApp.h"

/// �V�[���̍\�z�ƃC���^���N�V�����̊�{�N���X
class Handler : public UTRefCount{
public:
	enum {
		MENU_CHAIN = SampleApp::MENU_SCENE,
		MENU_LINK,
		MENU_ARM,
	};
	
	PHSolidDesc		sdFloor;					///< �����̂̃f�B�X�N���v�^
	PHSolidDesc		sdBox;						///< �����̂̃f�B�X�N���v�^

	PHSolidIf*					soFloor;		///< �����̂̃C���^�t�F�[�X
	std::vector<PHSolidIf*>		soBox;			///< �����̂̃C���^�t�F�[�X
	std::vector<PHJointIf*>		jntLink;		///< �֐߂̃C���^�t�F�[�X
	std::vector<PHTreeNodeIf*>	nodeTree;		///< ABA�i�֐ߍ��W�V�~�����[�V�����j�̂��߂̃c���[�m�[�h

	SampleApp*		app;

public:
	Handler(SampleApp* a){ app = a; }

	FWSdkIf*	GetFWSdk(){ return app->GetSdk(); }
	PHSdkIf*	GetPHSdk(){ return GetFWSdk()->GetPHSdk(); }
	FWSceneIf*	GetFWScene(){ return GetFWSdk()->GetScene(app->curScene); }
	PHSceneIf*	GetPHScene(){ return GetFWScene()->GetPHScene(); }

	virtual void BuildScene(){}
	virtual void OnAction(int id){}
	virtual void OnStep(){}
	virtual void OnDraw(GRRenderIf* render){}
	
};

#endif