#include "FWVrmlFileLoader.h"
/**
	�V���v����Xfile���[�_�[
	�V�~�����[�V������glutIdleFunc�Ői�߂�
	q, ESC	: �A�v���P�[�V�����I��
	r		: �t�@�C���̃����[�h
	w		: �J�����s��̏�����
	d		: �f�o�b�N�\���؂�ւ�
*/

FWVrmlfileLoader app;
int _cdecl main(int argc, char* argv[]){
	app.Init(argc, argv);
	app.StartMainLoop();
	return 0;
}
