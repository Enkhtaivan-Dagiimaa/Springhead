/**
\page pageSDKNote SDK�J�����̒��ӓ_

\section secCast �L���X�g
API�N���X�C�����N���X�́C���d�p�������邱�ƂɂȂ邽�߁C
�|�C���^���L���X�g����Ƃ��́C�p���̊֌W�ɒ��ӂ��ăL���X�g���s��Ȃ��ƁC
���ꂽ�ꏊ���w���Ă��܂����Ƃ�����܂��D

dynamics_cast��p����Ζ�肠��܂��񂪁C
dynamic_cast���T�|�[�g���Ȃ������n�������̂ŁC���̂悤��Cast()�����o�֐��܂��́C�}�N�� DCAST �ő�p���܂��D
<pre>
 Scene* s = new PHScene;
 PHScene* ps = s->Cast();
 PHScene* ps2 = DCAST(PHScene, s);
</pre>


\section sec_TypeInfo �^���ɂ���
 Springhead2�ł́CRTTI���T�|�[�g���Ă��Ȃ������n�ł������悤�ɁC���O�Ō^����
 �p�ӂ��Ă��܂��D
\subsection sec_typeInfoForObject �I�u�W�F�N�g�̌^���
 Springhead�̃I�u�W�F�N�g�̃N���X�̐錾�ɂ́C
<pre>
 SPR_OBJECTDEF???(�N���X��);
</pre>
�������܂��D���̃}�N���́C�I�u�W�F�N�g����C���^�t�F�[�X�̏����擾���邽�߂̊֐�
GetIfInfo()��GetIfInfoStatic()�̐錾�C�^�ϊ������o�֐�Cast()�C
�I�u�W�F�N�g�̌^���ƌp���֌W�̏������� GetTypeInfo()��GetTypeInfoStatic()
�̏��𐶐����܂��D
*/
