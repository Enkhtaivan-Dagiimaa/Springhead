/**
\page pageGettingStarted	�Ƃ肠�����g���Ă݂悤



\section contentsGettingStarted	�� Contents ��
 &nbsp;&nbsp;&nbsp; \ref requirement <br>
 &nbsp;&nbsp;&nbsp; \ref directory   <br>
 &nbsp;&nbsp;&nbsp; \ref install     <br>
 &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; \ref vc_install <br>
 &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; \ref cygwin_install <br>
 &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; \ref linux_install <br>
 &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; \ref mac_install <br>
 &nbsp;&nbsp;&nbsp; \ref test <br>
<br><br>
<hr>



\section requirement	1. �����
�{�v���O�����͈ȉ������ꂩ�̊��œ��삵�܂��B<br>
���̊��͓�����m�F�������̂ł��B���̊��ł����삷��ꍇ������܂��B</li>
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



\section directory	2. �f�B���N�g���\��
�f�B���N�g���\���͈ȉ��̒ʂ�ł��B
  <img src="../../include/docsrc/01UsersGuide/directory.jpg" width="486" height="351">
<br><br>
<hr>



\section install	3. �C���X�g�[������r���h�܂�
�\�[�X�t�@�C������R���p�C������ꍇ�́A�\�[�X�t�@�C����K���ȏꏊ�ɒu���ĉ������B<br>
�����ł͉��� C:��Home��C��Project��Springhead2�Ƃ��܂��B<br>
���L�ɂ��ꂼ��̊��ł̍\�z���@���L�ڂ��Ă��܂��B
 
\subsection vc_install	(�P) Visual C++ .NET 2003 + DirectX SDK 9.0
<ol>
   <li><strong>Visual C++ .NET 2003 �̃C���X�g�[�� </strong><br>
  				 Visual C++ .NET 2003 ���C���X�g�[�����ĉ������B<br>
   </li><br>
<!--
	�����_�ł�DirectX�ɂ͑Ή����Ă��炸�C���X�g�[���͕s�v�ł��D<br>
   <li><strong> DirectX SDK 9.0 �̃C���X�g�[�� </strong><br>
  				 DirectX SDK 9.0 ���C���X�g�[�����ĉ������B<br>
  				 Microsoft�̃_�E�����[�h�T�C�g�����ɓ���邱�Ƃ��ł��܂��B<br>
  				 �C���X�g�[����� C:��DXSDK �Ƃ��ĉ������B<br>
   </li><br>
-->
   <li><strong> ���ϐ��̐ݒ� </strong><br>
  				 <ol type="i">
  				    <li>�u�}�C�R���s���[�^���E�N���b�N �� �ڍאݒ�^�u �� ���ϐ��v��I�����܂��B</li>
  				    <li> ���ϐ���ʂŁA�V�X�e�����ϐ�(���邢�̓��[�U���ϐ�)�́u�V�K�v�{�^���������A�ȉ���ݒ肷��B</li>
  				       <ul type="circle">
  				         <li>&nbsp; �ϐ��� �F SPRINGHEAD2 </li>
  				         <li>&nbsp; �ϐ��l �F C:��Home��C��Project��Springhead2 </li>
  				       </ul>
  				    </li>
  				 </ol>
  				 <img src="../../include/docsrc/01UsersGuide/setenv.jpg" width="423" height="154"><br>
   </li>
   <li><strong> GLUT�̐ݒ� </strong><br>
				\$(SPRINGHEAD2)/bin/win32/glut32.dll ��C:\Windows �Ȃ�
				PATH�̒ʂ����t�H���_�ɃR�s�[���邩�C
				\$(SPRINGHEAD2)/bin/win32 �����ϐ�PATH�ɒǉ����Ă��������D
   </li>
   <li><strong> Visual C++ �J�����̐ݒ� </strong><br>
                Springhead2�̊J�����n�߂�O�ɁAVisual C++ �̊��ݒ���s���K�v������܂��B
				���̊��ݒ�̓v���W�F�N�g���ɕۑ������̂ł͂Ȃ��AVisual C++ �J�����̐ݒ�Ƃ��ĕۑ�����܂��̂ŁA
				��x�s���΁A����s���΍���s���K�v�͂���܂���B �܂��ADirectX SDK���C���X�g�[�������i�K�ŁA�����I�ɁA
				DirectX SDK �ւ̃p�X���ݒ肳��܂����A�ݒ肳��Ȃ��ꍇ�͈ȉ��̎菇(vi)�A(vii)�ɏ]���A�p�X��ݒ肵�ĉ������B
				<ol type="i">
				   <li> Visual C++ ���N�����܂��B</li>
				   <li> ���j���[����[�c�[��]�|[�I�v�V����]��I�����āA�I�v�V�����_�C�A���O���J���܂��B</li>
				   <li> [�v���W�F�N�g]�|[VC++�f�B���N�g��]���N���b�N���܂��B</li>
				   <li> [�f�B���N�g����\������v���W�F�N�g]����[���s�\�t�@�C��]��I�����܂��B</li>
				   <li> [�V�����s]�{�^���������āAOpenGL �̃w�b�_�̂���f�B���N�g�� ��(Springhead2)��bin��win32 ��ǉ����܂��B</li>
				   <li> ���l�ɁA [�f�B���N�g����\������v���W�F�N�g]����[�C���N���[�h�t�@�C��]��I�����AC:��DXSDK��Include ��ǉ����܂��B</li>
				   <li> ���l�ɁA [�f�B���N�g����\������v���W�F�N�g]����[���C�u�����t�@�C��]��I�����AC:��DXSDK��Lib ��ǉ����܂��B</li>
					<img src="../../include/docsrc/01UsersGuide/vcpath.jpg" width="709" height="372"><br>
				</ol>
   </li><br>
   <li><strong>�r���h </strong><br>
   				���s�������T���v���v���O�������J���A���j���[��[�r���h]�|[�\�����[�V�����̃r���h]���w�肵�܂��B
   </li><br>
</ol>
 
\subsection cygwin_install	(�Q) Cygwin
<ol>
 	<li>�\�[�X�t�@�C���̃g�b�v�f�B���N�g���ɂāAmake�R�}���h�����s���܂��B<br>
 		���C�u�����̍\�z�A����уe�X�g�p�v���O�����A�T���v���v���O�����̃R���p�C���������I�ɍs���܂��B<br>
 		&nbsp;&nbsp;&nbsp; % make
 	</li>			
</ol>

\subsection linux_install	(�R) RedHat Linux 9
<ol>
 	<li>�\�[�X�t�@�C���̃g�b�v�f�B���N�g���ɂāAmake�R�}���h�����s���܂��B<br>
 		���C�u�����̍\�z�A����уe�X�g�p�v���O�����A�T���v���v���O�����̃R���p�C���������I�ɍs���܂��B<br>
 		&nbsp;&nbsp;&nbsp; % make
 	</li>			
</ol>

\subsection mac_install	(�S) Mac OS X�iPowerPC�AAquq�j
<ol>
 	<li>�\�[�X�t�@�C���̃g�b�v�f�B���N�g���ɂāAmake�R�}���h�����s���܂��B<br>
 		���C�u�����̍\�z�A����уe�X�g�p�v���O�����A�T���v���v���O�����̃R���p�C���������I�ɍs���܂��B<br>
 		&nbsp;&nbsp;&nbsp; % make
 	</li>			
</ol>
<br>
<hr>


\section test	4. �T���v���𓮂���
�T���v���E�v���O�����𓮂����Ă݂܂��傤�B<br>
����\�ȃe�X�g�p�v���O��������уT���v���v���O�����͈ȉ��̃f�B���N�g���ɂ���܂��i�Q�� \ref directory)�B
<ul type="circle"><li> Springhead2��src��tests
				  <li> Springhead2��src��Samples</ul> 
�����ł́A��Ƃ��āASpringhead2��src��tests��Graphics��GRSimple �����s������@�������܂��B<br>

\subsection vc_test	(�P) Visual C++ .NET 2003 + DirectX SDK 9.0
<ol>
	<li> GRSimple�f�B���N�g���ɂ���\�����[�V�����t�@�C�����J���܂��B
	<li> �\�����[�V�����G�N�X�v���[���ŁAGRSimple ���X�^�[�g�A�b�v�v���W�F�N�g�ɐݒ肳��Ă��邱�Ƃ��m�F���܂��B</li>
	<li> �A�v���P�[�V���������s���܂��B</li>
</ol>

\subsection cygwin_test	(�Q) Cygwin
<ol>
	<li> GRSimple �f�B���N�g���ֈړ����AGRSimple.exe �����邱�Ƃ��m�F���܂��B</li>
	<li> �ȉ��̃R�}���h�����s���A�A�v���P�[�V�������N�����܂��B<br>
		 &nbsp;&nbsp;&nbsp; %  �D/GRSimple.exe
	</li>
</ol>

\subsection linux_test	(�R) RedHat Linux 9
<ol>
	<li> GRSimple �f�B���N�g���ֈړ����AGRSimple.exe �����邱�Ƃ��m�F���܂��B</li>
	<li> �ȉ��̃R�}���h�����s���A�A�v���P�[�V�������N�����܂��B<br>
		 &nbsp;&nbsp;&nbsp; %  �D/GRSimple.exe
	</li>
</ol>

\subsection mac_test	(�S) Mac OS X�iPowerPC�AAquq�j
<ol>
	<li> GRSimple �f�B���N�g���ֈړ����AGRSimple.exe �����邱�Ƃ��m�F���܂��B</li>
	<li> �ȉ��̃R�}���h�����s���A�A�v���P�[�V�������N�����܂��B<br>
		 &nbsp;&nbsp;&nbsp; %  �D/GRSimple.exe
	</li>
</ol>
<br>


*/
