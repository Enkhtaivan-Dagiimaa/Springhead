/**
\page pageFileIOImp �t�@�C�����o��SDK�̎���

\section secFileIMech FileIO�̎d�g��

\subsection secFileLoadSave �t�@�C���̃��[�h�E�Z�[�u
�t�@�C�������[�h���邽�߂ɂ́C�t�@�C���t�H�[�}�b�g�ɍ������p�[�T��p�ӂ���K�v������܂��D
����́CFIFile�N���X�̔h���N���X���s���Ă��܂��D
�Ⴆ��FIFileX�́CX�t�@�C�����p�[�X���܂��D
Boost::spirit�Ƃ����������@����̃p�[�T�W�F�l���[�^��p���Ď������Ă��܂��D

�t�@�C���̃Z�[�u��FIFileX���s���܂��D�p�[�X�ɔ�ׂĊȒP�Ȃ̂ŁC
�V�[���O���t�����񂵂Ȃ���C�ړI�̃t�H�[�}�b�g�̃t�@�C�����o�͂����悤�ɁC���z�֐����������čs���܂��D

\subsection secLoadProc ���[�h�̎菇
\subsubsection secLoadObject �I�u�W�F�N�g�̃��[�h
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
�����FILoadContext�����ݓǂݏo�����̃f�B�X�N���v�^�ɂ��Ă� \ref secTypeDesc �������Ă��āC
�ǂݏo�������l���f�B�X�N���v�^�ɏ�������ōs���܂��D
Vec3f��Material�̂悤�ȑg�ݗ��Č^�́C�ċA�I�ɏ�������C�ŏI�I�ɂ́C
���l��ǂݍ��߂΁CFILoadContext::WriteNumber(), ������Ȃ�FILoadContext::WriteString()
���Ăяo����܂��D

<li>�I�u�W�F�N�g���쐬����D
�f�B�X�N���v�^����I�u�W�F�N�g�𐶐����܂��D
����́CFIFile::LoadNode()���C�e�I�u�W�F�N�g��CreateObject()
���Ăяo���Ă������ƂŎ������Ă��܂��D
�e������Ă���Ȃ��ꍇ�́C���̂܂��e��CreateObject()���Ăяo���C
�Ō�܂ō���Ȃ��ꍇ�̓O���[�o����CreateSdk()�֐����Ăяo���܂��D
CreateObject()�ɂ��ẮC\ref pageFoundationImp �� \ref secCreateObject ���Q�Ƃ��Ă��������D

<li>�I�u�W�F�N�g��ǉ�����B
�Ō�Ƀ��[�h�����I�u�W�F�N�g��e�I�u�W�F�N�g�ɒǉ����܂��B
����ɂ���āA�V�[���O���t���c���[�`���ɂȂ�킯�ł��B

�e�I�u�W�F�N�g�ւ̒ǉ��́A �e�I�u�W�F�N�g (FILoadContext::objects.Top() �ɂ܂�Ă���j
��AddChildObject()���Ăяo�����Ƃōs���܂��B
</ol>

���̂悤�ɃI�u�W�F�N�g�̐�����f�[�^�̐ݒ�ɂ�API��p���Ă���C
���̃��W���[��(���Ƃ��� src/Physics)�̎����ɂ͈ˑ����Ă��܂���D

\subsection secLoadHandler �I�u�W�F�N�g�̃��[�h�ȊO�̏���
�m�[�h�����[�h�����Ƃ��C�P���ɃI�u�W�F�N�g�𐶐����邾���łȂ��C���炩�̏������s���������Ƃ�����܂��D

���Ƃ��΁CDirectX��X�t�@�C���� Frame��FrameTransformMatrix �� GRFrame�Ǝ��Ă��܂����C�኱�Ⴂ�܂��D
X�t�@�C�������[�h���邽�߂ɂ́CFrame��FrameTransformMatrix����CGRFrame�𐶐����Ȃ���΂Ȃ�܂���D

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

\subsection secLoadLink ���[�h��̎Q�Ƃ̉���
���ׂẴI�u�W�F�N�g�����[�h���؂�Ă��Ȃ����߁A���[�h���ɃI�u�W�F�N�g�ւ̎Q�Ƃ��������邱�Ƃ�
�ł��܂���B
�����ŁA���[�h��ɂ܂Ƃ߂ăI�u�W�F�N�g�̎Q�Ƃ�����(Link)���܂��B
���[�h���ɁAFILoadContext::AddLink() ���Ăяo���ƃ����N���L�^����܂��B
���[�h�I������FIFile���AFIFileContext::LinkTask::Execute()���Ăяo���ă����N�����s���܂��B


\subsection secSaveProc �Z�[�u�̎菇

\subsection secTypeDesc �f�B�X�N���v�^�̌^���
�f�B�X�N���v�^(PHSolidDesc�Ȃ�)�̂ǂ��ɂǂ̂悤�ȃf�[�^���i�[����Ă��邩��������ƁC
���[�h�����t�@�C���̃f�[�^�������I�Ƀf�B�X�N���v�^�Ɋi�[������C
�Z�[�u����f�[�^�������I�ɓǂݏo������ł��܂��D

�����ŁC�f�B�X�N���v�^�̐錾��ǂ�ŁC�^�̏���o�^����C++�̃\�[�X�R�[�h�𐶐�����c�[��
(SWIG�����ς��� bin/swig/swig.exe)���g���āC�^�������s���ɓo�^���Ă��܂��D
swig.exe�̃\�[�X�́C /bin/src/swig/ �ɂ���܂��D
Foundation, Physics, Graphics �̃��C�u�������r���h����ƁC�R���p�C���O��
<pre>
	/src/Foundation/RunSwig.bat SDK��
</pre>
�����s����� swig.exe ���g���Č^���o�^�\�[�X�R�[�h(SDK��Stub.cpp)
�𐶐����Ă���܂��D���̃\�[�X�́A
void RegisterTypeDescSDK��(UTTypeDescDb* db)
���Ăяo���A�^����UTTypeDescDb::theTypeDescDb �ɓo�^���܂��D�^���́CUTTypeDesc�I�u�W�F�N�g�ŕ\����܂��D

�f�B�X�N���v�^�̐錾�̋L�@��swig.exe�̑z��O���� SDK��Stub.cpp �ŃG���[���N���邱�Ƃ�����܂��D
���̂悤�ȏꍇ�́C���̃f�B�X�N���v�^���Q�l�ɋL�@�ɒ����Ă��������D

\subsection secTypeDecl �f�X�N���v�^��p���p�̃}�N����`�w�b�_�t�@�C��
�t�@�C���Z�[�u�E���[�h�̑Ώۂ́A�f�X�N���v�^�̕ϐ��ł����A
�����N���X�Ńf�X�N���v�^���p���������Ȃ��ꍇ������܂��B
�i�Ⴆ�΁A�����N���X�̊�{�N���X�����łɊ�{�̃f�X�N���v�^���p�����Ă���ꍇ�Ȃǁj
���̏ꍇ�A�h���̃f�X�N���v�^�̃����o�݂̂������N���X�ɒǉ��������Ȃ�܂��B
swig.exe �́A���̂��߂̃}�N��(SPR_DECLMEMBEROF_XXX)��
SDK��Decl.hpp �Ƃ����t�@�C���ɍ쐬���܂��B�ڂ����� \ref secCreateNonInheritLoadableObject
���Q�Ƃ��Ă��������B
*/
