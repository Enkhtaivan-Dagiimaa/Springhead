/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef BASETYPEINFO_H
#define BASETYPEINFO_H
#include "BaseUtility.h"
/**	@file BaseTypeInfo.h
	���s���^���e���v���[�g���C�u����.
	�R���p�C����rtti���g�킸�}�N���ƃe���v���[�g�Ŏ������Ă���D
*/
namespace Spr{;

///	���s���^���
class UTTypeInfo: public UTRefCount{
public:
	const char* className;
	const UTTypeInfo** base;

	UTTypeInfo(const char* cn, const UTTypeInfo** b): className(cn), base(b){}
	virtual bool Inherit(const UTTypeInfo* key) const ;
	virtual bool Inherit(const char* str) const ;
	virtual const char* ClassName() const = 0;
	virtual void* CreateInstance() const = 0;
	virtual ~UTTypeInfo() {};
};

///	���s���^���N���X�̎���
template <class T>
class UTTypeInfoImp: public UTTypeInfo{
public:
	UTTypeInfoImp(const char* cn, const UTTypeInfo** b): UTTypeInfo(cn, b){}
	virtual void* CreateInstance() const { return new T; }
	virtual const char* ClassName() const { return className; }
};

///	���s���^���N���X�̎����D���ۃN���X��
template <class T>
class UTTypeInfoImpAbst: public UTTypeInfo{
public:
	UTTypeInfoImpAbst(const char* cn, const UTTypeInfo** b): UTTypeInfo(cn, b){}
	virtual void* CreateInstance() const { return 0; }
	virtual const char* ClassName() const { return className; }
};

///	���s���^�������N���X�̊�{�N���X
class UTTypeInfoObjectBase{
public:
	virtual const UTTypeInfo* GetTypeInfo() const =0;
protected:
	virtual ~UTTypeInfoObjectBase() {};
};

//----------------------------------------------------------------------
//	�N���X�̐錾(�w�b�_)�ɏ�������
///	���s���^�������N���X�����ׂ������o�̐錾��
#define DEF_UTTYPEINFODEF(cls)							\
public:													\
	virtual const UTTypeInfo* GetTypeInfo() const {		\
		return GetTypeInfoStatic();						\
	}													\
	static const UTTypeInfo* GetTypeInfoStatic();		\
	virtual void* /*cls::*/GetBaseAddress(const UTTypeInfo* to) const;	\

///	���s���^�������N���X�����ׂ������o�̐錾���D���ۃN���X��
#define DEF_UTTYPEINFOABSTDEF(cls)						\
public:													\
	virtual const UTTypeInfo* GetTypeInfo() const {		\
		return GetTypeInfoStatic();						\
	}													\
	static const UTTypeInfo* GetTypeInfoStatic();		\
	virtual void* /*cls::*/GetBaseAddress(const UTTypeInfo* to) const;	\


///	���s���^�������N���X�����ׂ������o�̎����D
#define DEF_UTTYPEINFO(cls)									\
	const UTTypeInfo* cls::GetTypeInfoStatic(){				\
		static const UTTypeInfo* base[] = {NULL};			\
		static UTTypeInfoImp<cls> info(#cls, base);			\
		return &info;										\
	}														\
	void* cls::GetBaseAddress(const UTTypeInfo* to) const{		\
		if (to==GetTypeInfoStatic()) return (cls*)this;		\
		return NULL;										\
	}														\

///	���s���^�������N���X�����ׂ������o�̎����D1�̃N���X�p��������ꍇ
#define DEF_UTTYPEINFO1(cls, base1)										\
	const UTTypeInfo* cls::GetTypeInfoStatic(){							\
		static const UTTypeInfo* base[] = {base1::GetTypeInfoStatic(),NULL};	\
		static UTTypeInfoImp<cls> info(#cls, base);						\
		return &info;													\
	}																	\
	void* cls::GetBaseAddress(const UTTypeInfo* to) const{				\
		if (to==GetTypeInfoStatic()) return (cls*)this;					\
		return base1::GetBaseAddress(to);								\
	}																	\

///	���s���^�������N���X�����ׂ������o�̎����D2�̃N���X�p��������ꍇ
#define DEF_UTTYPEINFO2(cls, base1, base2)								\
	const UTTypeInfo* cls::GetTypeInfoStatic(){							\
		static const UTTypeInfo* base[] = {								\
			base1::GetTypeInfoStatic(), base2::GetTypeInfoStatic(),NULL};	\
		static UTTypeInfoImp<cls> info(#cls, base);						\
		return &info;													\
	}																	\
	void* cls::GetBaseAddress(const UTTypeInfo* to) const{				\
		if (to==GetTypeInfoStatic()) return (cls*)this;					\
		void* rv = base1::GetBaseAddress(to);							\
		if (!rv) rv = base2::GetBaseAddress(to);						\
		return rv;														\
	}																	\

///	���s���^�������N���X�����ׂ������o�̎����D3�̃N���X�p��������ꍇ
#define DEF_UTTYPEINFO3(cls, base1, base2, base3)						\
	const UTTypeInfo* cls::GetTypeInfoStatic(){							\
		static const UTTypeInfo* base[] = {								\
			base1::GetTypeInfoStatic(), base2::GetTypeInfoStatic(),		\
			base3::GetTypeInfoStatic(), NULL};							\
		static UTTypeInfoImp<cls> info(#cls, base);						\
		return &info;													\
	}																	\
	void* cls::GetBaseAddress(const UTTypeInfo* to) const{				\
		if (to==GetTypeInfoStatic()) return (cls*)this;					\
		void* rv = base1::GetBaseAddress(to);							\
		if (!rv) rv = base2::GetBaseAddress(to);						\
		if (!rv) rv = base3::GetBaseAddress(to);						\
		return rv;														\
	}																	\


///	���s���^�������N���X�����ׂ������o�̎����D���ۃN���X��
#define DEF_UTTYPEINFOABST(cls)											\
	const UTTypeInfo* cls::GetTypeInfoStatic(){							\
		static const UTTypeInfo* base[] = { NULL };						\
		static UTTypeInfoImpAbst<cls> info(#cls, base);					\
		return &info;													\
	}																	\
	void* cls::GetBaseAddress(const UTTypeInfo* to) const{				\
		if (to==GetTypeInfoStatic()) return (cls*)this;					\
		return NULL;													\
	}																	\

///	���s���^�������N���X�����ׂ������o�̎����D���ۃN���X�ŁD1�̃N���X���p������ꍇ
#define DEF_UTTYPEINFOABST1(cls, base1)									\
	const UTTypeInfo* cls::GetTypeInfoStatic(){							\
		static const UTTypeInfo* base[] = { base1::GetTypeInfoStatic(), NULL };	\
		static UTTypeInfoImpAbst<cls> info(#cls, base);					\
		return &info;													\
	}																	\
	void* cls::GetBaseAddress(const UTTypeInfo* to) const{				\
		if (to==GetTypeInfoStatic()) return (cls*)this;					\
		void* rv = base1::GetBaseAddress(to);							\
		return rv;														\
	}																	\

///	���s���^�������N���X�����ׂ������o�̎����D���ۃN���X�ŁD2�̃N���X���p������ꍇ
#define DEF_UTTYPEINFOABST2(cls, base1, base2)							\
	const UTTypeInfo* cls::GetTypeInfoStatic(){							\
		static const UTTypeInfo* base[] = { base1::GetTypeInfoStatic(), \
			base2::GetTypeInfoStatic(), NULL };							\
		static UTTypeInfoImpAbst<cls> info(#cls, base);					\
		return &info;													\
	}																	\
	void* cls::GetBaseAddress(const UTTypeInfo* to) const{				\
		if (to==GetTypeInfoStatic()) return (cls*)this;					\
		void* rv = base1::GetBaseAddress(to);							\
		if (!rv) rv = base2::GetBaseAddress(to);						\
		return rv;														\
	}																	\

///	���s���^�������N���X�����ׂ������o�̎����D���ۃN���X�ŁD2�̃N���X���p������ꍇ
#define DEF_UTTYPEINFOABST3(cls, base1, base2, base3)					\
	const UTTypeInfo* cls::GetTypeInfoStatic(){							\
		static const UTTypeInfo* base[] = { base1::GetTypeInfoStatic(), \
			base2::GetTypeInfoStatic(), base3::GetTypeInfoStatic(), NULL };	\
		static UTTypeInfoImpAbst<cls> info(#cls, base);					\
		return &info;													\
	}																	\
	void* cls::GetBaseAddress(const UTTypeInfo* to) const{				\
		if (to==GetTypeInfoStatic()) return (cls*)this;					\
		void* rv = base1::GetBaseAddress(to);							\
		if (!rv) rv = base2::GetBaseAddress(to);						\
		if (!rv) rv = base3::GetBaseAddress(to);						\
		return rv;														\
	}																	\

#define GETCLASSNAME(p)		(p->GetTypeInfo()->className)
#define GETCLASSNAMES(T)	(T::GetTypeInfoStatic()->className)

///	�_�C�i�~�b�N�L���X�g
#define DCAST(T, p)	UTDcastImp<T>(p)
template <class T, class P> T* UTDcastImp(P p){
	if (p && p->GetTypeInfo()->Inherit(T::GetTypeInfoStatic())) return (T*)&*(p);
	return NULL;
}

///	�L���X�g�Ɏ��s�����assert����_�C�i�~�b�N�L���X�g
#define ACAST(T, p)	UTAcastImp<T>(p)
template <class T, class P> T* UTAcastImp(P p){
	if (p && p->GetTypeInfo()->Inherit(T::GetTypeInfoStatic())) return (T*)&*(p);
	UTAcastError((p)->GetTypeInfo()->className);
	return NULL;
}
///	�_�C�i�~�b�N�L���X�g�D�^���Ƀ|�C���^�^���g���o�[�W����
#define DCASTP(TP, p)	UTDcastpImp<TP>(p)
template <class TP, class P> TP UTDcastpImp(P p){
	TP tp=NULL;
	if (p && p->GetTypeInfo()->Inherit(tp->GetTypeInfoStatic())) return (TP)&*(p);
	return NULL;
}
///	�L���X�g�Ɏ��s�����assert����_�C�i�~�b�N�L���X�g�D�^���Ƀ|�C���^�^���g���o�[�W����
#define ACASTP(TP, p)	UTAcastpImp<TP>(p)
template <class TP, class P> TP UTAcastpImp(P p){
	TP tp=NULL;
	if (p && p->GetTypeInfo()->Inherit(tp->GetTypeInfoStatic())) return (TP)&*(p);
	UTAcastError((p)->GetTypeInfo()->className);
	return NULL;
}
///	�L���X�g�Ɏ��s�����ꍇ��assert�֐�
char* UTAcastError(const char* );

}
#endif
