/**
\page pageFileIOImp �t�@�C�����o��SDK�̎���

\section secFileIMech FileIO�̎d�g��

\subsection secFileLoadSave �t�@�C���̃��[�h�E�Z�[�u
�t�@�C�������[�h���邽�߂ɂ́C�t�@�C���t�H�[�}�b�g�ɍ������p�[�T��p��
����K�v������܂��D
����́CFIFile�N���X�̔h���N���X���s���Ă��܂��D
�Ⴆ��FIFileX�́CX�t�@�C�����p�[�X���܂��D
Boost::spirit�Ƃ����������@����̃p�[�T�W�F�l���[�^��p���Ď������Ă��܂��D

�t�@�C���̃Z�[�u��FIFileX���s���܂��D�p�[�X�ɔ�ׂĊȒP�Ȃ̂ŁC
�V�[���O���t�����񂵂Ȃ���C�ړI�̃t�H�[�}�b�g�̃t�@�C�����o�͂����悤�ɁC
���z�֐����������čs���܂��D

\subsection secLoadProc ���[�h�̎菇
�����ł́C�t�@�C���ɏ����ꂽ�ЂƂ̃I�u�W�F�N�g�̃f�[�^����C
�I�u�W�F�N�g�𐶐�����菇��������܂��D

<ol>
<li>�I�u�W�F�N�g�̌^����ǂ݁C����ɂ������f�B�X�N���v�^��p�ӂ���D
�Ⴆ�΁CGRFrame�����[�h����ꍇ�C
<pre>
 GRFrame{
 	0,0,0,1, 0,1,0,0, 0,0,1,0, 0,1,2,0;;
 }
</pre>
��GRFrame��ǂ񂾂Ƃ���ŁC �p�[�T�� FILoadContext::PushType() ���Ăяo���āC
GRFrameDesc ��p�ӂ��܂��D

<li>�f�[�^��ǂݏo���C�p�ӂ����f�B�X�N���v�^�Ɋi�[����D
0,0,0,1, 0,1,0,0, 0,0,1,0, 0,1,2,0;;��ǂݏo�� GRFrame::transform �Ɋi�[���čs���܂��D
�����FILoadContext�����ݓǂݏo�����̃f�B�X�N���v�^�ɂ��Ă̏��������Ă��āC
�ǂݏo�������l���f�B�X�N���v�^�ɏ�������ōs���܂��D
Vec3f��Material�̂悤�ȑg�ݗ��Č^�́C�ċA�I�ɏ�������C�ŏI�I�ɂ́C
���l��ǂݍ��߂΁CFILoadContext::WriteNumber(), ������Ȃ�FILoadContext::WriteString()
���Ăяo����܂��D

<li>�I�u�W�F�N�g���쐬����D
�f�B�X�N���v�^����I�u�W�F�N�g�𐶐����܂��D
����́CFILoad::LoadNode()���C�e�I�u�W�F�N�g��CreateObject()
���Ăяo���Ă������ƂŎ������Ă��܂��D
�e������Ă���Ȃ��ꍇ�́C���̂܂��e��CreateObject()���Ăяo���C
�Ō�܂ō���Ȃ��ꍇ�̓O���[�o����CreateSdk()�֐����Ăяo���܂��D

CreateObject()�ɂ��ẮC\ref pageFoundationImp �� \ref secCreateObject ���Q�Ƃ��Ă��������D
</ol>

���̂悤�ɃI�u�W�F�N�g�̐�����f�[�^�̐ݒ�ɂ�API��p���Ă���C
���̃��W���[��(���Ƃ��� src/Physics)�̎����ɂ͈ˑ����Ă��܂���D

\subsection secLoadHandler �I�u�W�F�N�g�̃��[�h�ȊO�̏���
�m�[�h�����[�h�����Ƃ��C�P���ɃI�u�W�F�N�g�𐶐����邾���łȂ��C
���炩�̏������s���������Ƃ�����܂��D

���Ƃ��΁CDirectX��X�t�@�C���� Frame��FrameTransformMatrix �� GRFrame�Ǝ��Ă��܂����C
�኱�Ⴂ�܂��DX�t�@�C�������[�h���邽�߂ɂ́CFrame��FrameTransformMatrix����C
GRFrame�𐶐����Ȃ���΂Ȃ�܂���D

����́C���̂悤�Ȏ菇�ōs���܂��D
<ol>
 <li> Frame, FrameTransformMatrix �̃f�[�^�����[�h�ł���悤�� 
      FrameDesc, FrameTransformMatrixDesc �����D
 <li> Frame, FrameTransformMatrix �ɑΉ������n���h����FINodeHandler(�̔h���N���X)�����C�o�^����
      (FIOldSpringheadNodeHandler.cpp���Q��)�D<br>
      FrameIf�CFrameTransformMatrixIf �͍��Ȃ��D�i���Ƃ��̃I�u�W�F�N�g����������Ă��܂��j
 <li> �n���h���̒��ŁCFrameDesc, FrameTransformMatrixDesc ���󂯎���āC
      GRFrame �𐶐�������C�e�� GRFrame�� transform ��ݒ肵����Ƃ������C�������s���D
</ol>

\subsection secSaveProc �Z�[�u�̎菇

*/
