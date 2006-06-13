\page howtoCreateAPIClass API�N���X�̍����E�����̎d��

\section secCreateAPIClass API�N���X�̍���

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

\subsection secTypeInfoOfAPIClass{API�N���X�̌^���}
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

�ڍׂ́C\ref pageFileIOImp ���Q�Ƃ��Ă�������

�������[�h�E�Z�[�u�\�ȃI�u�W�F�N�g Loadable ���`����ɂ́C

+���[�h�������f�[�^���܂�LoadableDesc�����܂��D
+�C���^�t�F�[�X�N���X LoadableIf �����܂��D
+�����N���X Loadable�����܂��D
���̂Ƃ��N���X�ɁC
 // ������Desc��((LoadableDesc*)d)�ɃR�s�[����D
 virtual bool GetDesc(void* d);    
 // sizeof(LoadableDesc)��Ԃ��D
 virtual size_t GetDescSize() const;
��錾���ăI�[�o�[���[�h���Ă��������D~
Loadable��LoadableDesc���p�����Ă���Ȃ�C
 DEF_DESC(Lodable);
�Ƃ���ƁC�}�N���������܂ł���Ă���܂��D

+Loadable�N���X�̐e�N���X�����߂āC�e�N���X�ɁC
 LoadableIf* CreateLoadable(const LoadableDesc& desc);
�����܂��D
+�e�N���X��CreateObject��������΁C
 ObjectIf* CreateObject(const IfInfo* info, const void* desc);
�����܂��D
+�e�N���X��CreateObject�ɁCinfo==LoadableIf::GetStateicInfo() �̏ꍇ�̏�����ǋL���܂��D
 }else if(info == LoadableIf::GetIfInfoStatic()){
  return CreateLoadable(*(const LoadableDesc*)desc);
 }else if ...

**��Ԃ̕ۑ��E�Č����ł���N���X�̍���
�N���XRegret���`����ɂ́C
+�ۑ���������Ԃ�ێ�����\����RegretState�����܂��D
+�C���^�t�F�[�X�N���X RegretIf �����܂��D
+�����N���X Regret �����܂��D
���̂Ƃ��N���X�ɁC
 // ������State��((RegretState*)s)�ɃR�s�[����D
 virtual bool GetState(void* s);
 // (RegretState*)s)�̏�ԂɎ����̏�Ԃ��Z�b�g����D
 virtual bool SetState(void* s);
 // sizeof(LoadableState)��Ԃ��D
 virtual size_t GetStateSize() const;
 virtual void ConstructState(void* m){ new(m) RegretState;}
 virtual void DestructState(void* m){ ((RegretState*)m)->~RegretState();}	
��錾���ăI�[�o�[���[�h���Ă��������D~
Regret��RegretState���p�����Ă���Ȃ�C
 DEF_State(Regret);
�Ƃ���ƁC�}�N���������܂ł���Ă���܂��D
 DEF_DESC_STATE(Regert)�Ƃ���ƁCDesc��State�����������܂ł��Ă���܂��D
