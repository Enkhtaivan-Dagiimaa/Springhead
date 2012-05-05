#ifndef FWHAPTIC_SAMPLE_H
#define FWHAPTIC_SAMPLE_H

#include <Springhead.h>
#include <Framework/SprFWApp.h>

using namespace Spr;

class FWHapticSample : public FWApp{
public:
	enum HumanInterface{
		SPIDAR,
		XBOX,
		FALCON,
	} humanInterface;
	enum EngineType{
		SINGLE = 0,
		MULTI,
		LD,
	} engineType;
	PHSceneIf* phscene;			// PHScene�ւ̃|�C���^
	PHHapticPointerIf* pointer; // �͊o�|�C���^�ւ̃|�C���^
	float pdt;
	float hdt;
	int physicsTimerID;			// �����X���b�h�̃^�C�}
	int hapticTimerID;			// �͊o�X���b�h�̃^�C�}
	UTRef<HIBaseIf> spg;		// �͊o�C���^�t�F�[�X�ւ̃|�C���^

	FWHapticSample();
	void InitInterface();						// �͊o�C���^�t�F�[�X�̏�����
	void Init(int argc = 0, char* argv[] = 0);	// �A�v���P�[�V�����̏�����
	virtual void BuildScene();							// �I�u�W�F�N�g�̍쐬
	virtual void Keyboard(int key, int x, int y);		// �L�[�{�[�h�֐�
	void TimerFunc(int id);				// �e�^�C�}���ĂԃR�[���o�b�N�֐�
};


#endif