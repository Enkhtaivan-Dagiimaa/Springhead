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
����Get�֐��� const �ł� non const �ł����Ƃ��Ȃǎ��������������ꍇ�́C
�w�b�_�ɏ������C Springhead2/src/SDKNAME/�I�u�W�F�N�g��.cpp (PHSolid.cpp �Ȃ�) �ɏ����܂��D

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
API�N���X���������C������p�����������N���X�����܂��D

\subsection defImplementClass �����N���X�̐錾
�����N���X�́C~
<pre>
 class SceneObject:public InheritNamedObject<SceneObjectIf, NamedObject>{
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
���ۃN���X�̏ꍇ�́C���̉��ł��Ȃ��Ƃ����G���[���ł�̂ŁC
�����OBJECT_DEF_ABST���g���܂��D

\subsubsection secAutoOverride ��cAPI�N���X�̉��z�֐��̎����I�[�o�[���C�h
�����N���X�́C
<pre>
 class SceneObject:public InheritNamedObject<SceneObjectIf, NamedObject>{...}
</pre>
�̂悤�ɁCAPI�N���X�ȊO�̃N���X(���̏ꍇNamedObject�N���X)���p�����Ă���D
<pre>
 class NamedObject:public NamedObjectIf, public Object{...}
</pre>
�̂悤�ɁC���̃N���X(NamedObject)�N���X���ʂ�API�N���X(NamedObjectIf)��
�p�����Ă���ꍇ�C�ʂ�API�N���X(NamedObjectIf)�̉��z�֐�(API)��
�I�[�o�[���C�h���Ă����Ȃ��ƁC�R���p�C���G���[�ɂȂ�D
�p�����邽�тɁC���ׂĂ̊֐��̎����������͖̂ʓ|�Ȃ̂ŁC
�悭�p�����ꂻ���ȃN���X��錾�����Ƃ��́C
<pre>
 tempalte<class intf, class base>
 struct InheritNamedObject: public InheritObject<intf,base>{
 	const char* GetName() const { return base::GetName(); }
 	void SetName(const char* n) { base::SetName(n); }
 };
</pre>
�̂悤�ɁC�p�����Ď�������e���v���[�g��p�ӂ��Ă����C�h���N���X�́C
<pre>
 class SceneObject: public InheritNamedObject<SceneIf, NamedObject>{...};
</pre>
�̂悤�ɂ��̃N���X���p�����܂��D

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
 	SetNameManager(OCAST(NameManager, s));
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
