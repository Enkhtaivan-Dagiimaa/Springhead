/** \page pageMoreTutoFISdk	�t�@�C�����o��SDK �`���[�g���A��

<br>
\section contentsMoreTutoFISdk	�� Contents ��
�@�@ \ref moreTutoFISdk						<br>
�@�@�@�@ \ref moreTutoCreateFISdk			<br>
�@�@ \ref moreTutoFileIO					<br>
�@�@�@�@ \ref moreTutoCreateFileX			<br>
�@�@�@�@ \ref moreTutoLoadFileX				<br>
�@�@�@�@ \ref moreTutoSaveFileX				<br>
<br><br>
<hr>

<!-------------------- 1�D�@�t�@�C�����o��SDK ----------------------------------------->
\section moreTutoFISdk		1�D�@�t�@�C�����o��SDK
�͂��߂ɁA�t�@�C�����o�͂ɕK�v�ƂȂ�SDK��p�ӂ��܂��D

\subsection moreTutoCreateFISdk		1.1�D�@�t�@�C�����o��SDK�̍쐬
�t�@�C�����o��SDK���쐬���܂��D
\verbatim
    FISdkIf* fiSdk = CreateFISdk();
\endverbatim
<br><br>
<div align="right">
<a href="#pageMoreTutoFISdk">top of page</a>
</div> 
<hr>


<!-------------------- 2�D�@X�t�@�C�� ���[�_���Z�[�o -------------------------------------------->
\section  moreTutoFileIO		2�D�@���[�_���Z�[�o
�C�ӂ�X�t�@�C���̓ǂݍ��݁A�����o�����\�ȁA���[�_���Z�[�o��Springhead2�ɂ͗p�ӂ���Ă��܂��D
���[�_�@�\��p����΁A�I�u�W�F�N�g�̒��_���W�╨���p�����[�^�Ȃǂ��C�ɂ����A\ref pageGraphics ��
DrawScene()���\�b�h�Ȃǂ��Ăяo�������ŕ`��ł��܂��D

\subsection moreTutoCreateFileX		2.1�D�@�t�@�C���n���h���̍쐬
�t�@�C���̓��o�͂ɂ̓t�@�C���n���h�����g���܂��D
\verbatim 
    FIFileXIf* fileX = fiSdk->CreateFileX();     // �t�@�C���n���h���̍쐬
\endverbatim
<br>

\subsection moreTutoLoadFileX		2.2�D�@X�t�@�C�����[�_�[
�w�肳�ꂽX�t�@�C�������[�h���܂��D
\verbatim 
�@�@ObjectIfs objs;�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@// �C���^�t�F�[�X�N���X�ւ̃|�C���^�z���p��
�@�@fileX->Load(objs, "test.x");                 // X�t�@�C�������[�h
\endverbatim
<br>

\subsection moreTutoSaveFileX		2.3�D�@X�t�@�C���Z�[�o�[
�w�肵�����O�ŁAX�t�@�C���̃Z�[�u���s���܂��D
\verbatim 
�@�@fileX->Save(objs, "out.x");                  // X�t�@�C���̃Z�[�u
\endverbatim
<br><br>
<div align="right">
<a href="#pageMoreTutoFISdk">top of page</a>
</div> 


*/
