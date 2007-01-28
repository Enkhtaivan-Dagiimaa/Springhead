#ifndef SPR_FWAPP_H
#define SPR_FWAPP_H

#include <Framework/SprFWSdk.h>
#include <Framework/SprFWScene.h>

namespace Spr{;

class FWVFuncBridge : public UTRefCount{
public:
	virtual void Link(void* pObj) = 0;
	virtual bool Display() = 0;
	virtual bool Reshape(int w, int h) = 0;
	virtual bool Keyboard(unsigned char key, int x, int y) = 0;
	virtual bool MouseButton(int button, int state, int x, int y) = 0;
	virtual bool MouseMove(int x, int y) = 0;
	virtual bool Step() = 0;
	virtual void AtExit() = 0;
};

class FWApp{
protected:
	UTRef<FWSdkIf> fwSdk;
	UTRef<FWSceneIf> fwScene;
	UTRef<GRDebugRenderIf> grRender;
	UTRef<GRDeviceIf> grDevice;
	bool isRunning;

	void CallDisplay(){
		if(!vfBridge || !vfBridge->Display())
			Display();
	}
	void CallReshape(int w, int h){
		if(!vfBridge || !vfBridge->Reshape(w, h))
			Reshape(w, h);
	}
	void CallKeyboard(unsigned char key, int x, int y){
		if(!vfBridge || !vfBridge->Keyboard(key, x, y))
			Keyboard(key, x, y);
	}
	void CallMouseButton(int button, int state, int x, int y){
		if(!vfBridge || !vfBridge->MouseButton(button, state, x, y))
			MouseButton(button, state, x, y);
	}
	void CallMouseMove(int x, int y){
		if(!vfBridge || !vfBridge->MouseMove(x, y))
			MouseMove(x, y);
	}
	void CallStep(){
		if(!vfBridge || !vfBridge->Step())
			Step();
	}
public:

	UTRef<FWVFuncBridge>	vfBridge;

	enum DebugMode{
		DM_NONE,
		DM_DEBUG,
	} debugMode;
	FWApp();

	/// �����_�����擾
	GRRenderIf* GetRender(){return grRender;}
	/// �V�[�����擾
	FWSceneIf* GetScene(){return fwScene;}

	virtual ~FWApp();

	/** @brief ������
		FWApp�I�u�W�F�N�g�̏��������s���D�ŏ��ɕK���ĂԁD
	 */
	virtual void Init(int argc, char* argv[]);

	/** @brief �R�}���h���C�������̏���
		�A�v���P�[�V�����ɓn���ꂽ�R�}���h���C�������������������ꍇ�ɃI�[�o���C�h����
	 */
	virtual void ProcessArguments(int argc, char* argv[]);

	/** @brief �V�[�����t�@�C�����烍�[�h����
		@param filename �t�@�C����
		�w�肳�ꂽ�t�@�C������V�[�������[�h����
	 */
	virtual void LoadScene(UTString filename);

	/** @brief �V�[����`�悷��
		�V�[�����\�������Ƃ��ɌĂ΂��D
		�`�揈�����J�X�^�}�C�Y�������ꍇ�ɃI�[�o���C�h����D
	 */
	virtual void Display();

	/** @brief �`��̈�ύX���̏���
		@param w �`��̈�̉���
		@param h �`��̈�̏c��
		���[�U�ɂ���ăE�B���h�E�T�C�Y���ύX���ꂽ�Ƃ��ȂǂɌĂ΂��D
	 */
	virtual void Reshape(int w, int h);

	/** @brief �L�[�{�[�h�C�x���g�̃n���h��

	 */
	virtual void Keyboard(unsigned char key, int x, int y);

	/** @brief �}�E�X�C�x���g�̃n���h��

	 */
	virtual void MouseButton(int button, int state, int x, int y);

	/** @brief �}�E�X�C�x���g�̃n���h��

	 */
	virtual void MouseMove(int x, int y);

	/** @brief �V�~�����[�V�����̎��s

	 */
	virtual void Step();

	/** @brief �f�o�b�O���[�h�̎擾

	 */
	DebugMode GetDebugMode(){ return debugMode; }

	/** @brief �f�o�b�O���[�h�̐ݒ�

	 */
	void SetDebugMode(DebugMode m){ debugMode = m; }

	FWSceneIf* GetFWScene(){ return fwScene; }
};

}
#endif
