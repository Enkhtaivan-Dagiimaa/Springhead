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
 	SPR_IFDEF(SceneObject);
 	///	����Scene�̎擾
 	virtual SceneIf* GetScene() =0;
 };
</pre>
�̂悤�ɁC
<ol>
 <li> ��{�̃C���^�t�F�[�X�N���X���p������D
 <li> SPR_IFDEF(�N���X��)�����o�����D
 <li> API�֐����������z�֐��Ƃ��Đ錾����D
</ol>
�����܂��D
SPR_IFDEF()�́C�������̃����o�̐錾���܂Ƃ߂����̂ł��D
SPR_IFDEF()�ɑΉ���������́CSpringhead2/bin/swig/swig.exe �������������܂��D
Springhead2/src/SDKNAME/SDKNAMEStub.cpp
�ɁC
<pre>
SPR_IFIMP?(�N���X��, ��{�N���X��);
</pre>
�Ƃ����s���ł��܂��D�����SPR_IFDEF()�ɑΉ����郁���o�̎����ɂȂ�܂��D
Cast()�����o�֐��CDCAST()�}�N�������p����^���������ɓ���܂��D

\section secCreateImplementClass �����N���X�̍���
API�N���X���������C�������������N���X�����܂��D

\subsection defImplementClass �����N���X�̐錾
�����N���X�́C~
<pre>
 class SceneObject:public NamedObject, SceneObjectIfInit{
 	SPR_OBJECTDEF(SceneObject);		///<	�N���X���̎擾�Ȃǂ̊�{�@�\�̎���
 public:
 	virtual void SetScene(SceneIf* s);
 	virtual SceneIf* GetScene();
 	///	�f�o�b�O�p�̕\��
 	virtual void Print(std::ostream& os) const {NamedObject::Print(os);}
 };
</pre>
�̂悤�ɁC�錾���܂��D

\subsubsection secOBJECT_DEF SPR_OBJECTDEF()�}�N��
�N���X�̐錾�̂Ȃ���SPR_OBJECTDEF()�́C
�^���̂��߂�Static�֐�(GetTypeInfo(), GetTypeInfoStatic())�Ȃǂ�錾���܂��D
���ۃN���X�̏ꍇ�́C���̉��ł��Ȃ��Ƃ����G���[���ł�̂ŁC�����SPR_OBJECTDEF_ABST���g���܂��D
�܂��C�C���^�t�F�[�X�������Ȃ��N���X�̏ꍇ�́C�����SPR_OBJECTDEF_NOIF���g���܂��D

SPR_OBJECTDEF()�̎������CSPR_IFDEF()�̏ꍇ�Ɠ��l�ɁC
 Springhead2/src/SDKNAME/SDKNAMEStub.cpp �ɁC
<pre>
SPR_OBJECTIMP?(�N���X��, ��{�N���X��);
</pre>
�Ƃ����s���ł��܂��D�����SPR_OBJECTDEF()�ɑΉ����郁���o�̎����ɂȂ�܂��D
Cast()�����o�֐��CDCAST()�}�N�������p����^���������ɓ���܂��D

\subsubsection secGetAPI API�̎擾
Object::GetObjectIf() �� this��ObjectIf*�ɃL���X�g���ĕԂ��܂��D
API�N���X�� this��
<pre>
size_t ObjectIf::NChildObject() const {
	return ((Object*)this)->NChildObject();
}
</pre>
�̂悤�ɁCthis�̌^�������N���X�ɖ߂��Ċ֐����Ăяo���Ă��܂��D

\subsection secImpAPIClass �錾����API�̎���
�܂��CSpringhead/src/SDKNAME/SceneObject.h �ŁC
<pre>
class SceenObject: public NamedObject{
	SPR_OBJECTDEF(SceneObject);
	virutal void SetScene(SceneIf* s);	// �������̐錾
};
</pre>
�̂悤�ɁC�֐���錾���C
�܂��CSpringhead/src/SDKNAME/SceneObject.cpp �ŁC
<pre>
 void SceneObject::SetScene(SceneIf* s){
 	SetNameManager(s->GetObj<NameManager>());
 	nameManager->GetNameMap();
 }
</pre>
�̂悤�ɁC�錾����API���������܂��D
API�N���X�����ɂ���C�����N���X�ɂȂ��֐�������ƁC
Springhead2/src/SDKNAME/SDKNAMEStub.cpp ���R���p�C������Ƃ��ɁC
�G���[�ɂȂ�܂��D

\section secFileLoadSave �t�@�C������̃��[�h�E�t�@�C���ւ̃Z�[�u
FileIO SDK(\ref pageFileIO) �Ń��[�h�E�Z�[�u���ł���悤�ɂ��邽�߂ɂ́CAPI�N���X�̒�`��
�኱�̍׍H������K�v������܂��D
FileIO SDK �̏ڍׂ́C\ref pageFileIOImp ���Q�Ƃ��Ă�������

�������[�h�E�Z�[�u�\�ȃI�u�W�F�N�g���`����ɂ́C

�ȉ��኱�Â��D SPR_DECLMEMBEROF�N���X���Ɍ��y���ׂ�
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
