/** 
 @file  Springhead2/src/tests/Graphics/GRSimple/main.cpp

@brief �`��̃e�X�g

 <PRE>
 <B>�T�v�F</B>
  �E���̂̉^�����m�F����B
  �E�ʒu���o�͂��A�����_�����O�͍s��Ȃ��B
  
 <B>�I����F</B>
  �E�v���O����������I��������0��Ԃ��B
 
 <B>�����̗���F</B>
  �E�V�~�����[�V�����ɕK�v�ȏ��(���ʁE�d�S�E�����e���\���Ȃ�)��ݒ肷��B
  �E2�̍��̂�p�ӂ��A�e���̂ɑ΂��ďd�S��2m����E��10N�̗͂�������B
  �E�^����ꂽ�����ɂ�臙t�b��̈ʒu�̕ω���10�X�e�b�v�ϕ����A���̓s�x�ʒu���o�͂���B
  �E10�X�e�b�v���̌v�Z���I������A�v���O�������I��������B
 </PRE>
    
 */
#include <Springhead.h>		//	Springhead�̃C���^�t�F�[�X
#pragma hdrstop
using namespace Spr;


/**
 @brief		���C���֐�
 @param		<in/--> argc�@�@�R�}���h���C�����͂̌�
 @param		<in/--> argv�@�@�R�}���h���C������
 @return	0 (����I��)
 */
int main(int argc, char* argv[]){
	GRSdkIf* graphics = CreateGRSdk();
	GRDebugRenderIf* debugRend = graphics->CreateDebugRender();
	GRDeviceGLIf* device = graphics->CreateDeviceGL();
	debugRend->SetDevice(device);
	return 0;
}
