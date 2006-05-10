/** \page pagePhysics �����V�~�����[�V����SDK

- \ref sec_PHSdk
- \ref scene
 - \ref scene_create
 - \ref scene_gravity
- \ref solid
 - \ref solid_create
 - \ref solid_shape
- \ref joint

\section sec_PHSdk �����G���W���̃I�u�W�F�N�g�̃C���[�W
1�̕����G���W��(PHSdk)�́C�������̃V�[��(PHScene)�������Ƃ��ł��C
�V�[���ɂ͍���(PHSolid)�C�֐�(PHJoint)������܂��D
���̂͌`��(CDConvexMesh, CDSphere, CDBox, ...)���Q�Ƃ��Č`�������Ƃ��ł��܂��D
�֐߂�2�̍��̂����ѕt���܂��D
�����G���W�����̃I�u�W�F�N�g�̎Q�Ɗ֌W�̗��}�Ɏ����܂��D
<img src="../../include/docsrc/01UsersGuide/PHSdkScene.png">

\section scene �V�[��

�V�[���Ƃ͕����V�~�����[�V�����̕���ƂȂ��Ԃ̂��Ƃł��D
�V�[���̑����PHSceneIf�N���X����čs�Ȃ��܂��D

\subsection scene_create �V�[���̍쐬

�V�[�����쐬����ɂ�\link Spr::PHSdkIf::CreateScene() PHSdkIf::CreateScene\endlink���Ăт܂��D

\subsection scene_gravity �d�͂̐ݒ�

�V�[���ɓ����d�͂�ݒ肷��ɂ�\link Spr::PHSceneIf::SetGravity() PHSceneIf::SetGravity\endlink���Ăт܂��D
�f�t�H���g�ŁC�V�[���̏d�͉����x��Vec3f(0.0f, -9.8f, 0.0f)�ɐݒ肳��Ă��܂��D
�d�͂𖳌�������ɂ�SetGravity(Vec3f(0.0f, 0.0f, 0.0f))�Ƃ��܂��D
�܂��C�X�̍��̂ɑ΂��ďd�͂̍�p��L���E����������ɂ�
\link Spr::PHSolidIf::SetGravity() PHSolidIf::SetGravity\endlink���g���܂��D

\section solid	����

���̂Ƃ́C�����@���ɏ]���ăV�[�������^�����镨�̂ł��D
���̂ɑ΂��鑀���\link Spr::PHSolidIf PHSolidIf\endlink����čs���܂��D

\subsection solid_create ���̂̍쐬

���̂��쐬���C�V�[���ɒǉ�����ɂ�\link Spr::PHSceneIf::CreateSolid() PHSceneIf::CreateSolid\endlink���Ăт܂��D

\subsection solid_pose ���̂̈ʒu�Ƒ��x
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
�d�S�ʒu�̓f�t�H���g�ł͍��̍��W�n�̌��_�ƈ�v���Ă��܂����C
\link Spr::PHSolidIf::GetCenterOfMass() PHSolidIf::GetCenterOfMass \endlink�C\link Spr::PHSolidIf::SetCenterOfMass() PHSolidIf::SetCenterOfMass \endlink�C
���邢��\link Spr::PHSolidDesc::center PHSolidDesc::center\endlink��p���Ď擾�E�ݒ�ł��܂��D

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
�܂��C���鍄�̂��񓮗͊w�I���ǂ�����\link Spr::PHSolidIf::IsDynamical PHSolidIf::IsDynamical \endlink���ĂԂ��ƂŎ擾�ł��܂��D

�񓮗͊w�I�ȍ��̂ł��ʒu�C�X���C���x�C�p���x�̕ύX�͉\�ł��D

\subsection solid_shape ���̂ւ̌`��̊��蓖��
���̂ɂ�1�ȏ�̌`������蓖�Ă邱�Ƃ��ł��܂��D
Springhead �����V�~�����[�V����SDK�́C�`��̌��������m���C
���̊Ԃɓ����ڐG�͂������I�Ɍv�Z���܂��D
���̂Ɋ��蓖�Ă邱�Ƃ̂ł���`��̎�ނ���э쐬���@�ɂ��Ă�\ref shape���Q�Ƃ��Ă��������D

���̂Ɍ`������蓖�Ă�ɂ�\link Spr::PHSolidIf::AddShape() PHSolidIf::AddShape\endlink���g���܂��D
���̂́C���蓖�Ă�ꂽ�`��ւ̎Q�Ƃ�ێ����邾���ł��̂ŁC1�̌`��𕡐��̍��̂Ɋ��蓖�ĂĂ���肠��܂���D
�������邱�Ƃɂ���āC�����̓����`�̍��̂��쐬�������ꍇ�Ƀ�������ߖ�ł��܂��D

\section shape	�`��
���̂ɂ�1�ȏ�̌`������蓖�Ă邱�Ƃ��ł��܂��D
�`��ɂ͈ȉ��̂��̂�����܂��D
- �C�ӂ̓ʃ��b�V��
- ��
- ������

Springhead Physics���������Ƃ̂ł���`��͓ʌ`��݂̂ł��D
�ʌ`��Ƃ́C�ꌾ�ł����Ή��݂⌊�̖����`��̂��Ƃł��D
�ʂłȂ��i���݂⌊�����j�`������̂Ɋ��蓖�Ă�ɂ́C���炩���߂��̌`���
�����̓ʌ`��ɕ������Ċ��蓖�Ă�K�v������܂��D

\section joint �֐�

�֐߂Ƃ́C2�̍��̂̑��Ή^���ɍS������������̂ł��D
2�̍��̂̑��Ή^���́C�S�������݂��Ȃ��ꍇ�͕��i3���R�x�C��]3���R�x�̌v6�̎��R�x�������܂��D
���̓��ǂ̎��R�x���S�����邩�ɉ����ėl�X�Ȋ֐߂̎�ނ��l�����܂��D
Springhead���T�|�[�g����֐߂̎�ނ��ȉ��Ɏ����܂��D

<img src="../../include/docsrc/01UsersGuide/joints.png">

���֐߃t���[��

���̓��m���֐߂łȂ���ɂ́C���̂̂ǂ̏ꏊ�Ɋ֐߂����t���邩���w�肷��K�v������܂��D
���̂��߂�Springhead�ł͊֐ߍ��W�n�Ƃ����T�O��p���܂��D

�� �֐߃t���[���̐} ��

�}�̂悤�ɁC�A�����邻�ꂼ��̍��̂Ɋ֐ߍ��W�n�����t���܂�
�i�\�P�b�g�ƃv���O�̂悤�ɍl����ƕ�����Ղ��ł��傤�j�D

��̊֐ߍ��W�n�i�\�P�b�g�ƃv���O�j���ǂ̂悤�ɍS������邩�́C�֐߂̎�ޖ��ɈقȂ�܂��D
�Ⴆ�΃q���W�̏ꍇ�C�֐ߍ��W�n�̌��_����v�����C����Z�����m�����s�ƂȂ�悤�ɍS������܂��D

���֐߂̗L��/������

��1���R�x�֐�
1���R�x�֐߂Ƃ́C1���R�x�̑��Ή^������������i�����������5�̎��R�x���S������j�֐߂ŁC
�ȉ��̂��̂�����܂��F
- �q���W
- �X���C�_
- �p�X�W���C���g

�����ł�1���R�x�֐߂ɋ��ʂ���@�\���Љ�܂��D
���֐ߕψ�
���֐ߑ��x
�����͈�
���o�l�A�_���p
�����[�^��

���q���W

���q���W�̐}��

�q���W��1�{�̎�����̉�]�^������������֐߂ł��D
�\�P�b�g�ƃv���O�̌��_����v�����C����Z�����m�𓯂������ɍS�����܂��D

���X���C�_

���X���C�_�̐}��

�X���C�_��1�����̕��s�ړ�����������֐߂ł��D
�\�P�b�g�ƃv���O�̌X������v�����C���v���O�̌��_���\�P�b�g��Z����ɍS�����܂��D


���p�X�W���C���g

���o�l�E�_���p

�����j�o�[�T���W���C���g

�����j�o�[�T���W���C���g�̐}��

���{�[���W���C���g

���{�[���W���C���g�̐}��

�{�[���W���C���g��1�_�𒆐S�Ƃ���C�ӕ����̉�]����������3���R�x�֐߂ł��D
�\�P�b�g�ƃv���O�̌��_����v����悤�ɍS�����܂��D

*/