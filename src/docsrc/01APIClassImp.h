/**
\page pageAPIClassImp API�N���X�̍����E�����̎d��

\section secCreateAPIClass API�N���X�̍���
�����ł́CSpringhead��API�N���X�̐錾�Ǝ����̎菇�ɂ��Đ������܂��D
API�N���X�̊T�v�C�g�����ɂ��ẮC\ref pageApiBasic �����Q�Ƃ��������D

\subsection secDefAPIClass API�N���X�̐錾
API�N���X�̃w�b�_�t�@�C���́CSpringhead2/include/SDKNAME �ɍ��܂��D
(SDKNAME��SDK�̖��O Graphics�CPhysics�Ȃ�)<br>
�w�b�_�t�@�C���̖��O�́CSpr�I�u�W�F�N�g��.h (SprPHSolid.h �Ȃ�)�Ƃ��܂��D
API�N���X��錾����ɂ́C
<pre>
 struct SceneObjectIf: NamedObjectIf{
 	IF_DEF(SceneObject);
 	///	����Scene�̎擾
 	virtual SceneIf* GetScene() =0;
 };
</pre>
�̂悤�ɁC
<ol>
 <li> ��{�̃C���^�t�F�[�X�N���X���p������D
 <li> IFDEF(�N���X��)�����o�����D
 <li> API�֐����������z�֐��Ƃ��Đ錾����D
</ol>
�����܂��D

\subsection secTypeInfoOfAPIClass API�N���X�̌^���
API�N���X��錾������C���C�u�����̃\�[�X�t�@�C��
Springhead2/src/SDKNAME/�I�u�W�F�N�g��.cpp (PHSolid.cpp �Ȃ�)��
<pre>
 IF_IMP(SceneObject, NamedObject)
</pre>
���`���܂��D
���̃}�N���́C�N���X�̌p���֌W���O���[�o���ϐ��Ƃ��ĕۑ����܂��D
�L���X�g�̃}�N�� DCAST �͂��̏����g���ăL���X�g���܂��D


\section secCreateImplementClass �����N���X�̍���
API�N���X���������C�������������N���X�����܂��D

\subsection defImplementClass �����N���X�̐錾
�����N���X�́C~
<pre>
 class SceneObject:public NamedObject, SceneObjectIfInit{
 	OBJECT_DEF(SceneObject);		///<	�N���X���̎擾�Ȃǂ̊�{�@�\�̎���
 public:
 	virtual void SetScene(SceneIf* s);
 	virtual SceneIf* GetScene();
 	///	�f�o�b�O�p�̕\��
 	virtual void Print(std::ostream& os) const {NamedObject::Print(os);}
 };
</pre>
�̂悤�ɁC�錾���܂��D

\subsubsection secOBJECT_DEF OBJECT_DEF�}�N��
�N���X�̐錾�̂Ȃ���OBJECT_DEF�́C
<ul>
 <li> �^���̂��߂�Static�ϐ�(UTTypeInfo* typeInfo, GetTypeInfo(), GetTypeInfoStatic())
 <li> API�N���X���擾���郁�\�b�h GetIf()
</ul>
��錾���܂��D
���ۃN���X�̏ꍇ�́C���̉��ł��Ȃ��Ƃ����G���[���ł�̂ŁC�����OBJECT_DEF_ABST���g���܂��D

\subsubsection secGetAPI API�̎擾
Object�́CAPI�N���X�̗̈�(ObjectIfBuf��)�������Ă��܂��D
�iAPI�N���X�́C���z�֐��e�[�u���ւ̃|�C���^(vftable�|�C���^)�����������܂��D
MSVC++7.1�̏ꍇ�CObject�̐擪����S�o�C�g�ڂ���̂S�o�C�g�ɂȂ�܂��D�j

Object::GetIf() �� this+4�o�C�g��ObjectIf*�ɃL���X�g���ĕԂ��܂��D
Object�̒��ɂ���API�N���X�̃A�h���X��Ԃ��킯�ł��D
�t��ObjectIf::GetObj()�́Cthis-4�o�C�g��Ԃ��܂��D
�ł��̂ŁCAPI�N���X�͕K�������N���X�̒��ɂȂ���΂Ȃ�܂���D

�h���N���X�C���Ƃ���PHSolid::GetIf() �����l�ł����CPHSolidIf*�ɃL���X�g���ĕԂ��܂��D

\subsubsection secInitAPI API�Ə�����
�����N���X�́C
<pre>
 class SceneObject:public NamedObject, SceneObjectIfInit{...}
</pre>
�̂悤�ɁC��{�����N���X���p�����܂����CAPI�N���X(SceneObjectIf��)
�͌p�����܂���D����ɁC�C���^�t�F�[�X�������N���X(SceneObjectIfInit��)��
�p�����܂��D
SceneObjectIfInit�́C�R���X�g���N�^��Object��ObjectIfBuf�����C������API�N���X�ɏ��������܂��D

\subsubsection secConnectAPI API�Ǝ����̐ڑ��D
SceneObjectIfInit�́CObjectIfBuf��������������ۂɁCAPI�N���X(SceneObjectIf)��
����������̂ł͂Ȃ��C�X�^�u�N���X(SceneObjectIfStub)�ɏ��������܂��D
�������N���X�ƃX�^�u�N���X�́Csrc/���C�u������/IfStubDump���C�u������.h �ɂ���܂��D
�X�^�u�N���X�́C�eAPI�֐��̌Ăяo���������֐��̌Ăяo���ɂȂ��Ă��܂��D
�X�^�u�N���X�Ə������N���X�̃\�[�X�R�[�h�́CAPI�N���X�̃\�[�X�R�[�h���玩����������܂��D

\section secImpImplementationClass �����N���X�̒�`
\subsection secOBJECT_IMP OBJECT_IMP�}�N��
OBJECT_DEF�Œ�`�������\�b�h��ϐ��̎����́C�}�N�� OBJECT_IMP ��
<pre>
 OBJECT_IMP(SceneObject, NamedObject);
</pre>
�̂悤�ɍs���܂��D���ۃN���X�̏ꍇ�́COBJECT_IMP_ABST ���g���܂��D

API�N���X�̒�`�ɕK�v�� IF_IMP �� OBEJCT_IMP�́C���������킹���}�N��
IF_OBJECT_IMP�CIF_OBJECT_IMP_ABST���g����1��ŏ����܂��D

\subsection secImpAPIClass �錾����API�̎���
<pre>
 void SceneObject::SetScene(SceneIf* s){
 	SetNameManager(s->GetObj<NameManager>());
 	nameManager->GetNameMap();
 }
</pre>
�̂悤�ɁC�錾����API���������܂��D�֐����̂ق��C�^���Ȃǂ�����Ă��C
�I�[�o�[���C�h�ł��Ȃ��̂ŁC�܂����������錾�ɂȂ�悤�ɋC�����Ă��������D

\section secFileLoadSave �t�@�C������̃��[�h�E�t�@�C���ւ̃Z�[�u
FileIO SDK(\ref pageFileIO) �Ń��[�h�E�Z�[�u���ł���悤�ɂ��邽�߂ɂ́CAPI�N���X�̒�`��
�኱�̍׍H������K�v������܂��D
FileIO SDK �̏ڍׂ́C\ref pageFileIOImp ���Q�Ƃ��Ă�������

�������[�h�E�Z�[�u�\�ȃI�u�W�F�N�g���`����ɂ́C
<ol>
 <li>���[�h�������f�[�^���܂񂾃f�B�X�N���v�^ (��:PHSolidDesc)�����܂��D
 <li>�C���^�t�F�[�X�N���X (��FPHSolidIf) �����܂��D
 <li>�����N���X (��FPHSolid)�����܂��D���̂Ƃ������N���X�ŁC
  <pre>
  	///	�f�X�N���v�^�̓ǂݏo��(�R�s�[��)
  	virtual bool GetDesc(void* desc) const { return false; }
  	///	�f�X�N���v�^�̓ǂݏo��(�Q�Ɣ�)
  	virtual const void* GetDescAddress() const { return NULL; }
  	///	�f�X�N���v�^�̃T�C�Y
  	virtual size_t GetDescSize() const { return 0; };
  </pre>
  ���I�[�o�[���[�h���Ă��������D�f�B�X�N���v�^���p�����Ă���Ȃ�΁C
  Loadable��LoadableDesc���p�����Ă���Ȃ�C
  <pre>
	ACCESS_DESC(�����N���X��);
  </pre>
  �}�N���������N���X�̐錾�̒��ɒu���΁C�I�[�o�[���C�h���Ă���܂��D
 <li> �V�[���O���t��ŁC�����N���X(��FPHSolid)�̐�c�ɂȂ�N���X�̂Ȃ�����C
      �����N���X(��FPHSolid)�𐶐�����N���X(��FPHScene)�����߂܂��D
 <li> �����N���X(��FPHScene)��CreateObject()�������N���X(PHSolid)������悤�ɁC
      <pre>
	PHSceneIf::GetIfInfoStatic()->RegisterFactory(new FactoryImp(PHSolid));
	  </pre>
      �̂悤�Ƀt�@�N�g����o�^���܂��D
      �o�^�́C�t�@�C���̃��[�h���O�ɍs��Ȃ���΂Ȃ�܂���D
      SDK�̃R���X�g���N�^��1�x�����Ăяo���̂��ǂ��ł��傤�D
      �ڂ����� \ref secFactory ���Q�Ƃ��������D
</ol>

\section secStateLoadSave ��Ԃ̕ۑ��E�Č�
�t�@�C���ւ̃��[�h�Z�[�u�ł́C����������Ԃ���I�u�W�F�N�g�𐶐����ăV�[���O���t�����܂����C
�V�~�����[�V�������ɁC10�X�e�b�v�O�̏�Ԃɖ߂������ȂǂƂ������Ƃ��ǂ�����ł��傤�D
����ȗp�r�Ɏg���̂���Ԃ̕ۑ��E�Č��ł��D

��Ԃ̕ۑ��E�Č����ł���N���X�����ɂ́C
<ol>
 <li> �ۑ���������Ԃ�ێ�����\����(��FPHSolidState)�����܂��D
 <li> �����N���X (��FPHSolid �����܂��D
	���̂Ƃ������N���X�ŁC
	<pre>
	///	��Ԃ̓ǂݏo��(�R�s�[��)
	virtual bool GetState(void* state) const { return false; }
	///	��Ԃ̓ǂݏo��(�Q�Ɣ�)
	virtual const void* GetStateAddress() const { return NULL; }
	///	��Ԃ̐ݒ�
	virtual void SetState(const void* state){}
	///	��Ԃ̃T�C�Y
	virtual size_t GetStateSize() const { return 0; };
	///	�������u���b�N����Ԍ^�ɏ�����
	virtual void ConstructState(void* m) const {}
	///	��Ԍ^���������u���b�N�ɖ߂�
	virtual void DestructState(void* m) const {}
	</pre>
	���I�[�o�[���[�h���Ă��������D<br>
	�����N���X(��FPHSolid)����ԍ\����(��FPHSolidState)���p�����Ă���Ȃ�C
	<pre>
	ACCESS_STATE(�����N���X��);
	</pre>
	�}�N���������N���X�̐錾�̒��ɒu���΁C�I�[�o�[���C�h���Ă���܂��D
	ACCESS_DESC_STATE() �}�N����u���΁CDesc��State�������I�[�o�[���C�h���Ă���܂��D
*/
