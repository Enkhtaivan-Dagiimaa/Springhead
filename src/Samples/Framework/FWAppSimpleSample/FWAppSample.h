#ifndef FWAPPSAMPLE_H								// �w�b�_�t�@�C������鎞�̂��܂��Ȃ�
#define FWAPPSAMPLE_H								// ����

#include <Springhead.h>								// Springhead��If�^���g�����߂̃w�b�_�t�@�C��
#include <Framework/SprFWAppSimple.h>					// Framework�N���X���錾����Ă���w�b�_�t�@�C��

using namespace Spr;								// Springhead�̖��O���
class FWAppSample;
class FWAppSample : public FWAppSimple{
public:	
	virtual void BuildObject();								// ���̂��쐬
	virtual void Keyboard(int key, int x, int y);		// �L�[�{�[�h�֐�
}; 

#endif