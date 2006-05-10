/** \page pageMoreTutorial	�`���[�g���A���i�ڍוҁj

<br>
\section contentsMoreTutorial	�� Contents ��
�@�@ \ref moreTutPHSDK						<br>
�@�@�@�@ \ref moreTutCreatePHSdk			<br>
�@�@ \ref moreTutSceneDesc					<br>
�@�@�@�@ \ref moreTutContactSolver			<br>
�@�@�@�@ \ref moreTutTimeStep				<br>
�@�@ \ref moreTutScene						<br>
�@�@�@�@ \ref moreTutCreateScene			<br>
�@�@ \ref moreTutSolidDesc					<br>
�@�@�@�@ \ref moreTutSolidDescMass			<br>
�@�@�@�@ \ref moreTutSolidDescInertia		<br>
�@�@�@�@ \ref moreTutSolidDescGravity		<br>
�@�@ \ref moreTutSolid						<br>
�@�@�@�@ \ref moreTutCenter					<br>
�@�@�@�@ \ref moreTutPose					<br>
�@�@�@�@ \ref moreTutOri					<br>
�@�@�@�@ \ref moreTutPos					<br>
�@�@�@�@ \ref moreTutForce					<br>
<br><br>
<hr>

<!-------------------- 1�D�@�����G���W��SDK ---------------------------------------------------->
\section moreTutPHSDK		1�D�@�����G���W��SDK
�܂��A�V�~�����[�V�����ɕK�v�ƂȂ�SDK��p�ӂ��܂��D

\subsection moreTutCreatePHSdk	1.1�D�@SDK�̍쐬
SDK���쐬���܂��D
\verbatim
    PHSdkIf* sdk = CreatePHSdk();
\endverbatim
<br><br>
<div align="right">
<a href="#pageMoreTutorial">top of page</a>
</div> 
<hr>

<!-------------------- 2�D�@�V�[���̃f�X�N���v�^ ----------------------------------->
\section moreTutSceneDesc	2�D�@�V�[���̃f�X�N���v�^
�V�[�����쐬����ۂ́A�܂��A�V�[���̃f�X�N���v�^��ݒ肷��K�v������܂��D
�������A�ȉ��Ɏ����悤�Ȑݒ�������ݒ�̂܂ܗ��p����ꍇ�́A�V�[���̃f�X�N���v�^��p�ӂ���K�v�͂���܂���D
�f�X�N���v�^�ɂ��ẮA\ref pageApiBasic ���Q�Ɖ������D
�܂��A�T���v���Ƃ��ẮA<a href="../../src/html/BoxStack_2main_8cpp-example.html">BoxStack</a>���Q�Ɖ������D

\subsection moreTutContactSolver	2.1�D�@�ڐG�E�S�������G���W��
Springhead2�ł́A�ڐG�E�S�������G���W���Ƃ��āA�y�i���e�B�@�ŉ�����@�ƁA��͖@�ŉ�����@��p�ӂ��Ă��܂��D<br>
�����ݒ�ł́A��͖@�ɐݒ肳��Ă��܂��D�G���W���̐؂�ւ��͈ȉ��̂悤�ɍs���Ă��������D<br>
\verbatim
	PHSceneDesc dscene;
	dscene.contactSolver = PHSceneDesc::SOLVER_CONSTRAINT;      // ��͖@
	dscene.contactSolver = PHSceneDesc::SOLVER_PENALTY;         // �y�i���e�B�@
\endverbatim
<br>

\subsection moreTutTimeStep		2.2�D�@�X�e�b�v��
1�X�e�b�v�̎��s���Ԃ��w��ł��܂��D�����ݒ�ł́A0.005[s] �ɐݒ肳��Ă��܂��D
\verbatim
	dscene.timeStep = 0.01;
\endverbatim
<br><br>
<div align="right">
<a href="#pageMoreTutorial">top of page</a>
</div> 
<hr>

<!-------------------- 3�D�@�V�[�� ------------------------------------------------->
\section moreTutScene		3�D�@�V�[��
�V�[�����\�z���܂��D

\subsection 	moreTutCreateScene		3.1�D�@�V�[���̍쐬
�V�[�����\�z���܂��D�V�[�����\�z���邱�ƂŁA�I�u�W�F�N�g�𓊓����邱�Ƃ��\�ƂȂ�܂��D<br>
�����G���W��SDK�����ƂɁA�f�t�H���g�̃V�[���f�X�N���v�^�𗘗p����ꍇ�́A���̂悤�ɃV�[����ݒ肵�܂��D
\verbatim
    PHSceneIf* scene;
    scene = sdk->CreateScene();
\endverbatim
�܂��A�V�[���̃f�X�N���v�^���w�肷�邱�Ƃ��\�ł��D<br>
�V�[���̃f�X�N���v�^�ɂ��ẮA\ref moreTutSceneDesc ���Q�Ɖ������D
\verbatim
    scene = sdk->CreateScene(dscene);
\endverbatim
<br><br>
<div align="right">
<a href="#pageMoreTutorial">top of page</a>
</div> 
<hr>

<!-------------------- 4�D�@���̂̃f�X�N���v�^ ------------------------------------->
\section moreTutSolidDesc		4�D�@���̂̃f�X�N���v�^
���̂��쐬����ۂ́A�܂��A���̂̃f�X�N���v�^��ݒ肷��K�v������܂��D<br>
���Ɏ����悤�ɁA���ʁA�����e���\���A�d�S�̉e���̗L�����A���̂̃f�X�N���v�^�Őݒ肷�邱�Ƃ��ł��܂��D<br>
�܂��A�����̐ݒ�́A\ref moreTutSolid �ł��ݒ�\�ł��D

\subsection moreTutSolidDescMass	4.1�D�@����
�����ݒ�ł́A���ʂ� 1.0f �ɐݒ肳��Ă��܂��D<br>
���̃f�X�N���v�^�ɂāA���̂ɑ΂��鎿�ʂ�ݒ肷��ɂ́A�ȉ��̂悤�ɂ��܂��D
\verbatim
	PHSolidDesc desc;
	desc.mass = 2.0;
\endverbatim
<br>	

\subsection moreTutSolidDescInertia		4.2�D�@�����e���\��
�����ݒ�ł́A�����e���\���͒P�ʍs��ɐݒ肳��Ă��܂��D<br>
���̃f�X�N���v�^�ɂāA���̂ɑ΂��銵���e���\����ݒ肷��ɂ́A�ȉ��̂悤�ɂ��܂��D
\verbatim
	PHSolidDesc desc;
	desc.inertia = 2.0 * Matrix3d::Unit();
\endverbatim
<br>

\subsection moreTutSolidDescGravity		4.3�D�@�d�͂̉e���̗L��
�����ݒ�ł́A�d�ʂ̉e�����󂯂�悤�ɐݒ肳��Ă��܂��D<br>
���̃f�X�N���v�^�ɂāA���̂ɑ΂���d�͂̉e���̗L����ݒ肷��ɂ́A�ȉ��̂悤�ɂ��܂��D
\verbatim
	PHSolidDesc desc;
	desc.gravity = false;                   // �d�͂̉e�����󂯂Ȃ��悤�ɐݒ�
\endverbatim
<br><br>
<div align="right">
<a href="#pageMoreTutorial">top of page</a>
</div> 
<hr>

<!-------------------- 5�D�@���� --------------------------------------------------->
\section moreTutSolid		5�D�@����
���̂ɑ΂��āA�d�S�A�p���A�����Ȃǂ�ݒ肷�邱�Ƃ��ł��܂��D<br>
�܂��A���̂̎��ʂ�A�����e���\���A�d�͂̉e���̗L���ɂ��ẮA���̂̃f�X�N���v�^����^���邱�Ƃ��ł��܂��D<br>
���̂̃f�X�N���v�^�ɂ��ẮA\ref moreTutSolidDesc ���Q�Ɖ������D

\subsection moreTutCenter		5.1�D�@���̂̏d�S
���̂̏d�S�́A�ȉ��̂悤�ɂ��Đݒ肵�܂��D
\verbatim	
	Vec3d center = Vec3d(0.0, 1.0, 0.0);
	solid->SetCenterPosition(center);
\endverbatim
�܂��A���̂悤�ɂ��āA���̂̏d�S���擾���邱�Ƃ��ł��܂��D
\verbatim	
	Vec3d center = solid->GetCenterPosition();
\endverbatim
<br>

\subsection moreTutPose		5.2�D�@���̂̎p��
���̂̎p���́APosed�^�ŕ\����܂��D <br>
Posed�^�́AAffine�Ɠ����@�\������7�����x�N�g���ŁA\ref moreTutOri �� \ref moreTutPos �����킹���x�N�g���ƂȂ��Ă��܂��D
���̂̎p���͈ȉ��̂悤�ɂ��Đݒ肵�܂��D
\verbatim
    Posed p = Posed::Rot(Rad(30.0), 'z');                       // z������30�x��]������Posed�^p��p�ӂ���
    solid->SetPose(p);                                          // Posed�^p�����̂̎p���Ƃ��Đݒ肷��
\endverbatim
�܂��A���̂悤�ɂ��āA���̂̎p�����擾���邱�Ƃ��ł��܂��D
\verbatim
    Posed pose = solid->GetPose();                              // ���̂̎p�����擾
\endverbatim
<br>

\subsection moreTutOri			5.3�D�@���̂̌���
���͎̂��_�ƈႢ�A�傫���������Ă���̂ŁA���̂̌������l�����Ȃ��Ă͂Ȃ�܂���D<br>
���̂̌����͈ȉ��̂悤�ɐݒ肵�܂��D
\verbatim	
    solid->SetOrientation(Quaternionf::Rot(Rad(30), 'z'));      // z������30�x��]
\endverbatim
�܂��A���̂悤�ɂ��āA���̂̌������擾���邱�Ƃ��ł��܂��D
\verbatim	
    Quaternion ori;
    ori = solid->GetOrientation();                              // ���̂̌������擾
�@�@ori = solid->GetPose().Ori();                               // ���̂̌������擾
\endverbatim
<br>

\subsection moreTutPos			5.4�D�@���̂̈ʒu
�ʏ�A�V�~�����[�V�������s���ɂ́A���̕��̂��ǂ��ɂ��邩�Ƃ�����񂪕K�v�ɂȂ�܂��D
���̏��Ƃ����̂��A�ʒu�ł��D�ʒu�ɂ��A���̂���Ԃ̂ǂ̈ʒu�ɂ��邩���������Ƃ��ł��܂��D
���̂̈ʒu�͈ȉ��̂悤�ɐݒ肵�܂��D
\verbatim	
    solid->SetFramePosition(Vec3f(0,-3,0));                     // ���̂̈ʒu��(0,-3,0)�ɐݒ�
\endverbatim
�܂��A���̂悤�ɂ��āA���̂̈ʒu���擾���邱�Ƃ��ł��܂��D
\verbatim
    Vec3d pos;	
    pos = solid->GetFramePosition();                            // ���̂̈ʒu���擾
    pos = solid->GetPose().Pos();                               // ���̂̈ʒu���擾
\endverbatim
<br>

\subsection moreTutForce		5.5�D�@���̂ɗ͂�������
���̂ɑ΂��āA���}�̂悤�ȗ͂��������ꍇ���l���܂��D
<img src="../../include/docsrc/02Tutorial/force.jpg" width="682" height="146">
���̂ɗ͂���p����ꍇ���l���A���ʒ��S�ɗ͂�������ɂ́A�ȉ��̂悤�ɐݒ肷�邱�Ƃ��ł��܂��D
\verbatim
    solid->AddForce(Vec3f(10, 0, 0);                                                // ���ʒ��S�ɉE������10N�̗͂�������
\endverbatim
�܂��A���ʒ��S�����łȂ��A�ʒu���w�肵�āA�͂������邱�Ƃ��\�ł��D
\verbatim    
    solid->AddForce(f, r);                                                          // �͂� �ʒur(World�n) �ɉ�����
    solid->AddForce(Vec3f(10,0,0), Vec3f(0,2,0)+solid1->GetCenterPosition());       // �d�S��2m����E������10N�̗͂�������
\endverbatim
<br><br>
<div align="right">
<a href="#pageMoreTutorial">top of page</a>
</div>    	
*/
