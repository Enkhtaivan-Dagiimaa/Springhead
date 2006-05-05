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

�V�[�����쐬����ɂ�PHSdkIf::CreateScene�֐����Ăт܂��D

\subsection scene_gravity �d�͂̐ݒ�

�V�[���ɓ����d�͂�ݒ肷��ɂ�\link Spr::PHSceneIf::SetGravity() PHSceneIf::SetGravity\endlink���Ăяo���܂��D
�f�t�H���g�ŁC�V�[���̏d�͉����x��(0.0, -9.8, 0.0)�ɐݒ肳��Ă��܂��D
�d�͂𖳌�������ɂ�SetGravity(Vec3f(0.0f, 0.0f, 0.0f))�Ƃ��܂��D
�܂��C�X�̍��̂ɑ΂��ďd�͂̍�p��L���E����������ɂ�
\link Spr::PHSolidIf::SetGravity() PHSolidIf::SetGravity\endlink���g���܂��D

\section solid	����

���̂Ƃ́C�����@���ɏ]���ăV�[�������^�����镨�̂ł��D

\subsection solid_create ���̂̍쐬

���̂��쐬���C�V�[���ɒǉ�����ɂ�PHSceneIf

\subsection solid_pose ���̂̈ʒu���
1�̍��̂ɂ́C1�̍��W�n���\��t���Ă��܂��D��������̍��W�n�ƌĂт܂��D
�V�[�����̍��̂̈ʒu�ƌX���́C�V�[�����W�n�ɑ΂��鍄�̍��W�n�̈ʒu�ƌX���Ƃ��ĕ\������܂��D

���̂̈ʒu�ƌX����ݒ�E�擾����ɂ�
- \link Spr::PHSceneIf::SetPose \endlink

\subsection solid_mass ���̂̎���

���ʁC�������[�����g�C�d�S�ʒu�������C

�ʒu�E�����C���x�E�p���x����ԂƂ��Ď����܂��D

\subsection solid_shape ���̂ւ̌`��̊��蓖��

�܂��C\ref shape �������C�݂��ɏd�Ȃ�Ȃ��悤�ɕۂ���܂��D

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