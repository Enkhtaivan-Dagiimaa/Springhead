#ifndef MYAPP_H
#define MYAPP_H

#include <Springhead.h>
#include <Framework/SprFWApp.h>

using namespace Spr;

class MyApp : public FWApp{
public:
	PHSceneIf* phscene;			// PHScene�ւ̃|�C���^
	PHHapticPointerIf* pointer; // �͊o�|�C���^�ւ̃|�C���^
	int physicsTimerID;			// �����X���b�h�̃^�C�}
	int hapticTimerID;			// �͊o�X���b�h�̃^�C�}
	UTRef<HIBaseIf> spg;		// �͊o�C���^�t�F�[�X�ւ̃|�C���^

	void InitInterface();						// �͊o�C���^�t�F�[�X�̏�����
	void Init(int argc = 0, char* argv[] = 0);	// �A�v���P�[�V�����̏�����
	void Keyboard(int key, int x, int y);		// �L�[�{�[�h�֐�
	virtual void TimerFunc(int id);				// �e�^�C�}���ĂԃR�[���o�b�N�֐�
};


#endif