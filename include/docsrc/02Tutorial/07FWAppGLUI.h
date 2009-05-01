/** \page pageFWAppGLUI Framework���g����GLUI�A�v���P�[�V����

\contents

<div>
	<p>
		FWAppGLUI��FWAppGLUT�̋@�\���p�����CGLUI��GUI�@�\�������ꂽ�N���X�ł��D
		�ʓ|�ȏ�����FWAppGLUT�Ɠ��lSpringhead��Framework���S�ď������Ă���邽�߁C
		�����ōs�������ݒ�����Ȃ����邱�Ƃ��\�ł��D
		�Ȃ��C���̃N���X�̓I�u�W�F�N�g���������鎖��������Ă���܂���D
		���̂��߁C�����̐݌v�̓V���O���g���p�^�[���ƂȂ��Ă��܂��D
		FWAppGLUI��p���ăA�v���P�[�V�������쐬����ɂ́C
		�����̍�肽���A�v���P�[�V�����N���X��FWAppGLUI���p��������K�v������܂��D
		�ȉ��̗�ł͎����̃A�v���P�[�V�����N���X��class MYGUI�ŕ\�����ƂƂ��܂��D
	</p>
	<h2>�g����</h2>
	<p>
in MYGUI.h
\verbatim

#include <Framework/SprFWAppGLUI.h>

class MYGUI : public FWAppGLUI{

public:
	GLUI* glui;
	// GUI�Ɋւ���֐���K���I�[�o�[���C�h����
	void DesignGUI();
	
	// �K�v�ɉ�����FWAppGLUT�̉��z�֐����I�[�o�[���C�h����D
	void Init(int argc, char* argv[]); //< Init�֐��͕K���I�[�o�[���C�h����
	void ....

public:
	// �����ō���������o�֐���ǉ�
private:
	// �����ō���������o�֐��⃁���o�ϐ���ǉ�
	static void __cdecl CallHoge(int control);
	void RealHoge(int control);
};
\endverbatim
	</p>
	<p>
in MYGUI.cpp
\verbatim

#include "MYGUI.h"

void MYGUI::DesignGUI(){
	glui = CreateGUI();
	new GLUI_Hoge(glui, ....., ..., CallHoge);
}

static void MYGUI::CallHoge(int control){
	((*MYGUI)instance)->RealHoge(control);
}
void MYGUI::RealHoge(int control){
	...;
}

void MYGUI::Init(int argc, char* argv[]){
	FWAppGLUT::Init(argc, argv);
	DesignGUI();
}
//:::::::::::::::::::::::::::::::::::::::::
	// FWAppGLUT�Ɠ���
//:::::::::::::::::::::::::::::::::::::::::
\endverbatim
	</p>

	<p>
in main.cpp
\verbatim

#include <Springhead.h>
#include "MYGUI.h"

int main(int argc, char* argv[]){
	MYGUI gui;
	gui.Init(argc, argv);
	gui.Start();

	return 0;
}
\endverbatim
</div>

*/