/** \page pageFileIO �t�@�C�����o��SDK 


\contents
�t�@�C�����o��\c SDK �́C
�t�@�C�����烍�[�h�����f�[�^����ɃV�[���O���t�̐���������C
�V�[���O���t�̃f�[�^��ǂݏo���āC�t�@�C���Ƀf�[�^��ۑ������肵�܂��D
���̐}�́C�V�[���O���t�ƃt�@�C�����o�͂̃C���[�W�ł��D
�i�t�@�C����ł̃m�[�h�̐e�q�֌W�ƃV�[���O���t�̐e�q�֌W�͕K���Ή�����킯�ł͂���܂���D�j

\image html FileScene.png

\section secSprFile         Spr�t�@�C��
Springhead�Ǝ��̃t�@�C���`���ł��B
�l���ǂݏ������₷���ASpringhead�̎d�l���ω����Ă��]��e�����󂯂Ȃ��悤�Ȍ`���ɂȂ��Ă��܂��B
�t�@�C�����菑������ꍇ�͂��̌`�����g���Ă��������B

spr�t�@�C���̓m�[�h��`�̌J��Ԃ��ł��Bspr�t�@�C���̗�������܂��B
\verbatim
PHSdk{                  #PHSdk�m�[�h
    CDSphere sphere{    #���̎q�m�[�h��CDSphere�m�[�h��ǉ�
        material = {    # CDSphere �� material(PHMaterial�^)��
            mu = 0.2    # ���C�W�� mu ��0.2����
        }
        radius = 0.5    # radius��0.5����
    }
    CDBox bigBox{
        boxsize = 2.0 1.1 0.9
    }
\endverbatim

Spr�t�@�C���̃m�[�h��Springhead2�̃f�B�X�N���v�^�i\ref secStateDesc ���Q�Ɓj
�ɂP�΂P�őΉ����܂��B�f�B�X�N���v�^�����p�ӂ���Ύ����I�Ɏg����m�[�h�̌^�������܂��B
�t�@�C���Œl�������Ȃ��ƁA�f�B�X�N���v�^�̏����l�ɂȂ�܂��B
��̗�ł́APHSdk�ɒǉ������sphere(CDSphere�^)�́A
\code
    CDSphereDesc desc;
    desc.material.mu = 0.2;
    desc.radius = 0.5;
\endcode
�Ƃ����f�B�X�N���v�^desc�ō��̂Ɠ������ƂɂȂ�܂��B

Spr�t�@�C���̕��@��BNF�{���K�\���ŏ�����
\verbatim
spr   = node*
node  = node type, (node id)?, block
block = '{' (node|refer|data)*  '}'
refer = '*' node id
data  = field id, '=', (block | right)
right = '[' value*, ']' | value
value = bool | int | real | str | right
\endverbatim
�ƂȂ�܂��Bright�ȍ~�̉��߂�field�̌^�Ɉˑ����܂��B



\section secXFile       X�t�@�C��
�u X �t�@�C�� �v�́ADirect3D�̃t�@�C���t�H�[�}�b�g�ŁA�g���q�� .x �ł��B
���f�����O�\�t�gXSI�Ŏg���Ă���A�����̃��f�����O�c�[���ŏo�͂ł��܂��B
3D�̌`��f�[�^�A�}�e���A���A�e�N�X�`���A�{�[���Ȃǂ��܂߂邱�Ƃ��ł��܂��B
Springhead2�ł́A�W���I��X�t�@�C���̃��[�h�ƁASpringhead2�Ǝ��̃m�[�h��
���[�h�ƃZ�[�u���ł��܂��B�������Ǝ��m�[�h���菑������ꍇ�� \ref secSprFile 
�̕��������₷���֗��ł��̂ł�����̎g�p���������߂��܂��B

X�t�@�C���̗�������܂��B
<table width=100% height="50" style="border:3px double #1841a5;">
<tr><td><tt>
�@xof 0302txt 0064�@�@�@�@#�@�ŏ��̍s�͂��ꂩ��n�܂�               <br>
                                                                    <br>
�@#�@�@�m�[�h�́C                                                   <br>
�@#�@�@�@�@�^���C�m�[�h�� { �t�B�[���h�̌J��Ԃ�   �q�m�[�h }       <br>
�@#�@�@����Ȃ�D                                                   <br>
�@PHScene scene1{                                                   <br>
�@�@�@0.01;0;;�@�@�@�@�@�@#�@�t�B�[���h �� �l; �̌J��Ԃ�           <br>
�@�@�@1;0;-9.8;0;;�@�@�@�@#�@�l�� ���l�C������܂��̓t�B�[���h      <br>
�@�@�@PHSolid soFloor{�@�@#�@�q�m�[�h�́C�m�[�h�Ɠ���               <br>
�@�@�@�@�@(�ȗ�)                                                    <br>
�@�@�@}                                                             <br>
�@}                                                                 <br>
�@# �R�����g�� #�ȊO�� // ���g����                                  <br>
</tt></td></tr></table>


\section secXNodeDef �Ǝ��m�[�h�̒�`
\c Springhead2 �̒ʏ�̃m�[�h�́C�I�u�W�F�N�g�̃f�B�X�N���v�^�i\ref secStateDesc ���Q�Ɓj�ɂP�΂P�őΉ����܂��D
���[�h���ɂ́C�f�B�X�N���v�^�ɑΉ�����I�u�W�F�N�g����������C�V�[���O���t�ɒǉ�����܂��D
�Z�[�u���ɂ́C�I�u�W�F�N�g����f�B�X�N���v�^��ǂݏo���C�m�[�h�̌`���Ńt�@�C���ɕۑ�����܂��D

�I�u�W�F�N�g�̃f�B�X�N���v�^�ɂ́C�K���Ή�����m�[�h������܂��D<br>
�Ⴆ�΁C\c SprPHScene.h �ɂ́C

\verbatim
 struct PHSceneState{
�@�@�@double timeStep;      ///< �ϕ��X�e�b�v
�@�@�@unsigned count;       ///< �ϕ�������
 };
 struct PHSceneDesc:PHSceneState{
�@�@�@�@/// �ڐG�E�S�������G���W���̎��
�@�@�@�@enum ContactMode{ MODE_NONE, MODE_PENALTY, MODE_LCP};
�@�@�@�@Vec3f gravity;      ///< �d�͉����x�x�N�g���D�f�t�H���g�l��(0.0f, -9.8f, 0.0f)�D
 };
\endverbatim

�̂悤�ɁC�X�e�[�g�ƃf�B�X�N���v�^���錾����Ă��܂��D���� \c PHSceneDesc �ɑΉ����� \c X �t�@�C���̃m�[�h�́C

<table width=100% height="50" style="border:3px double #1841a5;">
<tr><td><tt>
�@PHScene scene1{                                                                            <br>
�@�@�@0.01;�@�@�@�@�@�@#�@PHSceneState::timeStep                                             <br>
�@�@�@0;;�@�@�@�@�@�@�@#�@PHSceneState::count�@�@�@�@ �Ō��;��PHSceneState���̏I���������D<br>
�@�@�@1;�@�@�@�@�@�@�@ #�@PHSceneDesc::ContactMode                                           <br>
�@�@�@0;-9.8;0;;�@�@�@ #�@PHSceneDesc::gravity�@�@�@�@�Ō��;��PHSceneDesc���̏I���������D <br>
�@}                                                                                          <br>
</tt></td></tr></table>

�̂悤�ɂȂ�܂��D�N���X�̃����o�ϐ������̂܂܃t�B�[���h�ɂȂ�܂��D
�܂��C��{�N���X�́C�擪�Ƀt�B�[���h���ǉ����ꂽ�`�ɂȂ�܂��D

�ʏ�m�[�h�̈ꗗ��\ref pageNodeDefList ���Q�Ɖ������D
<br><br><br>

\section secXSpecialNode    X�t�@�C���̃m�[�h
Springhead2�̓Ǝ��m�[�h�����łȂ��A���ʂ�X�t�@�C���̃m�[�h�����[�h�ł��܂��B
X�t�@�C���ɂ́A
<table width=100% height="50" style="border:3px double #1841a5;">
<tr><td><tt>
�@Frame{                                                                <br>
�@�@�@�@FrameTransfromMatrix{ 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1; }     <br>
�@}                                                                     <br>
</tt></td></tr></table>
�̂悤�ȃt���[���̃m�[�h�^������܂����A
Sprinhead2 �ɂ͑Ή�����f�B�X�N���v�^��I�u�W�F�N�g������܂���D
�����ŁC�����́AGRFrame�ɕϊ�����ă��[�h����܂��D
\ref pageNodeDefList �Ƀm�[�h�̈ꗗ������܂��B
<br><br><br>


<!------------------------------ �m�[�h�̃��[�h ---------------------------------------->
\section secXLoadNode           �m�[�h�̃��[�h
TODO�F�L�q���Â��B�v�X�V�B

�f�B�X�N���v�^�ɑΉ�����m�[�h�͎��̎菇�Ő�������܂��D

<tt>
�@�@ 1�D�m�[�h�̌^�ɑΉ�����f�B�X�N���v�^�𐶐�����D                      <br>
�@�@ 2�D�f�B�X�N���v�^�Ƀm�[�h�̃t�B�[���h�̓��e���L������D                <br>
�@�@ 3�D��c�m�[�h�� CreateObject() API ��p���āC�I�u�W�F�N�g�𐶐�����D  <br>
�@�@ 4�D���������I�u�W�F�N�g�ɖ��O��ݒ肷��D                              <br>
�@�@ 5�D���������I�u�W�F�N�g��e�I�u�W�F�N�g�ɒǉ�����D                    <br>
�@�@ 6�D�q�m�[�h�����[�h����D                                              <br>
</tt>
<br><br><br>


\section secFileIOLoadSave �t�@�C���̃��[�h�ƃZ�[�u
TODO�F�L�q���Â��B�v�X�V�B

�t�@�C���̃��[�h��Z�[�u���s���ɂ́C�܂�FISdk���쐬���CFISdk����C
FIFileX���쐬���܂��D���̂悤�Ȏ菇�ɂȂ�܂��D

<tt>
�@�@ 1�DCreateFISdk() �ŁCFISdk�̃C���X�^���X fiSdk ���쐬����D                                                <br>
�@�@ 2�DFISdk::CreateFileX() �� FIFileX �̃C���X�^���X fileX �𐶐�����D                                   <br>
�@�@ 3�DObjectOfs objs ��p�ӂ���D���[�h�����m�[�h�̂����C���[�g�̃m�[�h�������ɓ���D                     <br>
�@�@ 4�DFIFileX::Load() ���Ăяo���āC���[�h���s���D                                                        <br>
�@�@ 5�D�Z�[�u����Ȃ�΁C�Z�[�u�������I�u�W�F�N�g������������ objs ��p�ӂ��CFIFileX::Save() ���Ăяo���D  <br>
</tt>
<br>



\section secXDataFormat     �e�f�[�^�^�̋L�q
TODO�F�L�q���Â��B�v�X�V�BX�ł͂Ȃ��Aspr�Ɉړ����ׂ��B

\subsection subXModelShape  ���f���`��
���f���`�����̗v�f�Ƃ��Ē�`���܂��D�����Œ�`�����`��́C�I�u�W�F�N�g�^�C�v�Ƃ��ĔF������C
�V�[�����̔C�ӂ̈ʒu�ɔz�u���邱�Ƃ��ł��܂��D���f���`���p�ӂ��Ă����ƁC�����`���V����
�쐬������C�R�s�[�����肷��K�v���Ȃ��C���f�����O�������I�ɍs�����Ƃ��ł��܂��D

\htmlonly 
<h3><a class="anchor">(1)�@�ʌ`�󃁃b�V�� </a></h3> 
\endhtmlonly
�@�ʌ`�󃁃b�V�����`����ɂ́C<tt>[�^��]</tt>�� \c "CDConvexMesh" �ɁC
<tt>[���f���`��^�C�v]</tt>�� \c "1" �ɐݒ肵�ĉ������D
\htmlonly
<table width=100% height="50" style="border:3px double #1841a5;">
<tr><td><tt>
�@CDConvexMesh [�m�[�h��] {                                                                     <br>
�@�@�@�@���_���W���X�g;                                                                         <br>
�@�@�@�@�����C���C�W��; �Î~���C�W��; ���˕Ԃ�W��;;�@�@# [�����V�~�����[�V�����Ɋ֌W����ގ�]  <br>
�@}                                                                                             <br>
</tt></td></tr></table>
\endhtmlonly

�@���Ɏ����̂́C���� \c 1 �̗����̂��C�ʌ`�󃁃b�V���Œ�`������ł��D
\htmlonly
<table width=100% height="50" style="border:3px double #1841a5;">
<tr><td><tt>
�@CDConvexMesh meshBlock {                                                                  <br>
�@�@�@�@0.5; 0.5; 0.5;, 0.5; 0.5;-0.5;, -0.5; 0.5;0.5;, -0.5; 0.5;-0.5;,                    <br>
�@�@�@�@0.5;-0.5; 0.5;, 0.5;-0.5;-0.5;, -0.5;-0.5;0.5;, -0.5;-0.5;-0.5;;                    <br>
�@�@�@�@0.2; 0.5; 0.6;;�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@# phmaterial <br>
�@}                                                                                         <br>
</tt></td></tr></table>
\endhtmlonly
<br>


\htmlonly 
<h3><a class="anchor">(2)�@�� </a></h3> 
\endhtmlonly
�@�����`����ɂ́C<tt>[�^��]</tt>�� \c "CDSphere" �ɐݒ肵�ĉ������D
\htmlonly
<table width=100% height="50" style="border:3px double #1841a5;">
<tr><td><tt>
�@CDSphere [�m�[�h��] {                                                                             <br>
�@�@�@�@���a;                                                                                       <br>
�@�@�@�@�����C���C�W��; �Î~���C�W��; ���˕Ԃ�W��;;�@�@�@ # [�����V�~�����[�V�����Ɋ֌W����ގ�]   <br>
�@}                                                                                                 <br>
</tt></td></tr></table>
\endhtmlonly

�@���Ɏ����̂́C���a \c 1 �̋����`������ł��D
\htmlonly
<table width=100% height="50" style="border:3px double #1841a5;">
<tr><td><tt>
�@CDSphere sphere {                                 <br>
�@�@�@�@1.0;�@�@�@�@�@�@�@�@�@ //  radius           <br>
�@�@�@�@0.2; 0.5; 0.6;;�@�@�@�@//  phmaterial       <br>
�@}                                                 <br>
</tt></td></tr></table>
\endhtmlonly
<br><br>


\htmlonly 
<h3><a class="anchor">(3)�@������ </a></h3> 
\endhtmlonly
�@�����̂��`����ɂ́C<tt>[�^��]</tt>�� \c "CDBox" �ɐݒ肵�ĉ������D
\htmlonly
<table width=100% height="50" style="border:3px double #1841a5;">
<tr><td><tt>
�@CDBox [�m�[�h��] {                                                                                <br>
�@�@�@�@�����̃T�C�Y;                                                                               <br>
�@�@�@�@�����C���C�W��; �Î~���C�W��; ���˕Ԃ�W��;;�@�@�@ # �����V�~�����[�V�����Ɋ֌W����ގ� <br>
�@}                                                                                                 <br>
</tt></td></tr></table>
\endhtmlonly

�@���Ɏ����̂́C\c X �����̃T�C�Y�� \c 1.0�CY �����̃T�C�Y�� \c 2.0�C\c Z �����̃T�C�Y�� \c 3.0 �̒����̂��`������ł��D
\htmlonly
<table width=100% height="50" style="border:3px double #1841a5;">
<tr><td><tt>
  CDBox box {                                       <br>
�@�@�@�@1.0; 2.0; 3.0;;�@�@�@�@//  boxsize          <br>
�@�@�@�@0.2; 0.5; 0.6;;�@�@�@�@//  phmaterial       <br>
�@}                                                 <br>
</tt></td></tr></table>
\endhtmlonly
<br><br><br>

*/
