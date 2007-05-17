/** \page pageNewProject	�V�K�v���W�F�N�g

\contents

�����ł́CVisual C++ .NET 2003 ��p���āC�V����Springhead2��p�����v���O�������쐬������@��������܂��D
�����ɂ��������č쐬����� \ref pageSampleNewProject ���ł��܂��D

\section secCreateNewProj �V�K�v���W�F�N�g�̍쐬
VisualC++�𗧂��グ�āC�t�@�C��(F)-�V�K�쐬(N)-�v���W�F�N�g(P)��I�����āC
���̂悤�ȁu�V�����v���W�F�N�g�v�_�C�A���O�𗧂��グ�C�uWin32�R���\�[���v���W�F�N�g�v
��I�����Ă��������D
\image html newProj.png
�v���W�F�N�g����쐬�ꏊ���w�肵�āCOK�Ńv���W�F�N�g���쐬���Ă��������D
��̐}�ł́Cfirst.vcproj �� first.cpp ���쐬����܂��D

\section secAddHeaders �C���N���[�h�f�B���N�g���̒ǉ�
�쐬�����v���W�F�N�g���E�N���b�N���C�v���p�e�B(R)�Ńv���p�e�B��\�����C
\image html addIncludeFolder.png 
�̂悤�ɁC�u���ׂĂ̍\���v�ɂ��āuC/C++ �S�ʁv�́u�ǉ��̃C���N���[�h�f�B���N�g���v�ɁC
�u$(SPRINGHEAD2)/include�v��ǉ����Ă��������D
����ŁCSpringhead�̃w�b�_�t�@�C�����C���N���[�h�ł���悤�ɂȂ�܂��D

\section secSetOption �I�v�V�����̐ݒ�
�쐬�����v���W�F�N�g���E�N���b�N���C�v���p�e�B(R)�Ńv���p�e�B��\�����C
C C++-�R�[�h������I�сA�����^�C�����C�u������MT(�����[�X��)�܂���MTd(�f�o�b�O��)�ɐݒ肷��

\section secAddLibs ���C�u�����̒ǉ�
Springhead���g�����߂ɂ́C���C�u������ǉ�����K�v������܂��D
�V����������v���W�F�N�g(���}��first)���E�N���b�N���� �ǉ�(D)-�������ڂ̒ǉ�(G)��I�сC
\image html addLib.png
\ref pageGettingStarted �ō쐬���� lib/win32/SpringheadD.lib ��
lib/win32/Springhead.lib ��ǉ����܂��D
�����́C���ꂼ��f�o�b�O�ŁC�����[�X�ł̃��C�u�����Ȃ̂ŁC���ꂼ����E�N���b�N����
�v���p�e�B(R)�Ńv���p�e�B��\�����C���}�̂悤��Debug�łł�Springhead.lib���r���h����
���O���CRelease�łł͋t��SpringheadD.lib���r���h���珜�O���Ă��������D
\image html nonBuildSetting.png

\section secEditSrc �\�[�X�t�@�C���̕ҏW
���Ƃ́C�\�[�X�t�@�C����ҏW���Ď����̃v���O���������ΏI���ł��D
�������P���ȗ���o���ƁF
<pre>
// first.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//
##include "stdafx.h"
##include <springhead.h>
using namespace Spr;

int _tmain(int argc, _TCHAR* argv[])
{
	PHSdkIf* sdk = PHSdkIf::CreateSdk();		//	�����G���W��SDK�̍쐬
	PHSceneIf* scene = sdk->CreateScene();		//	�V�[���̍쐬
	PHSolidIf* solid = scene->CreateSolid();	//	���̂̍쐬
	for(int i=0; i<10; ++i){
		//	�V�~�����[�V������i�߂�
		scene->Step();
		//	���̂̈ʒu���o�́F���R�����̗l�q�����l�ŏo�͂����
		std::cout << solid->GetPose().Pos() << std::endl;
	}
	return 0;
}
</pre>
�̂悤�ɂȂ�܂��D
*/
