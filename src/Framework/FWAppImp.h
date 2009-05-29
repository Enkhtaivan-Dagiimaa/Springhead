#ifndef FW_APP_IMP_H
#define FW_APP_IMP_H

#include<sstream>
#include<string>
#include<vector>
#include<Springhead.h>

using namespace Spr;
using namespace std;

class FWAppAdapter;

class FWAppImpDesc{
public:
	string sceneName;									// �V�[���̖��O
	double pdt;
	int nIter;
	Vec3d gravity;
	double range;
	FWAppImpDesc();
};

class FWAppImp :public FWAppImpDesc{
protected:
	// �A�_�v�^�C�V�[��
	FWAppAdapter* adapter;
	FWSceneIf* fwscene;
	HISpidarGIf* spidarG6;
public:	
	// Sdk�֌W
	FWAppImp();
	void SetAdapter(FWAppAdapter* a);			// FWAppAdapter�̐ݒ�
	FWSdkIf* GetSdk();									// FWSdkIf�̎擾
	void SetScene(FWSceneIf* s);					// FWSceneIf�̐ݒ�
	FWSceneIf* GetScene();							// FWSceneIf�̎擾
	void SetHapticInterface(HISpidarGIf* s);		// HISpidarGIf�̐ݒ�
	HISpidarGIf* GetHapticInterface();				// HISpidarGIf�̎擾
	string GetSceneName();							// �V�[���̖��O���擾

	//�f�o�b�N�\���Ɏg���ϐ�
	bool bDebug;
	bool bStep;
	bool bOneStep;

	// �V�~�����[�V�����֌W
	virtual void Init();									// �V�[���̍쐬
	virtual void ResetScene();							// �V�[���̃��Z�b�g
	virtual void BuildScene();							// �����V�[���̍\�z
	virtual void Idle();									// glutIdleFunc�ŌĂ΂��
	virtual void Step();									// �V�~�����[�V������i�߂�
	virtual void CallBack();								// MultiMediaTimer�ł̃R�[���o�b�N
	virtual void DebugMode(bool bDebug, GRDebugRenderIf* render);		// DebugMode��true�̎��̏���
	virtual void Keyboard(int key, int x, int y);	// �L�[�{�[�h
};

#endif