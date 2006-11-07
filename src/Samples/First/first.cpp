// first.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//
#include "stdafx.h"
#include <springhead.h>
using namespace Spr;

int _tmain(int argc, _TCHAR* argv[])
{
	UTRef<PHSdkIf> sdk = PHSdkIf::CreateSdk();			//	�����G���W��SDK�̍쐬
	PHSceneIf* scene = sdk->CreateScene();				//	�V�[���̍쐬
	PHSolidIf* solid = scene->CreateSolid();			//	���̂̍쐬
	for(int i=0; i<10; ++i){
		//	�V�~�����[�V������i�߂�
		scene->Step();
		//	���̂̈ʒu���o�́F���R�����̗l�q�����l�ŏo�͂����
		std::cout << solid->GetPose().Pos() << std::endl;
	}
	return 0;
}
