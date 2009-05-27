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
	double nIter;
	Vec3d gravity;
	double range;
	FWAppImpDesc(){
		sceneName = "";									// �V�[���̖��O
		pdt = 0.05f;
		nIter = 45;
		gravity =  Vec3d(0, -9.8f, 0);
		range = 0.7;
	}
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
	void SetSpidar(HISpidarGIf* s);					// HISpidarGIf�̐ݒ�
	HISpidarGIf* GetSpidar();							// HISpidarGIf�̎擾
	string GetSceneName();							// �V�[���̖��O���擾

	// �V�~�����[�V�����֌W
	virtual void Init();									// �V�[���̍쐬
	virtual void ResetScene();							// �V�[���̃��Z�b�g
	virtual void BuildScene();							// �����V�[���̍\�z 
	virtual void Idle();									// glutIdleFunc�ŌĂ΂��
	virtual void Step();									// �V�~�����[�V������i�߂�
	virtual void UpdateHapticPointer();				// �͊o�|�C���^�̈ʒu�C�p���̍X�V								
	virtual void CallBack();								// MultiMediaTimer�ł̃R�[���o�b�N
	virtual void DebugMode(bool bDebug, GRDebugRenderIf* render);		// DebugMode��true�̎��̏���
	virtual void Keyboard(int key, int x, int y);	// �L�[�{�[�h
};

#endif