/** \page pageMoreTutoPHSdk	�����V�~�����[�V����SDK �`���[�g���A��

<br>
\section contentsMoreTutoPHSdk	�� Contents ��
�@�@ \ref moreTutoPHSdk						<br>
�@�@�@�@ \ref moreTutoCreatePHSdk			<br>
�@�@ \ref moreTutoSceneDesc					<br>
�@�@�@�@ \ref moreTutoContactSolver			<br>
�@�@�@�@ \ref moreTutoTimeStep				<br>
�@�@ \ref moreTutoScene						<br>
�@�@�@�@ \ref moreTutoCreateScene			<br>
�@�@ \ref moreTutoSolidDesc					<br>
�@�@�@�@ \ref moreTutoSolidDescMass			<br>
�@�@�@�@ \ref moreTutoSolidDescInertia		<br>
�@�@�@�@ \ref moreTutoSolidDescGravity		<br>
�@�@ \ref moreTutoSolid						<br>
�@�@�@�@ \ref moreTutoCreateSolid			<br>
�@�@�@�@ \ref moreTutoCenter				<br>
�@�@�@�@ \ref moreTutoPose					<br>
�@�@�@�@ \ref moreTutoOri					<br>
�@�@�@�@ \ref moreTutoPos					<br>
�@�@�@�@ \ref moreTutoForce					<br>
<br><br>
<hr>

<!-------------------- 1�D�@�����V�~�����[�V����SDK ---------------------------------------------------->
\section moreTutoPHSdk		1�D�@�����V�~�����[�V����SDK
�͂��߂ɁA�����V�~�����[�V�����ɕK�v�ƂȂ�SDK��p�ӂ��܂��D

\subsection moreTutoCreatePHSdk	1.1�D�@�����V�~�����[�V����SDK�̍쐬
�����V�~�����[�V����SDK���쐬���܂��D
\verbatim
    PHSdkIf* sdk = CreatePHSdk();
\endverbatim
<br><br>
<div align="right">
<a href="#pageMoreTutoPHSdk">top of page</a>
</div> 
<hr>

<!-------------------- 2�D�@�V�[���f�X�N���v�^ ----------------------------------->
\section moreTutoSceneDesc	2�D�@�V�[���f�X�N���v�^
�V�[���f�X�N���v�^����A�V�[�����쐬���邱�Ƃ��ł��܂��D
�ȉ��Ɏ����悤�Ȑݒ�������ݒ�̂܂ܗ��p����ꍇ�́A�V�[���f�X�N���v�^��p�ӂ���K�v�͂���܂���D
�f�X�N���v�^�ɂ��ẮA\ref pageApiBasic ���Q�Ɖ������D
�܂��A�T���v���Ƃ��ẮA<a href="../../src/html/BoxStack_2main_8cpp-example.html">BoxStack</a>���Q�Ɖ������D

\subsection moreTutoContactSolver	2.1�D�@�ڐG�E�S�������G���W��
Springhead2�ł́A�ڐG�E�S�������G���W���Ƃ��āA�y�i���e�B�@�ŉ�����@�ƁA��͖@�ŉ�����@��p�ӂ��Ă��܂��D<br>
�����ݒ�ł́A��͖@�ɐݒ肳��Ă��܂��D�G���W���̐؂�ւ��͈ȉ��̂悤�ɍs���Ă��������D<br>
\verbatim
    PHSceneDesc dscene;
    dscene.contactSolver = PHSceneDesc::SOLVER_CONSTRAINT;      // ��͖@
    dscene.contactSolver = PHSceneDesc::SOLVER_PENALTY;         // �y�i���e�B�@
\endverbatim
<br>

\subsection moreTutoTimeStep		2.2�D�@�X�e�b�v��
1�X�e�b�v�̎��s���Ԃ��w��ł��܂��D�����ݒ�ł́A0.005[s] �ɐݒ肳��Ă��܂��D
\verbatim
	dscene.timeStep = 0.01;
\endverbatim
<br><br>
<div align="right">
<a href="#pageMoreTutoPHSdk">top of page</a>
</div> 
<hr>

<!-------------------- 3�D�@�V�[�� ------------------------------------------------->
\section moreTutoScene		3�D�@�V�[��
�V�[�����\�z���܂��D

\subsection 	moreTutoCreateScene		3.1�D�@�V�[���̍쐬
�V�[�����\�z���܂��D�V�[�����\�z���邱�ƂŁA�I�u�W�F�N�g�𓊓����邱�Ƃ��\�ƂȂ�܂��D<br>
�����G���W��SDK�����ƂɁA�f�t�H���g�̃V�[���f�X�N���v�^�𗘗p����ꍇ�́A�ȉ��̂悤�ɃV�[����ݒ肵�܂��D
\verbatim
    PHSceneIf* scene;
    scene = sdk->CreateScene();
\endverbatim
�܂��A�V�[���f�X�N���v�^���w�肷�邱�Ƃ��\�ł��D<br>
�V�[���f�X�N���v�^�ɂ��ẮA\ref moreTutoSceneDesc ���Q�Ɖ������D
\verbatim
    scene = sdk->CreateScene(dscene);
\endverbatim
<br><br>
<div align="right">
<a href="#pageMoreTutoPHSdk">top of page</a>
</div> 
<hr>

<!-------------------- 4�D�@���̃f�X�N���v�^ ------------------------------------->
\section moreTutoSolidDesc		4�D�@���̃f�X�N���v�^
���̃f�X�N���v�^���獄�̂��쐬���邱�Ƃ��ł��܂��D<br>
�ȉ��Ɏ����悤�Ȑݒ�������l�̂܂ܗ��p����ꍇ�́A���̃f�X�N���v�^��p�ӂ���K�v�͂���܂���D
�����ł́A���ʁA�����e���\���A�d�S�̉e���̗L����ݒ肷�邱�Ƃ��ł��܂��D<br>
�܂��A�����̐ݒ�́A\ref moreTutoSolid �ł��ݒ�\�ł��D

\subsection moreTutoSolidDescMass	4.1�D�@����
�����ݒ�ł́A���ʂ� 1.0f �ɐݒ肳��Ă��܂��D<br>
���̃f�X�N���v�^�ɂāA���̂ɑ΂��鎿�ʂ�ݒ肷��ɂ́A�ȉ��̂悤�ɂ��܂��D
\verbatim
    PHSolidDesc desc;
    desc.mass = 2.0;
\endverbatim
<br>	

\subsection moreTutoSolidDescInertia		4.2�D�@�����e���\��
�����ݒ�ł́A�����e���\���͒P�ʍs��ɐݒ肳��Ă��܂��D<br>
���̃f�X�N���v�^�ɂāA���̂ɑ΂��銵���e���\����ݒ肷��ɂ́A�ȉ��̂悤�ɂ��܂��D
\verbatim
    PHSolidDesc desc;
    desc.inertia = 2.0 * Matrix3d::Unit();
\endverbatim
<br>

\subsection moreTutoSolidDescGravity		4.3�D�@�d�͂̉e���̗L��
�����ݒ�ł́A�d�ʂ̉e�����󂯂�悤�ɐݒ肳��Ă��܂��D<br>
���̃f�X�N���v�^�ɂāA���̂ɑ΂���d�͂̉e���̗L����ݒ肷��ɂ́A�ȉ��̂悤�ɂ��܂��D
\verbatim
    PHSolidDesc desc;
    desc.gravity = false;                   // �d�͂̉e�����󂯂Ȃ��悤�ɐݒ�
\endverbatim
<br><br>
<div align="right">
<a href="#pageMoreTutoPHSdk">top of page</a>
</div> 
<hr>

<!-------------------- 5�D�@���� --------------------------------------------------->
\section moreTutoSolid		5�D�@����
���̂ɑ΂��āA�d�S�A�p���A�����Ȃǂ�ݒ肷�邱�Ƃ��ł��܂��D
�܂��A���̂̎��ʂ�A�����e���\���A�d�͂̉e���̗L���ɂ��ẮA���̃f�X�N���v�^����^���邱�Ƃ��ł��܂��D
���̃f�X�N���v�^�ɂ��ẮA\ref moreTutoSolidDesc ���Q�Ɖ������D

\subsection moreTutoCreateSolid		5.1�D�@���̂̍쐬
�f�t�H���g�̍��̃f�X�N���v�^�𗘗p����ꍇ�́A�ȉ��̂悤�ɍ��̂��쐬���܂��D
\verbatim
    PHSolidIf* solid;
    solid = scene->CreateSolid();
\endverbatim
�܂��A�C�ӂ̍��̃f�X�N���v�^���w�肵�A���̂��쐬���邱�Ƃ��\�ł��D<br>
\verbatim
    PHSolidIf* solid;
    solid = scene->CreateSolid(desc);
\endverbatim
<br>

\subsection moreTutoCenter		5.2�D�@���̂̏d�S
���̂̏d�S�́A�ȉ��̂悤�ɐݒ肵�܂��D
\verbatim	
    Vec3d center = Vec3d(0.0, 1.0, 0.0);
    solid->SetCenterPosition(center);
\endverbatim
�܂��A�ȉ��̂悤�ɁA���̂̏d�S���擾���邱�Ƃ��ł��܂��D
\verbatim	
    Vec3d center = solid->GetCenterPosition();
\endverbatim
<br>

\subsection moreTutoPose		5.3�D�@���̂̎p��
���̂̎p���́APosed�^�ŕ\����܂��D <br>
Posed�^�́AAffine�Ɠ����@�\������7�����x�N�g���ŁA\ref moreTutoOri �� \ref moreTutoPos �����킹���x�N�g���ƂȂ��Ă��܂��D
���̂̎p���͈ȉ��̂悤�ɐݒ肵�܂��D
\verbatim
    Posed p = Posed::Rot(Rad(30.0), 'z');                       // z������30�x��]������Posed�^p��p�ӂ���
    solid->SetPose(p);                                          // Posed�^p�����̂̎p���Ƃ��Đݒ肷��
\endverbatim
�܂��A�ȉ��̂悤�ɁA���̂̎p�����擾���邱�Ƃ��ł��܂��D
\verbatim
    Posed pose = solid->GetPose();                              // ���̂̎p�����擾
\endverbatim
<br>

\subsection moreTutoOri			5.4�D�@���̂̌���
���͎̂��_�ƈႢ�A�傫���������Ă���̂ŁA���̂̌������l�����Ȃ��Ă͂Ȃ�܂���D<br>
���̂̌����͈ȉ��̂悤�ɐݒ肵�܂��D
\verbatim	
    solid->SetOrientation(Quaternionf::Rot(Rad(30), 'z'));      // z������30�x��]
\endverbatim
�܂��A�ȉ��̂悤�ɁA���̂̌������擾���邱�Ƃ��ł��܂��D
\verbatim	
    Quaternion ori;
    ori = solid->GetOrientation();                              // ���̂̌������擾
�@�@ori = solid->GetPose().Ori();                               // ���̂̌������擾
\endverbatim
<br>

\subsection moreTutoPos			5.5�D�@���̂̈ʒu
�ʏ�A�V�~�����[�V�������s���ɂ́A���̕��̂��ǂ��ɂ��邩�Ƃ�����񂪕K�v�ɂȂ�܂��D
���̏��Ƃ����̂��A�ʒu�ł��D�ʒu�ɂ��A���̂���Ԃ̂ǂ̈ʒu�ɂ��邩���������Ƃ��ł��܂��D
���̂̈ʒu�͈ȉ��̂悤�ɐݒ肵�܂��D
\verbatim	
    solid->SetFramePosition(Vec3f(0,-3,0));                     // ���̂̈ʒu��(0,-3,0)�ɐݒ�
\endverbatim
�܂��A�ȉ��̂悤�ɁA���̂̈ʒu���擾���邱�Ƃ��ł��܂��D
\verbatim
    Vec3d pos;	
    pos = solid->GetFramePosition();                            // ���̂̈ʒu���擾
    pos = solid->GetPose().Pos();                               // ���̂̈ʒu���擾
\endverbatim
<br>

\subsection moreTutoForce		5.6�D�@���̂ɗ͂�������
���̂ɑ΂��āA���}�̂悤�ȗ͂��������ꍇ���l���܂��D
	\image html force.jpg
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
<a href="#pageMoreTutoPHSdk">top of page</a>
</div>    	
*/
