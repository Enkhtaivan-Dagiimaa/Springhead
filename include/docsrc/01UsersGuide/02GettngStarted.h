/**
\page pageGettingStarted	�͂��߂Ă݂悤
<hr>
<br>
&nbsp; \ref secRequirement<br>
&nbsp; \ref secDirectory<br>
&nbsp; \ref secInstall<br>
&nbsp;&nbsp; \ref secVcInstall<br>
&nbsp;&nbsp; \ref secCygwinInstall<br>
&nbsp;&nbsp; \ref secLinuxInstall<br>
&nbsp;&nbsp; \ref secMacInstall<br>
&nbsp; \ref secTest<br>
&nbsp;&nbsp; \ref secVcTest<br>
&nbsp;&nbsp; \ref secCygwinTest<br>
&nbsp;&nbsp; \ref secLinuxTest<br>
&nbsp;&nbsp; \ref secMacTest<br>
<br>
<hr>

\section secRequirement	1. �����
�{�v���O�����͈ȉ������ꂩ�̊��œ��삵�܂��D<br>
          <td width="64%"> 
            <table width="100%" border="0" cellspacing="1" cellpadding="2" bgcolor="#305C78">
              <tr> 
                <td> 
                  <table width="100%" border="0" cellspacing="1" cellpadding="1" bgcolor="#FFFFFF">
                    <tr> 
                      <td> 
                        <p><br> 
                        	&nbsp;&nbsp;&nbsp; \ref secVcInstall <br>
                        	&nbsp;&nbsp;&nbsp; \ref secCygwinInstall <br>
                        	&nbsp;&nbsp;&nbsp; \ref secLinuxInstall <br>
                        	&nbsp;&nbsp;&nbsp; \ref secMacInstall <br><br>
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



\section secDirectory	2. �f�B���N�g���\��
�f�B���N�g���\���͈ȉ��̒ʂ�ł��D
  \image html directory.jpg
<br><br>
<hr>



\section secInstall	3. �C���X�g�[������r���h�܂�
�\�[�X�t�@�C������R���p�C������ꍇ�́C�\�[�X�t�@�C����K���ȏꏊ�ɒu���ĉ������D<br>
�����ł͉��� \c C:\\Home\\C\\Project\\Springhead2 �Ƃ��܂��D<br>
���L�ɂ��ꂼ��̊��ł̍\�z���@���L�ڂ��Ă��܂��D
  
\subsection secVcInstall	(1) Visual C++ .NET 2003 + DirectX SDK 9.0
\htmlonly
<strong>
<font color="gray">
�@���@�����_�ł�DirectX�ɂ͑Ή����Ă��炸�C���X�g�[���͕s�v�ł��D<br>
�@�@�@(1-2)�̃C���X�g�[���C����� (1-4)��(v)�C(vi)�ł̃p�X�̐ݒ�͕K�v����܂���D
</font>
</strong>
\endhtmlonly
<br>

\par �@�@(1-1) �@ Visual C++ .NET 2003 �̃C���X�g�[��
	<tt>Visual C++ .NET 2003</tt> ���C���X�g�[�����ĉ������D<br><br>

\par �@�@(1-2) �@ DirectX SDK 9.0 �̃C���X�g�[��
	<tt>DirectX SDK 9.0</tt> ���C���X�g�[�����ĉ������D
	<tt>Microsoft</tt> �̃_�E�����[�h�T�C�g�����ɓ���邱�Ƃ��ł��܂��D
	�C���X�g�[����� \c C:\\DXSDK �Ƃ��ĉ������D<br><br>

\par �@�@(1-3) �@ ���ϐ��̐ݒ�
	\c Springhead2 �̊��ϐ������̎菇�Őݒ肵�ĉ������D
	<ol type="i">
 		<li>�u�}�C�R���s���[�^���E�N���b�N �� �ڍאݒ�^�u �� ���ϐ��v��I�����܂��D</li>
		<li> ���ϐ���ʂŁC�V�X�e�����ϐ��i���邢�̓��[�U���ϐ��j�́u�V�K�v�{�^���������C�ȉ���ݒ肵�܂��D</li>
  	</ol>
  	\image html setenv.jpg

\par �@�@(1-4) �@ Visual C++ �J�����̐ݒ�COpenGL ����� DirectX �ւ̃p�X�ݒ�
	<tt>Springhead2</tt> �̊J�����n�߂�O�ɁC<tt>Visual C++</tt> �̊��ݒ���s���K�v������܂��D
	���̊��ݒ�̓v���W�F�N�g���ɕۑ������̂ł͂Ȃ��C<tt>Visual C++</tt> �J�����̐ݒ�Ƃ���
	�ۑ�����܂��̂ŁC��x�s���΁C����s���K�v�͂���܂���D
	�܂��C<tt>DirectX SDK</tt> ���C���X�g�[�������i�K�ŁC�����I�ɁC<tt>DirectX SDK</tt> �ւ̃p�X��
	�ݒ肳��܂����C�ݒ肳��Ȃ��ꍇ�͈ȉ��̎菇 �D�C�E �ɏ]���C�p�X��ݒ肵�ĉ������D<br>
	<ol type="i">
		<li> <tt>Visual C++</tt> ���N�����܂��D</li>
		<li> ���j���[���� <tt>[�c�[��]�|[�I�v�V����]</tt> ��I�����āC�I�v�V�����_�C�A���O���J���܂��D</li>
		<li> <tt>[�v���W�F�N�g]�|[VC++�f�B���N�g��]</tt> ���N���b�N���܂��D</li>
		<li> <tt>[�f�B���N�g����\������v���W�F�N�g]</tt> ���� <tt>[���s�\�t�@�C��]</tt> ��I�����C
			<tt>OpenGL</tt> �̃w�b�_�̂���f�B���N�g����ǉ����܂��D
			 \code
�@[���s�\�t�@�C��]�ɒǉ��@�@�@�@�@�F$(SPRINGHEAD2)\bin\win32
		 	 \endcode
			 �܂��C����ȊO�ɂ́C\htmlonly <tt>$(SPRINGHEAD2)\bin\win32\glut32.dll</tt> \endhtmlonly ��
			 \c C:\\Windows �ȂǁCPATH �̒ʂ����t�H���_�ɃR�s�[���邩�C
			 \htmlonly <tt>$(SPRINGHEAD2)\bin\win32 </tt> \endhtmlonly ��
			 ���ϐ� PATH �ɒǉ�������@������܂��D
		</li>
		<li> ���l�ɁC <tt>[�f�B���N�g����\������v���W�F�N�g]</tt> ���� <tt>[�C���N���[�h�t�@�C��]</tt>
			 ��I�����C<tt>DirectX SDK</tt> �̃C���N���[�h�t�@�C���փp�X��ʂ��܂��D
			 \code
�@[�C���N���[�h�t�@�C��]�ɒǉ��@�@�@�FC:\DXSDK\Include
			 \endcode
		</li>
		<li> ���l�ɁC <tt>[�f�B���N�g����\������v���W�F�N�g]</tt> ���� 
			 <tt>[���C�u�����t�@�C��]</tt> ��I�����C<tt>DirectX SDK</tt> �̃��C�u�����t�@�C���փp�X��ʂ��܂��D</tt>
			 \code
�@[���C�u�����t�@�C��]�ɒǉ��@�@�@�@�FC:\DXSDK\Lib
			 \endcode	   
		</li>
		\image html vcpath.jpg
	</ol>
		
\par �@�@(1-5) �@ VC �Ńr���h
	���s�������T���v���v���O�������J���C���j���[�� <tt>[�r���h]�|[�\�����[�V�����̃r���h]</tt> ���w�肵�܂��D
<br>



\subsection secCygwinInstall (2) Cygwin
\par �@�@(2-1) �@ Cygwin �Ńr���h
�\�[�X�t�@�C���̃g�b�v�f�B���N�g���ɂāC<tt>make</tt>�R�}���h�����s���܂��D<br>
���C�u�����̍\�z�C����уe�X�g�p�v���O�����C�T���v���v���O�����̃R���p�C���������I�ɍs���܂��D
\code
�@% make
\endcode
<br>



\subsection secLinuxInstall	(3) RedHat Linux 9
\par �@�@(3-1) �@ RedHat Linux 9 �Ńr���h
�\�[�X�t�@�C���̃g�b�v�f�B���N�g���ɂāC<tt>make</tt>�R�}���h�����s���܂��D<br>
���C�u�����̍\�z�C����уe�X�g�p�v���O�����C�T���v���v���O�����̃R���p�C���������I�ɍs���܂��D
\code
�@% make
\endcode
<br>



\subsection  secMacInstall	(4) Mac OS X�iPowerPC�CAquq�j
\par �@�@(4-1) �@ Mac OS X �Ńr���h
�\�[�X�t�@�C���̃g�b�v�f�B���N�g���ɂāC<tt>make</tt>�R�}���h�����s���܂��D<br>
���C�u�����̍\�z�C����уe�X�g�p�v���O�����C�T���v���v���O�����̃R���p�C���������I�ɍs���܂��D
\code
�@% make
\endcode
<br>
<hr>



\section secTest	4. �T���v���𓮂���
�T���v���E�v���O�����𓮂����Ă݂܂��傤�D<br>
\c Springhead2 �ł́C����\�ȃe�X�g�p�v���O���� \c tests ����уT���v���v���O����
\c Samples ��p�ӂ��Ă��܂��i�Q�� �F \ref directory)�D
�����ł͗�Ƃ��āC�e�X�g�p�v���O���� \c tests ��
<a href="../../src/html/GRSimple_2main_8cpp-example.html">GRSimple</a>
�����s������@���Љ�܂��D<br>

\subsection secVcTest	(1) Visual C++ .NET 2003 + DirectX SDK 9.0
<ol>
	<li> \c GRSimple�f�B���N�g���ɂ���\�����[�V�����t�@�C�����J���܂��D
	<li> �\�����[�V�����G�N�X�v���[���ŁC\c GRSimple ���X�^�[�g�A�b�v�v���W�F�N�g��
		 �ݒ肳��Ă��邱�Ƃ��m�F���܂��D</li>
	<li> �A�v���P�[�V���������s���܂��D</li>
</ol>
<br>

\subsection secCygwinTest	(2) Cygwin
<ol>
	<li> \c GRSimple �f�B���N�g���ֈړ����C\c GRSimple.exe �����邱�Ƃ��m�F���܂��D</li>
	<li> �ȉ��̃R�}���h�����s���C�A�v���P�[�V�������N�����܂��D<br>
 		\code
�@% ./GRSimple.exe
 		\endcode
	</li>
</ol>
<br>

\subsection secLinuxTest	(3) RedHat Linux 9
<ol>
	<li> \c GRSimple �f�B���N�g���ֈړ����C\c GRSimple.exe �����邱�Ƃ��m�F���܂��D</li>
	<li> �ȉ��̃R�}���h�����s���C�A�v���P�[�V�������N�����܂��D<br>
 		\code
�@% ./GRSimple.exe
 		\endcode
	</li>
</ol>
<br>

\subsection secMacTest	(4) Mac OS X�iPowerPC�CAquq�j
<ol>
	<li> \c GRSimple �f�B���N�g���ֈړ����C\c GRSimple.exe �����邱�Ƃ��m�F���܂��D</li>
	<li> �ȉ��̃R�}���h�����s���C�A�v���P�[�V�������N�����܂��D<br>
 		\code
�@% ./GRSimple.exe
 		\endcode
	</li>
</ol>
<br>


*/
