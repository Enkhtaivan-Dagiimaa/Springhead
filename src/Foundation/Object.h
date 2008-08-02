/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef FOUNDATION_OBJECT_H
#define FOUNDATION_OBJECT_H

#include <SprFoundation.h>
#include <Base/Base.h>

/*	hase memo
	1. Object��ObjectIf���p�����Ȃ��D
	2. ObjectIf�̓o�[�`�����֐��������Ȃ��D
	3. ObjectIf�̃A�h���X = Object�̃A�h���X
	�T�v�F
	- If�N���X��Object��If���p�����Ȃ��D
	  �����������ꂽObjectIf�̊֐��̎�����Object�̊֐����Ăяo���D
	
	�ǂ��_�F
		- Object�̌p���֌W���������肷��D
		- vtable_ptr��1�ōςށDIf��vtable_ptr�͕s�v�D
		- �Ăяo�����ɎQ�Ƃ���Virtual��Object�̕��P��̂݁D
		- If�̃|�C���^�I�t�Z�b�g���Œ�Ȃ̂ŁCStatic_Cast�� If<-->Object �̕ϊ����ł���D
		- Debugger�ł��C�����L���X�g����Ό�����:(Obj*)(Object*)intf
	�����_�F
		- If�͐�΂ɕϐ������ĂȂ��D
*/

namespace Spr{;

//	�^��� IfInfo �� TypeInfo�̃|�C���^��ێ����Ă����āA�Ō�ɊJ������N���X
class TypeInfoManager:public UTRefCount{
protected:
	std::vector< UTRef< IfInfo > > ifs;
	std::vector< UTRef< UTTypeInfo > > objects;
	static TypeInfoManager* typeInfoManager;
public:
	TypeInfoManager();
	~TypeInfoManager();
	static TypeInfoManager* SPR_CDECL Get();
	void RegisterIf(IfInfo* i);
	void RegisterObject(UTTypeInfo* t);
	IfInfo* FindIfInfo(const char* cn);
	UTTypeInfo* FindTypeInfo(const char* cn);
};

///	�ȉ��C���^�t�F�[�X�̌^���̎���
//@{
#define SPR_DESCIMP0(cls)	
#define SPR_DESCIMP1(cls, b1)	
#define SPR_DESCIMP2(cls, b1, b2)	

#define SPR_IFIMP_COMMON(cls)										\
	const IfInfo* SPR_CDECL cls##If::GetIfInfo() const {			\
		cls* p = DCAST(cls, (Object*)(ObjectIf*)this);				\
		return p->GetIfInfo();	/* �h���̏�񂪓����邩�� */		\
	}																\
	const UTTypeInfo* SPR_CDECL 									\
		IfInfoImp<cls##If>::GetTypeInfo(){							\
		return cls::GetTypeInfoStatic();							\
	}																\

#define SPR_IFIMP0(cls)	SPR_IFIMP_COMMON(cls)										\
	const IfInfo* SPR_CDECL cls##If::GetIfInfoStatic(){								\
		static IfInfoImp<cls##If>* i;												\
		if (!i){																	\
			static const IfInfo* base[] = {NULL};									\
			i= DBG_NEW IfInfoImp<cls##If>(#cls, base, cls::GetTypeInfoStatic());	\
			TypeInfoManager::Get()->RegisterIf(i);									\
		}																			\
		return i;																	\
	}																				\

#define SPR_IFIMP1(cls, b1)	SPR_IFIMP_COMMON(cls)									\
	const IfInfo* SPR_CDECL cls##If::GetIfInfoStatic(){								\
		static IfInfoImp<cls##If>* i;												\
		if (!i){																	\
			static const IfInfo* base[] = {											\
				b1##If::GetIfInfoStatic(), NULL};									\
			i= DBG_NEW IfInfoImp<cls##If>(#cls, base, cls::GetTypeInfoStatic());	\
			TypeInfoManager::Get()->RegisterIf(i);									\
		}																			\
		return i;																	\
	}																				\

#define SPR_IFIMP2(cls, b1, b2)	SPR_IFIMP_COMMON(cls)								\
	const IfInfo* SPR_CDECL cls##If::GetIfInfoStatic(){								\
		static IfInfoImp<cls##If>* i;												\
		if (!i){																	\
			static const IfInfo* base[] = {											\
				b1##If::GetIfInfoStatic(), b3##If::GetIfInfoStatic(), NULL};		\
			i= DBG_NEW IfInfoImp<cls##If>(#cls, base, cls::GetTypeInfoStatic());	\
			TypeInfoManager::Get()->RegisterIf(i);									\
		}																			\
		return i;																	\
	}																				\

//@}

///	Object�h���N���X�̎��s���^���
//@{
//	���ׂẴN���X�ɋ���
#define OBJECTDEF_COMMON(cls)														\
	/*	If Object �ǂ���ɂ��Ȃ� �����^�ϊ��^ XCastPtr��Ԃ��D*/					\
	XCastPtr<cls>& Cast() const{													\
		return *(XCastPtr<cls>*)(void*)this;										\
	}																				\
	/*	�ٌ^��If����Object�ւ̓��I�ϊ�	*/											\
	static cls* GetMe(const ObjectIf* p) {											\
		return (cls*)((Object*)p)->GetBaseAddress(GetTypeInfoStatic());				\
	}																				\
	/*	�ٌ^��Object����Object�ւ̓��I�ϊ�	*/										\
	static cls* GetMe(const Object* p) {											\
		return (cls*)(p)->GetBaseAddress(GetTypeInfoStatic());						\
	}																				\


#ifdef SWIGSPR

#define DEF_UTTYPEINFOABSTDEF(cls)
#define DEF_UTTYPEINFODEF(cls)

#else


#define	SPR_OBJECTDEF_NOIF(cls)	DEF_UTTYPEINFODEF(cls) OBJECTDEF_COMMON(cls)
#define	SPR_OBJECTDEF(cls)	SPR_OBJECTDEF_NOIF(cls)									\
	virtual const IfInfo* GetIfInfo() {												\
		return cls##If::GetIfInfoStatic();											\
	}																				\

#define	SPR_OBJECTDEF_ABST_NOIF(cls)	DEF_UTTYPEINFOABSTDEF(cls) OBJECTDEF_COMMON(cls)
#define	SPR_OBJECTDEF_ABST(cls)	SPR_OBJECTDEF_ABST_NOIF(cls)						\
	virtual const IfInfo* GetIfInfo() {												\
		return cls##If::GetIfInfoStatic();											\
	}																				\

#define SPR_OBJECTDEF_NOIF1(cls, base)		SPR_OBJECTDEF_NOIF(cls)
#define SPR_OBJECTDEF1(cls, base)			SPR_OBJECTDEF(cls)
#define SPR_OBJECTDEF_ABST_NOIF1(cls, base)	SPR_OBJECTDEF_ABST_NOIF(cls)
#define SPR_OBJECTDEF_ABST1(cls, base)		SPR_OBJECTDEF_ABST(cls)

#define SPR_OBJECTDEF_NOIF2(cls, b1, b2)		SPR_OBJECTDEF_NOIF(cls)
#define SPR_OBJECTDEF2(cls, b1, b2)				SPR_OBJECTDEF(cls)
#define SPR_OBJECTDEF_ABST_NOIF2(cls, b1, b2)	SPR_OBJECTDEF_ABST_NOIF(cls)
#define SPR_OBJECTDEF_ABST2(cls, b1, b2)		SPR_OBJECTDEF_ABST(cls)

#endif

//@}

///	���s���^��������Object�̔h���N���X�����ׂ������o�̎����D
//@{
#define SPR_OBJECTIMP0(cls)					DEF_UTTYPEINFO(cls)
#define SPR_OBJECTIMP1(cls, base)			DEF_UTTYPEINFO1(cls, base)
#define SPR_OBJECTIMP2(cls, b1,b2)			DEF_UTTYPEINFO2(cls, b1, b2)
#define SPR_OBJECTIMP_ABST0(cls)			DEF_UTTYPEINFOABST(cls)
#define SPR_OBJECTIMP_ABST1(cls, base)		DEF_UTTYPEINFOABST1(cls, base)
//@}


///	�X�e�[�g�̃A�N�Z�X�p�֐��̒�`
#define ACCESS_STATE_PRIVATE(cls)													\
	virtual size_t GetStateSize() const {											\
		return sizeof(cls##State) + sizeof(cls##StatePrivate); }					\
	virtual void ConstructState(void* m) const {									\
		new(m) cls##State; new ((char*)m + sizeof(cls##State)) cls##StatePrivate; }	\
	virtual void DestructState(void* m) const {										\
		((cls##State*)m)->~cls##State();											\
		((cls##StatePrivate*)((char*)m+sizeof(cls##State)))->~cls##StatePrivate(); }\
	virtual bool GetState(void* s) const { *(cls##State*) s = *this;				\
		*(cls##StatePrivate*)((char*)s+sizeof(cls##State)) = *this;	return true; }	\
	virtual void SetState(const void* s){ *(cls##State*)this = *(cls##State*)s;		\
		*(cls##StatePrivate*)this =													\
			*(cls##StatePrivate*) ((char*)s + sizeof(cls##State) ); }				\

#define ACCESS_STATE(cls)															\
	virtual size_t GetStateSize() const { return sizeof(cls##State); }				\
	virtual void ConstructState(void* m) const { new(m) cls##State;}				\
	virtual void DestructState(void* m) const { ((cls##State*)m)->~cls##State(); }	\
	virtual const void* GetStateAddress() const { return (cls##State*)this; }		\
	virtual bool GetState(void* s) const { *(cls##State*)s=*this; return true; }	\
	virtual void SetState(const void* s){ *(cls##State*)this = *(cls##State*)s;}	\

#define ACCESS_PRIVATE(cls)																			\
	virtual size_t GetStateSize() const { return sizeof(cls##StatePrivate); }						\
	virtual void ConstructState(void* m) const { new(m) cls##StatePrivate;}							\
	virtual void DestructState(void* m) const { ((cls##StatePrivate*)m)->~cls##StatePrivate(); }	\
	virtual const void* GetStateAddress() const { return (cls##StatePrivate*)this; }				\
	virtual bool GetState(void* s) const { *(cls##StatePrivate*)s=*this; return true; }				\
	virtual void SetState(const void* s){ *(cls##StatePrivate*)this = *(cls##StatePrivate*)s;}		\

///	�f�X�N���v�^�̐ݒ�E�擾�ȂǃA�N�Z�X�p�֐��̒�`
#define ACCESS_DESC(cls)															\
	virtual const void* GetDescAddress() const {									\
		((cls*)this)->BeforeGetDesc(); return (cls##Desc*)this; }					\
	virtual bool GetDesc(void* d) const {											\
		((cls*)this)->BeforeGetDesc(); *(cls##Desc*)d=*this; return true; }			\
	virtual void SetDesc(const void* d) {											\
		*(cls##Desc*)this = *(const cls##Desc*)d; ((cls*)this)->AfterSetDesc();}	\
	virtual size_t GetDescSize() const { return sizeof(cls##Desc); }				\

///	�X�e�[�g�ƃf�X�N���v�^���܂Ƃ߂Ē�`
#define ACCESS_DESC_STATE(cls) ACCESS_STATE(cls) ACCESS_DESC(cls)
#define ACCESS_DESC_STATE_PRIVATE(cls) ACCESS_STATE_PRIVATE(cls) ACCESS_DESC(cls)
#define ACCESS_DESC_PRIVATE(cls) ACCESS_PRIVATE(cls) ACCESS_DESC(cls)

}	//	namespace Spr;


namespace Spr{;

/**	�SObject�̊�{�^	*/
class Object: public UTTypeInfoObjectBase, public UTRefCount{
public:
	SPR_OBJECTDEF(Object);		///<	�N���X���̎擾�Ȃǂ̊�{�@�\�̎���
	ObjectIf* GetObjectIf(){
		return (ObjectIf*) this; 
	}
	const ObjectIf* GetObjectIf() const {
		return (ObjectIf*) this; 
	}

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
	///	���ׂĂ̎q�I�u�W�F�N�g�̍폜�ƃv���p�e�B�̃N���A
	virtual void Clear(){}

	///	�f�X�N���v�^�̓ǂݏo��(�R�s�[��)
	virtual bool GetDesc(void* desc) const { return false; }
	/// �f�X�N���v�^�̐ݒ�
	virtual void SetDesc(const void* desc) {}
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
	virtual void PrintHeader(std::ostream& os, bool bClose) const;
	virtual void PrintChildren(std::ostream& os) const;
	virtual void PrintFooter(std::ostream& os) const;

	///	GetDesc()�̑O�ɌĂ΂��
	virtual void BeforeGetDesc() const {}
	///	SetDesc()�̂��ƂɌĂ΂��
	virtual void AfterSetDesc(){}

	///	s��o��State�Ƃ��ď���������D
	static void ConstructState(ObjectIf* o, char*& s);
	///	s��o��State���烁�����u���b�N�ɖ߂��D
	static void DestructState(ObjectIf* o, char*& s);
};
///	cout�Ȃǂ̃X�g���[����Object���o�͂��鉉�Z�q�DObject��Print���Ăяo���D
inline std::ostream& operator << (std::ostream& os, const Object& o){
	o.Print(os);
	return os;
}

class NameManager;
/**	���O������Object�^�D
	SDK��Scene�ɏ��L�����D	*/
class NamedObject: public Object{
	SPR_OBJECTDEF(NamedObject);			///<	�N���X���̎擾�Ȃǂ̊�{�@�\�̎���
protected:
	friend class ObjectNames;
	UTString name;					///<	���O
	NameManager* nameManager;		///<	���O�̌�����d���Ǘ���������́DScene��SDK�ȂǁD
public:
	NamedObject():nameManager(NULL){}
	NamedObject(const NamedObject& n);
	NamedObject& operator=(const NamedObject& n);
	~NamedObject();
	///	���O�̎擾
	const char* GetName() const { return name.c_str(); }
	///	���O�̐ݒ�
	void SetName(const char* n);
	///	NameManager��ݒ�
	void SetNameManager(NameManagerIf* s);
	///	NameManager�̎擾
	virtual NameManagerIf* GetNameManager() const ;
protected:
	virtual void PrintHeader(std::ostream& os, bool bClose) const;
};

class Scene;
/**	Scene�����L����Object�^�D
	��������Scene�ւ̃|�C���^������	*/
class SceneObject:public NamedObject{
	SPR_OBJECTDEF(SceneObject);		///<	�N���X���̎擾�Ȃǂ̊�{�@�\�̎���
public:
	virtual void SetScene(SceneIf* s);
	virtual SceneIf* GetScene() const;
};

///	Object�ւ̎Q�Ƃ̔z��
class ObjectRefs:public UTStack< UTRef<Object> >{
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
			NameManagerIf* m = DCAST(NameManagerIf, parent);
			if (o && !m){	//	�e��NameManger�ł͂Ȃ��ꍇ�C�e��NameManager�ɓo�^
				NamedObject* po = DCAST(NamedObject, parent);
				if (po) m = po->GetNameManager();
			}
			if (o && m) o->SetNameManager(m);
		}
		return t->Cast();
	}
	virtual const IfInfo* GetIfInfo() const {
		return IF::GetIfInfoStatic();
	}
};
///	�t�@�N�g���[�̎���(����I�u�W�F�N�g�����L����ꍇ�A�Ō�ɍ����AddChildObject����)
template <class T, class IF, class DESC>
class FactoryImpTemplateOwned: public FactoryImpTemplate<T, IF, DESC>{
public:
	virtual ObjectIf* Create(const void* desc, ObjectIf* parent){
		ObjectIf* rv = FactoryImpTemplate<T, IF, DESC>::Create(desc, parent);
		if (rv) parent->AddChildObject(rv);
		return rv;
	}
};
///	�t�@�N�g���[�̎���(�f�X�N���v�^���Ȃ��I�u�W�F�N�g�p)
template <class T, class IF>
class FactoryImpTemplateNoDesc: public FactoryBase{
public:
	virtual ObjectIf* Create(const void* desc, ObjectIf* parent){
		T* t = DBG_NEW T;

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
			NameManagerIf* m = DCAST(NameManagerIf, parent);
			if (o && !m){	//	�e��NameManger�ł͂Ȃ��ꍇ�C�e��NameManager�ɓo�^
				NamedObject* po = DCAST(NamedObject, parent);
				if (po) m = po->GetNameManager();
			}
			if (o && m) o->SetNameManager(m);
		}
		return t->Cast();
	}
	virtual const IfInfo* GetIfInfo() const {
		return IF::GetIfInfoStatic();
	}
};

#define FactoryImp(cls)	FactoryImpTemplate<cls, cls##If, cls##Desc>
#define FactoryImpOwned(cls)	FactoryImpTemplateOwned<cls, cls##If, cls##Desc>
#define FactoryImpNoDesc(cls)	FactoryImpTemplateNoDesc<cls, cls##If>

///	�V�[���O���t�̏�Ԃ�ۑ��D�Đ�����d�g��
class ObjectStates:public Object{
protected:
	char* state;	///<	���(XXxxxxState)����ׂ�����
	size_t size;	///<	��Ԃ̒���
	///	��ԃZ�[�u�̍ċN����
	void SaveState(ObjectIf* o, char*& s);
	///	��ԃ��[�h�̍ċN����
	void LoadState(ObjectIf* o, char*& s);

public:
	SPR_OBJECTDEF(ObjectStates);
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
