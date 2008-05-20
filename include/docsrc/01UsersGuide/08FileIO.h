/** \page pageFileIO �t�@�C�����o��SDK 


\contents


�t�@�C�����o��\c SDK �́C
�t�@�C�����烍�[�h�����f�[�^����ɃV�[���O���t�̐���������C
�V�[���O���t�̃f�[�^��ǂݏo���āC�t�@�C���Ƀf�[�^��ۑ������肵�܂��D
���̐}�́C�V�[���O���t�ƃt�@�C�����o�͂̃C���[�W�ł��D
�i�t�@�C����ł̃m�[�h�̐e�q�֌W�ƃV�[���O���t�̐e�q�֌W�͕K���Ή�����킯�ł͂���܂���D�j

\image html FileScene.png


<!------------------------------ X�t�@�C�� --------------------------------------------->
\section secXFile 		X�t�@�C��
�u \c X �t�@�C�� �v�Ƃ́C�t�@�C������ <tt>.x</tt> �g���q���t�����t�@�C�����w���C  
\c Direct3D �ɂ����郂�f���f�[�^�̕W���t�H�[�}�b�g�ł��D 
\c 3D �̌`��f�[�^�C�}�e���A���C�e�N�X�`���Ȃǂ��e���v���[�g�Ƃ��Ē�`����C 
����Ƀ��[�U�ɂ���Ď��R�Ƀe���v���[�g�̒�`�̊g�����s�����Ƃ��ł��܂��D
\c Springhead2 �̃t�@�C�����o��\c SDK �ł́C���̔ėp�I�� \c X �t�@�C���ɑ΂��āC
���[�h��Z�[�u���s�����Ƃ��ł��܂��D�i���� \c VRML �Ȃǂ����[�h�ł���悤�Ɋg������\��ł��D�j
\c X �t�@�C���ł́C\c Springhead2 �Ǝ��̍\���Ŏ��R�Ƀf�[�^�^�̒�`���s���܂��D
�t�@�C���̏����́C�w ���@ �x �� �w �m�[�h�̒�` �x �ɕ�����Ă��܂��D<br>


�Ⴆ�΁C�ȉ��̂悤�ɁC�w ���@ �x �����߂��Ă��܂��D<br>
�w �m�[�h�̒�` �x�i�m�[�h�̌^���Ƃ��̃m�[�h�^�ɂǂ̂悤�ȃt�B�[���h�����邩�j
�ɂ��ẮC�w ���@ �x �ł͌��߂��Ă��܂���D

\htmlonly
<table width=100% height="50" style="border:3px double #1841a5;">
<tr><td><tt>
�@xof 0302txt 0064�@�@�@�@#�@�ŏ��̍s�͂��ꂩ��n�܂� 				<br>
 																	<br>
�@#�@�@�m�[�h�́C													<br>
�@#�@�@�@�@�^���C�m�[�h�� { �t�B�[���h�̌J��Ԃ�   �q�m�[�h }		<br>
�@#�@�@����Ȃ�D													<br>
�@PHScene scene1{													<br>
�@�@�@0.01;0;;�@�@�@�@�@�@#�@�t�B�[���h �� �l; �̌J��Ԃ�			<br>
�@�@�@1;0;-9.8;0;;�@�@�@�@#�@�l�� ���l�C������܂��̓t�B�[���h		<br>
�@�@�@PHSolid soFloor{�@�@#�@�q�m�[�h�́C�m�[�h�Ɠ���				<br>
�@�@�@�@�@(�ȗ�)													<br>
�@�@�@}																<br>
�@}																	<br>
</tt></td></tr></table>
\endhtmlonly

�Ȃ��C\c X �t�@�C���ł́C�R�����g�̊J�n�L���Ƃ��āC\c C++ �`���̃_�u���X���b�V�� \c "//" �C�܂��͓���L�� \c "#" ���g�p���܂��D
�R�����g�̊J�n�L������s�[�܂ł��C�R�����g�Ɖ��߂��܂��D

\htmlonly
<table width=100% height="50" style="border:3px double #1841a5;">
<tr><td><tt>
�@�@# This is a comment.�@�@<br>
�@�@// ������R�����g�ł��D
</tt></td></tr></table>
\endhtmlonly
<br><br>


<!------------------------------ �m�[�h�̒�` ------------------------------------------>
\section secNodeDef �m�[�h�̒�`
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

\htmlonly
<table width=100% height="50" style="border:3px double #1841a5;">
<tr><td><tt>
�@PHScene scene1{																			 <br>
�@�@�@0.01;�@�@�@�@�@�@#�@PHSceneState::timeStep											 <br>
�@�@�@0;;�@�@�@�@�@�@�@#�@PHSceneState::count�@�@�@�@ �Ō��;��PHSceneState���̏I���������D<br>
�@�@�@1;�@�@�@�@�@�@�@ #�@PHSceneDesc::ContactMode											 <br>
�@�@�@0;-9.8;0;;�@�@�@ #�@PHSceneDesc::gravity�@�@�@�@�Ō��;��PHSceneDesc���̏I���������D <br>
�@}																							 <br>
</tt></td></tr></table>
\endhtmlonly

�̂悤�ɂȂ�܂��D�N���X�̃����o�ϐ������̂܂܃t�B�[���h�ɂȂ�܂��D
�܂��C��{�N���X�́C�擪�Ƀt�B�[���h���ǉ����ꂽ�`�ɂȂ�܂��D

�ʏ�m�[�h�̈ꗗ��\ref pageNodeDefList ���Q�Ɖ������D
<br><br><br>


<!------------------------------ ����m�[�h -------------------------------------------->
\section secSpecialNode 	����m�[�h
����m�[�h�́C�ߋ��̃t�@�C���t�H�[�}�b�g�Ƃ̌݊����̂��߂̃m�[�h�ł��D
���Ƃ��΁C\c DirectX �� \c X �t�@�C���ɂ͂��Ƃ��ƁC

\htmlonly
<table width=100% height="50" style="border:3px double #1841a5;">
<tr><td><tt>
�@Frame{																<br>
�@�@�@�@FrameTransfromMatrix{ 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1; }		<br>
�@}																		<br>
</tt></td></tr></table>
\endhtmlonly

�̂悤�ȃt���[���̃m�[�h�^������܂����C\c Sprinhead2 �ɂ͑Ή�����f�B�X�N���v�^��I�u�W�F�N�g������܂���D
�����ŁC�����́C\c GRFrame �ɕϊ�����ă��[�h����܂��D
���̂悤�ȁC�Ή�����I�u�W�F�N�g���Ȃ��m�[�h������m�[�h�ł��D

����m�[�h�̈ꗗ�́C\ref pageNodeDefList ���Q�Ɖ������D
<br><br><br>


<!------------------------------ �m�[�h�̃��[�h ---------------------------------------->
\section secLoadNode			�m�[�h�̃��[�h
�f�B�X�N���v�^�ɑΉ�����m�[�h�͎��̎菇�Ő�������܂��D

<tt>
�@�@ 1�D�m�[�h�̌^�ɑΉ�����f�B�X�N���v�^�𐶐�����D						<br>
�@�@ 2�D�f�B�X�N���v�^�Ƀm�[�h�̃t�B�[���h�̓��e���L������D				<br>
�@�@ 3�D��c�m�[�h�� CreateObject() API ��p���āC�I�u�W�F�N�g�𐶐�����D	<br>
�@�@ 4�D���������I�u�W�F�N�g�ɖ��O��ݒ肷��D								<br>
�@�@ 5�D���������I�u�W�F�N�g��e�I�u�W�F�N�g�ɒǉ�����D					<br>
�@�@ 6�D�q�m�[�h�����[�h����D												<br>
</tt>
<br><br><br>


<!------------------------- �t�@�C���̃��[�h�ƃZ�[�u ----------------------------------->
\section secFileIOLoadSave �t�@�C���̃��[�h�ƃZ�[�u
�t�@�C���̃��[�h��Z�[�u���s���ɂ́C�܂�FISdk���쐬���CFISdk����C
FIFileX���쐬���܂��D���̂悤�Ȏ菇�ɂȂ�܂��D

<tt>
�@�@ 1�DCreateFISdk() �ŁCISdk�̃C���X�^���X fiSdk ���쐬����D												<br>
�@�@ 2�DFISdk::CreateFileX() �� FIFileX �̃C���X�^���X fileX �𐶐�����D									<br>
�@�@ 3�DObjectOfs objs ��p�ӂ���D���[�h�����m�[�h�̂����C���[�g�̃m�[�h�������ɓ���D						<br>
�@�@ 4�DFIFileX::Load() ���Ăяo���āC���[�h���s���D														<br>
�@�@ 5�D�Z�[�u����Ȃ�΁C�Z�[�u�������I�u�W�F�N�g������������ objs ��p�ӂ��CFIFileX::Save() ���Ăяo���D	<br>
</tt>
<br>

�ȉ��́C�t�@�C�����[�h�ƃZ�[�u�̃R�[�h�̃T���v���ł��D
\verbatim
	FISdkIf* fiSdk = CreateFISdk();	                           // FISdk�̍쐬
	FIFileXIf* fileX = fiSdk->CreateFileX();                        // �t�@�C���n���h�� FIFileX �̍쐬
	// �I�u�W�F�N�g�̔z��D
	// �����Ƀt�@�C�����烍�[�h�����I�u�W�F�N�g���ǉ������D
	ObjectIfs objs;	
	if (! fileX->Load(objs, "test.x") ) {                           // X�t�@�C���̃��[�h
		DSTR << "Error: Cannot open load file. " << std::endl;
		exit(EXIT_FAILURE);
	}
	PHSdk* phSdk = NULL;
	for(unsigned  i=0; i<objs.size(); ++i){
		phSdk = DCAST(PHSdkIf, objs[i]);                       // ���[�h����PHSdk�����o���D
		if(phSdk) break;
	}
	objs.clear();
	objs.push_back(phSdk);
	fileX->Save(objs, "save.x");                                     // X�t�@�C���̃Z�[�u
\endverbatim
<br><br>


<!------------------------- �e�f�[�^�^�̋L�q ------------------------------------------->
\section secDataFormat		�e�f�[�^�^�̋L�q

\subsection subModelShape	���f���`��
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
�@CDConvexMesh [�m�[�h��] {																		<br>
�@�@�@�@���_���W���X�g;																			<br>
�@�@�@�@�����C���C�W��; �Î~���C�W��; ���˕Ԃ�W��;;�@�@# [�����V�~�����[�V�����Ɋ֌W����ގ�]	<br>
�@}																								<br>
</tt></td></tr></table>
\endhtmlonly

�@���Ɏ����̂́C���� \c 1 �̗����̂��C�ʌ`�󃁃b�V���Œ�`������ł��D
\htmlonly
<table width=100% height="50" style="border:3px double #1841a5;">
<tr><td><tt>
�@CDConvexMesh meshBlock {																	<br>
�@�@�@�@0.5; 0.5; 0.5;, 0.5; 0.5;-0.5;, -0.5; 0.5;0.5;, -0.5; 0.5;-0.5;, 					<br>
�@�@�@�@0.5;-0.5; 0.5;, 0.5;-0.5;-0.5;, -0.5;-0.5;0.5;, -0.5;-0.5;-0.5;;					<br>
�@�@�@�@0.2; 0.5; 0.6;;�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@# phmaterial	<br>
�@}																							<br>
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
�@CDSphere [�m�[�h��] {																				<br>
�@�@�@�@���a;																						<br>
�@�@�@�@�����C���C�W��; �Î~���C�W��; ���˕Ԃ�W��;;�@�@�@ # [�����V�~�����[�V�����Ɋ֌W����ގ�]	<br>
�@}																									<br>
</tt></td></tr></table>
\endhtmlonly

�@���Ɏ����̂́C���a \c 1 �̋����`������ł��D
\htmlonly
<table width=100% height="50" style="border:3px double #1841a5;">
<tr><td><tt>
�@CDSphere sphere {									<br>
�@�@�@�@1.0;�@�@�@�@�@�@�@�@�@ //  radius			<br>
�@�@�@�@0.2; 0.5; 0.6;;�@�@�@�@//  phmaterial		<br>
�@}													<br>
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
�@CDBox [�m�[�h��] {																				<br>
�@�@�@�@�����̃T�C�Y;																				<br>
�@�@�@�@�����C���C�W��; �Î~���C�W��; ���˕Ԃ�W��;;�@�@�@ # �����V�~�����[�V�����Ɋ֌W����ގ�	<br>
�@}																									<br>
</tt></td></tr></table>
\endhtmlonly

�@���Ɏ����̂́C\c X �����̃T�C�Y�� \c 1.0�CY �����̃T�C�Y�� \c 2.0�C\c Z �����̃T�C�Y�� \c 3.0 �̒����̂��`������ł��D
\htmlonly
<table width=100% height="50" style="border:3px double #1841a5;">
<tr><td><tt>
  CDBox box {										<br>
�@�@�@�@1.0; 2.0; 3.0;;�@�@�@�@//  boxsize			<br>
�@�@�@�@0.2; 0.5; 0.6;;�@�@�@�@//  phmaterial		<br>
�@}													<br>
</tt></td></tr></table>
\endhtmlonly
<br><br><br>

*/
