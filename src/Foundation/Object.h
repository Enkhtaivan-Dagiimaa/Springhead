/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef FOUNDATION_OBJECT_H
#define FOUNDATION_OBJECT_H

#include <SprFoundation.h>
#include <Base/Base.h>

namespace Spr{;

#define IF_IMP_COMMON(cls)															\
	IfInfoImp<cls##If> cls##If::ifInfo = IfInfoImp<cls##If>(#cls, cls##_BASEIF);	\
	template <> \
	void* IfInfoImp<cls##If>::GetSprObject(ObjectIf* i)const{						\
		return (Object*)(cls*)(cls##If*)i;											\
	}																				\
	template <> \
	ObjectIf* IfInfoImp<cls##If>::GetIf(void* obj)const{							\
		return (ObjectIf*)(cls##If*)DCAST(cls, (Object*)obj);						\
	}																				\

///	ObjectIf �C���^�t�F�[�X�N���X�̎��s���^���
#define IF_IMP_BASE(cls)															\
	IfInfo* cls##_BASEIF[] = {NULL};												\
	IF_IMP_COMMON(cls)

///	�C���^�t�F�[�X�N���X�̎��s���^���
#define IF_IMP(cls, base1)															\
	IfInfo* cls##_BASEIF[] = {&base1##If::ifInfo, NULL};							\
	IF_IMP_COMMON(cls)


#define OBJECT_CAST(cls, p)											\
	(((Object*)p)->GetTypeInfo()->Inherit(cls::GetTypeInfoStatic())	\
		? (cls*)(Object*)p :  NULL)									\

///	Object�h���N���X�̎��s���^���
#define OBJECT_DEF_ABST_NOIF(cls)		DEF_UTTYPEINFOABSTDEF(cls)	\
	static cls* GetSelfFromObject(void* o) {						\
		return OBJECT_CAST(cls, o);									\
	}																\

#define OBJECT_DEF_NOIF(cls)			DEF_UTTYPEINFODEF(cls)		\
	static cls* GetSelfFromObject(void* o) {						\
		return OBJECT_CAST(cls, o);									\
	}																\

#define	OBJECT_DEF_ABST(cls)			OBJECT_DEF_ABST_NOIF(cls)	\
	virtual ObjectIf* GetIf() const { return (cls##If*)this; }		\

#define	OBJECT_DEF(cls)					OBJECT_DEF_NOIF(cls)		\
	virtual ObjectIf* GetIf() const { return (cls##If*)this; }		\


///	���s���^��������Object�̔h���N���X�����ׂ������o�̎����D
#define OBJECT_IMP_BASEABST(cls)			DEF_UTTYPEINFOABST(cls)
#define OBJECT_IMP_BASE(cls)				DEF_UTTYPEINFO(cls)
#define OBJECT_IMP(cls, base)				DEF_UTTYPEINFO1(cls, base)
#define OBJECT_IMP2(cls, b1, b2)			DEF_UTTYPEINFO2(cls, b1, b2)
#define OBJECT_IMP3(cls, b1, b2, b3)		DEF_UTTYPEINFO3(cls, b1, b2, b3)
#define OBJECT_IMP_ABST(cls, base)			DEF_UTTYPEINFOABST1(cls, base)
#define OBJECT_IMP_ABST2(cls, b1, b2)		DEF_UTTYPEINFOABST2(cls, b1,b2)
#define OBJECT_IMP_ABST3(cls, b1, b2, b3)	DEF_UTTYPEINFOABST3(cls, b1,b2,b3)

#define IF_OBJECT_IMP(cls, base)		IF_IMP(cls, base) OBJECT_IMP(cls, base)
#define IF_OBJECT_IMP_ABST(cls, base)	IF_IMP(cls, base) OBJECT_IMP_ABST(cls, base)

///	�X�e�[�g�̍쐬�Ɣj���̊֐���`
#define ACCESS_STATE_NOINHERIT(cls)													\
	virtual size_t GetStateSize() const { return sizeof(cls##State); }				\
	virtual void ConstructState(void* m) const { new(m) cls##State;}				\
	virtual void DestructState(void* m) const { ((cls##State*)m)->~cls##State(); }	\

///	�X�e�[�g�̐ݒ�E�擾���܂߂��A�N�Z�X�p�֐��̒�`
#define ACCESS_STATE(cls)															\
	ACCESS_STATE_NOINHERIT(cls)														\
	virtual const void* GetStateAddress() const { return (cls##State*)this; }		\
	virtual bool GetState(void* s) const { *(cls##State*)s=*this; return true; }	\
	virtual void SetState(const void* s){ *(cls##State*)this = *(cls##State*)s;}	\

///	�f�X�N���v�^�̐ݒ�E�擾�ȂǃA�N�Z�X�p�֐��̒�`
#define ACCESS_DESC(cls)															\
	virtual const void* GetDescAddress() const { return (cls##Desc*)this; }			\
	virtual bool GetDesc(void* d) const { *(cls##Desc*)d=*this; return true; }		\
	virtual size_t GetDescSize() const { return sizeof(cls##Desc); }				\

///	�X�e�[�g�ƃf�X�N���v�^���܂Ƃ߂Ē�`
#define ACCESS_DESC_STATE(cls) ACCESS_STATE(cls) ACCESS_DESC(cls)

/**	�SObject�̊�{�^	*/
class Object:private ObjectIf, public UTTypeInfoObjectBase, public UTRefCount{
	void GetIfInfo() { assert(0); }	///	don't call me
public:
	OBJECT_DEF(Object);		///<	�N���X���̎擾�Ȃǂ̊�{�@�\�̎���


	virtual int AddRef(){return UTRefCount::AddRef();}
	virtual int DelRef(){return UTRefCount::DelRef();}
	virtual int RefCount(){return UTRefCount::RefCount();}

	///	�f�o�b�O�p�̕\��
	virtual void Print(std::ostream& os) const;
	virtual void PrintShort(std::ostream& os) const;
	///	�I�u�W�F�N�g�̍쐬
	virtual ObjectIf* CreateObject(const IfInfo* info, const void* desc);
	///	�q�I�u�W�F�N�g�̐�
	virtual size_t NChildObject() const { return 0; }
	///	�q�I�u�W�F�N�g�̎擾
	virtual ObjectIf* GetChildObject(size_t pos) { return NULL; }
	virtual const ObjectIf* GetChildObject(size_t pos) const { 
		return ((Object*) this)->GetChildObject(pos);
	}
	///	�q�I�u�W�F�N�g�̒ǉ�
	virtual bool AddChildObject(ObjectIf* o){ return false; }
	///	�q�I�u�W�F�N�g�̍폜
	virtual bool DelChildObject(ObjectIf* o){ return false; }
	///	���ׂĂ̎q�I�u�W�F�N�g�̍폜
	virtual void ClearChildObjects(){}

	///	�f�X�N���v�^�̓ǂݏo��(�R�s�[��)
	virtual bool GetDesc(void* desc) const { return false; }
	///	�f�X�N���v�^�̓ǂݏo��(�Q�Ɣ�)
	virtual const void* GetDescAddress() const { return NULL; }
	///	�f�X�N���v�^�̃T�C�Y
	virtual size_t GetDescSize() const { return 0; };
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
protected:
	virtual void PrintHeader(std::ostream& os) const;
	virtual void PrintChildren(std::ostream& os) const;
	virtual void PrintFooter(std::ostream& os) const;
};

template <class T> T* SprDcastImp(const Object* p){
	if (p && p->GetTypeInfo()->Inherit(T::GetTypeInfoStatic())) return (T*)&*(p);
	return NULL;
}


template <class intf, class base>
struct InheritObject:public intf, public base{
	typedef base baseType;
	virtual int AddRef(){return base::AddRef();}
	virtual int DelRef(){return base::DelRef();}
	virtual int RefCount(){return base::RefCount();}				
	virtual ObjectIf* CreateObject(const IfInfo* i, const void* d){
		return base::CreateObject(i,d);
	}
	virtual void Print(std::ostream& os) const{ base::Print(os); }
	virtual void PrintShort(std::ostream& os) const{ base::PrintShort(os); }
	virtual size_t NChildObject() const { return base::NChildObject(); }
	virtual ObjectIf* GetChildObject(size_t pos){ return base::GetChildObject(pos); }
	virtual const ObjectIf* GetChildObject(size_t pos) const{
		return base::GetChildObject(pos);
	}
	virtual bool AddChildObject(ObjectIf* o){
		return base::AddChildObject(o);		
	}
	virtual bool DelChildObject(ObjectIf* o){
		return base::DelChildObject(o);		
	}
	virtual void ClearChildObjects(){
		base::ClearChildObjects();		
	}
	virtual bool GetDesc(void* desc) const { return base::GetDesc(desc); }
	virtual const void* GetDescAddress() const { return base::GetDescAddress(); }
	virtual size_t GetDescSize() const { return base::GetDescSize(); }
	virtual bool GetState(void* state) const { return base::GetState(state); }
	virtual const void* GetStateAddress() const { return base::GetStateAddress(); }
	virtual size_t GetStateSize() const { return base::GetStateSize(); }
	virtual void SetState(const void* s){ return base::SetState(s); }
	virtual void ConstructState(void* m) const { base::ConstructState(m); }
	virtual void DestructState(void* m) const { base::DestructState(m); }
};

class NameManager;
/**	���O������Object�^�D
	SDK��Scene�ɏ��L�����D	*/
class NamedObject: public InheritObject<NamedObjectIf, Object>{
	OBJECT_DEF(NamedObject);		///<	�N���X���̎擾�Ȃǂ̊�{�@�\�̎���
protected:
	friend class ObjectNames;
	UTString name;					///<	���O
	NameManager* nameManager;	///<	���O�̌�����d���Ǘ���������́DScene��SDK�ȂǁD
public:
	NamedObject():nameManager(NULL){}
	~NamedObject();
	///	���O�̎擾
	const char* GetName() const { return name.c_str(); }
	///	���O�̐ݒ�
	void SetName(const char* n);
	///	�f�o�b�O�p�̕\��
	virtual void Print(std::ostream& os) const;
	virtual void PrintShort(std::ostream& os) const;
	virtual void SetNameManager(NameManager* s);
	virtual NameManager* GetNameManager(){ return nameManager; }
	///	�f�o�b�O�p�̕\��
};

template <class intf, class base>
struct InheritNamedObject:public InheritObject<intf, base>{
	const char* GetName() const { return base::GetName(); }
	void SetName(const char* n) { base::SetName(n); }
};

class Scene;
/**	Scene�����L����Object�^�D
	��������Scene�ւ̃|�C���^������	*/
class SceneObject:public InheritNamedObject<SceneObjectIf, NamedObject>{
	OBJECT_DEF(SceneObject);		///<	�N���X���̎擾�Ȃǂ̊�{�@�\�̎���
public:
	virtual void SetScene(SceneIf* s);
	virtual SceneIf* GetScene();
};
template <class intf, class base>
struct InheritSceneObject:public InheritNamedObject<intf, base>{
	SceneIf* GetScene() { return base::GetScene(); }
	void SetScene(SceneIf* s) { base::SetScene(s); }
};

///	Object�̃|�C���^�̔z��
class Objects:public UTStack< UTRef<Object> >{
public:
	///	T�̔h���N���X���X�^�b�N��Top������T���D
	template <class T> bool Find(T*& t){
		for(iterator it = end(); it != begin();){
			--it;
			T* tmp = DCAST(T, *it);
			if (tmp){
				t = tmp;
				return true;
			}
		}
		return false;
	}
};

///	�t�@�N�g���[�̎���
template <class T, class IF, class DESC>
class FactoryImpTemplate: public FactoryBase{
public:
	virtual ObjectIf* Create(const void* desc, ObjectIf* parent){
		T* t = DBG_NEW T(*(DESC*)desc);

		//	�V�[���̐ݒ�
		SceneObject* o = DCAST(SceneObject, t);
		SceneIf* s = DCAST(SceneIf, parent);
		if (o && !s){		//	�e���V�[���ł͖����ꍇ�C�e������scene�ɓo�^
			SceneObject* po = DCAST(SceneObject, parent);
			if (po) s = po->GetScene();
		}
		if (o && s){
			o->SetScene(s);
		}else{	//	�V�[���ɐݒ�ł��Ȃ��ꍇ�C���O�Ǘ��I�u�W�F�N�g�̐ݒ�
			NamedObject* o = DCAST(NamedObject, t);
			NameManager* m = DCAST(NameManager, parent);
			if (o && !m){	//	�e��NameManger�ł͂Ȃ��ꍇ�C�e��NameManager�ɓo�^
				NamedObject* po = DCAST(NamedObject, parent);
				if (po) m = po->GetNameManager();
			}
			if (o && m) o->SetNameManager(m);
		}
		return t->GetIf();
	}
	virtual const IfInfo* GetIfInfo() const {
		return IF::GetIfInfoStatic();
	}
};
#define FactoryImp(cls)	FactoryImpTemplate<cls, cls##If, cls##Desc>


///	�V�[���O���t�̏�Ԃ�ۑ��D�Đ�����d�g��
class ObjectStates:public InheritObject<ObjectStatesIf, Object>{
protected:
	char* state;	///<	���(XXxxxxState)����ׂ�����
	size_t size;	///<	��Ԃ̒���
	///	s��o��State�Ƃ��ď���������D
	void ConstructState(ObjectIf* o, char*& s);
	///	s��o��State���烁�����u���b�N�ɖ߂��D
	void DestructState(ObjectIf* o, char*& s);
	///	��ԃZ�[�u�̍ċN����
	void SaveState(ObjectIf* o, char*& s);
	///	��ԃ��[�h�̍ċN����
	void LoadState(ObjectIf* o, char*& s);

public:
	OBJECT_DEF(ObjectStates);
	ObjectStates():state(NULL), size(0){}
	~ObjectStates(){ delete state; }
	///	o�Ƃ��̎q�����Z�[�u���邽�߂ɕK�v�ȃ��������m�ۂ���D
	void AllocateState(ObjectIf* o);
	///	��Ԃ��Z�[�u����D
	void SaveState(ObjectIf* o);
	///	��Ԃ����[�h����D
	void LoadState(ObjectIf* o);
	///	��Ԃ̃��������������
	void ReleaseState(ObjectIf* o);
	///	��Ԃ̃T�C�Y�����߂�
	size_t CalcStateSize(ObjectIf* o);
};


}
#endif
