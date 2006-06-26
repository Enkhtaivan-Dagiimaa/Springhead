/** \page pageGraphics �O���t�B�b�N�XSDK


\contents


<!-------------------- �O���t�B�b�N�XSDK�̃I�u�W�F�N�g�̃C���[�W ----------------------------------------->
\section sec_GRSdkImage �O���t�B�b�N�XSDK�̃I�u�W�F�N�g�̃C���[�W
1�̃O���t�B�b�N�X SDK�iGRSdk�j�́C�������̃V�[���iGRScene�j�������Ƃ��ł��C
�V�[���ɂ͍��W�n�iFrame�j�̃c���[�̒��ɁC���b�V���iGRMesh�j�C�����iGRLight�j�C�ގ��iGRMaterial�j
�Ȃǂ�����܂��D
�O���t�B�b�N�X SDK ���̃I�u�W�F�N�g�̎Q�Ɗ֌W�̗��}�Ɏ����܂��D
<br><br>
	\image html GRSdkScene.png
<br><br>
<div align="right">
<a href="#pageGraphics">top of page</a>
</div> 
<hr>

<!-------------------- �O���t�B�b�N�XSDK ----------------------------------------------------------------->
\section sec_GRSdk	�O���t�B�b�N�XSDK
�͂��߂ɁC�O���t�B�b�N�X�ɕK�v�ƂȂ�SDK��p�ӂ��܂��D

\subsection subsec_CreateGRSdk	�O���t�B�b�N�XSDK�̍쐬
�O���t�B�b�N�XSDK���쐬���܂��D
\verbatim
    GRSdkIf* grSdk = CreateGRSdk();
\endverbatim
<br><br>
<div align="right">
<a href="#pageGraphics">top of page</a>
</div> 
<hr>



<!-------------------- �f�o�b�O��񃌃��_���[ ------------------------------------------------------------>
\section sec_GRRender �f�o�b�O��񃌃��_���[
�O���t�B�b�N�XSDK����C�f�o�b�O��񃌃��_���[���쐬���܂��D

\subsection subsec_CreateGRRender	�f�o�b�O��񃌃��_���[�̍쐬
�f�o�b�O��񃌃��_���[���쐬���܂��D
\verbatim
    GRDebugRenderIf* render = grSdk->CreateDebugRender();
\endverbatim
<br><br>
<div align="right">
<a href="#pageGraphics">top of page</a>
</div> 
<hr>

<!-------------------- �����_���[�p�f�o�C�X -------------------------------------------------------------->
\section sec_GRDevice	�����_���[�p�f�o�C�X
�����_�����O�p�f�o�C�X�Ƃ��āCOpenGL �܂��� Direct 3D ��I���ł��܂��D
�����������_�ł� Direct 3D �� API ����������Ă��܂���̂ŕ`�悳��܂���D
����Ή�����\��ł��D�f�o�C�X�́COpenGL ��I�����ĉ������D

\subsection subsec_CreateGRDevice	�����_���[�p�f�o�C�X�̍쐬
OpenGL�����_���[�p�f�o�C�X���쐬���܂��D
\verbatim
    GRDeviceGLIf* grDevice = grSdk->CreateDeviceGL(window);
\endverbatim
<br>

\subsection subsec_InitGRDevice		�����_���[�p�f�o�C�X�̏�����
�����_���[�p�f�o�C�X�̏��������s���܂��D<br>
���C�e�B���O�C�f�v�X�e�X�g�C�@���̐��K���Ȃǂ�L���ɂ��܂��D
\verbatim
    grDevice->Init();
\endverbatim
<br>


\subsection subsec_SetGRDevice		�����_���[�p�f�o�C�X�̐ݒ�
�����_���[�p�f�o�C�X��ݒ肵�܂��D
\verbatim
    render->SetDevice(grDevice);	// �f�o�C�X�̐ݒ�
\endverbatim
<br><br>
<div align="right">
<a href="#pageGraphics">top of page</a>
</div> 
<hr>

<!-------------------- ���W�ϊ� -------------------------------------------------------------------------->
\section sec_TransMatrix	�ϊ��s��
Springhead2 �ł́C���_�s��C���f���s��C�ˉe�s����L������s�������Ŏ����Ă��܂��D<br>
�����̍s����`���邱�Ƃɂ���āC�Ӑ}����ϊ����s�����Ƃ��ł��܂��D

\subsection subsec_LookAt	���_�E�Q�Ɠ_�̈ʒu�̎w��
���_�ƎQ�Ɠ_�̈ʒu��ݒ�ł��܂��D�ȉ��̎菇�ɏ]���ĉ������D
\verbatim
    Affinef view;                                                            // �A�t�B���s��
	view.Pos() = Vec3f(0.0, 15.0, 15.0);                                // ���_
	view.LookAtGL(Vec3f(0.0, 0.0, 0.0), Vec3f(0.0, 1.0, 0.0));          // �Q�Ɠ_�C������x�N�g��
	view = view.inv();                                                  // �t�s������߂�
	render->SetViewMatrix(view);                                        // ���_�s��֓o�^
\endverbatim
<br>

\subsection subsec_MultModelMatrix		���f���s���������
���݂̍s��ɏ�Z����s����w�肵�܂��D
\verbatim
    render->MultModelMatrix(afw);
\endverbatim
<br><br>
<div align="right">
<a href="#pageGraphics">top of page</a>
</div> 




*/
