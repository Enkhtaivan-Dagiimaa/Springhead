/** 
 @file  Springhead2/src/tests/Physics/PHSimple/main.cpp

@brief 2�̍��̂̈ʒu�̕ω����m�F����e�X�g�v���O�����i�ʒu���o�́j

 <PRE>
 <B>�e�X�g���e</B>
  �E�v���O����������I��������0��Ԃ��B
 
 <B>�v���O�����T�v</B>
  �E�V�~�����[�V�����ɕK�v�ȏ��(���ʁE�d�S�E�����e���\���Ȃ�)��ݒ肷��B
  �E2�̍��̂�p�ӂ��A�e���̂ɑ΂��ďd�S��2m����E��10N�̗͂�������B
  �E�^����ꂽ�����ɂ�臙t�b��̈ʒu�̕ω���10�X�e�b�v�ϕ����A���̓s�x�ʒu���o�͂���B
  �E10�X�e�b�v���̌v�Z���I������A�v���O�������I��������B
 </PRE>
    
 */
#include <Springhead.h>		//	Springhead�̃C���^�t�F�[�X
#pragma hdrstop
using namespace Spr;


PHSdkIf* sdk;
PHSceneIf* scene;
PHSolidIf* solid1, *solid2;

/**
 @brief  ���C���֐�
 @param	 <in/--> argc    �R�}���h���C�����͂̌�
 @param  <in/--> argv    �R�}���h���C������
 @return  0 (����I��)
 */
int main(int argc, char* argv[]){
	sdk = CreatePHSdk();				//	SDK�̍쐬
	scene = sdk->CreateScene();			//	�V�[���̍쐬
	PHSolidDesc desc;
	desc.mass = 2.0;
	desc.inertia *= 2.0;
	solid1 = scene->CreateSolid(desc);	//	���̂�desc�Ɋ�Â��č쐬
	desc.center = Vec3f(1,0,0);
	solid2 = scene->CreateSolid(desc);	//	���̂�desc�Ɋ�Â��č쐬
	for(int i=0; i<10; ++i){
		solid1->AddForce(Vec3f(10,0,0), Vec3f(0,2,0)+solid1->GetCenterPosition());
		solid2->AddForce(Vec3f(10,0,0), Vec3f(0,2,0)+solid2->GetCenterPosition());
		scene->Step();
		std::cout << solid1->GetFramePosition();
		std::cout << solid2->GetFramePosition() << std::endl;
		// std::cout << solid1->GetOrientation() << std::endl;
	}
	//	SDK�͊J�����Ȃ��Ă��ǂ��D���Ȃ��Ă�main�𔲂��Ă���J�������D
	delete sdk;

	//	���������[�N�̃e�X�g�D
	//	�f�o�b�O����Ŏ��s����ƃ��������[�N��VC�̃f�o�b�O�o�͂ɕ\�������D
	char* memoryLeak = new char[123];
	
	return 0;
}
