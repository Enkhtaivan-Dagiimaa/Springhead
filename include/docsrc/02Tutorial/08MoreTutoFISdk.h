/** \page pageMoreTutoFISdk	�t�@�C�����o��SDK �`���[�g���A��

<br>
\section contentsMoreTutoFISdk	�� Contents ��
�@�@ \ref moreTutoFISdk						<br>
�@�@�@�@ \ref moreTutoCreateFISdk			<br>
�@�@ \ref moreTutoFileIO					<br>
�@�@�@�@ \ref moreTutoCreateFileX			<br>
�@�@�@�@ \ref moreTutoLoadFileX				<br>
�@�@�@�@ \ref moreTutoSaveFileX				<br>
�@�@ \ref moreTutoFileX						<br>
�@�@�@�@ \ref moreTutoFileXComment			<br>
�@�@�@�@ \ref moreTutoFileXHeader			<br>
�@�@�@�@ \ref moreTutoFileXDataForm			<br>
�@�@�@�@ \ref moreTutoFileXModelShape		<br>
<br><br>
<hr>

<!-------------------- 1�D�@�t�@�C�����o��SDK ------------------------------------------>
\section moreTutoFISdk		1�D�@�t�@�C�����o��SDK
�͂��߂ɁA�t�@�C�����o�͂ɕK�v�ƂȂ� \c SDK ��p�ӂ��܂��D

\subsection moreTutoCreateFISdk		1.1�D�@�t�@�C�����o��SDK�̍쐬
�t�@�C�����o�� \c SDK ���쐬���܂��D
\verbatim
    FISdkIf* fiSdk = CreateFISdk();
\endverbatim
<br><br>
<div align="right">
<a href="#pageMoreTutoFISdk">top of page</a>
</div> 
<hr>


<!-------------------- 2�D�@X�t�@�C�� ���[�_���Z�[�o ----------------------------------->
\section  moreTutoFileIO		2�D�@���[�_���Z�[�o
�C�ӂ� \c X �t�@�C���̓ǂݍ��݁A�����o�����\�ȁA���[�_���Z�[�o�� \c Springhead2 �ɂ͗p�ӂ���Ă��܂��D
���[�_�@�\��p����΁A�I�u�W�F�N�g�̒��_���W�╨���p�����[�^�Ȃǂ��C�ɂ����A\ref pageGraphics �� 
<tt>DrawScene()</tt> ���\�b�h�Ȃǂ��Ăяo�������ŕ`��ł��܂��D

\subsection moreTutoCreateFileX		2.1�D�@�t�@�C���n���h���̍쐬
�t�@�C���̓��o�͂ɂ̓t�@�C���n���h�����g���܂��D
\verbatim 
    FIFileXIf* fileX = fiSdk->CreateFileX();     // �t�@�C���n���h���̍쐬
\endverbatim
<br>

\subsection moreTutoLoadFileX		2.2�D�@X�t�@�C�����[�_�[
�w�肳�ꂽ \c X �t�@�C�������[�h���܂��D
\verbatim 
�@�@ObjectIfs objs;�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@// �C���^�t�F�[�X�N���X�ւ̃|�C���^�z���p��
�@�@fileX->Load(objs, "test.x");                 // X�t�@�C�������[�h
\endverbatim
<br>

\subsection moreTutoSaveFileX		2.3�D�@X�t�@�C���Z�[�o�[
�w�肵�����O�ŁA\c X �t�@�C���̃Z�[�u���s���܂��D
\verbatim 
�@�@fileX->Save(objs, "out.x");                  // X�t�@�C���̃Z�[�u
\endverbatim
<br><br>
<div align="right">
<a href="#pageMoreTutoFISdk">top of page</a>
</div> 
<hr>

<!-------------------- 3�D�@X�t�@�C��  ------------------------------------------------->
\section moreTutoFileX		3�D�@X�t�@�C��
�u\c X �t�@�C���v�Ƃ́A\c DirectX �̔ėp�I�ȃt�@�C���`���ł���A\ref pageFileIO �ł́A
\c X �t�@�C���ɑ΂��āA���[�h��Z�[�u�A�\����͂��s���܂��D�Ǝ��� \c template �\����
���R�Ƀf�[�^�^�̒�`���s����̂������ł��D

\subsection moreTutoFileXComment		3.1�D�@�R�����g
\c X �t�@�C���ł́A�R�����g�̊J�n�L���Ƃ��āA\c C++ �`���̃_�u���X���b�V�� \c "//" �A�܂��͓���L�� \c "#" ���g�p���܂��D
�R�����g�̊J�n�L������s�[�܂ł��A�R�����g�Ɖ��߂��܂��D
\verbatim 
�@�@# This is a comment.
�@�@// ������R�����g�ł��D
\endverbatim
<br>

\subsection moreTutoFileXHeader		3.2�D�@�w�b�_�[
\c X �t�@�C���̃w�b�_�[�Ƃ��āA�L���ȗ���ȉ��Ɏ����܂��D
\verbatim 
�@�@xof 0302txt 0064
\endverbatim
<br>

\subsection moreTutoFileXDataForm		3.3�D�@��{�I�ȃf�[�^�`��
�f�[�^�`���͊�{�I�Ɏ��̒ʂ�ł��D<br>
\verbatim 
�@�@<�f�[�^�^> [name] {
�@�@    �����o�[ 1;
�@�@    �@ �F
�@�@    �����o�[ n;
�@�@ }
\endverbatim
<br>

\subsection moreTutoFileXModelShape		3.4�D�@���f���`��
���f���`�����̗v�f�Ƃ��Ē�`���܂��D�����Œ�`�����`��́A�I�u�W�F�N�g�^�C�v�Ƃ��ĔF������A
�V�[�����̔C�ӂ̈ʒu�ɔz�u���邱�Ƃ��ł��܂��D���f���`���p�ӂ��Ă����ƁA�����`���V����
�쐬������A�R�s�[�����肷��K�v���Ȃ��A���f�����O�������I�ɍs�����Ƃ��ł��܂��D

\b 3.4.1.�@ \b �ʌ`�󃁃b�V�� <br>
�@�ʌ`�󃁃b�V�����`����ɂ́A<tt><�f�[�^�^></tt>�� \c "CDConvexMesh" �ɁA
<tt>[���f���`��^�C�v]</tt>�� \c "1" �ɐݒ肵�Ă��������D
\verbatim 
�@�@CDConvexMesh [name] {
�@�@�@�@1;;                                                 # ���f���`��^�C�v          
�@�@�@�@���_���W���X�g;
�@�@�@�@�����C���C�W��; �Î~���C�W��; ���˕Ԃ�W��;;�@�@�@�@# �����V�~�����[�V�����Ɋ֌W����ގ�
�@�@}
\endverbatim
�@���Ɏ����̂́A���� \c 1 �̗����̂��A�ʌ`�󃁃b�V���Œ�`������ł��D
\verbatim 
�@�@CDConvexMesh meshBlock {
�@�@�@�@1;;                                                                 # type=mesh
�@�@�@�@0.5; 0.5; 0.5;, 0.5; 0.5;-0.5;, -0.5; 0.5;0.5;, -0.5; 0.5;-0.5;, 
�@�@�@�@0.5;-0.5; 0.5;, 0.5;-0.5;-0.5;, -0.5;-0.5;0.5;, -0.5;-0.5;-0.5;;
�@�@�@�@0.2; 0.5; 0.6;;                                                     # phmaterial
�@�@}
\endverbatim
<br>

\b 3.4.2.�@ \b �� <br>
�@�����`����ɂ́A<tt><�f�[�^�^></tt>�� \c "CDSphere" �ɁA<tt>[���f���`��^�C�v]</tt>�́A\c "2" �ɐݒ肵�Ă��������D
\verbatim 
�@�@CDSphere [name] {
�@�@�@�@2;;                                                 # ���f���`��^�C�v          
�@�@�@�@���a;
�@�@�@�@�����C���C�W��; �Î~���C�W��; ���˕Ԃ�W��;;�@�@�@�@# �����V�~�����[�V�����Ɋ֌W����ގ�
�@�@}
\endverbatim
�@���Ɏ����̂́A���a \c 1 �̋����`������ł��D
\verbatim 
�@�@CDSphere sphere {
�@�@�@�@2;;                     //  type=sphere
�@�@�@�@1.0;                    //  radius
�@�@�@�@0.2; 0.5; 0.6;;         //  phmaterial
�@�@}
\endverbatim
<br>

\b 3.4.3.�@ \b ������ <br>
�@�����̂��`����ɂ́A<tt><�f�[�^�^></tt>�� \c "CDBox" �ɁA<tt>[���f���`��^�C�v]</tt>�́A\c "3" �ɐݒ肵�Ă��������D
\verbatim 
�@�@CDBox [name] {
�@�@�@�@3;;                                                 # ���f���`��^�C�v          
�@�@�@�@�����̃T�C�Y;
�@�@�@�@�����C���C�W��; �Î~���C�W��; ���˕Ԃ�W��;;�@�@�@�@# �����V�~�����[�V�����Ɋ֌W����ގ�
�@�@}
\endverbatim
�@���Ɏ����̂́A\c X �����̃T�C�Y�� \c 1.0�AY �����̃T�C�Y�� \c 2.0�A\c Z �����̃T�C�Y�� \c 3.0 �̒����̂��`������ł��D
\verbatim 
    CDBox box {
�@�@�@�@3;;                     //  type=box
�@�@�@�@1.0; 2.0; 3.0;;         //  boxsize
�@�@�@�@0.2; 0.5; 0.6;;         //  phmaterial
�@�@}	
\endverbatim
<br><br>
<div align="right">
<a href="#pageMoreTutoFISdk">top of page</a>
</div> 


*/
