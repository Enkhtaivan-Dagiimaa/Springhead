/**
\page pagePhysicsImp �����V�~�����[�V����SDK�̎���

\section secPhysSim �����V�~�����[�V�����@�\
\subsection �����V�~�����[�V�����v�Z�̊T�v
Springhead2�́A���x�x�[�XLCP�ɂ��S���͌v�Z(PHConstraintEngineIf) ��
�y�i���e�B�@�ɂ��ڐG�͌v�Z(PHPenaltyEngineIf)��2�̃G���W���������Ă��܂��B
�قƂ�ǂ̏ꍇ�A���x�x�[�XLCP�ɂ��S���͌v�Z(PHConstraintEngineIf)�������g�����ƂɂȂ�܂��B

���̍S���͌v�Z�╨���V�~�����[�V�����̌����ɂ��ẮA 
EC2006�ł̍u������(EC2006PBM.ppt)�� src/Physics/lcpnote.txt ���Q�Ƃ��Ă��������B
�����ɂ́A�ǂ̃N���X�ŁA�e�v�Z���s���Ă���̂����L�������ɂƂǂ߂܂��B

\subsection �����V�~�����[�V�����ƃN���X�̑Ή�
�����V�~�����[�V�����𓝊����Ă���̂́APHSceneIf �ł��B
�ЂƂ̃V�[���ɂ��Ă̂��ׂĂ̏��PHSceneIf �I�u�W�F�N�g����Q�Ƃł��܂��B
PHSceneIf �́A�������̃G���W�� PHEngineIf �̎q���������܂��B
���̒��ł��d�v�Ȃ͍̂S���͂��v�Z���� PHConstraintEngineIf �ł��B

\subsubsection �S���͌v�Z�ƃN���X�̑Ή�
�S�������ɂ́APHConstraintIf �Ƃ��̔h���N���X���Ή����Ă��܂��B
�V�[���̍S�������B�������āA�S���v�Z�̌Ăяo�����s���Ă���̂��A
PHConstraintEngineIf �ł��B

�S���͌v�Z�̎菇�Ɗ֌W����N���X�E�֐��������܂��B
-# LCP�̃Z�b�g�A�b�v���s�� (PHConstraintEngineIf::SetupLCP() �� PHConstraintIf::SetupLCP()���Ăяo���j
	-# PHConstraint::SetupLCP()�́Aw = A��+b �� A �� b ���v�Z�������ƁAModifyJacobian(), CompBias()���Ăяo���B
	-# ModifyJacobian()�́A�S�����W�n��ύX����ꍇ�ɁA���R�r�A�� J ������������B
	-# CompBias()�́A�S�������ɂ��킹�� A��b�̕␳dA��db�̌v�Z���s���B
-# �K�E�X�U�C�f���@�̌J��Ԃ��v�Z(numIter��)���s���B(PHConstraintEngineIf::IterateLCP() �� PHConstraintIf::IterateLCP()���Ăяo���B�j
	-# PHConstraintIf::IterateLCP()�́A��(�\�[�X�ł�f)�̌v�Z���������ƁAPHConstraintIf::Projection()���Ăяo���ăɂ��s��������𖞂������`�F�b�N���A����𖞂����悤�Ƀɂ�ύX����B

*/
