/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
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
#include <iostream>
#include <fstream>

namespace Spr{;

class Object;

#if !defined SWIG

#undef DCAST
#define DCAST(T,p) DCastImp((T*)0, p)

///	�����^�ւ̃L���X�g
///	�ÓI�ϊ����\�ȏꍇ�� If->Obj �̃L���X�g
template <class TO, class FROM> TO* DCastImp(TO* dmmy, FROM* p){
	return p ? TO::GetMe(p) : NULL;
}
template <class TO, class FROM> UTRef<TO> DCastImp(TO* dmmy, UTRef<FROM> p){
	return p ? TO::GetMe(&*p) : NULL;
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
	///	��{�C���^�t�F�[�X���X�g
	const IfInfo** baseList;
	///	�Ή�����I�u�W�F�N�g�̌^���
	const UTTypeInfo* objInfo;
	///	�N���X��
	const char* className;

	typedef std::vector< UTRef<FactoryBase> > Factories;
	///	�q�I�u�W�F�N�g�𐶐�����N���X(�t�@�N�g���[)
	Factories factories;
	///	�R���X�g���N�^
	IfInfo(const char* cn, const IfInfo** bl, const UTTypeInfo* o):baseList(bl), objInfo(o), className(cn){id = ++maxId;}
	///	�f�X�g���N�^
	virtual ~IfInfo() {};
	///	�N���X��
	virtual const char* ClassName() const =0;
	///	�t�@�N�g��(�I�u�W�F�N�g�𐶐�����N���X)�̓o�^
	void RegisterFactory(FactoryBase* f) const ;
	///	�w��(info)�̃I�u�W�F�N�g�����t�@�N�g��������
	FactoryBase* FindFactory(const IfInfo* info) const;
	///
	static IfInfo* Find(const char* cname);
	///	info���p�����Ă��邩�ǂ����D
	bool Inherit(const IfInfo* info) const;
};
///	IfInfo�̎����D1�N���X1�C���X�^���X
template <class T>
class IfInfoImp: public IfInfo{
public:
	IfInfoImp(const char* cn, const IfInfo** b, const UTTypeInfo* o): IfInfo(cn, b, o){}
	virtual const char* ClassName() const { return className; }
	virtual void* CreateInstance() const{ return 0;}
	static const UTTypeInfo* SPR_CDECL GetTypeInfo();
};

///	�C���^�t�F�[�X�����ׂ������o�̐錾���D
#define SPR_IFDEF_BASE(cls)												\
public:																	\
	const IfInfo* SPR_CDECL GetIfInfo() const ;							\
	static const IfInfo* SPR_CDECL GetIfInfoStatic();					\
	static void* operator new(size_t) {									\
		assert(0);	/*	Don't allocate interfaces	*/					\
		return (void*)1;												\
	}																	\
	static void operator delete(void* pv) { /*	nothing	to do */ }		\
	/*	���݃L���X�g	*/												\
	XCastPtr<cls##If>& Cast() const{									\
		return *(XCastPtr<cls##If>*)(void*)this; }						\
	/*	�L���X�g�̎���	*/												\
	template <typename FROM> static cls##If* GetMe(FROM* f){			\
		if ( f && f->GetObjectIf()->GetIfInfo()->Inherit(				\
			cls##If::GetIfInfoStatic()) )								\
			return (cls##If*)f->GetObjectIf();							\
		return NULL;													\
	}																	\

#endif // !SWIG

///	�f�B�X�N���v�^�����ׂ������o�̐錾���D
#ifndef SWIG
#define SPR_DESCDEF(cls)												\
public:																	\
	const static IfInfo* GetIfInfo(){									\
		return cls##If::GetIfInfoStatic();								\
	}																	\

#else
#define SPR_DESCDEF(cls)
#endif
// Ruby�Ȃǂ̃|�[�g�Ŏg�p�����L���X�g
#define SPR_IF_HLANG_CAST(cls) static cls##If* Cast(ObjectIf* o){return DCAST(cls##If, o);}


#ifdef SWIGSPR
 #define SPR_IFDEF(cls)	SPR_IFDEF(cls)
 #define SPR_VIFDEF(cls)	SPR_VIFDEF(cls)
#else
 #define SPR_VIFDEF(cls)	SPR_IFDEF(cls)
 #ifdef SWIG
 #define SPR_IFDEF(cls)	SPR_IF_HLANG_CAST(cls)
 #else
 #define SPR_IFDEF(cls)	SPR_IFDEF_BASE(cls) SPR_IF_HLANG_CAST(cls)
 #endif
#endif

#ifndef SWIG
#define SPR_OVERRIDEMEMBERFUNCOF(cls, base)	SPR_OVERRIDEMEMBERFUNCOF_##cls(base)
#else
#define SPR_OVERRIDEMEMBERFUNCOF(cls, base)
#endif

///	���ׂẴC���^�t�F�[�X�N���X�̊�{�N���X
struct ObjectIf{
	SPR_IFDEF(Object);
	~ObjectIf();
#ifndef SWIGSPR
	ObjectIf* GetObjectIf(){ return this; }
	const ObjectIf* GetObjectIf() const { return this; }
	//	DelRef�̂݃I�[�o�[���C�h�D
	//	�P��Object��DelRef���ĂԂ̂ł͂Ȃ��C delete �̐��䂪�K�v�Ȃ��߁D
	int DelRef() const ;
#endif

	///	�f�o�b�O�p�̕\���B�q�I�u�W�F�N�g���܂ށB
	void Print(std::ostream& os) const;	
	///	�f�o�b�O�p�̕\���B�q�I�u�W�F�N�g���܂܂Ȃ��B
	void PrintShort(std::ostream& os) const;	

	///	@name �Q�ƃJ�E���^�֌W
	//@{
	///
	int AddRef();
	///
	int RefCount();
	//@}


	///	@name �q�I�u�W�F�N�g
	//@{
	///	�q�I�u�W�F�N�g�̐�
	size_t NChildObject() const;
	///	�q�I�u�W�F�N�g�̎擾
	ObjectIf* GetChildObject(size_t pos);
	const ObjectIf* GetChildObject(size_t pos) const;
	/**	�q�I�u�W�F�N�g�̒ǉ��D�����̃I�u�W�F�N�g�̎q�I�u�W�F�N�g�Ƃ��Ēǉ����Ă悢�D
		�Ⴆ�΁CGRFrame�̓c���[����邪�C�S�m�[�h��GRScene�̎q�ł�����D*/
	bool AddChildObject(ObjectIf* o);
	///	�q�I�u�W�F�N�g�̍폜
	bool DelChildObject(ObjectIf* o);
	///	���ׂĂ̎q�I�u�W�F�N�g�̍폜�ƁA�v���p�e�B�̃N���A
	void Clear();
	///	�I�u�W�F�N�g���쐬���CAddChildObject()���ĂԁD
	ObjectIf* CreateObject(const IfInfo* info, const void* desc);
	///	CreateObject���Ăяo�����[�e�B���e�B�֐�
	template <class T> ObjectIf* CreateObject(const T& desc){
		return CreateObject(desc.GetIfInfo(), &desc);
	}
	//@}

	///	@name �f�X�N���v�^�Ə��
	//@{
	/**	�f�X�N���v�^�̓ǂݏo��(�Q�Ɣ� NULL��Ԃ����Ƃ�����)�D
		���ꂪ��������Ă��Ȃ��Ă��CObjectIf::GetDesc()�͎�������Ă��邱�Ƃ������D	*/
	const void* GetDescAddress() const;
	/**	�f�X�N���v�^�̓ǂݏo��(�R�s�[�� ���s����(false��Ԃ�)���Ƃ�����)�D
		ObjectIf::GetDescAddress() ����������Ă��Ȃ��Ă��C������͎�������Ă��邱�Ƃ�����D	*/
	bool GetDesc(void* desc) const;
	/** �f�X�N���v�^�̐ݒ� */
	void SetDesc(const void* desc);
	/**	�f�X�N���v�^�̃T�C�Y	*/
	size_t GetDescSize() const;
	/**	��Ԃ̓ǂݏo��(�Q�Ɣ� NULL��Ԃ����Ƃ�����)�D
		���ꂪ��������Ă��Ȃ��Ă��CObjectIf::GetState()�͎�������Ă��邱�Ƃ�����D	*/
	const void* GetStateAddress() const;
	/**	��Ԃ̓ǂݏo��(�R�s�[�� ���s����(false��Ԃ�)���Ƃ�����)�D
		ObjectIf::GetStateAddress() ����������Ă��Ȃ��Ă��C������͎�������Ă��邱�Ƃ�����D	*/
	bool GetState(void* state) const;
	/**	��Ԃ̐ݒ�	*/
	void SetState(const void* state);
	/** ��Ԃ̏����o�� */
	bool WriteStateR(std::ostream& fout);
	bool WriteState(std::string fileName);
	/** ��Ԃ̓ǂݍ��� */
	bool ReadStateR(std::istream& fin);
	bool ReadState(std::string fileName);
	/**	��Ԃ̃T�C�Y	*/
	size_t GetStateSize() const;
	/**	�������u���b�N����Ԍ^�ɏ�����	*/
	void ConstructState(void* m) const;
	/**	��Ԍ^���������u���b�N�ɖ߂�	*/
	void DestructState(void* m) const;
	//@}
	///	�I�u�W�F�N�g�c���[�̃������C���[�W���_���v
	void DumpObjectR(std::ostream& os, int level=0) const;
};

///	�C���^�t�F�[�X�N���X�ւ̃|�C���^�̔z��
struct ObjectIfs
#if !defined SWIG	
	: public UTStack<ObjectIf*>
#endif
{
	/*void PrintShort(std::ostream& os) const{
		for(const_iterator it = begin(); it!=end(); ++it){
			(*it)->PrintShort(os);
		}
	}
	void Print(std::ostream& os) const{
		for(const_iterator it = begin(); it!=end(); ++it){
			(*it)->Print(os);
		}
	}*/
	typedef UTStack<ObjectIf*> container_t;
	void Push(ObjectIf* obj){container_t::Push(obj);}
	void Pop(){container_t::Pop();}
	ObjectIf* Top(){return container_t::Top();}
};

struct NameManagerIf;

///	���O�����I�u�W�F�N�g�̃C���^�t�F�[�X
struct NamedObjectIf: public ObjectIf{
	SPR_IFDEF(NamedObject);
	///	���O�̎擾
	const char* GetName() const;
	///	���O�̐ݒ�
	void SetName(const char* n);
	///	���O�Ǘ��I�u�W�F�N�g�̎擾
	NameManagerIf* GetNameManager();
};

struct SceneIf;
///	�V�[���O���t���\������m�[�h�̃C���^�t�F�[�X
struct SceneObjectIf: NamedObjectIf{
	SPR_IFDEF(SceneObject);
	///	����Scene�̎擾
	SceneIf* GetScene();
	/// ����������Scene�ɕ�������
	SceneObjectIf* CloneObject();
};


/**	�I�u�W�F�N�g�c���[�̏�Ԃ���������ɕۑ����邽�߂̃N���X�D
	SaveState(scene) �̂悤�ɌĂяo���ƁCscene�̎q���S�̂̏�Ԃ��������ɕۑ�����D
	�Z�[�u������CLoadState(scene) ���Ăяo���ƁCscene�̏�Ԃ�ۑ����̏�Ԃɖ߂��D
	�Z�[�u��C�V�[���̃I�u�W�F�N�g�̐���\�����ς���Ă��܂��Ɛ��������Ƃꂸ�C
	��������j�󂵂Ă��܂��̂Œ��ӂ��K�v�D

	���߂� SaveState(scene) ���Ăяo���ƁC�܂������� AllocateState(scene)���Ăяo����
	���������m�ۂ���D���Ɋm�ۂ����������ɃI�u�W�F�N�g�̏�Ԃ�ۑ�����D
	
	�Z�[�u�E���[�h�Ɏg���I������Ƃ��́CReleaseState(scene)���Ăяo���ă��������J������
	�K�v������DReleaseState(scene)�́CAllocateState(scene)���ƃV�[���̍\���������łȂ���
	���������Ƃꂸ�C��������j�󂵂Ă��܂��D

	�V�[���̍\����ύX����Ƃ��́C��xReleaseState()���Ăяo���āC�ēx�Z�[�u���Ȃ����K�v������D
*/
struct ObjectStatesIf: public ObjectIf{
	SPR_IFDEF(ObjectStates);

	///	o�Ƃ��̎q�����Z�[�u���邽�߂ɕK�v�ȃ��������m�ۂ���D
	void AllocateState(ObjectIf* o);
	///	��Ԃ̃��������������
	void ReleaseState(ObjectIf* o);
	///	��Ԃ̃T�C�Y�����߂�
	size_t CalcStateSize(ObjectIf* o);

	///	��Ԃ��Z�[�u����D
	void SaveState(ObjectIf* o);
	///	��Ԃ����[�h����D
	void LoadState(ObjectIf* o);
	///	ObjectState�I�u�W�F�N�g���쐬����D
	static ObjectStatesIf* SPR_CDECL Create();
};

}

#endif
