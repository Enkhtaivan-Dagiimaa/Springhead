/** \page pageMoreTutorial	�`���[�g���A���i�ڍוҁj

<br>

\section contentsMoreTutorial	�� Contents ��
	&nbsp;&nbsp;&nbsp; \ref moreTutPHSDK					     	<br>
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; \ref moreTutCreatePHSdk	  	<br>
	&nbsp;&nbsp;&nbsp; \ref moreTutSceneDesc						<br>
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; \ref moreTutContactSolver	<br>
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; \ref moreTutTimeStep			<br>
	&nbsp;&nbsp;&nbsp; \ref moreTutScene 							<br>
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; \ref moreTutCreateScene		<br>
	&nbsp;&nbsp;&nbsp; \ref moreTutSolid 						<br>
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; \ref morTutCenter			<br>
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; \ref moreTutPose 			<br>
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; \ref moreTutOri 				<br>
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; \ref moreTutPos 				<br>
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; \ref moreTutForce 			<br>
<br><br>
<hr>

<!-------------------- 3�D�@�����G���W��SDK ---------------------------------------------------->
\section moreTutPHSDK		3�D�@�����G���W��SDK
�܂��A�V�~�����[�V�����ɕK�v�ƂȂ�SDK��p�ӂ��܂��B

\subsection moreTutCreatePHSdk	3.1�D�@SDK�̍쐬
SDK���쐬���܂��B
\verbatim
    PHSdkIf* sdk = CreatePHSdk();
\endverbatim
<br><br><br>
<div align="right">
<a href="#pageMoreTutorial">top of page</a>
</div> 
<hr>

<!-------------------- 4�D�@�V�[���̃f�X�N���v�^ ----------------------------------->
\section moreTutSceneDesc	4�D�@�V�[���̃f�X�N���v�^
�V�[�����쐬����ۂ́A�܂��A�V�[���̃f�X�N���v�^��ݒ肷��K�v������܂��B
�������A�ȉ��Ɏ����悤�Ȑݒ���f�t�H���g�̂܂ܗ��p����ꍇ�́A�V�[���̃f�X�N���v�^��ݒ肷��K�v�͂���܂���B
�f�X�N���v�^�ɂ��ẮA\ref pageApiBasic ���Q�Ɖ������B
�܂��A�T���v���Ƃ��ẮA<a href="../../src/html/BoxStack_2main_8cpp-example.html">BoxStack</a>���Q�Ɖ������B

\subsection moreTutContactSolver	4.1�D�@�ڐG�G���W��
�ڐG�G���W���Ƃ��āASpringhead2�ł́A�y�i���e�B�@�ŉ�����@�ƁA��͖@�ŉ�����@��p�ӂ��Ă��܂��B<br>
�f�t�H���g�ł́A��͖@�ɐݒ肳��Ă��܂��B�ڐG�G���W���̐؂�ւ��͈ȉ��̂悤�ɍs���Ă��������B<br>
\verbatim
	PHSceneDesc dscene;
	dscene.contactSolver = PHSceneDesc::SOLVER_CONSTRAINT;      // ��͖@
	dscene.contactSolver = PHSceneDesc::SOLVER_CONSTRAINT;      // �y�i���e�B�@
\endverbatim
<br>

\subsection moreTutTimeStep		4.2�D�@�X�e�b�v��
1�X�e�b�v�̎��s���Ԃ��w��ł��܂��B�f�t�H���g�ł� 0.005[s] �ɐݒ肳��Ă��܂��B
\verbatim
	dscene.timeStep = 0.01;
\endverbatim
<br><br><br>
<div align="right">
<a href="#pageMoreTutorial">top of page</a>
</div> 
<hr>

<!-------------------- 5�D�@�V�[�� ------------------------------------------------->
\section moreTutScene		5�D�@�V�[��
�V�[�����\�z���܂��B

\subsection 	moreTutCreateScene		5.1�D�@�V�[���̍쐬
�V�[�����\�z���܂��B�V�[�����\�z���邱�ƂŁA�I�u�W�F�N�g�𓊓����邱�Ƃ��\�ƂȂ�܂��B<br>
���炩���ߗp�ӂ��Ă�����SDK�����ƂɁA�f�t�H���g�̃V�[���f�X�N���v�^�𗘗p����ꍇ�́A���̂悤�ɃV�[����ݒ肵�܂��B
\verbatim
    PHSceneIf* scene;
    scene = sdk->CreateScene();
\endverbatim
�܂��A�V�[���̃f�X�N���v�^���w�肷�邱�Ƃ��\�ł��B<br>
�V�[���̃f�X�N���v�^�ɂ��ẮA\ref moreTutSceneDesc ���Q�Ɖ������B
\verbatim
    scene = sdk->CreateScene(dscene);
\endverbatim
<br><br><br>
<div align="right">
<a href="#pageMoreTutorial">top of page</a>
</div> 
<hr>

<!-------------------- 6�D�@���� --------------------------------------------------->
\section moreTutSolid		6�D�@����
���̂ɑ΂��āA������A���ʁA�d�S�A�����e���\���Ȃǂ�ݒ肷�邱�Ƃ��ł��܂��B

\subsection morTutCenter		6.1�D�@���̂̏d�S
���̂̏d�S�́A�ȉ��̂悤�ɂ��Đݒ肵�܂��B
\verbatim	
	Vec3d center = Vec3d(0.0, 1.0, 0.0);
	solid->SetCenterPosition(center);
\endverbatim
�܂��A���̂悤�ɂ��āA���̂̏d�S���擾���邱�Ƃ��ł��܂��B
\verbatim	
	Vec3d center = solid->GetCenterPosition();
\endverbatim
<br>

\subsection moreTutPose		6.2�D�@���̂̎p��
���̂̎p���́APosed�^�ŕ\����܂��B <br>
Posed�^�́AAffine�Ɠ����@�\������7�����x�N�g���ŁA\ref moreTutOri �� \ref moreTutPos �����킹���x�N�g���ƂȂ��Ă��܂��B
���̂̎p���͈ȉ��̂悤�ɂ��Đݒ肵�܂��B
\verbatim
    Posed p = Posed::Rot(Rad(30.0), 'z');                       // z������30�x��]������Posed�^p��p�ӂ���
    solid->SetPose(p);                                          // Posed�^p�����̂̎p���Ƃ��Đݒ肷��
\endverbatim
�܂��A���̂悤�ɂ��āA���̂̎p�����擾���邱�Ƃ��ł��܂��B
\verbatim
    Posed pose = solid->GetPose();                              // ���̂̎p�����擾
\endverbatim
<br>

\subsection moreTutOri			6.3�D�@���̂̌���
���͎̂��_�ƈႢ�A�傫���������Ă���̂ŁA���̂̌������l�����Ȃ��Ă͂Ȃ�܂���B<br>
���̂̌����͈ȉ��̂悤�ɐݒ肵�܂��B
\verbatim	
    solid->SetOrientation(Quaternionf::Rot(Rad(30), 'z'));      // z������30�x��]
\endverbatim
�܂��A���̂悤�ɂ��āA���̂̌������擾���邱�Ƃ��ł��܂��B
\verbatim	
    Quaternion ori;
    ori = solid->GetOrientation();                              // ���̂̌������擾
�@�@ori = solid->GetPose().Ori();                               // ���̂̌������擾
\endverbatim
<br>

\subsection moreTutPos			6.4�D�@���̂̈ʒu
�ʏ�A�V�~�����[�V�������s���ɂ́A���̕��̂��ǂ��ɂ��邩�Ƃ�����񂪕K�v�ɂȂ�܂��B
���̏��Ƃ����̂��A�ʒu�ł��B�ʒu�ɂ��A���̂���Ԃ̂ǂ̈ʒu�ɂ��邩���������Ƃ��ł��܂��B
���̂̈ʒu�͈ȉ��̂悤�ɐݒ肵�܂��B
\verbatim	
    solid->SetFramePosition(Vec3f(0,-3,0));                     // ���̂̈ʒu��(0,-3,0)�ɐݒ�
\endverbatim
�܂��A���̂悤�ɂ��āA���̂̈ʒu���擾���邱�Ƃ��ł��܂��B
\verbatim
    Vec3d pos;	
    pos = solid->GetFramePosition();                            // ���̂̈ʒu���擾
    pos = solid->GetPose().Pos();                               // ���̂̈ʒu���擾
\endverbatim
<br>

\subsection moreTutForce		6.5�D�@���̂ɗ͂�������
���̂ɑ΂��āA���}�̂悤�ȗ͂��������ꍇ���l���܂��B
<img src="../../include/docsrc/02Tutorial/force.jpg" width="682" height="146">
���̂ɗ͂���p����ꍇ���l���A���ʒ��S�ɗ͂�������ɂ́A�ȉ��̂悤�ɐݒ肷�邱�Ƃ��ł��܂��B
\verbatim
    solid->AddForce(Vec3f(10, 0, 0);                                                // ���ʒ��S�ɉE������10N�̗͂�������
\endverbatim
�܂��A���ʒ��S�����łȂ��A�ʒu���w�肵�āA�͂������邱�Ƃ��\�ł��B
\verbatim    
    solid->AddForce(f, r);                                                          // �͂� �ʒur(World�n) �ɉ�����
    solid->AddForce(Vec3f(10,0,0), Vec3f(0,2,0)+solid1->GetCenterPosition());       // �d�S��2m����E������10N�̗͂�������
\endverbatim



<br>
<div align="right">
<a href="#pageMoreTutorial">top of page</a>
</div>    	
*/
