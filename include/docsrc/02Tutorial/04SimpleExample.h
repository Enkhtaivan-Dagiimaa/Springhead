/** \page pageSimpleExample		�ȒP�ȃv���O����

<br>

�����ł́C�ȒP�ȃv���O�����Ƃ��āC<a class="el" href="../../src/html/PHSimple_2main_8cpp-example.html">PHSimple</a>
�ɉ����Đ������܂��D<br>
2�̃e�B�[�|�b�g�ɂ�����̗͂������C10�X�e�b�v��̈ʒu���o�͂���T���v���v���O�����ł��D<br>
<a class="el" href="../../src/html/PHSimple_2main_8cpp-example.html">PHSimple</a>��OpenGL�Ń����_�����O����
�T���v���v���O�����́C<a class="el" href="../../src/html/PHSimpleGL_2main_8cpp-example.html">PHSimpleGL</a>�ɂ���܂��D<br>
<br>

�܂��̓C���N���[�h�t�@�C�����`���܂��D
\verbatim
 #include <Springhead.h>		//	Springhead�̃C���^�t�F�[�X
\endverbatim
<br>

Springhead2��namespace�����p�錾���Ă����܂��D
\verbatim
 using namespace Spr;
\endverbatim
<br>

�V�~�����[�V�����ɕK�v�ȕ����G���W��SDK�̃C���X�^���X���쐬�E�擾���C
���̂ɑ΂��鎿�ʁE�d�S�E�����e���\���Ȃǂ̕K�v�ȏ���ݒ肵�܂��D
����Ɋ֌W������e��\ref pageApiBasic�ɂ��L�ڂ���Ă��܂��D
\verbatim
 int main(int argc, char* argv[]){
    sdk = CreatePHSdk();                //  �����G���W��SDK�̍쐬
    scene = sdk->CreateScene();         //  �V�[���̍쐬
    PHSolidDesc desc;                   //  ���̂̃f�B�X�N���v�^
    desc.mass = 2.0;                    //  ���ʂ�2�ɐݒ�
    desc.inertia *= 2.0;                //  �����e���\���̑̊i������2�{����iinertia�̏����l�͒P�ʍs��j
    solid1 = scene->CreateSolid(desc);  //  ���̂P��desc�Ɋ�Â��č쐬
    desc.center = Vec3f(1,0,0);         //  ���̂̏d�S��(1,0,0)�ɐݒ�
    solid2 = scene->CreateSolid(desc);  //  ���̂Q��desc�Ɋ�Â��č쐬 	
	    :
\endverbatim 	
 	
�����ݒ���I������C�V�~�����[�V�������s���܂��D
���[�v�����ŁCStep()�֐����ĂсC�V�~�����[�V�����X�e�b�v�����X�V���Ă����܂��D
�����ł́C���ꂼ��̍��̂̏d�S����2m����E������10N�̗͂������CStep()�֐����Ă�ł��܂��D
�܂��C�X�e�b�v��i�߂邲�ƂɁC���̂̈ʒu�����o�͂��Ă��܂��D
\verbatim
    for(int i=0; i<10; ++i){
        solid1->AddForce(Vec3f(10,0,0), Vec3f(0,2,0)+solid1->GetCenterPosition());
        solid2->AddForce(Vec3f(10,0,0), Vec3f(0,2,0)+solid2->GetCenterPosition());
        scene->Step();
        std::cout << solid1->GetFramePosition();
        std::cout << solid2->GetFramePosition() << std::endl;
	}
\endverbatim 

�V�~�����[�V�����I�����ɂ́C�����G���W��SDK�̊J�����s���܂��D
�������C�����G���W��SDK�͊J�����Ȃ��Ă��C�A�v���P�[�V�����I�����ɊJ�������悤�ɂȂ��Ă��܂��D
\verbatim
    delete sdk;
\endverbatim  	
 	
�ȏ�ɂ��C���̂ɂ�����̗͂��������V�~�����[�V�������s���܂��D
�Ȃ��C�����ł͍Ō�ɁCDBG_NEW��char�^�̃������z���p�ӂ��CVC�ł̃��������[�N���o�e�X�g���s���Ă��܂��D
\verbatim 	
    char* memoryLeak = DBG_NEW char[123];
 	
 	return 0;
 }
\endverbatim 
<br><br><br>

*/
