/** \page page_usersGuide		���[�U�[�Y�K�C�h

\section introduction �͂��߂�
Springhead2�́C����ō����ȁC�����̗p�r�ɏ\���ȋ@�\���������V�~�����[�^�ł��D

\subsection characteristic	����

\subsection others	���̃V�~�����[�^


\section install	�C���X�g�[��
�ڍׂ� \ref tutorial ���Q�Ƃ��������D

\subpage page_concept
\section apiBasic		API�̊�b
SDK�́C�����������Ȃ�API�N���X����Ďg���܂��D

\subsection apiClass API�N���X
API�N���X�́C���O�̍Ō�ɃC���^�t�F�[�X��\��If�����܂��D
�܂��CSDK�N���X�̃C���X�^���X��Ԃ��O���[�o���֐���p���āC
SDK�N���X�̃C���X�^���X���쐬�E�擾���C
����SDK�N���X����ʂ�API�N���X�̃C���X�^���X���쐬�E�擾���Ă����܂��D<br>
���̃R�[�h�͕����G���W��API���g�����v���O�����̗�ł��D
\verbatim
int main(int argc, char* argv[]){
    PHSdkIf* sdk = CreatePHSdk();                   //  �����G���W��SDK�̍쐬
    PHSceneIf* scene = sdk->CreateScene();          //  �V�[���̍쐬
    :
\endverbatim

\subsection state �X�e�[�g�ƃf�X�N���v�^
API�N���X�̃C���X�^���X�����Ƃ��ɂ́C�f�X�N���v�^�Ƃ����\���̂��g����
�����l��n���܂��D�f�X�N���v�^�́CAPI�N���X�̖��O����If���Ƃ���Desc���������O�ł��D
\verbatim
int main(int argc, char* argv[]){
    PHSdkIf* sdk = CreatePHSdk();                   //  �����G���W��SDK�̍쐬
    PHSceneIf* scene = sdk->CreateScene();          //  �V�[���̍쐬
    PHSolidDesc desc;                               //  ���̂̃f�B�X�N���v�^
    desc.mass = 2.0;                                //  ���ʂ�2�ɐݒ�
    desc.inertia *= 2.0;                            //  �����e���\���̑Ίp������2�ɐݒ�
    PHSolidIf* solid1 = scene->CreateSolid(desc);   //  ���̂P��desc�Ɋ�Â��č쐬
    desc.center = Vec3f(1,0,0);                     //  ���̂̏d�S��(1,0,0)�ɐݒ�
    PHSolidIf* solid2 = scene->CreateSolid(desc);   //  ���̂Q��desc�Ɋ�Â��č쐬
    :
\endverbatim
�f�B�X�N���v�^�̂Ȃ��ɂ́C Spr::PHSolidDesc �̂悤�ɁC�X�e�[�g(Spr::PHSolidState)���p�����Ă�����̂�����܂��D
�X�e�[�g�́C���̃C���X�^���X�̏�Ԃ�\���ϐ��ŃV�~�����[�V������i�߂�ƕω����܂��D
�X�e�[�g������ۑ����Ă����΁C���̃I�u�W�F�N�g��ۑ����Ă��������_�̏�Ԃɖ߂����Ƃ��ł��܂��D

\section structure		API�\��

\subsection physics		�����V�~�����[�V�������C�u����
\subsection graphics	�O���t�B�b�N�X���C�u����
\subsection fileio		�t�@�C��IO���C�u����
*/
