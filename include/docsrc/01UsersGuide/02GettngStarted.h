/**
\page pageGettingStarted	�Ƃ肠�����g���Ă݂悤

\section contentsGettingStarted	�� Contents ��
�@�@ \ref requirement				<br>
�@�@ \ref directory					<br>
�@�@ \ref install					<br>
�@�@�@�@�@ \ref vc_install			<br>
�@�@�@�@�@ \ref cygwin_install		<br>
�@�@�@�@�@ \ref linux_install		<br>
�@�@�@�@�@ \ref mac_install			<br>
�@�@ \ref test 						<br>
�@�@�@�@�@ \ref vc_test				<br>
�@�@�@�@�@ \ref cygwin_test			<br>
�@�@�@�@�@ \ref linux_test			<br>
�@�@�@�@�@ \ref mac_install			<br>
<br><br>
<hr>



\section requirement	1�D�@�����
�{�v���O�����͈ȉ������ꂩ�̊��œ��삵�܂��D<br>
���̊��͓�����m�F�������̂ł��D���̊��ł����삷��ꍇ������܂��D</li>
          <td width="64%"> 
            <table width="100%" border="0" cellspacing="1" cellpadding="2" bgcolor="#305C78">
              <tr> 
                <td> 
                  <table width="100%" border="0" cellspacing="1" cellpadding="1" bgcolor="#FFFFFF">
                    <tr> 
                      <td> 
                        <p><br> 
                        	&nbsp;&nbsp;&nbsp; \ref vc_install <br>
                        	&nbsp;&nbsp;&nbsp; \ref cygwin_install <br>
                        	&nbsp;&nbsp;&nbsp; \ref linux_install <br>
                        	&nbsp;&nbsp;&nbsp; \ref mac_install <br><br>
                        </p>
                      </td>
                    </tr>
                  </table>
                </td>
              </tr>
            </table>
          </td>
          <br><br>
<hr>



\section directory	2�D�@�f�B���N�g���\��
�f�B���N�g���\���͈ȉ��̒ʂ�ł��D
  \image html directory.jpg
<br><br>
<hr>



\section install	3�D�@�C���X�g�[������r���h�܂�
�\�[�X�t�@�C������R���p�C������ꍇ�́A�\�[�X�t�@�C����K���ȏꏊ�ɒu���ĉ������D<br>
�����ł͉��� C:/Home/C/Project/Springhead2 �Ƃ��܂��D<br>
���L�ɂ��ꂼ��̊��ł̍\�z���@���L�ڂ��Ă��܂��D
  
\subsection vc_install	(�P) Visual C++ .NET 2003 + DirectX SDK 9.0
\htmlonly
<strong>
<font color="gray">
�@���@�����_�ł�DirectX�ɂ͑Ή����Ă��炸�C���X�g�[���͕s�v�ł��D<br>
�@�@�@(1-2)�̃C���X�g�[���A����� (1-4)��(v)�A(vi)�ł̃p�X�̐ݒ�͕K�v����܂���D
</font>
</strong>
\endhtmlonly
<br>

\par �@�@(1-1) �@ Visual C++ .NET 2003 �̃C���X�g�[��
	Visual C++ .NET 2003 ���C���X�g�[�����ĉ������D					<br><br>

\par �@�@(1-2) �@ DirectX SDK 9.0 �̃C���X�g�[��
	DirectX SDK 9.0 ���C���X�g�[�����ĉ������D							
	Microsoft �̃_�E�����[�h�T�C�g�����ɓ���邱�Ƃ��ł��܂��D		
	�C���X�g�[����� C:/DXSDK �Ƃ��ĉ������D						<br><br>

\par �@�@(1-3) �@ ���ϐ��̐ݒ�
	Springhead2 �̊��ϐ���ݒ肵�ĉ������D
	<ol type="i">
 		<li>�u�}�C�R���s���[�^���E�N���b�N �� �ڍאݒ�^�u �� ���ϐ��v��I�����܂��D</li>
  		<li> ���ϐ���ʂŁA�V�X�e�����ϐ�(���邢�̓��[�U���ϐ�)�́u�V�K�v�{�^���������A�ȉ���ݒ肵�܂��D</li>
  	</ol>
  	\image html setenv.jpg

\par �@�@(1-4) �@ Visual C++ �J�����̐ݒ�AOpenGL ����� DirectX �ւ̃p�X�ݒ�
	Springhead2 �̊J�����n�߂�O�ɁAVisual C++ �̊��ݒ���s���K�v������܂��D							
	���̊��ݒ�̓v���W�F�N�g���ɕۑ������̂ł͂Ȃ��AVisual C++ �J�����̐ݒ�Ƃ��ĕۑ�����܂��̂ŁA
	��x�s���΁A����s���΍���s���K�v�͂���܂���D 													
	�܂��ADirectX SDK ���C���X�g�[�������i�K�ŁA�����I�ɁADirectX SDK �ւ̃p�X���ݒ肳��܂����A
	�ݒ肳��Ȃ��ꍇ�͈ȉ��̎菇 (vi)�A(vii) �ɏ]���A�p�X��ݒ肵�ĉ������D<br>
	<ol type="i">
		<li> Visual C++ ���N�����܂��D</li>
		<li> ���j���[���� [�c�[��]�|[�I�v�V����] ��I�����āA�I�v�V�����_�C�A���O���J���܂��D</li>
		<li> [�v���W�F�N�g]�|[VC++�f�B���N�g��] ���N���b�N���܂��D</li>
		<li> [�f�B���N�g����\������v���W�F�N�g] ���� [���s�\�t�@�C��] ��I�����AOpenGL �̃w�b�_�̂���f�B���N�g����ǉ����܂��D
			 \code
�@[���s�\�t�@�C��]�ɒǉ��@�@�@�@�@�F$(SPRINGHEAD2)\bin\win32
		 	 \endcode
			 �܂��A����ȊO�ɂ́A\$(SPRINGHEAD2)/bin/win32/glut32.dll ��C:/Windows �ȂǁA
			 PATH �̒ʂ����t�H���_�ɃR�s�[���邩�C\$(SPRINGHEAD2)/bin/win32 �����ϐ� PATH �ɒǉ�������@������܂��D
		</li>
		<li> ���l�ɁA [�f�B���N�g����\������v���W�F�N�g] ���� [�C���N���[�h�t�@�C��] ��I�����A
	         DirectX SDK �̃C���N���[�h�t�@�C���փp�X��ʂ��܂��D
			 \code
�@[�C���N���[�h�t�@�C��]�ɒǉ��@�@�@�FC:\DXSDK\Include
			 \endcode
		</li>
		<li> ���l�ɁA [�f�B���N�g����\������v���W�F�N�g] ���� [���C�u�����t�@�C��] ��I�����A
		   	 DirectX SDK �̃��C�u�����t�@�C���փp�X��ʂ��܂��D
			 \code
�@[���C�u�����t�@�C��]�ɒǉ��@�@�@�@�FC:\DXSDK\Lib
			 \endcode	   
		</li>
		\image html vcpath.jpg
	</ol>
	
\par �@�@(1-5) �@ VC �Ńr���h
	���s�������T���v���v���O�������J���A���j���[�� [�r���h]�|[�\�����[�V�����̃r���h] ���w�肵�܂��D	

<br>



\subsection cygwin_install	(�Q) Cygwin
\par �@�@(2-1) �@ Cygwin �Ńr���h
	�\�[�X�t�@�C���̃g�b�v�f�B���N�g���ɂāAmake�R�}���h�����s���܂��D<br>
	���C�u�����̍\�z�A����уe�X�g�p�v���O�����A�T���v���v���O�����̃R���p�C���������I�ɍs���܂��D<br>
	\code
�@% make
	\endcode

<br>



\subsection linux_install	(�R) RedHat Linux 9
\par �@�@(3-1) �@ RedHat Linux 9 �Ńr���h
	�\�[�X�t�@�C���̃g�b�v�f�B���N�g���ɂāAmake�R�}���h�����s���܂��D<br>
 	���C�u�����̍\�z�A����уe�X�g�p�v���O�����A�T���v���v���O�����̃R���p�C���������I�ɍs���܂��D<br>
	\code
�@% make
	\endcode

<br>



\subsection mac_install	(�S) Mac OS X�iPowerPC�AAquq�j
\par �@�@(4-1) �@ Mac OS X �Ńr���h
	�\�[�X�t�@�C���̃g�b�v�f�B���N�g���ɂāAmake�R�}���h�����s���܂��D<br>
 	���C�u�����̍\�z�A����уe�X�g�p�v���O�����A�T���v���v���O�����̃R���p�C���������I�ɍs���܂��D<br>
 	\code
�@% make
 	\endcode

<br>
<hr>



\section test	4�D�@�T���v���𓮂���
�T���v���E�v���O�����𓮂����Ă݂܂��傤�D<br>
Springhead2�ł́A����\�ȃe�X�g�p�v���O���� tests ����уT���v���v���O���� Samples ��p�ӂ��Ă��܂��i�Q�� �F \ref directory)�D
�����ł͗�Ƃ��āA�e�X�g�p�v���O���� tests ��
<a href="../../src/html/GRSimple_2main_8cpp-example.html">GRSimple</a>
�����s������@���Љ�܂��D<br>

\subsection vc_test	(�P) Visual C++ .NET 2003 + DirectX SDK 9.0
<ol>
	<li> GRSimple�f�B���N�g���ɂ���\�����[�V�����t�@�C�����J���܂��D
	<li> �\�����[�V�����G�N�X�v���[���ŁAGRSimple ���X�^�[�g�A�b�v�v���W�F�N�g�ɐݒ肳��Ă��邱�Ƃ��m�F���܂��D</li>
	<li> �A�v���P�[�V���������s���܂��D</li>
</ol>
<br>

\subsection cygwin_test	(�Q) Cygwin
<ol>
	<li> GRSimple �f�B���N�g���ֈړ����AGRSimple.exe �����邱�Ƃ��m�F���܂��D</li>
	<li> �ȉ��̃R�}���h�����s���A�A�v���P�[�V�������N�����܂��D<br>
 		\code
�@% ./GRSimple.exe
 		\endcode
	</li>
</ol>
<br>

\subsection linux_test	(�R) RedHat Linux 9
<ol>
	<li> GRSimple �f�B���N�g���ֈړ����AGRSimple.exe �����邱�Ƃ��m�F���܂��D</li>
	<li> �ȉ��̃R�}���h�����s���A�A�v���P�[�V�������N�����܂��D<br>
 		\code
�@% ./GRSimple.exe
 		\endcode
	</li>
</ol>
<br>

\subsection mac_test	(�S) Mac OS X�iPowerPC�AAquq�j
<ol>
	<li> GRSimple �f�B���N�g���ֈړ����AGRSimple.exe �����邱�Ƃ��m�F���܂��D</li>
	<li> �ȉ��̃R�}���h�����s���A�A�v���P�[�V�������N�����܂��D<br>
 		\code
�@% ./GRSimple.exe
 		\endcode
	</li>
</ol>
<br>


*/
