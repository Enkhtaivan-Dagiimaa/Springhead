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

/*	hase memo
	Object �� ObjectIf���p�����Ȃ������̒�āD��̗ǂ��������Ǝv���܂��D
	�T�v�F
	- If�N���X�͍��܂łǂ���DObject��If���p�����Ȃ��D
	  IfImp��Object�̊֐����Ăяo���i����������̂��ʓ|�Ȃ̂ŗv�������j
		- Object��IfBuf(If��vtable�������Ă������߂̃�����)���p��
		- IntfInit<Type>���CTypeIfStub��IfBuf��������
			class Object:IfBuf{}
			class PHSolid:PHObject, IfInit<PHSolid, PHSolidIf>{}
		- TypeIfStub�́CTypeIf���p���C(this-4)->Type::Func()���Ăяo���D
	
	�ǂ��_�F
		- Object�̌p���֌W���������肷��DInheritObject<>���s�v�ɁD�f�o�b�O���₷���D
		- vtable_ptr��2�ōςށDIf��vtable_ptr����������Ȃ��D�������ߖ�ɂȂ�D
		- �Ăяo�����ɎQ�Ƃ���Virtual��If�̕��P��̂݁D
		- If�̃|�C���^�I�t�Z�b�g���Œ�Ȃ̂ŁCStatic_Cast�� If<-->Object �̕ϊ����ł���D
		- Debugger�ł��C�����L���X�g����Ό�����:(Obj*)((char*)intf-4)
	�����_�F
		- Stub�������̂��ʓ|���v���������D�]���ƈ���Ė��[�̔h���N���X�܂�Stub���K�v�D
		- IfBuf�̏����������g���b�L�[�D
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
};

#define IF_IMP_COMMON(cls)															\
	IfInfoImp<cls##If> cls##If::ifInfo = IfInfoImp<cls##If>(#cls, cls##_BASEIF);	\
	const IfInfo* SPR_CDECL cls##If::GetIfInfoStatic(){								\
		static IfInfoImp<cls##If>* i;												\
		if (!i){																	\
			i= DBG_NEW IfInfoImp<cls##If>(#cls, cls##_BASEIF);						\
			TypeInfoManager::Get()->RegisterIf(i);									\
		}																			\
		return i;																	\
	}																				\

///	ObjectIf �C���^�t�F�[�X�N���X�̎��s���^���
#define IF_IMP_BASE(cls)															\
	IfInfo* cls##_BASEIF[] = {NULL};												\
	IF_IMP_COMMON(cls)

///	�C���^�t�F�[�X�N���X�̎��s���^���
#define IF_IMP(cls, base1)															\
	IfInfo* cls##_BASEIF[] = {(IfInfo*)base1##If::GetIfInfoStatic(), NULL};			\
	IF_IMP_COMMON(cls)


///	Object�h���N���X�̎��s���^���
#define OBJECT_GETSELFDEF(cls)											\
	static cls* GetSelfFromIf(const ObjectIf* p){						\
		if (!p) return NULL;											\
		Object* o = p->GetObj<Object>();								\
		if (o->GetTypeInfo()->Inherit(cls::GetTypeInfoStatic()))		\
			return (cls*)o;												\
		return NULL;													\
	}																	\
	XCastPtr<cls>& Cast() const{										\
		return *(XCastPtr<cls>*)(void*)this; }							\

#define	OBJECT_DEF_ABST_NOIF(cls)		DEF_UTTYPEINFOABSTDEF(cls) OBJECT_GETSELFDEF(cls)
#define	OBJECT_DEF_NOIF(cls)			DEF_UTTYPEINFODEF(cls) OBJECT_GETSELFDEF(cls)

#define OBJECT_GETSELFDEF_IF(cls)										\
	static cls* GetSelfFromIf(const cls##If* p){						\
		if (!p) return NULL;											\
		return p->GetObj<cls>();										\
	}																	\

#define OBJECT_IF_UTILITY(cls)	\
	cls##If* GetIf(){ return (cls##If*)(ObjectIfBuf*) this; }	\
	const cls##If* GetIf() const { return (const cls##If*)(ObjectIfBuf*) this; }

#define	OBJECT_DEF_ABST(cls)	OBJECT_DEF_ABST_NOIF(cls) OBJECT_IF_UTILITY(cls) OBJECT_GETSELFDEF_IF(cls)
#define	OBJECT_DEF(cls)			OBJECT_DEF_NOIF(cls) OBJECT_IF_UTILITY(cls) OBJECT_GETSELFDEF_IF(cls)
#define	OBJECT_DEF_FOR_OBJ(cls)	OBJECT_DEF_NOIF(cls) OBJECT_IF_UTILITY(cls)

///	���s���^��������Object�̔h���N���X�����ׂ������o�̎����D
#define OBJECT_IMP_BASEABST(cls)		DEF_UTTYPEINFOABST(cls)
#define OBJECT_IMP_BASE(cls)			DEF_UTTYPEINFO(cls)
#define OBJECT_IMP(cls, base)			DEF_UTTYPEINFO1(cls, base)
#define OBJECT_IMP_ABST(cls, base)		DEF_UTTYPEINFOABST1(cls, base)
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


///	ObjectIf�Ƃ��̔h���N���X�̃I�u�W�F�N�g�̂��߂̃o�b�t�@
class ObjectIfBuf{
	void* vtable;
};
///	ObjectIfBuf��h���N���X�̃C���^�t�F�[�X�ɏ���������
template <class I, class O>
struct IfInitTemplate{
	IfInitTemplate(){
		const int toBuf = int((char*)(ObjectIfBuf*)(O*)0x1000 - (char*)0x1000);
		const int fromThis = int((char*)0x1000 - (char*)(IfInitTemplate<I,O>*)(O*)0x1000);
		ObjectIfBuf* buf = (ObjectIfBuf*)(((char*)this) + fromThis + toBuf);
		::new (buf) I;
	}
};

}	//	namespace Spr;


#include "IfStubDumpFoundation.h"

namespace Spr{;

/**	�SObject�̊�{�^	*/
class Object:public ObjectIfBuf, ObjectIfInit, public UTTypeInfoObjectBase, public UTRefCount{
	void GetIfInfo() { assert(0); }	//	don't call me
public:
	OBJECT_DEF_FOR_OBJ(Object);		///<	�N���X���̎擾�Ȃǂ̊�{�@�\�̎���

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
	///	���ׂĂ̎q�I�u�W�F�N�g�̍폜�ƃv���p�e�B�̃N���A
	virtual void Clear(){}

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
	virtual void PrintHeader(std::ostream& os, bool bClose) const;
	virtual void PrintChildren(std::ostream& os) const;
	virtual void PrintFooter(std::ostream& os) const;
	///	s��o��State�Ƃ��ď���������D
	static void ConstructState(ObjectIf* o, char*& s);
	///	s��o��State���烁�����u���b�N�ɖ߂��D
	static void DestructState(ObjectIf* o, char*& s);
};
template <class T> T* DCastImp(const Object* o){
	if (!o) return NULL;
	return T::GetSelfFromIf(o->GetIf());
}

class NameManager;
/**	���O������Object�^�D
	SDK��Scene�ɏ��L�����D	*/
class NamedObject: public Object, NamedObjectIfInit{
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
	///	NameManager��ݒ�
	void SetNameManager(NameManagerIf* s);
	///	NameManager�̎擾
	virtual NameManagerIf* GetNameManager();
protected:
	virtual void PrintHeader(std::ostream& os, bool bClose) const;
};

class Scene;
/**	Scene�����L����Object�^�D
	��������Scene�ւ̃|�C���^������	*/
class SceneObject:public NamedObject, SceneObjectIfInit{
	OBJECT_DEF(SceneObject);		///<	�N���X���̎擾�Ȃǂ̊�{�@�\�̎���
public:
	virtual void SetScene(SceneIf* s);
	virtual SceneIf* GetScene();
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
			NameManagerIf* m = DCAST(NameManagerIf, parent);
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
class ObjectStates:public Object, ObjectStatesIfInit{
protected:
	char* state;	///<	���(XXxxxxState)����ׂ�����
	size_t size;	///<	��Ԃ̒���
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
