/** \page pagePhysics �����V�~�����[�V����SDK


\contents 

<!---------------------------------- �����V�~�����[�V����SDK�̏������ƏI�� ------------------------------------------>
\section sec_PHSdk 		�����V�~�����[�V����SDK�̏������ƏI��
�����V�~�����[�V����\c SDK �́C�����@���Ɋ�Â����������̂̃C���^���N�V�������v�Z����@�\��񋟂��܂��D<br>
�����V�~�����[�V����\c SDK �̎g�p�́C�O���[�o���֐� \link Spr::CreatePHSdk() CreatePHSdk \endlink ���ĂԂ��Ƃ���n�܂�܂��D
\verbatim
    PHSdkIf* sdk = CreatePHSdk();
\endverbatim
�ȍ~�C\c SDK �̑���� \link Spr::CreatePHSdk() CreatePHSdk\endlink �ɂ���ĕԂ��ꂽ \c PHSdkIf �^�̃|�C���^����čs���܂��D

\c SDK �́C1 �ȏ�̃V�[���������Ƃ��ł��܂��D
�܂��C1 �̃V�[���� 1 �ȏ�̍��� <tt>(PHSolid)</tt> �Ɗ֐� <tt>(PHJoint)</tt>�������Ƃ��ł��܂��D
����ɁC���̂͌`�� <tt>(CDConvexMesh�CCDSphere�CCDBox�C...)</tt> ���Q�Ƃ��Č`�������Ƃ��ł��܂��D
�֐߂� \c 2 �̍��̂����ѕt���܂��D
�����G���W�����̃I�u�W�F�N�g�̎Q�Ɗ֌W�̗��}�Ɏ����܂��D


	\image html PHSdkScene.png

\c SDK �̏I�������͓����Ŏ����I�ɍs���܂��D���[�U���疾���I�� \c SDK ���������K�v�͂���܂���D
<br><br>
<div align="right">
<a href="#pagePhysics">top of page</a>
</div> 
<hr>




<!---------------------------------- �V�[�� ------------------------------------------------------------------------->
\section scene 		�V�[��
�V�[���Ƃ͕����V�~�����[�V�����̕���ƂȂ��Ԃ̂��Ƃ������܂��D
�V�[�����쐬���邱�ƂŁC�I�u�W�F�N�g�𓊓����邱�Ƃ��\�ƂȂ�܂��D

\subsection sceneDesc		�V�[���f�X�N���v�^
�V�[���f�X�N���v�^����C�V�[�����쐬���邱�Ƃ��ł��܂��D
�ȉ��Ɏ����悤�Ȑݒ�������ݒ�̂܂ܗ��p����ꍇ�́C�V�[���f�X�N���v�^��p�ӂ���K�v�͂���܂���D
�f�X�N���v�^�ɂ��ẮC\ref pageApiBasic ���Q�Ɖ������D
�܂��C�V�[���f�X�N���v�^�̃T���v���Ƃ��ẮC
<a href="../../src/html/_2Samples_2BoxStack_2main_8cpp-example.html">BoxStack</a> ���Q�Ɖ������D

�V�[���f�X�N���v�^�̏����ݒ�ł́C�X�e�b�v���s���Ԃ� <tt>0.005[s]</tt> �ɐݒ肳��Ă��܂����C�ȉ��̂悤�ɐݒ肷�邱�Ƃ��ł��܂��D
\verbatim
    dscene.timeStep = 0.01;
\endverbatim
<br><br>


\subsection scene_create	�V�[���̍쐬
�V�[�����쐬����ɂ� \link Spr::PHSdkIf::CreateScene() PHSdkIf::CreateScene \endlink ���Ăт܂��D
�f�t�H���g�̐ݒ�ŃV�[�����쐬����ɂ́C�ȉ��̂悤�ɂ��܂��D
\verbatim
    //�f�t�H���g�̐ݒ�ŃV�[�����쐬
    PHSceneIf* scene = sdk->CreateScene();
\endverbatim

�܂��C�f�t�H���g�̃V�[���f�X�N���v�^�����蓖�Ă邱�Ƃł��C�f�t�H���g�ݒ�ɂ�����V�[���̍쐬���s���܂��D
\verbatim
    //�f�t�H���g�̐ݒ�ŃV�[�����쐬
    PHSceneDesc sceneDesc;
    PHSceneIf* scene = sdk->CreateScene(sceneDesc);
\endverbatim

���l�ɁC \link Spr::PHSdkIf::CreateScene() PHSdkIf::CreateScene \endlink �ɑ΂��āC
�C�ӂ̃V�[���f�X�N���v�^��^����΁C���̃V�[���f�X�N���v�^�ɉ������V�[�����쐬���邱�Ƃ��ł��܂��D
�V�[���f�X�N���v�^�ɂ��ẮC\ref sceneDesc ���Q�Ɖ������D
\verbatim
    PHSceneDesc sceneDesc;
    dscene.timeStep      = 0.05;                                // �X�e�b�v���� 0.05[s] �ɐݒ�
    PHSceneIf* scene     = sdk->CreateScene(sceneDesc);
\endverbatim

�Ȃ��C�V�[���̑���� \link Spr::PHSdkIf::CreateScene() PHSdkIf::CreateScene \endlink �ɂ���ĕԂ��ꂽ
 \c PHSceneIf �^�̃|�C���^����čs�Ȃ��܂��D<br>
�V�[���𕡐��쐬���邱�Ƃ͉\�ł����C�قȂ�V�[�����m�݂͌��ɉe�����y�ڂ��܂���D
<br><br><br>


\subsection scene_step 		�V�~�����[�V�����̎��s
�V�[���̎�����i�߂�ɂ� \link Spr::PHSceneIf::Step() PHSceneIf::Step \endlink ���Ăт܂��D
\verbatim
    //0.05 * 100 = 5�b�Ԃ̃V�~�����[�V����
    scene->SetTimeStep(0.05);
    for(int i = 0; i < 100; i++)  scene->Step();
\endverbatim

\link Spr::PHSceneIf::Step() Step\endlink �̗���͑�̈ȉ��̂悤�ɂȂ��Ă��܂��D<br>
�@�@ \c 1�D���̓��m�̏Փ˔���  			<br>
�@�@ \c 2�D�ڐG�́C�֐߂̍S���͂̌v�Z	<br>
�@�@ \c 3�D���̂̑��x�E�ʒu�̍X�V		<br>

��x�� \link Spr::PHSceneIf::Step() Step \endlink �̌Ăяo���Ői�ގ��Ԃ̕���
 \link Spr::PHSceneIf::GetTimeStep() PHSceneIf::GetTimeStep \endlink �Ŏ擾�C
 \link Spr::PHSceneIf::SetTimeStep() PHSceneIf::SetTimeStep \endlink �Őݒ�
���邱�Ƃ��ł��܂����C�����Őݒ肷��l�͂��̂܂ܐ��l�ϕ��̐ϕ��X�e�b�v�ƂȂ�܂��̂ŁC
\b ���܂�傫�Ȓl�i�Ⴆ�΂P�b�j�͐ݒ肵�Ȃ��ŉ������D<br>
�܂��C\ref paseBasicWord �́u�X�e�b�v���v���Q�Ɖ������D
<br><br><br>


\subsection scene_gravity 		�d�͂̐ݒ�
�V�[���ɓ����d�͂�ݒ肷��ɂ� \link Spr::PHSceneIf::SetGravity() PHSceneIf::SetGravity \endlink ���Ăт܂��D
\verbatim
    scene->SetGravity(Vec3f(0.0f, -9.8f, 0.0f));
\endverbatim

�f�t�H���g�ł́C�V�[���̏d�͉����x�� <tt>Vec3f(0.0f, -9.8f, 0.0f)</tt> �ɐݒ肳��Ă��܂��D<br>
���d�͋�ԂƂ���ɂ́C�ȉ��̂悤�ɂ��܂��D
\verbatim
    scene->SetGravity(Vec3f(0.0f, 0.0f, 0.0f));
\endverbatim

�܂��C�X�̍��̂ɑ΂��ďd�͂̍�p��L���E����������ɂ�
 \link Spr::PHSolidIf::SetGravity() PHSolidIf::SetGravity \endlink ���g���܂��D
\verbatim
    //solid��scene�ɓo�^����Ă��鍄�̂Ƃ���
    solid->SetGravity(false);
\endverbatim
<br><br>
<div align="right">
<a href="#pagePhysics">top of page</a>
</div> 
<hr>




<!---------------------------------- ���� --------------------------------------------------------------------------->
\section solid	����
���̂Ƃ́C�����@���ɏ]���ăV�[���̒����^�����镨�̂ŁC���ɁC�O����͂��������Ƃ��ɕό`�������ł���d�����̂������܂��D
���̂ɑ΂��ẮC�d�S�C�p���C�����Ȃǂ�ݒ肷�邱�Ƃ��ł��܂��D

\subsection solidDesc	���̃f�X�N���v�^
���̃f�X�N���v�^���獄�̂��쐬���邱�Ƃ��ł��܂��D<br>
�ȉ��Ɏ����悤�Ȑݒ�������l�̂܂ܗ��p����ꍇ�́C���̃f�X�N���v�^��p�ӂ���K�v�͂���܂���D
�����ł́C���ʁC�����e���\���C���ʒ��S�̈ʒu�C�d�͂̉e���̗L����ݒ肷�邱�Ƃ��ł��܂��D<br>
�����̐ݒ�́C���́i\link Spr::PHSceneIf::CreateSolid() PHSceneIf::CreateSolid \endlink ��������
���̃I�u�W�F�N�g�j����ł��ݒ�\�ł��D���̂���̐ݒ�ɂ��ẮC\ref solid_inertia_parameter ���Q�Ɖ������D
�܂��C�f�X�N���v�^�ɂ��ẮC\ref pageApiBasic ���Q�Ɖ������D

���ʂ́C�����l�Ƃ��� \c 1.0 ���ݒ肳��Ă��܂��D
���̃f�X�N���v�^�ŁC���̂ɑ΂��鎿�ʂ�ݒ肷��ɂ́C�ȉ��̂悤�ɂ��܂��D
\verbatim
    PHSolidDesc desc;
    desc.mass = 2.0;                            // ���� 2.0 �ɐݒ�
\endverbatim

�����e���\���́C�����l�Ƃ��ĒP�ʍs��ɐݒ肳��Ă��܂��D
���̃f�X�N���v�^�ŁC���̂ɑ΂��銵���e���\����ݒ肷��ɂ́C�ȉ��̂悤�ɂ��܂��D
\verbatim
    desc.inertia = 2.0 * Matrix3d::Unit();      // �Ίp������ 2.0 �̍s��ɐݒ�
\endverbatim

���̍��W�n�i���[�J�����W�n�j�ɂ����鎿�ʒ��S�̈ʒu�́C�����ݒ�ł́C���̍��W�n�̌��_�ƈ�v���Ă��܂��D
���̃f�X�N���v�^�ŁC���̈ʒu��ݒ肷��ɂ́C�ȉ��̂悤�ɂ��܂��D
\verbatim
    desc.center = Vec3d(0.0, 1.0, 0.0);         // ��� 1.0 ���炷
\endverbatim

�����ݒ�ł́C�X�̍��̂ɑ΂��C�d�͂̉e�����󂯂�悤�ɐݒ肳��Ă��܂��D
���̃f�X�N���v�^�ŁC���̂ɑ΂���d�͂̉e���̗L����ݒ肷��ɂ́C�ȉ��̂悤�ɂ��܂��D
\verbatim
    desc.gravity = false;                       // �d�͂̉e�����󂯂Ȃ��悤�ɐݒ�
\endverbatim
�Ȃ��C���̂���̏d�͐ݒ�ɂ��ẮC\ref scene_gravity ���Q�Ƃ��ĉ������D
<br><br><br>


\subsection solid_create 	���̂̍쐬
���̂��쐬���C�V�[���ɒǉ�����ɂ� \link Spr::PHSceneIf::CreateSolid() PHSceneIf::CreateSolid \endlink ���Ăт܂��D
�f�t�H���g�̐ݒ�ŁC���̂��쐬����ɂ́C�ȉ��̂悤�ɂ��܂��D
\verbatim
    PHSolidIf* solid;
    solid = scene->CreateSolid();
\endverbatim

�܂��C�f�t�H���g�̍��̃f�X�N���v�^�����蓖�Ă邱�Ƃł��C�f�t�H���g�ݒ�ɂ����鍄�̂̍쐬���s���܂��D
\verbatim
    //�f�t�H���g�̐ݒ�ō��̂��쐬
    PHSolidDesc solidDesc;
    PHSolidIf* solid = scene->CreateSolid(solidDesc);
\endverbatim

���l�ɁC\link Spr::PHSceneIf::CreateSolid() PHSceneIf::CreateSolid \endlink �ɑ΂��āC
�C�ӂ̍��̃f�X�N���v�^��^����΁C���̍��̃f�X�N���v�^�ɉ��������̂��쐬���邱�Ƃ��ł��܂��D
���̃f�X�N���v�^�ɂ��ẮC\ref solidDesc ���Q�Ɖ������D
\verbatim
    PHSolidDesc solidDesc;
    solidDesc.mass    = 0.1;                                   // ���� 0.1 �ɐݒ�
    solidDesc.inertia = 0.1 * Matrix3d::Unit();                // �Ίp������ 0.1 �̍s��ɐݒ�
    solidDesc.center  = Vec3d(0.1, 0.0, 0.0);                  // �d�S���E�� 0.1 ���炷
    PHSolidIf* solid  = scene->CreateSolid(solidDesc);
\endverbatim

�Ȃ��C���̂ɑ΂��鑀��� \link Spr::PHSceneIf::CreateSolid() PHSceneIf::CreateSolid \endlink �ɂ���ĕԂ��ꂽ
 \c PHSolidIf �^�̃|�C���^����čs���܂��D
<br><br><br>


\subsection solid_pose ���̂̈ʒu
�ʏ�C�V�~�����[�V�������s���ɂ́C���̕��̂��ǂ��ɂ��邩�Ƃ�����񂪕K�v�ɂȂ�܂��D
���̏��Ƃ����̂��C�ʒu�ł��D�ʒu�ɂ��C���̂���Ԃ̂ǂ̈ʒu�ɂ��邩���������Ƃ��ł��܂��D
1 �̍��̂ɂ́C1 �̍��W�n���\��t���Ă��܂��D��������̍��W�n�ƌĂт܂��D
�V�[�����̍��̂̈ʒu�ƌX���́C�V�[�����W�n�ɑ΂��鍄�̍��W�n�̈ʒu�ƌX���Ƃ��ĕ\������܂��D

	\image html scene.png

���̂̈ʒu��ݒ�E�擾����ɂ́C 
\link Spr::PHSolidIf::SetFramePosition() PHSolidIf::SetFramePosition \endlink �� 
\link Spr::PHSolidIf::GetFramePosition() PHSolidIf::GetFramePosition \endlink 
���Ăт܂��D
\verbatim	
    solid->SetFramePosition(Vec3f(0,-3,0));                     // ���̂̈ʒu��(0,-3,0)�ɐݒ�
    Vec3d pos = solid->GetFramePosition();                      // ���̂̈ʒu���擾    
\endverbatim

���͎̂��_�Ƃ͈Ⴂ�C�傫���������Ă���̂ŁC���̂̌X�����l�����Ȃ��Ă͂Ȃ�܂���D<br>
���̂̌X���i�����j��ݒ�E�擾����ɂ́C
\link Spr::PHSolidIf::SetOrientation() PHSolidIf::SetOrientation \endlink ��
\link Spr::PHSolidIf::GetOrientation() PHSolidIf::GetOrientation \endlink 
���Ăт܂��D
\verbatim	
    solid->SetOrientation(Quaternionf::Rot(Rad(30), 'z'));      // z������30�x��]
    Quaternion ori = solid->GetOrientation();                   // ���̂̌X�����擾
\endverbatim

�܂��C���̂̈ʒu�E�X�����C�����ɐݒ�E�擾���邱�Ƃ��ł��܂��D
����� \c Posed �^�ŕ\����܂��D \c Posed �^�́C \c Affine �Ɠ����@�\������ 7 �����x�N�g���ŁC
�ʒu�ƌX�������킹���x�N�g���ƂȂ��Ă��܂��D
���̈ʒu�ƌX�������킹���������C�ݒ�E�擾����ɂ́C
\link Spr::PHSolidIf::SetPose() PHSolidIf::SetPose \endlink ��
\link Spr::PHSolidIf::GetPose() PHSolidIf::GetPose \endlink 
���g���܂��D
\verbatim
    Posed p = Posed::Rot(Rad(30.0), 'z');       // z������30�x��]������Posed�^p��p�ӂ���
    solid->SetPose(p);                          // Posed�^p�����̂̎p���Ƃ��Đݒ肷��
    Posed pose = solid->GetPose();              // ���̂̎p�����擾
\endverbatim

�V�[�����W�n�ɂ����鍄�̂̏d�S��ݒ�E�擾����ɂ́C
\link Spr::PHSolidIf::SetCenterPosition() PHSolidIf::SetCenterPosition \endlink ��
\link Spr::PHSolidIf::GetCenterPosition() PHSolidIf::GetCenterPosition \endlink 
���Ăт܂��D
\verbatim	
    Vec3d center = Vec3d(0.0, 1.0, 0.0);                        // �d�S���������1m��ɂ��炷
    solid->SetCenterPosition(center);                           // �d�S��ݒ�
    Vec3d center = solid->GetCenterPosition();                  // �d�S���擾
\endverbatim
<br><br>

\subsection solid_velocity 			���̂̑��x
���̂̑��x�́C�V�[�����W�n����ϑ������C���̍��W�n���_�̑��x�܂莿�ʒ��S�̑��x�ŕ\�킳��܂��D
�Ȃ��C���x�́C \c Vec3d �ŕ\�����x�N�g���i���x�x�N�g���j�ƂȂ�܂��D
���̂̑��x��ݒ�E�擾����ɂ́C
\link Spr::PHSolidIf::SetVelocity() PHSolidIf::SetVelocity \endlink ��
\link Spr::PHSolidIf::GetVelocity() PHSolidIf::GetVelocity \endlink 
���Ăт܂��D
\verbatim	
    solid->SetVelocity(Vec3d(-20.0, 0.0, 0.0));�@�@             // ���x�̐ݒ�
    vec3d velocity = solid->GetVelocity();                      // ���x�̎擾
\endverbatim

���l�ɍ��̂̊p���x��
\link Spr::PHSolidIf::SetAngularVelocity() PHSolidIf::SetAngularVelocity \endlink ��
\link Spr::PHSolidIf::GetAngularVelocity() PHSolidIf::GetAngularVelocity \endlink 
���ĂԂ��ƂŐݒ�E�擾�ł��܂��D
���̂̊p���x�́C�V�[�����W�n����ϑ��������̍��W�n�̊p���x�ł��D
\verbatim	
    solid->SetAngularVelocity(Vec3d(1.0, 1.0, 1.0));            // �V�[���ɑ΂���p���x��ݒ�
    vec3d velocity = solid->GetAngularVelocity();               // �p���x�̎擾
\endverbatim
<br><br>

\subsection solid_inertia_parameter 		���̂̊����p�����[�^
���̂̎��ʓ����́C���ʂƁC���̍��W�n�ɂ����鍄�̂̎��ʒ��S�ʒu�C�܂������e���\���ŕ\������܂��D
�����́C���̂ɑ΂��C�ȉ��̂悤�ɐݒ肷�邱�Ƃ��\�ł����C���̃f�X�N���v�^����^���邱�Ƃ��ł��܂��D
���̃f�X�N���v�^����̐ݒ���@�ɂ��ẮC\ref solidDesc ���Q�Ɖ������D

���̂̎��ʂ�ݒ�E�擾����ɂ́C
\link Spr::PHSolidIf::SetMass() PHSolidIf::SetMass \endlink �� 
\link Spr::PHSolidIf::GetMass() PHSolidIf::GetMass \endlink 
���Ăт܂��D
\verbatim	
    solid->SetMass(2.0);                                // ���ʂ� 2.0 �ɐݒ�
    double mass = solid->GetMass();                     // ���ʂ��擾
\endverbatim

�d�S�ʒu�̓f�t�H���g�ł͍��̍��W�n�̌��_�ƈ�v���Ă��܂����C 
\link Spr::PHSolidIf::GetCenterOfMass() PHSolidIf::GetCenterOfMass \endlink �Ŏ擾���邱�Ƃ��ł��܂��D
�܂��A\link Spr::PHSolidIf::SetCenterOfMass() PHSolidIf::SetCenterOfMass \endlink ���g���A�ݒ肷�邱�Ƃ��ł��܂��D
\verbatim
    //���ʒ��S���C���̍��W���_����x������0.1���炷
	Vec3d center = solid->GetCenterOfMass();
	center.x += 0.1;
	solid->SetCenterOfMass(center);
\endverbatim

�����e���\����ݒ�E�擾����ɂ́C
\link Spr::PHSolidIf::SetInertia() PHSolidIf::SetInertia \endlink �� 
\link Spr::PHSolidIf::GetInertia() PHSolidIf::GetInertia \endlink 
���Ăт܂��D
\verbatim	
    solid->SetInertia(2.0 * Matrix3d::Unit());          // �Ίp������ 2.0 �̍s���ݒ�
    Matrix3d inertia = solid->GetInertia();             // �����e���\�����擾
\endverbatim
<br><br>

\subsection solid_force 		���̂֗͂�������
���̂ɍ�p����͂ɂ́C
-# �O��
-# �ڐG��
-# �֐߂̍S����
-# �֐߂̃��[�^�g���N
������܂��D

���̂֕��i�͂�������ɂ� \link Spr::PHSolidIf::AddForce() PHSolidIf::AddForce \endlink�C
�g���N��������ɂ� \link Spr::PHSolidIf::AddTorque() PHSolidIf::AddTorque \endlink ���Ăт܂��D<br>
�Ⴆ�΁C���̂ɑ΂��āC���}�̂悤�ȗ͂��������ꍇ���l���܂��D
	\image html force.jpg
���̂ɗ͂���p����ꍇ���l���C���ʒ��S�ɉE������ \c 10N �̗͂�������ɂ́C�ȉ��̂悤�ɐݒ肷�邱�Ƃ��ł��܂��D
\verbatim	
    solid->AddForce(Vec3f(10, 0, 0);              �@�@�@// ���ʒ��S�ɉE������10N�̗͂�������
\endverbatim

���ʒ��S�����łȂ��C�ʒu���w�肵�āC�͂������邱�Ƃ��\�ł��D
\verbatim    
    solid->AddForce(f, r);                                                          // �͂� �ʒur(World�n) �ɉ�����
    solid->AddForce(Vec3f(10,0,0), Vec3f(0,2,0)+solid1->GetCenterPosition());       // �d�S��2m����E������10N�̗͂�������
\endverbatim

�܂��C���̂ɑ΂��ăg���N��������ɂ́C
�ȉ��̂悤��\link Spr::PHSolidIf::AddTorque() PHSolidIf::AddTorque \endlink ���Ăт܂��D
\verbatim
    solid->AddTorque(f);                                // �g���Nf��������
    solid->AddTorque(Vec3d(0,0,0));                     // �g���N��������
\endverbatim

�������C���̂ɉ������Ă���́E�g���N�́C\link Spr::PHSceneIf::Step() PHSceneIf::Step \endlink ��
�Ăԓx�� 0 �ɃN���A����邱�Ƃɒ��ӂ��ĉ������D
���̂��߁C���I�ɗ́E�g���N�����������邽�߂ɂ́C\link Spr::PHSceneIf::Step() PHSceneIf::Step \endlink ���ĂԑO�ɖ���
  \link Spr::PHSolidIf::AddForce() PHSolidIf::AddForce \endlink \c /
  \link Spr::PHSolidIf::AddTorque() PHSolidIf::AddTorque \endlink ���ĂԕK�v������܂��D
<br><br><br>

\subsection solid_dynamical �����@���̉e�����󂯂Ȃ��悤�ɂ���
�ꍇ�ɂ���āC���̍��̂Ƃ̐ڐG��d�͂���̉e�����󂯂Ȃ����̂���肽���Ȃ邱�Ƃ�����܂��D
���̂悤�ȍ��̂�񓮗͊w�I�ȍ��̂ƌĂт܂��D
���̂�񓮗͊w�I�Ƃ�����C���ɖ߂����肷��ɂ� \link Spr::PHSolidIf::SetDynamical() PHSolidIf::SetDynamical \endlink ���Ăт܂��D
�񓮗͊w�I�Ƃ��邱�Ƃ��L���� 1 �̓T�^�I�ȗ�̓V�[���̒n�ʂł��D
���ʁC�n�ʂ͋�ԂɌŒ肳��Ă��āC���̍��̂Ƃ̏Փ˂�d�͂ɂ���Ĉʒu��ω������Ȃ����Ƃ����߂��܂��D
����́C
\verbatim
    floor->SetDynamical(false);		//floor��PHSolidIf*�^�̕ϐ�
\endverbatim
�Ƃ��邱�ƂŎ����ł��܂��D
�܂��C���鍄�̂��񓮗͊w�I���ǂ����� \link Spr::PHSolidIf::IsDynamical() PHSolidIf::IsDynamical \endlink ���ĂԂ��ƂŎ擾�ł��܂��D<br>
�񓮗͊w�I�ȍ��̂ł��ʒu�C�X���C���x�C�p���x�̕ύX�͉\�ł��D
<br><br><br>


\subsection solid_shape 		���̂ւ̌`��̊��蓖��
���̂ɂ� 1 �ȏ�̌`������蓖�Ă邱�Ƃ��ł��܂��D
�����V�~�����[�V����\c SDK �́C�`��̌��������m���C
���̊Ԃɓ����ڐG�͂������I�Ɍv�Z���܂��D
���̂Ɋ��蓖�Ă邱�Ƃ̂ł���`��̎�ނ���э쐬���@�ɂ��Ă� \ref shape ���Q�Ɖ������D

���̂Ɍ`������蓖�Ă�ɂ� \link Spr::PHSolidIf::AddShape() PHSolidIf::AddShape \endlink ���g���܂��D
���̂́C���蓖�Ă�ꂽ�`��i\c CDShape �^�̃I�u�W�F�N�g�j�ւ̎Q�Ƃ�ێ����邾���ł��̂ŁC
\b 1�̌`��𕡐��̈قȂ鍄�̂Ɋ��蓖�ĂĂ���肠��܂���D 
����ɂ���āC�����̓����`�̍��̂��쐬�������ꍇ�Ƀ�������ߖ�ł��܂��D
�܂��C1 �̍��̂ɓ��� \c CDShape �I�u�W�F�N�g�𕡐���o�^����ƁC���ꂼ��ʂ̌`��Ƃ��ĔF������܂��D

�`������̂Ɏ��t����ʒu���擾�E�ݒ肷��ɂ́C
\link Spr::PHSolidIf::GetShapePose() PHSolidIf::GetShapePose \endlink�C
\link Spr::PHSolidIf::SetShapePose() PHSolidIf::SetShapePose \endlink���Ăт܂��D
<br><br>
<div align="right">
<a href="#pagePhysics">top of page</a>
</div> 
<hr>




<!---------------------------------- �`�� --------------------------------------------------------------------------->
\section shape		�`��
�`���p�ӂ��Ă����ƁC�����`���V���� �쐬������C�R�s�[�����肷��K�v���Ȃ��C
���f�����O�������I�ɍs�����Ƃ��ł��܂��D\ref solid_shape ���Q�Ɖ������D<br>
���̂Ɋ��蓖�Ă邱�Ƃ��ł���`��ɂ͈ȉ��̂��̂�����܂��D
- \ref shape_mesh
- \ref shape_sphere
- \ref shape_box�@
    	
�������Ƃ̂ł���`��͓ʌ`��݂̂ł��D
�ʌ`��Ƃ́C�ꌾ�ł����Ή��݂⌊�̖����`��̂��Ƃł��D
�ʂłȂ��i���݂⌊�����j�`������̂Ɋ��蓖�Ă�ɂ́C���炩���߂��̌`���
�����̓ʌ`��ɕ������Ċ��蓖�Ă�K�v������܂��D


\subsection shape_mesh 		���b�V�� 
�e�ӂ̒����� \c 2 �̗����̂��쐬������@���ȉ��Ɏ����܂��D<br>
\c 1 �̃��b�V���`���p�ӂ��C����Ɋ�Â��C���̂� \c 2 �쐬���Ă��܂��D
\verbatim
    CDConvexMeshDesc md;                                                    // ���b�V���f�B�X�N���v�^(md)
    md.vertices.push_back(Vec3f(-1,-1,-1));�@�@                             // �����̂̒��_���W��o�^
    md.vertices.push_back(Vec3f(-1,-1, 1));	
    md.vertices.push_back(Vec3f(-1, 1,-1));	
    md.vertices.push_back(Vec3f(-1, 1, 1));
    md.vertices.push_back(Vec3f( 1,-1,-1));	
    md.vertices.push_back(Vec3f( 1,-1, 1));
    md.vertices.push_back(Vec3f( 1, 1,-1));
    md.vertices.push_back(Vec3f( 1, 1, 1));
    	
    // ���� solid1
    CDConvexMeshIf* mesh1 = DCAST(CDConvexMeshIf, sdk->CreateShape(md));�@�@// md �Ɋ�Â��āC���b�V���`��(mesh1)���쐬
    PHSolidIf* solid1 = scene->CreateSolid();                               // ����(solid1)���쐬
    solid1->AddShape(mesh1);                                                // ����(solid1)�ɁC���b�V���`��(mesh1)�����蓖�Ă�
    
    // ���� solid2
    CDConvexMeshIf* mesh2 = DCAST(CDConvexMeshIf, sdk->CreateShape(md));�@�@// md �Ɋ�Â��āC���b�V���`��(mesh2)���쐬
    PHSolidIf* solid2 = scene->CreateSolid();                               // ����(solid2)���쐬
    solid2->AddShape(mesh2);                                                // ����(solid2)�ɁC���b�V���`��(mesh2)�����蓖�Ă�
\endverbatim
<br><br>    	
    	
\subsection shape_sphere 	����
���a�� \c 1 �̋��̂��쐬������@���ȉ��Ɏ����܂��D<br>
\c 1 �̋��̌`���p�ӂ��C����Ɋ�Â��C���̂� \c 2 �쐬���Ă��܂��D
\verbatim
    CDSphereDesc sd;                                                         // ���̃f�B�X�N���v�^(sd)
    sd.radius = 1.0;                                                         // ���̂̔��a
    
    CDSphereIf* sphere1 = DCAST(CDSphereIf, sdk->CreateShape(sd));           // sd �Ɋ�Â���, ���̌`��(sphere1)���쐬
    PHSolidIf* solid1 = scene->CreateSolid();                                // ����(solid1)���쐬
    solid1->AddShape(sphere1)                                                // ����(solid1)�ɁC���̌`��(sphere1)�����蓖�Ă�

    CDSphereIf* sphere2 = DCAST(CDSphereIf, sdk->CreateShape(sd));           // sd �Ɋ�Â���, ���̌`��(sphere2)���쐬
    PHSolidIf* solid2 = scene->CreateSolid();                                // ����(solid2)���쐬
    solid2->AddShape(sphere2)                                                // ����(solid2)�ɁC���̌`��(sphere2)�����蓖�Ă�
\endverbatim
<br><br>    	

\subsection shape_box	 	������ 
\c X �����̒����� \c 1�CY �����̒��� \c 2�C\c Z �����̒����� \c 3 �̒����̂��쐬������@���ȉ��Ɏ����܂��D<br>
\c 1 �̒����̌`���p�ӂ��C����Ɋ�Â��C���̂� \c 2 �쐬���Ă��܂��D
\verbatim    
    CDBoxDesc bd;                                                            // �����̃f�B�X�N���v�^(bd);
    bd.boxsize = Vec3f(1,2,3);
    	
    CDBoxIf* box1 = DCAST(CDBoxIf, sdk->CreateShape(bd));                    // bd �Ɋ�Â��āC�����̌`��(box1)���쐬
    PHSolidIf* solid1 = scene->CreateSolid();                                // ����(solid1)���쐬
    solid1->AddShape(box1)                                                   // ����(solid1)�ɁC�����̌`��(box1)�����蓖�Ă�
    	
    CDBoxIf* box2 = DCAST(CDBoxIf, sdk->CreateShape(bd));                    // bd �Ɋ�Â��āC�����̌`��(box2)���쐬
    PHSolidIf* solid2 = scene->CreateSolid();                                // ����(solid2)���쐬
    solid2->AddShape(box2)                                                   // ����(solid2)�ɁC�����̌`��(box2)�����蓖�Ă�
\endverbatim
<br><br>
<div align="right">
<a href="#pagePhysics">top of page</a>
</div> 
<hr>




<!---------------------------------- �֐� --------------------------------------------------------------------------->
\section joint 		�֐�
�֐߂Ƃ́C2�̍��̂̑��Ή^���ɍS������������̂ł��D
2�̍��̂̑��Ή^���́C�S�������݂��Ȃ��ꍇ�͕��i3���R�x�C��]3���R�x�̌v6�̎��R�x�������܂��D
���̓��ǂ̎��R�x���S�����邩�ɉ����ėl�X�Ȋ֐߂̎�ނ��l�����܂��D
Springhead���T�|�[�g����֐߂̎�ނ��ȉ��Ɏ����܂��D
	\image html joints.png
<br>


\subsection joint_create	�֐߂̍쐬
�֐߂��쐬���C��̍��̂�A������T�^�I�ȃR�[�h���ȉ��Ɏ����܂��D
\verbatim
    PHHingeJointDesc descJoint;                         //�f�B�X�N���v�^���쐬
    descJoint.pose[0].Pos() = Vec3d( 1.0, 0.0, 0.0);    //�\�P�b�g�̈ʒu��ݒ�
    descJoint.pose[1].Pos() = Vec3d(-1.0, 0.0, 0.0);    //�v���O�̈ʒu��ݒ�
    scene->CreateJoint(solid0, solid1, descJoint));     //�֐߂��쐬
\endverbatim

�e�֐߂̃f�B�X�N���v�^�́C�Ή�����֐߃N���X������ If ���Ƃ��� Desc ���������O�������C
�ȉ��̂悤�Ȍp���֌W�ɂ���܂��D

�@�@�@�@�@�@�@�@�@�@�@�@���f�B�X�N���v�^�̌p���֌W�� 


�֐߂��쐬����ɂ́C�n�߂ɍ쐬�������֐߂̎�ނɑΉ�����f�B�X�N���v�^���쐬���C
�K�v�Ȃ�΃f�B�X�N���v�^�̃v���p�e�B��ݒ肵�܂��D
���ɁC\link Spr::PHSceneIf::CreateJoint() PHSceneIf::CreateJoint \endlink �֐����C
�A�����������̂ւ̎Q�ƂƁC��ɍ쐬�����f�B�X���v�^�������Ƃ��ČĂт܂��D
\link Spr::PHSceneIf::CreateJoint() PHSceneIf::CreateJoint \endlink �́C�n���ꂽ�f�B�X�N���v�^��
��ނ�����Ŕ��ʂ��C�Ή�����֐߂��쐬���C���̎Q�Ƃ�Ԃ��܂��D
<br><br><br>


\subsection joint_frame 	�v���O�ƃ\�P�b�g 
���̂Ɗ֐߂̈ʒu�֌W�͉��̐}�̂悤�ɂȂ��Ă��܂��D

�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�� �֐߃t���[���̐} ��
	
�Е��̍��̂ɂ̓\�P�b�g�C�����Е��̍��̂ɂ̓v���O�ƌĂ΂����W�n�����t�����܂��D
�֐߂̎�ނɉ����āC�\�P�b�g�ƃv���O�̑��Έʒu�֌W���S������܂��D
�Ⴆ�΃q���W�̏ꍇ�C�\�P�b�g�ƃv���O�̌��_����v�����C�����ɗ��҂�Z�����m�����s�ƂȂ�悤�ɍS������܂��D

�\�P�b�g����уv���O�̎��t���ʒu�� \link Spr::PHConstraintDesc::pose PHConstraintDesc::pose[2] \endlink ��
�w�肵�܂��i���ۂɂ� PHConstraintDesc ����h�������f�B�X�N���v�^���g���܂��j�D 
pose[0] �̓\�P�b�g�����t���鍄�́iCreateJoint �̑�1����)�̍��W�n�ɑ΂���\�P�b�g�̈ʒu�E�X����\�킵�܂��D 
���l�� pose[1] �̓v���O�����t���鍄�́iCreateJoint �̑�2����)�̍��W�n�ɑ΂���v���O�̈ʒu�E�X����\�킵�܂��D
<br><br><br>


\subsection joint_enable 	�֐߂̗L�����E������
�֐߂��쐬���鏈���͔�r�I�n�C�R�X�g�ł��D���̂��߁C�ꎞ�I�Ɋ֐߂ɂ��S�����������C���΂炭����
���ɖ߂��悤�ȏ���������Ƃ��ɁC���̓s�x�֐߂̍쐬�E�폜���J��Ԃ����Ƃ͂����߂��܂���D 
\link Spr::PHConstraintIf::Enable() PHConstraintIf::Enable \endlink ���ĂԂ��Ƃɂ���Ċ֐߂��ꎞ�I��
������������C�ĂїL�������邱�Ƃ��ł��܂��D�܂��C���鎞�_�Ŋ֐߂��L�����ǂ����� 
\link Spr::PHConstraintIf::IsEnabled() PHConstraintIf::IsEnabled \endlink �ĂԂ��ƂŒm�邱�Ƃ��ł��܂��D
<br><br><br>


\subsection joint_1dof 		1���R�x�֐�
1 ���R�x�֐߂Ƃ́C1 ���R�x�̑��Ή^������������i�����������\c 5 �̎��R�x���S������j�֐߂ŁC
�ȉ��̂��̂�����܂��F
- \ref joint_hinge
- \ref joint_slider

�S�Ă�1���R�x�֐߂� PHJoint1DIf ��e�N���X�Ɏ����C���ʂ̃C���^�t�F�[�X�ŏ�Ԃ̎擾�⑀�삪�\�ł��D

�֐ߕψʂ��擾����ɂ�\link Spr::PHJoint1DIf::GetPosition() PHJoint1DIf::GetPosition\endlink���Ăт܂��D
�֐ߕψʂ̒�`�͊֐߂̎�ނɈ˂�܂��D�Ⴆ�΃q���W�̏ꍇ�̓\�P�b�g�ƃv���O�̐����p�x�C�X���C�_�̏ꍇ��
�\�P�b�g�ɑ΂���v���O�̕��s�ړ��ʂɂȂ�܂��D
�֐ߑ��x�i�֐ߕψʂ̎��ԕω����j���擾����ɂ� 
\link Spr::PHJoint1DIf::GetVelocity() PHJoint1DIf::GetVelocity \endlink ���Ăт܂��D

�֐ߗ͂�ݒ肷��ɂ� \link Spr::PHJoint1DIf::SetTorque() PHJoint1DIf::SetTorque \endlink ���Ăт܂��D
�֐ߗ͂Ƃ́C1���R�x�֐߂̉^�������ɉ����ĉ�������͂ł��D
�֐߂Ɏ��t����ꂽ���[�^���o���͂ƍl����Ε�����₷���ł��傤�D
���̂ɉ����́i\link Spr::PHSolidIf::AddForce() PHSolidIf::AddForce\endlink�Őݒ肳���j�ƈقȂ�C
�֐ߗ͂� \link Spr::PHSceneIf::Step() PHSceneIf::Step\endlink �����s���Ă��N���A����܂���D
���ݐݒ肳��Ă���֐ߗ͂��擾����ɂ� \link Spr::PHJoint1DIf::GetTorque() PHJoint1DIf::GetTorque \endlink ���Ăт܂��D

\link Spr::PHJointIf::SetRange PHJointIf::SetRange\endlink ���ĂԂ��Ƃŉ��͈͂𐧌��ł��܂��D

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
<br><br><br>


\subsection joint_hinge 	�q���W

\image html hinge.png

�q���W��1�{�̎�����̉�]�^������������֐߂ł��D
�\�P�b�g�ƃv���O�̌��_����v�����C����Z�����m�𓯂������ɍS�����܂��D
<br><br><br>


\subsection joint_slider �X���C�_

\image html slider.png

�X���C�_�� 1 �����̕��s�ړ�����������֐߂ł��D
�\�P�b�g�ƃv���O�̌X������v�����C���v���O�̌��_���\�P�b�g�� Z ����ɍS�����܂��D
<br><br><br>


\subsection joint_boll  �{�[���W���C���g

\image html balljoint.png

�{�[���W���C���g��  1�_�𒆐S�Ƃ���C�ӕ����̉�]���������� 3 ���R�x�֐߂ł��D
�\�P�b�g�ƃv���O�̌��_����v����悤�ɍS�����܂��D

<br><br>
<div align="right">
<a href="#pagePhysics">top of page</a>
</div> 


*/