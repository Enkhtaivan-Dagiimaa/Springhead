/** \page pageSimpleExample		�ȒP�ȃv���O����

<br>

�����ł́A�ȒP�ȃv���O�����Ƃ��āA<a href="../../src/html/PHSimple_2main_8cpp-example.html">PHSimple</a>
�ɉ����Đ������܂��B<br>
2�̃e�B�[�|�b�g�ɂ�����̗͂������A10�X�e�b�v��̈ʒu���o�͂���T���v���v���O�����ł��B<br>
<a href="../../src/html/PHSimple_2main_8cpp-example.html">PHSimple</a>��OpenGL�Ń����_�����O����
�T���v���v���O�����́A<a href="../../src/html/PHSimpleGL_2main_8cpp-example.html">PHSimpleGL</a>�ɂ���܂��B<br>
<br>

�܂��̓C���N���[�h�t�@�C�����`���܂��B
\verbatim
 #include <Springhead.h>		//	Springhead�̃C���^�t�F�[�X
\endverbatim
<br>

Springhead2��namespace�����p�錾���Ă����܂��B
\verbatim
 using namespace Spr;
\endverbatim
<br>

�V�~�����[�V�����ɕK�v�ȕ����G���W��SDK�̃C���X�^���X���쐬�E�擾���A
���̂ɑ΂��鎿�ʁE�d�S�E�����e���\���Ȃǂ̕K�v�ȏ���ݒ肵�܂��B
����Ɋ֌W������e��\ref pageApiBasic�ɂ��L�ڂ���Ă��܂��B
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
 	
�����ݒ���I������A�V�~�����[�V�������s���܂��B
���[�v�����ŁAStep()�֐����ĂсA�V�~�����[�V�����X�e�b�v�����X�V���Ă����܂��B
�����ł́A���ꂼ��̍��̂̏d�S����2m����E������10N�̗͂������AStep()�֐����Ă�ł��܂��B
�܂��A�X�e�b�v��i�߂邲�ƂɁA���̂̈ʒu�����o�͂��Ă��܂��B
\verbatim
    for(int i=0; i<10; ++i){
        solid1->AddForce(Vec3f(10,0,0), Vec3f(0,2,0)+solid1->GetCenterPosition());
        solid2->AddForce(Vec3f(10,0,0), Vec3f(0,2,0)+solid2->GetCenterPosition());
        scene->Step();
        std::cout << solid1->GetFramePosition();
        std::cout << solid2->GetFramePosition() << std::endl;
	}
\endverbatim 

�V�~�����[�V�����I�����ɂ́A�����G���W��SDK�̊J�����s���܂��B
�������A�����G���W��SDK�͊J�����Ȃ��Ă��A�A�v���P�[�V�����I�����ɊJ�������悤�ɂȂ��Ă��܂��B
\verbatim
    delete sdk;
\endverbatim  	
 	
�ȏ�ɂ��A���̂ɂ�����̗͂��������V�~�����[�V�������s���܂��B
�Ȃ��A�����ł͍Ō�ɁADBG_NEW��char�^�̃������z���p�ӂ��AVC�ł̃��������[�N���o�e�X�g���s���Ă��܂��B
\verbatim 	
    char* memoryLeak = DBG_NEW char[123];
 	
 	return 0;
 }
\endverbatim 
<br><br><br>

*/
