#include "FWAppSample.h"
/**
\page pageFWAppSample �t���[�����[�NSDK���g������ԊȒP�ȃT���v��
 Springhead2/src/Samples/FWAppSample

- �v���O�����̋@�\
 - ���̏�ɔ����ڂ��Ă��邾���̃V�[���B
 - �`��� \ref pagePhysics �̃f�o�b�O�\���̂݁B
 - i �L�[�ŁA�ڐG�͂�ڐG�����̕\����ON/OFF�B
- �\�[�X�ɂ���
 - �V�[���̍\�z�́AC++�����API���Ăяo���čs���B
 - \ref pageFramework �� \ref pagePhysics �݂̂��g�p
*/

FWAppSample app;
int _cdecl main(int argc, char* argv[]){
	app.Init(argc, argv);
	app.Start();
	return 0;
}
