/** \page pagePhysics �����V�~�����[�V����SDK

- \ref sec_PHSdk
 - \ref sdk_create
 - \ref sdk_finalize
- \ref scene
 - \ref scene_create
 - \ref scene_gravity
 - \ref scene_step
- \ref solid
 - \ref solid_create
 - \ref solid_pose
 - \ref solid_mass
 - \ref solid_force
 - \ref solid_dynamical
 - \ref solid_shape
- \ref shape
- \ref joint

\section sec_PHSdk �����V�~�����[�V����SDK�̏������ƏI��

�����V�~�����[�V����SDK�́C�����@���ɂ��ƂÂ����������̂̃C���^���N�V�������v�Z����@�\��񋟂��܂��D

�����V�~�����[�V����SDK�̎g�p�́C�O���[�o���֐�\link Spr::CreatePHSdk() CreatePHSdk\endlink ���ĂԂ��Ƃ���n�܂�܂��D
\verbatim
	PHSdkIf* sdk = CreatePHSdk();
\endverbatim
�ȍ~�CSDK�̑����\link Spr::CreatePHSdk() CreatePHSdk\endlink �ɂ���ĕԂ��ꂽPHSdkIf�^�̃|�C���^����čs���܂��D

SDK�́C1�ȏ�̃V�[���������Ƃ��ł��܂��D
�܂��C1�̃V�[����1�ȏ�̍���(PHSolid)�Ɗ֐�(PHJoint)�������Ƃ��ł��܂��D
����ɁC���̂͌`��(CDConvexMesh, CDSphere, CDBox, ...)���Q�Ƃ��Č`�������Ƃ��ł��܂��D
�֐߂�2�̍��̂����ѕt���܂��D
�����G���W�����̃I�u�W�F�N�g�̎Q�Ɗ֌W�̗��}�Ɏ����܂��D
<img src="../../include/docsrc/01UsersGuide/PHSdkScene.png">

SDK�̏I�������͓����Ŏ����I�ɍs���܂��D���[�U���疾���I��SDK���������K�v�͂���܂���D

\section scene �V�[��

�V�[���Ƃ͕����V�~�����[�V�����̕���ƂȂ��Ԃ̂��Ƃł��D

\subsection scene_create �V�[���̍쐬

�V�[�����쐬����ɂ�\link Spr::PHSdkIf::CreateScene() PHSdkIf::CreateScene\endlink���Ăт܂��D

\verbatim
	//�f�t�H���g�̐ݒ�ŃV�[�����쐬
	PHSceneDesc sceneDesc;
	PHSceneIf* scene = sdk->CreateScene(sceneDesc);
\endverbatim

�V�[���̑����\link Spr::PHSdkIf::CreateScene() PHSdkIf::CreateScene\endlink�ɂ���ĕԂ��ꂽ
PHSceneIf�^�̃|�C���^����čs�Ȃ��܂��D

�V�[���𕡐��쐬���邱�Ƃ͉\�ł����C�قȂ�V�[�����m�݂͌��ɉe��������ڂ��܂���D

\subsection scene_step �V�~�����[�V�����̎��s

�V�[���̎�����i�߂�ɂ�\link Spr::PHSceneIf::Step() PHSceneIf::Step\endlink���Ăт܂��D

\verbatim
	//0.05 * 100 = 5�b�Ԃ̃V�~�����[�V����
	scene->SetTimeStep(0.05);
	for(int i = 0; i < 100; i++)
		scene->Step();
\endverbatim

\link Spr::PHSceneIf::Step() Step\endlink �̗���͑�̈ȉ��̂悤�ɂȂ��Ă��܂��D
-# ���̓��m�̏Փ˔���
-# �ڐG�́C�֐߂̍S���͂̌v�Z
-# ���̂̑��x�E�ʒu�̍X�V

��x��\link Spr::PHSceneIf::Step() Step\endlink�̌Ăяo���Ői�ގ��Ԃ̕���
\link Spr::PHSceneIf::GetTimeStep() PHSceneIf::GetTimeStep\endlink��
\link Spr::PHSceneIf::SetTimeStep() PHSceneIf::SetTimeStep\endlink
�Ŏ擾�E�ݒ�ł��܂��D<br>
�������Őݒ肷��l�͂��̂܂ܐ��l�ϕ��̐ϕ��X�e�b�v�ƂȂ�܂��̂ŁC���܂�傫�Ȓl(�Ⴆ��1�b)��
�ݒ肵�Ȃ��ł��������D

\subsection scene_gravity �d�͂̐ݒ�

�V�[���ɓ����d�͂�ݒ肷��ɂ�\link Spr::PHSceneIf::SetGravity() PHSceneIf::SetGravity\endlink���Ăт܂��D

\verbatim
	scene->SetGravity(Vec3f(0.0f, -9.8f, 0.0f));
\endverbatim

�f�t�H���g�ŁC�V�[���̏d�͉����x��Vec3f(0.0f, -9.8f, 0.0f)�ɐݒ肳��Ă��܂��D
���d�͋�ԂƂ���ɂ�

\verbatim
	scene->SetGravity(Vec3f(0.0f, 0.0f, 0.0f));
\endverbatim

�܂��C�X�̍��̂ɑ΂��ďd�͂̍�p��L���E����������ɂ�
\link Spr::PHSolidIf::SetGravity() PHSolidIf::SetGravity\endlink���g���܂��D

\verbatim
	//solid��scene�ɓo�^����Ă��鍄�̂Ƃ���
	solid->SetGravity(false);
\endverbatim

\section solid	����

���̂Ƃ́C�����@���ɏ]���ăV�[�������^�����镨�̂ł��D

\subsection solid_create ���̂̍쐬

���̂��쐬���C�V�[���ɒǉ�����ɂ�\link Spr::PHSceneIf::CreateSolid() PHSceneIf::CreateSolid\endlink���Ăт܂��D

\verbatim
	//�f�t�H���g�̐ݒ�ō��̂��쐬
	PHSolidDesc solidDesc;
	PHSolidIf* solid = scene->CreateSolid(solidDesc);
\endverbatim

���̂ɑ΂��鑀���\link Spr::PHSceneIf::CreateSolid() PHSceneIf::CreateSolid\endlink�ɂ���ĕԂ��ꂽ
PHSolidIf�^�̃|�C���^����čs���܂��D

\subsection solid_pose ���̂̈ʒu�Ƒ��x

<img src="../../include/docsrc/01UsersGuide/scene.png">

1�̍��̂ɂ́C1�̍��W�n���\��t���Ă��܂��D��������̍��W�n�ƌĂт܂��D
�V�[�����̍��̂̈ʒu�ƌX���́C�V�[�����W�n�ɑ΂��鍄�̍��W�n�̈ʒu�ƌX���Ƃ��ĕ\������܂��D

���̂̈ʒu��ݒ�E�擾����ɂ�
\link Spr::PHSolidIf::SetFramePosition() PHSolidIf::SetFramePosition\endlink��
\link Spr::PHSolidIf::GetFramePosition() PHSolidIf::GetFramePosition\endlink
���Ăт܂��D
�܂��C�V�[�����W�n�ɑ΂���d�S�̈ʒu�ɂ���č��̂̈ʒu��ݒ�E�擾���邱�Ƃ��o���܂��D����ɂ�
\link Spr::PHSolidIf::SetCenterPosition() PHSolidIf::SetCenterPosition\endlink��
\link Spr::PHSolidIf::GetCenterPosition() PHSolidIf::GetCenterPosition\endlink
���g���܂��D

���̂̌X�����擾����ɂ�
\link Spr::PHSolidIf::SetOrientation() PHSolidIf::SetOrientation\endlink��
\link Spr::PHSolidIf::GetOrientation() PHSolidIf::GetOrientation\endlink
���g���܂��D

���̂̈ʒu�ƌX���𓯎��ɐݒ�E�擾���邱�Ƃ��o���܂��D����ɂ�
\link Spr::PHSolidIf::SetPose() PHSolidIf::SetPose\endlink��
\link Spr::PHSolidIf::GetPose() PHSolidIf::GetPose\endlink
���g���܂��D

���̂̑��x��
\link Spr::PHSolidIf::SetVelocity() PHSolidIf::SetVelocity\endlink��
\link Spr::PHSolidIf::GetVelocity() PHSolidIf::GetVelocity\endlink
���ĂԂ��ƂŐݒ�E�擾�ł��܂��D
�����ō��̂̑��x�́C�V�[�����W�n����ϑ��������̍��W�n���_�̑��x�ŕ\�킳��܂��D
���l�ɍ��̂̊p���x��
\link Spr::PHSolidIf::SetVelocity() PHSolidIf::SetVelocity\endlink��
\link Spr::PHSolidIf::GetVelocity() PHSolidIf::GetVelocity\endlink
���ĂԂ��ƂŐݒ�E�擾�ł��܂��D
���̂̊p���x�́C�V�[�����W�n����ϑ��������̍��W�n�̊p���x�ł��D

\subsection solid_mass ���̂̎���
���̂̎��ʏ��́C�S���ʂƁC�d�S�Ɋւ��銵���s��ŕ\������܂��D
���̂̎��ʂ��擾�E�ݒ肷��ɂ�
\link Spr::PHSolidIf::GetMass() PHSolidIf::GetMass \endlink��
\link Spr::PHSolidIf::SetMass() PHSolidIf::SetMass \endlink
���g���܂��D
���l�ɁC���̂̊����s����擾�E�ݒ肷��ɂ�
\link Spr::PHSolidIf::GetInertia() PHSolidIf::GetInertia \endlink��
\link Spr::PHSolidIf::SetInertia() PHSolidIf::SetInertia \endlink
���g���܂��D

\verbatim
	//���ʂ�0.1�C�����s���Ίp������0.1�̍s��ɐݒ�
	solid->SetMass(0.1);
	solid->SetInertia(0.1 * Matrix3d::Unit());
\endverbatim

�d�S�ʒu�̓f�t�H���g�ł͍��̍��W�n�̌��_�ƈ�v���Ă��܂����C
\link Spr::PHSolidIf::GetCenterOfMass() PHSolidIf::GetCenterOfMass \endlink�C\link Spr::PHSolidIf::SetCenterOfMass() PHSolidIf::SetCenterOfMass \endlink�C

\verbatim
	solid->SetCenterOfMass(Vec3d(0.1, 0.0, 0.0));
\endverbatim

���̂̍쐬���ɁC�f�B�X�N���v�^��p���Ă����̏���ݒ肷�邱�Ƃ��ł��܂��D

\verbatim
	PHSolidDesc solidDesc;
	solidDesc.mass = 0.1;
	solidDesc.inertia = 0.1 * Matrix3d::Unit();
	solidDesc.center = Vec3d(0.1, 0.0, 0.0);
	PHSolidIf* solid = scene->CreateSolid(solidDesc);
\endverbatim

\subsection solid_force ���̂֗͂�������
���̂֕��i�͉�����ɂ�\link Spr::PHSolidIf::AddForce() PHSolidIf::AddForce\endlink�C
�g���N��������ɂ�\link Spr::PHSolidIf::AddTorque() PHSolidIf::AddTorque\endlink���Ăт܂��D

���̂ɉ������Ă���́E�g���N�́C\link Spr::PHSceneIf::Step() PHSceneIf::Step\endlink���Ăԓx��0�ɃN���A����܂��D
���̂��߁C���I�ɗ́E�g���N�����������邽�߂ɂ́C\link Spr::PHSceneIf::Step() PHSceneIf::Step\endlink���ĂԑO�ɖ���
AddForce/AddTorque���ĂԕK�v������܂��D

\subsection solid_dynamical �����@���̉e�����󂯂Ȃ��悤�ɂ���
�ꍇ�ɂ���āC���̍��̂Ƃ̐ڐG��d�͂���̉e�����󂯂Ȃ����̂���肽���Ȃ邱�Ƃ�����܂��D
���̂悤�ȍ��̂�񓮗͊w�I�ȍ��̂ƌĂт܂��D
���̂�񓮗͊w�I�Ƃ�����C���ɖ߂����肷��ɂ�\link Spr::PHSolidIf::SetDynamical() PHSolidIf::SetDynamical\endlink���Ăт܂��D
�񓮗͊w�I�Ƃ��邱�Ƃ��L����1�̓T�^�I�ȗ�̓V�[���̒n�ʂł��D
���ʁC�n�ʂ͋�ԂɌŒ肳��Ă��āC���̍��̂Ƃ̏Փ˂�d�͂ɂ���Ĉʒu��ω������Ȃ����Ƃ����߂��܂��D
����́C
\verbatim
floor->SetDynamical(false);		//floor��PHSolidIf*�^�̕ϐ�
\endverbatim
�Ƃ��邱�ƂŎ����ł��܂��D
�܂��C���鍄�̂��񓮗͊w�I���ǂ�����\link Spr::PHSolidIf::IsDynamical() PHSolidIf::IsDynamical \endlink���ĂԂ��ƂŎ擾�ł��܂��D

�񓮗͊w�I�ȍ��̂ł��ʒu�C�X���C���x�C�p���x�̕ύX�͉\�ł��D

\subsection solid_shape ���̂ւ̌`��̊��蓖��
���̂ɂ�1�ȏ�̌`������蓖�Ă邱�Ƃ��ł��܂��D
Springhead �����V�~�����[�V����SDK�́C�`��̌��������m���C
���̊Ԃɓ����ڐG�͂������I�Ɍv�Z���܂��D
���̂Ɋ��蓖�Ă邱�Ƃ̂ł���`��̎�ނ���э쐬���@�ɂ��Ă�\ref shape�̍����Q�Ƃ��Ă��������D

���̂Ɍ`������蓖�Ă�ɂ�\link Spr::PHSolidIf::AddShape() PHSolidIf::AddShape\endlink���g���܂��D
���̂́C���蓖�Ă�ꂽ�`��iCDShape�^�̃I�u�W�F�N�g�j�ւ̎Q�Ƃ�ێ����邾���ł��̂ŁC1�̌`��𕡐��̈قȂ鍄�̂Ɋ��蓖�ĂĂ���肠��܂���D
����ɂ���āC�����̓����`�̍��̂��쐬�������ꍇ�Ƀ�������ߖ�ł��܂��D
�܂��C1�̍��̂ɓ���CDShape�I�u�W�F�N�g�𕡐���o�^����ƁC���ꂼ��ʂ̌`��Ƃ��ĔF������܂��D

�`������̂Ɏ��t����ʒu���擾�E�ݒ肷��ɂ́C
\link Spr::PHSolidIf::GetShapePose() PHSolidIf::GetShapePose \endlink�C
\link Spr::PHSolidIf::SetShapePose() PHSolidIf::SetShapePose \endlink���Ăт܂��D

\section shape	�`��
���̂Ɋ��蓖�Ă邱�Ƃ��ł���`��ɂ͈ȉ��̂��̂�����܂��D
- \ref shape_sphere
- \ref shape_box
- \ref shape_mesh

Springhead Physics���������Ƃ̂ł���`��͓ʌ`��݂̂ł��D
�ʌ`��Ƃ́C�ꌾ�ł����Ή��݂⌊�̖����`��̂��Ƃł��D
�ʂłȂ��i���݂⌊�����j�`������̂Ɋ��蓖�Ă�ɂ́C���炩���߂��̌`���
�����̓ʌ`��ɕ������Ċ��蓖�Ă�K�v������܂��D

\subsection shape_sphere ��

\subsection shape_box ������

\subsection shape_mesh ���b�V��


\section joint �֐�

�֐߂Ƃ́C2�̍��̂̑��Ή^���ɍS������������̂ł��D
2�̍��̂̑��Ή^���́C�S�������݂��Ȃ��ꍇ�͕��i3���R�x�C��]3���R�x�̌v6�̎��R�x�������܂��D
���̓��ǂ̎��R�x���S�����邩�ɉ����ėl�X�Ȋ֐߂̎�ނ��l�����܂��D
Springhead���T�|�[�g����֐߂̎�ނ��ȉ��Ɏ����܂��D

<img src="../../include/docsrc/01UsersGuide/joints.png">

\subsection joint_create �֐߂̍쐬

�֐߂��쐬���C��̍��̂�A������T�^�I�ȃR�[�h���ȉ��Ɏ����܂��D

\verbatim
PHHingeJointDesc descJoint;                         //�f�B�X�N���v�^���쐬
descJoint.pose[0].Pos() = Vec3d( 1.0, 0.0, 0.0);    //�\�P�b�g�̈ʒu��ݒ�
descJoint.pose[1].Pos() = Vec3d(-1.0, 0.0, 0.0);    //�v���O�̈ʒu��ݒ�
scene->CreateJoint(solid0, solid1, descJoint));     //�֐߂��쐬
\endverbatim

�e�֐߂̃f�B�X�N���v�^�́C�Ή�����֐߃N���X������If���Ƃ���Desc���������O�������C
�ȉ��̂悤�Ȍp���֌W�ɂ���܂��D

���f�B�X�N���v�^�̌p���֌W��

�֐߂��쐬����ɂ́C�܂��쐬�������֐߂̎�ނɑΉ�����f�B�X�N���v�^���쐬���C
�K�v�Ȃ�΃f�B�X�N���v�^�̃v���p�e�B��ݒ肵�܂��D
���ɁC\link Spr::PHSceneIf::CreateJoint() PHSceneIf::CreateJoint\endlink�֐����C
�A�����������̂ւ̎Q�ƂƁC��ɍ쐬�����f�B�X���v�^�������Ƃ��ČĂт܂��D
\link Spr::PHSceneIf::CreateJoint() PHSceneIf::CreateJoint\endlink�́C�n���ꂽ�f�B�X�N���v�^��
��ނ�����Ŕ��ʂ��C�Ή�����֐߂��쐬���C���̎Q�Ƃ�Ԃ��܂��D

\subsection joint_frame �v���O�ƃ\�P�b�g

���̂Ɗ֐߂̈ʒu�֌W�͉��̐}�̂悤�ɂȂ��Ă��܂��D

�� �֐߃t���[���̐} ��

�Е��̍��̂ɂ̓\�P�b�g�C�����Е��̍��̂ɂ̓v���O�ƌĂ΂����W�n�����t�����܂��D
�֐߂̎�ނɉ����āC�\�P�b�g�ƃv���O�̑��Έʒu�֌W���S������܂��D
�Ⴆ�΃q���W�̏ꍇ�C�\�P�b�g�ƃv���O�̌��_����v�����C�����ɗ��҂�Z�����m�����s�ƂȂ�悤�ɍS������܂��D

�\�P�b�g����уv���O�̎��t���ʒu��\link Spr::PHConstraintDesc::pose PHConstraintDesc::pose[2]\endlink��
�w�肵�܂��i���ۂɂ�PHConstraintDesc����h�������f�B�X�N���v�^���g���܂��j�D
pose[0]�̓\�P�b�g�����t���鍄�́iCreateJoint�̑�1����)�̍��W�n�ɑ΂���\�P�b�g�̈ʒu�E�X����\�킵�܂��D
���l��pose[1]�̓v���O�����t���鍄�́iCreateJoint�̑�2����)�̍��W�n�ɑ΂���v���O�̈ʒu�E�X����\�킵�܂��D

\subsection joint_enable �֐߂̗L�����E������

�֐߂��쐬���鏈���͔�r�I�n�C�R�X�g�ł��D���̂��߁C�ꎞ�I�Ɋ֐߂ɂ��S�����������C���΂炭����
���ɖ߂��悤�ȏ���������Ƃ��ɁC���̓s�x�֐߂̍쐬�E�폜���J��Ԃ����Ƃ͂����߂��܂���D
\link Spr::PHConstraintIf::Enable() PHConstraintIf::Enable\endlink���ĂԂ��Ƃɂ���Ċ֐߂��ꎞ�I��
������������C�ĂїL�������邱�Ƃ��ł��܂��D�܂��C���鎞�_�Ŋ֐߂��L�����ǂ�����
\link Spr::PHConstraintIf::IsEnabled() PHConstraintIf::IsEnabled\endlink�ĂԂ��ƂŒm�邱�Ƃ��ł��܂��D

\subsection joint_1dof 1���R�x�֐�

1���R�x�֐߂Ƃ́C1���R�x�̑��Ή^������������i�����������5�̎��R�x���S������j�֐߂ŁC
�ȉ��̂��̂�����܂��F
- \ref joint_hinge
- \ref joint_slider

�S�Ă�1���R�x�֐߂�PHJoint1DIf��e�N���X�Ɏ����C���ʂ̃C���^�t�F�[�X�ŏ�Ԃ̎擾�⑀�삪�\�ł��D

�֐ߕψʂ��擾����ɂ�\link Spr::PHJoint1DIf::GetPosition() PHJoint1DIf::GetPosition\endlink���Ăт܂��D
�֐ߕψʂ̒�`�͊֐߂̎�ނɈ˂�܂��D�Ⴆ�΃q���W�̏ꍇ�̓\�P�b�g�ƃv���O�̐����p�x�C�X���C�_�̏ꍇ��
�\�P�b�g�ɑ΂���v���O�̕��s�ړ��ʂɂȂ�܂��D
�֐ߑ��x�i�֐ߕψʂ̎��ԕω����j���擾����ɂ�\link Spr::PHJoint1DIf::GetVelocity() PHJoint1DIf::GetVelocity\endlink���Ăт܂��D

�֐ߗ͂�ݒ肷��ɂ�\link Spr::PHJoint1DIf::SetTorque() PHJoint1DIf::SetTorque\endlink���Ăт܂��D
�֐ߗ͂Ƃ́C1���R�x�֐߂̉^�������ɉ����ĉ�������͂ł��D�֐߂Ɏ��t����ꂽ���[�^���o���͂ƍl����Ε�����₷���ł��傤�D
���̂ɉ����́i\link Spr::PHSolidIf::AddForce() PHSolidIf::AddForce\endlink�Őݒ肳���j�ƈقȂ�C
�֐ߗ͂�\link Spr::PHSceneIf::Step() PHSceneIf::Step\endlink�����s���Ă��N���A����܂���D
���ݐݒ肳��Ă���֐ߗ͂��擾����ɂ�\link Spr::PHJoint1DIf::GetTorque() PHJoint1DIf::GetTorque\endlink���Ăт܂��D

\link Spr::PHJointIf::SetRange PHJointIf::SetRange\endlink���ĂԂ��Ƃŉ��͈͂𐧌��ł��܂��D

�֐߂Ƀo�l��_���p�����t���邱�Ƃ��ł��܂��D
�o�l�W�����擾�E�ݒ肷��ɂ�
\link Spr::PHJoint1DIf::GetSpring() PHJoint1DIf::GetSpring \endlink, 
\link Spr::PHJoint1DIf::SetSpring() PHJoint1DIf::SetSpring \endlink
���Ăт܂��D�܂��C�o�l�̊�ψʂ��擾�E�ݒ肷��ɂ�
\link Spr::PHJoint1DIf::GetSpringOrigin() PHJoint1DIf::GetSpringOrigin \endlink, 
\link Spr::PHJoint1DIf::SetSpringOrigin() PHJoint1DIf::SetSpringOrigin \endlink
���Ăт܂��D
�_���p�W�����擾�E�ݒ肷��ɂ�
\link Spr::PHJoint1DIf::GetDamper() PHJoint1DIf::GetDamper \endlink, 
\link Spr::PHJoint1DIf::SetDamper() PHJoint1DIf::SetDamper \endlink
���Ăт܂��D
�o�l�W����\f$K\f$�C�o�l�̊�ψʂ�\f$q_0\f$�C�_���p�W����\f$D\f$�Ƃ���ƁC������֐߃g���N\f$\tau\f$��
\f[
\tau = -K (q - q_0) - D \dot{q}
\f]
�ƕ\�킳��܂��D������\f$q\f$�͊֐ߕψʁC\f$\dot{q}\f$�͊֐ߑ��x�ł��D


\subsection joint_hinge �q���W

���q���W�̐}��

�q���W��1�{�̎�����̉�]�^������������֐߂ł��D
�\�P�b�g�ƃv���O�̌��_����v�����C����Z�����m�𓯂������ɍS�����܂��D

\subsection joint_slider �X���C�_

���X���C�_�̐}��

�X���C�_��1�����̕��s�ړ�����������֐߂ł��D
�\�P�b�g�ƃv���O�̌X������v�����C���v���O�̌��_���\�P�b�g��Z����ɍS�����܂��D

\section �{�[���W���C���g

���{�[���W���C���g�̐}��

�{�[���W���C���g��1�_�𒆐S�Ƃ���C�ӕ����̉�]����������3���R�x�֐߂ł��D
�\�P�b�g�ƃv���O�̌��_����v����悤�ɍS�����܂��D

*/