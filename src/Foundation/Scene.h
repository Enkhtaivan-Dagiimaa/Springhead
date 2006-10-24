/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SCENE_H
#define SCENE_H
#include <SprFoundation.h>
#include "Object.h"
#include <set>
#include <map>

namespace Spr {;


class ObjectNamesLess{
public:
	bool operator () (const NamedObject* o1, const NamedObject* o2) const;
};
/**	���O�ƃV�[���O���t�̃I�u�W�F�N�g�̑Ή��\
	���O���L�[�ɂ����Z�b�g��p�ӂ��C���O�̈�Ӑ��̕ۏ؂ƃI�u�W�F�N�g��
	��������������D	*/
class ObjectNames:public std::set<NamedObject*, ObjectNamesLess >{
public:
	typedef std::map<UTString, UTString> TNameMap;
	class ObjectKey:public NamedObject{
	public:
		ObjectKey();
		~ObjectKey();
		DEF_UTTYPEINFODEF(ObjectKey);
	};
	TNameMap nameMap;
	static ObjectKey key;
	void Print(std::ostream& os) const;

	///	���O����I�u�W�F�N�g���擾
	typedef std::pair<iterator, iterator> range_type;
	NamedObject* Find(UTString name, UTString cn="") const {
		key.name = name;
		iterator it = ((ObjectNames*)this)->find(&key);
		if (it == end()) return NULL;
		NamedObject* obj = *it;
		if (!cn.length()) return obj;
		if (obj->GetTypeInfo()->Inherit(cn.c_str())) return obj;
		return NULL;
	}
	/**	�I�u�W�F�N�g�̒ǉ��C
		���O�̂Ȃ��I�u�W�F�N�g�͒ǉ��ł��Ȃ��D���̏ꍇ false ��Ԃ��D
		�ǉ��ɐ�������� true�D ���łɓo�^����Ă����ꍇ�� false ��Ԃ��D
		���O���Ԃ���ꍇ�́C�ǉ�����I�u�W�F�N�g�̖��O���ύX�����D	*/
	bool Add(NamedObject* obj);
	///	�I�u�W�F�N�g�̍폜
	bool Del(NamedObject* key){
		iterator it = find(key);
		if (it==end()) return false;
		erase(it);
		return true;
	}
	///	�I�u�W�F�N�g�̍폜
	bool Del(UTString name){
		NamedObject key;
		key.name = name;
		key.AddRef();
		iterator it = find(&key);
		key.DelRef();
		if (it==end()) return false;
		erase(it);
		return true;
	}
	///	���ׂč폜
	void Clear();
};
inline std::ostream& operator << (std::ostream& os, const ObjectNames& ns){
	ns.Print(os); return os;
}

class SPR_DLL NameManager:public InheritNamedObject<NameManagerIf, NamedObject>{
public:
	OBJECT_DEF(NameManager);
protected:
	/*	���O�ƃI�u�W�F�N�g�̑Ή��\  */
	ObjectNames names;
	
	///@name	���O��Ԃ��\�����邽�߂̃c���[
	//@{
	///
	typedef std::vector<NameManager*> NameManagers;
	///	�q�̖��O���
	NameManagers childManagers;
	//@}

public:
	NameManager();
	~NameManager();
	virtual void SetNameManager(NameManager* s);
	void AddChildManager(NameManager* c);
	void DelChildManager(NameManager* c);
	///	�^�Ɩ��O����I�u�W�F�N�g���擾
	template <class T> void FindObject(UTRef<T>& t, UTString name){
		T* p;
		FindObject(p, name);
		t = p;
	}
	template <class T> void FindObject(T*& t, UTString name){
		NamedObject* p = FindObject(name, GETCLASSNAMES(T));
		t = DCAST(T, p);
	}
	virtual NamedObjectIf* FindObject(UTString name, UTString cls="");
	typedef ObjectNames::iterator SetIt;
//	typedef std::pair<SetIt, SetIt> SetRange;
//	SetRange RangeObject(UTString n){ return names.Range(n); }
	
	ObjectNames::TNameMap& GetNameMap(){ return names.nameMap; }

	void Clear();

	///	�f�o�b�O�p
	void Print(std::ostream& os) const;
	friend class NamedObject;
protected:
	NamedObjectIf* FindObjectExact(UTString name, UTString cls="");
	NamedObjectIf* FindObjectFromDescendant(UTString name, UTString cls="");
};

template <class intf, class base>
struct InheritNameManager:public InheritNamedObject<intf, base>{
	///	�^�Ɩ��O����I�u�W�F�N�g���擾
	template <class T> void FindObject(UTRef<T>& t, UTString name){
		T* p;
		FindObject(p, name);
		t = p;
	}
	template <class T> void FindObject(T*& t, UTString name){
		NamedObjectIf* p = FindObject(name, GETCLASSNAMES(T));
		t = DCAST(T, p);
	}
	virtual NamedObjectIf* FindObject(UTString name, UTString cls=""){
		return base::FindObject(name, cls);
	}
};

/**	�V�[���̊�{�N���X	*/
class SPR_DLL Scene:public InheritNameManager<SceneIf, NameManager>{
	OBJECT_DEF(Scene);
public:
	///	�R���X�g���N�^
};
template <class intf, class base>
struct InheritScene:public InheritNameManager<intf, base>{
};

class SPR_DLL Sdk:public InheritNameManager<SdkIf, NameManager>{
	UTRef<TypeInfoManager> typeInfoManager;	//	typeInfo/IfInfo��sdk��������O�ɏ����邱�Ƃ������B
protected:
	friend struct SdkIf;
	static std::vector< UTRef<FactoryBase> > sdkFactories;
public:
	OBJECT_DEF(Sdk);
	static void SPR_CDECL RegisterFactory(FactoryBase* sdkFactory);
public:
	Sdk();
	virtual ~Sdk(){}
};

}
#endif
