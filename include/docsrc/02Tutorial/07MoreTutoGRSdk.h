/** \page pageMoreTutoGRSdk	�O���t�B�b�N�XSDK �`���[�g���A��

<br>
\section contentsMoreTutoGRSdk	�� Contents ��
�@�@ \ref moreTutoGRSdk						<br>
�@�@�@�@ \ref moreTutoCreateGRSdk			<br>
�@�@ \ref moreTutoGRRender					<br>
�@�@�@�@ \ref moreTutoCreateGRRender		<br>
�@�@ \ref moreTutoGRDevice					<br>
�@�@�@�@ \ref moreTutoCreateGRDevice		<br>
�@�@�@�@ \ref moreTutoInitGRDevice			<br>
�@�@�@�@ \ref moreTutoSetGRDevice			<br>
�@�@ \ref moreTutoTransMatrix				<br>
�@�@�@�@ \ref moreTutoLookAt				<br>
�@�@�@�@ \ref moreTutoMultModelMatrix		<br>
�@�@�@�@ 
<br><br>
<hr>

<!-------------------- 1�D�@�O���t�B�b�N�XSDK ----------------------------------------->
\section moreTutoGRSdk		1�D�@�O���t�B�b�N�XSDK
�͂��߂ɁA�O���t�B�b�N�X�ɕK�v�ƂȂ�SDK��p�ӂ��܂��D

\subsection moreTutoCreateGRSdk		1.1�D�@�O���t�B�b�N�XSDK�̍쐬
�O���t�B�b�N�XSDK���쐬���܂��D
\verbatim
    GRSdkIf* grSdk = CreateGRSdk();
\endverbatim
<br><br>
<div align="right">
<a href="#pageMoreTutoGRSdk">top of page</a>
</div> 
<hr>

<!-------------------- 2�D�@�f�o�b�O��񃌃��_���[ ------------------------------------>
\section moreTutoGRRender		2�D�@�f�o�b�O��񃌃��_���[
�O���t�B�b�N�XSDK����A�f�o�b�O��񃌃��_���[���쐬���܂��D

\subsection moreTutoCreateGRRender		2.1�D�@�f�o�b�O��񃌃��_���[�̍쐬
�f�o�b�O��񃌃��_���[���쐬���܂��D
\verbatim
    GRDebugRenderIf* render = grSdk->CreateDebugRender();
\endverbatim
<br><br>
<div align="right">
<a href="#pageMoreTutoGRSdk">top of page</a>
</div> 
<hr>

<!-------------------- 3�D�@�����_���[�p�f�o�C�X ------------------------------------>
\section moreTutoGRDevice		3�D�@�����_���[�p�f�o�C�X
�����_�����O�p�f�o�C�X�Ƃ��āAOpenGL �܂��� Direct 3D ��I���ł��܂��D
�����������_�ł� Direct 3D �� API ����������Ă��܂���̂ŕ`�悳��܂���D
����Ή�����\��ł��D�f�o�C�X�́AOpenGL ��I�����ĉ������D

\subsection moreTutoCreateGRDevice		3.1�D�@�����_���[�p�f�o�C�X�̍쐬
OpenGL�����_���[�p�f�o�C�X���쐬���܂��D
\verbatim
    GRDeviceGLIf* grDevice = grSdk->CreateDeviceGL(window);
\endverbatim
<br>

\subsection moreTutoInitGRDevice		3.2�D�@�����_���[�p�f�o�C�X�̏�����
�����_���[�p�f�o�C�X�̏��������s���܂��D<br>
���C�e�B���O�A�f�v�X�e�X�g�A�@���̐��K���Ȃǂ�L���ɂ��܂��D
\verbatim
    grDevice->Init();
\endverbatim
<br>


\subsection moreTutoSetGRDevice		3.3�D�@�����_���[�p�f�o�C�X�̐ݒ�
�����_���[�p�f�o�C�X��ݒ肵�܂��D
\verbatim
    render->SetDevice(grDevice);	// �f�o�C�X�̐ݒ�
\endverbatim
<br><br>
<div align="right">
<a href="#pageMoreTutoGRSdk">top of page</a>
</div> 
<hr>

<!-------------------- 4�D�@�ϊ��s��  ------------------------------->
\section moreTutoTransMatrix		4�D�@�ϊ��s��
Springhead2 �ł́A���_�s��A���f���s��A�ˉe�s����L������s�������Ŏ����Ă��܂��D<br>
�����̍s����`���邱�Ƃɂ���āA�Ӑ}����ϊ����s�����Ƃ��ł��܂��D

\subsection moreTutoLookAt		4.1�D�@���_�E�Q�Ɠ_�̈ʒu�̎w��
���_�ƎQ�Ɠ_�̈ʒu��ݒ�ł��܂��D�ȉ��̎菇�ɏ]���ĉ������D
\verbatim
    Affinef view;                                                            // �A�t�B���s��
	view.Pos() = Vec3f(0.0, 15.0, 15.0);                                // ���_
	view.LookAtGL(Vec3f(0.0, 0.0, 0.0), Vec3f(0.0, 1.0, 0.0));          // �Q�Ɠ_�A������x�N�g��
	view = view.inv();                                                  // �t�s������߂�
	render->SetViewMatrix(view);                                        // ���_�s��֓o�^
\endverbatim
<br>

\subsection moreTutoMultModelMatrix		4.2�D�@���f���s���������
���݂̍s��ɏ�Z����s����w�肵�܂��D
\verbatim
    render->MultModelMatrix(afw);
\endverbatim
<br>
<div align="right">
<a href="#pageMoreTutoGRSdk">top of page</a>
</div> 




*/

