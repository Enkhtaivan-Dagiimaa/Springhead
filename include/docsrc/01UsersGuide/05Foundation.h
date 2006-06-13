/** \page pageFoundation ��{SDK��API

\section sec_vectorMatrix	�x�N�g���N���X�C�s��N���X
Springhead2�ɂ́C��{�I�ȃx�N�g���E�s��̃N���X���p�ӂ���Ă��܂��D
�����́CSDK�S�̂Ŏg�p���Ă���C���[�U���g�p�ł��܂��D
<br>
���ڍׂȐ�����\ref pageAffine ���Q�Ƃ��ĉ������D
\subsection sec_baseVec2		2�����x�N�g��
2�̒P���xfloat�^�f�[�^�����x�N�g���^��Vec2f�^�A2�̔{���xdouble�^�f�[�^�����x�N�g���^��Vec2d�^�ƂȂ�܂��D
\verbatim	
    Vec2f vec(a, b);                            // a�Ab �ŏ�����
    Vec2f vec = Vec2f(a, b);                    // a�Ab �ŏ�����
    vec[0] = a;                                 // vec.x = a;
    vec[1] = b;                                 // vec.y = b;
    Vec2f vec2= vec + Vec2f(x, y);              // ���Z
\endverbatim
<br>

\subsection sec_baseVec3	3�����x�N�g��
3�̒P���xfloat�^�f�[�^�����x�N�g���^��Vec3f�^�A3�̔{���xdouble�^�f�[�^�����x�N�g���^��Vec3d�^�ƂȂ�܂��D
\verbatim	
    Vec3f vec(a, b, c);                         // a�Ab�Ac �ŏ�����
    Vec3f vec = Vec3f(a, b, c);                 // a�Ab�Ac �ŏ�����
    vec[0] = a;                                 // vec.x = a;
    vec[1] = b;                                 // vec.y = b;
    vec[2] = c;                                 // vec.z = c;
    Vec3f vec2= vec + Vec3f(x, y, z);           // ���Z
\endverbatim
<br>

\subsection sec_baseVec4		4�����x�N�g��
4�̒P���xfloat�^�f�[�^�����x�N�g���^��Vec4f�^�A4�̔{���xdouble�^�f�[�^�����x�N�g���^��Vec4d�^�ƂȂ�܂��D
\verbatim	
    Vec4f vec(a, b, c);                         // a�Ab�Ac �ŏ�����
    Vec4f vec = Vec4f(a, b, c);                 // a�Ab�Ac �ŏ�����
    vec[0] = a;                                 // vec.x = a;
    vec[1] = b;                                 // vec.y = b;
    vec[2] = c;                                 // vec.z = c;
    Vec4f vec2= vec + Vec4f(x, y, z);           // ���Z
\endverbatim
<hr>

\section sec_utilityType	���[�e�B���e�B�N���X
Springhead2�ɂ́C���[�e�B���e�B�N���X���p�ӂ���Ă��܂��D
�����́CSDK�S�̂Ŏg�p���Ă���C���[�U���g�p�ł��܂��D

\subsection sec_baseUTRef		�Q�ƃJ�E���^�ƎQ�ƃ|�C���^
���������[�N�΍�Ƃ��āA�Q�ƃJ�E���^�������N���X�ƃJ�E���^���Ǘ�����X�}�[�g�|�C���^�̃e���v���[�g���������Ă��܂��D<br>
<a href="../../src/html/classSpr_1_1UTRef.html">UTRef<�N���X��></a>���Q�ƃ|�C���^�A
<a href="../../src/html/classSpr_1_1UTRefCount.html">UTRefCount</a>���Q�ƃJ�E���^�ƂȂ�܂��D<br>
<a href="../../src/html/classSpr_1_1UTRefCount.html">UTRefCount</a>����{�N���X�Ɏ��N���XA������ꍇ�C���̂悤�Ɏg�����Ƃ��ł��܂��D<br>
for���[�v�ɂ�10��̃������m�ۂ�new�ōs���Adelete�ł̃������J�����s���Ă��܂��񂪁A���������[�N�͔������܂���D
\verbatim	
    UTRef<A> p;
    for(int i=0; i10; ++i){
        p = new A;
        p->A�̃��\�b�h()
    }
\endverbatim

\section sec_SceneGraph �V�[���O���t
 ��ʂɁC�����V�~�����[�^��o�[�`�������A���e�B��SDK�ł́C
 �o�[�`�������E���\������I�u�W�F�N�g�B�̂��Ƃ��V�[���O���t�ƌĂт܂��D
 �V�[���O���t�́CScene�Ƃ�World�Ƃ�Universe�Ƃ��Ă΂��V�[���S�̂��Ǘ�����I�u�W�F�N�g�ƁC
 ����ɏ��L�����C���W�n��|���S�����b�V���Ȃǂ̃I�u�W�F�N�g����Ȃ�܂��D
 �Ⴆ�΁CSpringhead�̕����V�~�����[�^�̃V�[���O���t�́C
 <img src="../../include/docsrc/01UsersGuide/GRSdkScene.png">
 �̂悤�ȃC���[�W�ł��D
 
 
\section sec_SceneObjectName �V�[���ƃI�u�W�F�N�g�Ɩ��O
Springhead�� \ref pagePhysics \ref pageGraphics �ł́C�ЂƂ�VR���E���V�[��(Scene)�ƌĂсC
Scene�N���X���p������PHScene�N���X��GRScene�N���X���Ή����܂��D
�V�[���̒��ɂ́C�I�u�W�F�N�g(SceneObject�̔h���N���X)�𑽐���邱�Ƃ��ł��܂��D
�I�u�W�F�N�g�ɂ͖��O�����邱�Ƃ��ł��C\ref pageFileIO �Ń��[�h�����ꍇ�Ȃǂ́C
���O���w�肵�ăI�u�W�F�N�g�ւ̃|�C���^���擾�ł��܂��D
�܂��C�t�@�C���ɕۑ�����Ƃ��ɂ́C�I�u�W�F�N�g�ւ̎Q�Ƃ͖��O�ŕۑ�����܂��D
������ \ref pagePhysics �� \ref pageGraphics �ŋ��ʂ� \ref pageFoundation �ɂ���܂��D


*/
