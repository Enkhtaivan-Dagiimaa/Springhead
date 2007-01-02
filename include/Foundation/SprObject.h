/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/** @file SprObject.h
	���s���^�������N���X�̊�{�N���X
 */
#ifndef SPR_OBJECT_H
#define SPR_OBJECT_H
#include "../SprBase.h"
#include <iosfwd>

namespace Spr{;

class Object;

#undef DCAST
#define DCAST(T,p) DCastImp((T*)0, p)

///	�����^�ւ̃L���X�g
///	�ÓI�ϊ����\�ȏꍇ�� If->Obj �̃L���X�g
template <class TO, class FROM> TO* DCastImp(TO* dmmy, FROM* p){
	return TO::GetSelf(p);
}
template <class TO, class FROM> TO* DCastImp(TO* dmmy, UTRef<FROM> p){
	return TO::GetSelf(&*p);
}

#undef XCAST
#define XCAST(p) CastToXCastPtr(p)
///	�C���^�t�F�[�X�N���X�̃L���X�g
template <class T>
class XCastPtr{
public:
	template <class X> operator X*() const {
		T* t = (T*) this;
		return DCastImp((X*)0, t);
	}
	template <class X> operator UTRef<X>() const {
		T* t = (T*) this;
		return DCastImp((X*)0, t);
	}
};

template <class T> XCastPtr<T>& CastToXCastPtr(const T* ptr){
	return *(XCastPtr<T>*)(void*)ptr;
}
template <class T> XCastPtr<T>& CastToXCastPtr(const UTRef<T> ptr){
	return *(XCastPtr<T>*)(void*)&*ptr;
}


struct ObjectIf;
class IfInfo;
class UTTypeDesc;

/**	Factory (�I�u�W�F�N�g�𐶐�����N���X)�̊�{�N���X
*/
class FactoryBase:public UTRefCount{
public:
	virtual ~FactoryBase(){}
	///	�쐬����IfInfo
	virtual const IfInfo* GetIfInfo() const =0;
	///	ii�̃I�u�W�F�N�g�� desc ���� parent�̎q�Ƃ��Đ�������D
	virtual ObjectIf* Create(const void* desc, ObjectIf* parent) =0;
};

/**	�C���^�t�F�[�X�̌^���N���X�̊�{�N���X�D�N���X����p���֌W�������Ă��āCDCAST()�Ȃǂ����p����D
1�̃C���^�t�F�[�X�N���X�ɕt��1�̃C���X�^���X���ł���D	*/
class IfInfo: public UTRefCount{
public:
	///	IfInfo��id �N���X���������R���D
	int id;
	///	id�̍ő�l
	static int maxId;
	///	desc�^�ɂ��Ă̏ڍׂȏ��
	UTTypeDesc* desc;
	///	�N���X��
	const char* className;
	///	��{�N���X����
	IfInfo** base;

	typedef std::vector< UTRef<FactoryBase> > Factories;
	///	�q�I�u�W�F�N�g�𐶐�����N���X(�t�@�N�g���[)
	Factories factories;
	///	�R���X�g���N�^
	IfInfo(const char* cn, IfInfo** b);
	///	�f�X�g���N�^
	virtual ~IfInfo() {};
	///	�N���X��
	virtual const char* ClassName() const =0;
	///	key ���p�����Ă��邩�ǂ����`�F�b�N
	bool Inherit(const char* key) const;
	///	key ���p�����Ă��邩�ǂ����`�F�b�N
	bool Inherit(const IfInfo* key) const;
	///	�t�@�N�g��(�I�u�W�F�N�g�𐶐�����N���X)�̓o�^
	void RegisterFactory(FactoryBase* f) const ;
	///	�w��(info)�̃I�u�W�F�N�g�����t�@�N�g��������
	FactoryBase* FindFactory(const IfInfo* info) const;
	///
	static IfInfo* Find(const char* cname);
};
///	IfInfo�̎����D1�N���X1�C���X�^���X
template <class T>
class IfInfoImp: public IfInfo{
public:
	IfInfoImp(const char* cn, IfInfo** b): IfInfo(cn, b){}
	virtual const char* ClassName() const { return className; }
	virtual void* CreateInstance() const{ return 0;}
};

///	�C���^�t�F�[�X�����ׂ������o�̐錾���D
#define IF_DEF_FOR_OBJECTIF(cls)										\
public:																	\
	static IfInfoImp<cls##If> ifInfo;	/*	If�̌^���	*/				\
	virtual const IfInfo* GetIfInfo() const {							\
		return GetIfInfoStatic();										\
	}																	\
	static const IfInfo* SPR_CDECL GetIfInfoStatic();					\
	static void* operator new(size_t) {									\
        assert(0);	/*	Don't allocate interfaces	*/					\
        return (void*)1;												\
    }																	\
	static void operator delete(void* pv) { /*	nothing	to do */ }		\
	/*	? -> If	*/														\
	template <class O>													\
	static cls##If* GetSelf(const O * p){								\
		return p ? p->GetIf((cls##If*)0) : NULL;						\
	}																	\
	/*	���^��If -> If	*/												\
	cls##If* GetIf(cls##If* i) const { return (cls##If*)this; }			\
	/*	���݃L���X�g	*/												\
	XCastPtr<cls##If>& Cast() const{									\
		return *(XCastPtr<cls##If>*)(void*)this; }						\
	/*	�ٌ^��If -> If�ւ̌^�ϊ� */										\
	template <class I> I* GetIf(I*) const {									\
		if (GetIfInfo()->Inherit(I::GetIfInfoStatic())) return (I*)this;	\
		return (I*)GetIfDynamic(I::GetIfInfoStatic());						\
	}																		\


#define IF_DEF(cls)	IF_DEF_FOR_OBJECTIF(cls)

///	���ׂẴC���^�t�F�[�X�N���X�̊�{�N���X
struct ObjectIf{
	IF_DEF_FOR_OBJECTIF(Object);
	virtual ~ObjectIf();
	///	�I�u�W�F�N�g�擾
	virtual Object* GetObj(const UTTypeInfo* info) const =0;
	///	���I�ȃC���^�t�F�[�X�̎擾
	virtual ObjectIf* GetIfDynamic(const IfInfo* info) const = 0;
	///	�f�o�b�O�p�̕\���B�q�I�u�W�F�N�g���܂ށB
	virtual void Print(std::ostream& os) const =0;	
	///	�f�o�b�O�p�̕\���B�q�I�u�W�F�N�g���܂܂Ȃ��B
	virtual void PrintShort(std::ostream& os) const =0;	

	///	@name �Q�ƃJ�E���^�֌W
	//@{
	///
	virtual int AddRef()=0;
	///
	virtual int DelRef()=0;
	///
	virtual int RefCount()=0;
	//@}


	///	@name �q�I�u�W�F�N�g
	//@{
	///	�q�I�u�W�F�N�g�̐�
	virtual size_t NChildObject() const =0;
	///	�q�I�u�W�F�N�g�̎擾
	virtual ObjectIf* GetChildObject(size_t pos) = 0;
	virtual const ObjectIf* GetChildObject(size_t pos) const = 0;
	/**	�q�I�u�W�F�N�g�̒ǉ��D�����̃I�u�W�F�N�g�̎q�I�u�W�F�N�g�Ƃ��Ēǉ����Ă悢�D
		�Ⴆ�΁CGRFrame�̓c���[����邪�C�S�m�[�h��GRScene�̎q�ł�����D*/
	virtual bool AddChildObject(ObjectIf* o)=0;
	///	�q�I�u�W�F�N�g�̍폜
	virtual bool DelChildObject(ObjectIf* o)=0;
	///	���ׂĂ̎q�I�u�W�F�N�g�̍폜�ƁA�v���p�e�B�̃N���A
	virtual void Clear()=0;
	///	�I�u�W�F�N�g���쐬���CAddChildObject()���ĂԁD
	virtual ObjectIf* CreateObject(const IfInfo* info, const void* desc)=0;
	//@}

	///	@name �f�X�N���v�^�Ə��
	//@{
	/**	�f�X�N���v�^�̓ǂݏo��(�Q�Ɣ� NULL��Ԃ����Ƃ�����)�D
		���ꂪ��������Ă��Ȃ��Ă��CObjectIf::GetDesc()�͎�������Ă��邱�Ƃ������D	*/
	virtual const void* GetDescAddress() const = 0;
	/**	�f�X�N���v�^�̓ǂݏo��(�R�s�[�� ���s����(false��Ԃ�)���Ƃ�����)�D
		ObjectIf::GetDescAddress() ����������Ă��Ȃ��Ă��C������͎�������Ă��邱�Ƃ�����D	*/
	virtual bool GetDesc(void* desc) const = 0;
	/**	�f�B�X�N���v�^�̃T�C�Y	*/
	virtual size_t GetDescSize() const = 0;
	/**	��Ԃ̓ǂݏo��(�Q�Ɣ� NULL��Ԃ����Ƃ�����)�D
		���ꂪ��������Ă��Ȃ��Ă��CObjectIf::GetState()�͎�������Ă��邱�Ƃ�����D	*/
	virtual const void* GetStateAddress() const = 0;
	/**	��Ԃ̓ǂݏo��(�R�s�[�� ���s����(false��Ԃ�)���Ƃ�����)�D
		ObjectIf::GetStateAddress() ����������Ă��Ȃ��Ă��C������͎�������Ă��邱�Ƃ�����D	*/
	virtual bool GetState(void* state) const = 0;
	/**	��Ԃ̐ݒ�	*/
	virtual void SetState(const void* state) = 0;
	/**	��Ԃ̃T�C�Y	*/
	virtual size_t GetStateSize() const = 0;
	/**	�������u���b�N����Ԍ^�ɏ�����	*/
	virtual void ConstructState(void* m) const = 0;
	/**	��Ԍ^���������u���b�N�ɖ߂�	*/
	virtual void DestructState(void* m) const = 0;
	//@}
};

///	�C���^�t�F�[�X�N���X�ւ̃|�C���^�̔z��
struct ObjectIfs: public UTStack<ObjectIf*>{
	void PrintShort(std::ostream& os) const{
		for(const_iterator it = begin(); it!=end(); ++it){
			(*it)->PrintShort(os);
		}
	}
	void Print(std::ostream& os) const{
		for(const_iterator it = begin(); it!=end(); ++it){
			(*it)->Print(os);
		}
	}
};

struct NameManagerIf;

///	���O�����I�u�W�F�N�g�̃C���^�t�F�[�X
struct NamedObjectIf: public ObjectIf{
	IF_DEF(NamedObject);
	///	���O�̎擾
	virtual const char* GetName() const =0;
	///	���O�̐ݒ�
	virtual void SetName(const char* n) =0;
	///	���O�Ǘ��I�u�W�F�N�g�̎擾
	virtual NameManagerIf* GetNameManager() =0;
};

struct SceneIf;
///	�V�[���O���t���\������m�[�h�̃C���^�t�F�[�X
struct SceneObjectIf: NamedObjectIf{
	IF_DEF(SceneObject);
	///	����Scene�̎擾
	virtual SceneIf* GetScene() =0;
};


///	�I�u�W�F�N�g�c���[�̏�Ԃ���������ɕۑ����Ă����N���X
struct ObjectStatesIf: public ObjectIf{
	IF_DEF(ObjectStates);

	///	o�Ƃ��̎q�����Z�[�u���邽�߂ɕK�v�ȃ��������m�ۂ���D
	virtual void AllocateState(ObjectIf* o) = 0;
	///	��Ԃ̃��������������
	virtual void ReleaseState(ObjectIf* o) = 0;
	///	��Ԃ̃T�C�Y�����߂�
	virtual size_t CalcStateSize(ObjectIf* o) = 0;

	///	��Ԃ��Z�[�u����D
	virtual void SaveState(ObjectIf* o) = 0;
	///	��Ԃ����[�h����D
	virtual void LoadState(ObjectIf* o) = 0;
};
ObjectStatesIf* SPR_CDECL CreateObjectStates();

}

#endif
