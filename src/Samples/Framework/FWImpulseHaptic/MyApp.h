//#ifndef MY_APP_H
//#define MY_APP_H
//
//#include <Springhead.h>
//#include <Framework/SprFWApp.h>
//
//using namespace Spr;
//
//class MyApp : public FWApp{
//public:
//	MyApp();									
//	virtual void Init(int argc, char* argv[]);			///< �A�v���P�[�V�����̏�����
//	virtual void InitCameraView();						///< �J�����s��̏�����
//	virtual void InitHumanInterface();					///< HapticInterface�̏�����
//	virtual void Reset();
//	virtual void IdleFunc();							///< Physic�̃R�[���o�b�N
//	virtual void TimerFunc(int id);						///< hapticloop�̃R�[���o�b�N
//	virtual void Display();								///< �`��֐�	
//	virtual void BuildScene();							///< ���̂̍쐬
//	virtual void Keyboard(int key, int x, int y);		///< �L�[�{�[�h�֐�
//
//
//	/// �I�v�V����
//	bool bStep;									///< �V�~�����[�V������i�߂邩�ǂ���
//	bool bOneStep;								///< �X�e�b�v���s
//	bool bDrawInfo;								///< �f�o�b�N�\�����邩�ǂ���									
//	void DisplayContactPlane();					///< �ڐG�ʂ̕`��
//	void DisplayLineToNearestPoint();			///< �|�C���^�̋ߖT�_���獄�̂̋ߖT�_�ւ̐�
//	void DisplayCutRing();
//	void DisplayContactPoints();
//	void SaveData();
//}; 
//
//#endif