/*
\page pageFoudationImp Foundation�̎���


\section secCreateObject �I�u�W�F�N�g�̐���
�I�u�W�F�N�g�́C�I�u�W�F�N�g�𐶐�����API ObjectIf::CreateObject() �������Ă���C
����ɂ���Đ����ł��܂��D
�Ⴆ�� GRSdk �|������ GRScene �|������ GRFrame �Ɛ����ł��܂��D
�������ꂽ�I�u�W�F�N�g�́C���������I�u�W�F�N�g�ɏ��L����邱�Ƃ������ł��D

PHSdk��GRSdk�́C���[�g�̃I�u�W�F�N�g�ŁC�e�I�u�W�F�N�g�������܂���D
�����́C�O���[�o���֐� CreateSdk() CreatePHSdk() CreateGRSdk()�ȂǂŐ����ł��܂��D


\subsection secFactory Factory�N���X
���Ƃ��΁CPHSolid ���p���� �`�󂩂玩���Ŏ��ʂƊ������[�����g�����߂� PHAutoSolid
��������Ƃ��āC������t�@�C�����[�_�[�Ń��[�h���āCPHScene�ɒǉ����邽�߂ɂ́C
PHAutoSolid��PHScene::CreateObject()�������ł��Ȃ���΂Ȃ�܂���D

���Ƃ���C�V�����N���X�𐶐��ł���悤�ɂ���d�g�݂��t�@�N�g���ł��D
FactoryBase�N���X��h�������ăI�u�W�F�N�g�����C
PHSceneIf::GetIfStatic()->RegisterFactory()���g���ēo�^����ƁCPHScene::CreateObject()
���o�^���ꂽ�t�@�N�g�����Ăяo���ăI�u�W�F�N�g�𐶐�����悤�ɂȂ�܂�
(���ۂ́CFactoryImpTemplate ���g���ƊȒP�ł�)�D
GRSdk.cpp �� GRRegisterFactories() �Ȃǂ��Q�Ƃ��������D

�t�@�N�g�����Ăяo�������́CObject::CreateObject() �ɂ���̂ŁC
CreateObject() ���I�[�o�[���C�h����Ƃ��́C������Ăяo���K�v������܂��D

*/
