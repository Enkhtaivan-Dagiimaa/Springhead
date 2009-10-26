#ifndef FWLDHAPTIC_SAMPLE_H
#define FWLDHAPTIC_SAMPLE_H

#include <Springhead.h>
#include <Framework/SprFWApp.h>
#include <vector>

using namespace Spr;

class FWLDHapticSample : public FWApp{
public:
	FWLDHapticSample();
	virtual void Init(int argc, char* argv[]);			///< �A�v���P�[�V�����̏�����
	virtual void InitCameraView();						///< �J�����s��̏�����
	virtual void InitHumanInterface();					///< HapticInterface�̏�����
	virtual void Reset(){};
	virtual void IdleFunc();							///< Physic�̃R�[���o�b�N
	virtual void TimerFunc(int id);						///< hapticloop�̃R�[���o�b�N
	virtual void Display();								///< �`��֐�	
	virtual void BuildScene(){};							///< ���̂̍쐬
	virtual void Keyboard(int key, int x, int y);		///< �L�[�{�[�h�֐�
	virtual void BuildPointer();						///< ���̂̍쐬	

	void TwoPointerCalib();								///< 2�_�̃|�C���^�̍��W�n����v������
	void TwoPointerSet();								///< 2�_�̃|�C���^�̍��W�n����v������
	std::vector<Vec3d> mainPosition;
	std::vector<Vec3d> subPosition;
	Posed tPos;

	/// �I�v�V����
	bool bStep;									///< �V�~�����[�V������i�߂邩�ǂ���
	bool bOneStep;								///< �X�e�b�v���s
	bool bDrawInfo;								///< �f�o�b�N�\�����邩�ǂ���									
	void DisplayContactPlane();					///< �ڐG�ʂ̕`��
	void DisplayLineToNearestPoint();			///< �|�C���^�̋ߖT�_���獄�̂̋ߖT�_�ւ̐�
	PH3ElementBallJointIf* Balljoint;
}; 

#endif