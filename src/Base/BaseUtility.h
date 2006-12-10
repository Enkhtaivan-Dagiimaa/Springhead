/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef Spr_BASE_UTILITIES_H
#define Spr_BASE_UTILITIES_H
#include "BaseDebug.h"
#include <algorithm>
#include <iosfwd>
#include <vector>
#include <typeinfo>

namespace Spr {

/**	\defgroup gpBaseUtility ���[�e�B���e�B�N���X	*/
//@{

/**	@file	BaseUtility.h	���̑��̃��[�e�B���e�B�[�N���X�E�֐��D*/

typedef std::string UTString;
inline bool operator < (const UTString& u1, const UTString& u2){
	return u1.compare(u2) < 0;
}
struct UTStringLess{
	bool operator ()(const UTString& t1, const UTString& t2) const{
		return t1.compare(t2) < 0;
	}
};

template <class T>
bool UTContentsEqual(const T& t1, const T& t2){
	return *t1 == *t2;
}

template <class T>
struct UTContentsLess{
	bool operator ()(const T& t1, const T& t2) const{
		return *t1 < *t2;
	}
};

template <class T, class M, M T::* m>
struct UTMemberLess{
	bool operator ()(const T& t1, const T& t2) const{
		return *t1.*m < *t2.*m;
	}
};

class UTPadding{
public:
	int len;
	UTPadding(int i){len = i;}
};
std::ostream& operator << (std::ostream& os, UTPadding p);

struct UTEatWhite{
	UTEatWhite(){}
};
std::istream& operator >> (std::istream& is, const UTEatWhite& e);

///	�Q�ƃJ�E���^�D�J�E���g��0�ɂȂ��Ă�����ɏ����͂��Ȃ��D
class SPR_DLL UTRefCount{
	mutable int refCount;
public:
#ifndef _DEBUG
//	UTRefCount(){refCount = 0;}
//	UTRefCount(const UTRefCount&){refCount = 0;}
//	~UTRefCount(){ assert(refCount==0); }
	UTRefCount();
	UTRefCount(const UTRefCount& r);
	~UTRefCount();
#else
	static int nObject;
	UTRefCount();
	UTRefCount(const UTRefCount& r);
	~UTRefCount();
#endif
	UTRefCount& operator = (const UTRefCount& r){ return *this; }

	int AddRef() const { return ++ refCount; }
	int DelRef() const {
		assert(refCount > 0);
		return -- refCount;
	}
	int RefCount() const { return refCount; }
};

/**	�Q�ƃJ�E���^�p�̃|�C���^�D�����I�ɎQ�ƃJ�E���^�𑝌��C
	�I�u�W�F�N�g��delete������D
*/
template <class T>
class UTRef{
	T* obj;
	T*& Obj() const {return (T*&) obj;}
public:
	UTRef(T* t = NULL){
		Obj() = t;
		if (Obj()) Obj()->AddRef();
	}
	template <class E>
	UTRef(const UTRef<E>& r){
		Obj() = (T*)(E*)r;
		if (Obj()) Obj()->AddRef();
	}
	UTRef(const UTRef<T>& r){
		Obj() = r.Obj();
		if (Obj()) Obj()->AddRef();
	}
	~UTRef(){ if (Obj() && Obj()->DelRef() == 0) delete Obj(); }
/*	UTRef& operator =(T* t){
		if (Obj() != t){
			if (Obj() && Obj()->DelRef() == 0) delete Obj();
			Obj() = t;
			if (Obj()) Obj()->AddRef();
		}
		return *this;
	}
*/	template <class E>
	UTRef& operator =(const UTRef<E>& r){
		if (Obj() != r){
			if (Obj() && Obj()->DelRef() == 0) delete Obj();
			Obj() = r;
			if (Obj()) Obj()->AddRef();
		}
		return *this;
	}
	UTRef& operator =(const UTRef<T>& r){
		if (Obj() != r.Obj()){
			if (Obj() && Obj()->DelRef() == 0) delete Obj();
			Obj() = r;
			if (Obj()) Obj()->AddRef();
		}
		return *this;
	}
	operator T*() const {return Obj();}
	T* operator->() const {return Obj();}
	bool operator <(const UTRef& r) const { return Obj() < r.Obj(); }
};

/// UTRef�̔z��
template<class T, class CO = std::vector< UTRef<T> > >
class UTRefArray : public CO{
public:
	typedef typename CO::iterator iterator;
	UTRef<T> Erase(const UTRef<T>& ref){
		iterator it = std::find(CO::begin(), CO::end(), ref);
		if (it == CO::end()) return NULL;
		UTRef<T> rv = *it;
		erase(it);
		return rv;
	}
	UTRef<T>* Find(const UTRef<T>& ref){
		iterator it = std::find(CO::begin(), CO::end(), ref);
		if (it == CO::end()) return NULL;
		else return &*it;
	}
	UTRef<T>* Find(const UTRef<T>& ref) const {
		return ((UTRefArray<T, CO>*)this)->Find(ref);
	}
};

/**	��ʃI�u�W�F�N�g�p�C����delete �|�C���^
	�|�C���^��������Ƃ��ɃI�u�W�F�N�g��delete������D
*/
template <class T>
class UTDeleteRef{
	T* obj;
	T*& Obj() const {return (T*&) obj;}
	UTDeleteRef(const UTDeleteRef<T>& r){
		assert(0);	//	�R�s�[�����Ⴞ�߁D
	}
public:
	UTDeleteRef(T* t = NULL){
		Obj() = t;
	}
	~UTDeleteRef(){ delete Obj(); }
	UTDeleteRef& operator =(T* t){
		Obj() = t;
		return *this;
	}
	operator T*() const {return Obj();}
	T* operator->() const {return Obj();}
	bool operator <(const UTDeleteRef& r) const { return Obj() < r.Obj(); }
};

///	�V���O���g���N���X
template <class T>
T& Singleton(){
	static T t;
	return t;
}

///	�X�^�b�N�� vector 
template <class T, class CO=std::vector<T> >
class UTStack: public CO{
public:
	typedef CO container;
	T Pop(){
		assert(CO::size());
		T t=CO::back(); CO::pop_back(); return t;
	}
	void Push(const T& t=T()){ CO::push_back(t); }
	T& Top(){
		assert(CO::size());
		return CO::back();
	}
};

///	�c���[�̃m�[�h(�e�q�������Q��)
template <class T, class CO=std DOUBLECOLON vector< UTRef<T> > >
class UTTreeNode{
private:
	void clear();
protected:
	T* parent;
	CO children;
public:
	typedef CO container_t;
	///
	UTTreeNode():parent(NULL){}
	virtual ~UTTreeNode(){
/*		for(CO::iterator it = children.begin(); it != children.end(); ++it){
			(*it)->parent = NULL;
		}
		children.clear();
*/
		while(children.size()){
			 children.back()->parent = NULL;
			 children.pop_back();
		}
	}
	///@name �c���[�̑���
	//@{
	///	�e�m�[�h���擾����D
	T* GetParent(){ return parent; }
	const T* GetParent() const { return parent; }
	///	�e�m�[�h��ݒ肷��D
	void SetParent(T* n){
		if (parent == n) return;						//	�ύX���Ȃ���Ή������Ȃ��D
		//	�r����RefCount��0�ɂȂ��ď����Ȃ��悤�ɁC��ɐV�����m�[�h�̎q�ɂ���D
		if (n) n->children.push_back((T*)this);
		if (parent) {									//	�Â��e�m�[�h�̎q���X�g����폜
			TYPENAME CO::iterator it = std::find(parent->children.begin(), parent->children.end(), UTRef<T>((T*)this));
			if (it != parent->children.end()) parent->children.erase(it);
		}
		parent = n;										//	parent ��V�����m�[�h�ɐ؂�ւ���D
	}
	///	�q�m�[�h�D
	CO& Children(){ return children; }
	///	�q�m�[�h�D
	const CO& Children() const { return children; }
	///	�q�m�[�h��ǉ�����
	void AddChild(UTRef<T> c){ c->SetParent((T*)this); }
	///	�q�m�[�h���폜����
	void DelChild(UTRef<T> c){ if(c->GetParent() == this) c->SetParent(NULL); }
	///	�q�m�[�h�����ׂč폜����D
	void ClearChildren(){
		while(children.size()){
			 children.back()->parent = NULL;
			 children.pop_back();
		}
	}
	///
	template <class M>
	void ForEachChild(M m){
		for(TYPENAME CO::iterator it = children.begin(); it !=children.end(); ++it){
			T* t = *it;
			(t->*m)();
		}
	}
	///
	template <class M>
	void Traverse(M m){
		  m(this);
		for(TYPENAME CO::iterator it = children.begin(); it !=children.end(); ++it){
			(*it)->Traverse(m);
		}
	}
	template <class M, class A>
	void Traverse(M m, A a){
		m((T*)this, a);
		for(TYPENAME CO::iterator it = children.begin(); it !=children.end(); ++it){
			(*it)->Traverse(m, a);
		}
	}
	template <class T2, class M>
	void MemberTraverse(T2 t, M m){
		(t->*m)(this);
		for(TYPENAME CO::iterator it = children.begin(); it !=children.end(); ++it){
			(*it)->MemberTraverse(t, m);
		}
	}
	template <class E, class M, class A>
	void MemberTraverse(E e, M m, A& a){
		(e->*m)((T*)this, a);
		for(TYPENAME CO::iterator it = children.begin(); it !=children.end(); ++it){
			(*it)->MemberTraverse(e, m, a);
		}
	}
};

	
/* assert_cast
		 SPR_DEBUG��`����dynamic_cast�A����ȊO�ł�static_cast�Ƃ��ē����B
		 dynamic_cast�Ɏ��s�����std::bad_cast��O�𔭐�����B
*/
template <class T, class U>
inline T assert_cast(U u){
#ifdef SPR_DEBUG
	T t= dynamic_cast<T>(u);
	if (u && !t) throw std::bad_cast();
	return t;
#else
	return static_cast<T>(u);
#endif
}
//@}

}	//	namespace Spr

#endif
