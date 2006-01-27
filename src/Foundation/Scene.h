#ifndef SCENE_H
#define SCENE_H
#include <SprFoundation.h>
#include <set>
#include <map>

namespace Spr {;


class ObjectNamesLess{
public:
	bool operator () (NamedObject* o1, NamedObject* o2) const;
};
/**	���O�ƃV�[���O���t�̃I�u�W�F�N�g�̑Ή��\
	���O���L�[�ɂ����Z�b�g��p�ӂ��C���O�̈�Ӑ��̕ۏ؂ƃI�u�W�F�N�g��
	��������������D	*/
class ObjectNames:public std::set<UTRef<NamedObject>, ObjectNamesLess >{
public:
	typedef std::map<UTString, UTString> TNameMap;
	class ObjectKey:public NamedObject{
	public:
		BASEIMP_NAMEDOBJECT(NamedObject)
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
		UTString className = cn;
		key.typeInfo.className = className.c_str();
		const_iterator lb = lower_bound(&key);
		if (lb==end()) return NULL;
		ObjectNamesLess less;
		if (less(*lb, &key)) return NULL;		//	���������̂��Ȃ��ꍇ
		const_iterator it = lb;
		++it;
		if (it == end()) return *lb;
		if(less(&key, *it)) return *lb;		//	���������̂�1��
		return NULL;						//	���������̂������L��ꍇ
	}
	range_type Range(UTString name, UTString cn=""){
		key.name = name;
		UTString className = cn;
		key.typeInfo.className = className.c_str();
		iterator lb = lower_bound(&key);
		if (lb==end()) return range_type(end(), end());
		ObjectNamesLess less;
		iterator it = lb;
		while(it != end() && !less(*it, &key)){
			++it;
		}
		return range_type(lb, it);
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
};
inline std::ostream& operator << (std::ostream& os, const ObjectNames& ns){
	ns.Print(os); return os;
}

class SPR_DLL NameManager:public NameManagerIf, public NamedObject{
public:
	OBJECTDEF(NameManager);
	BASEIMP_NAMEDOBJECT(NamedObject);
protected:
	///	���O�ƃI�u�W�F�N�g�̑Ή��\	
	ObjectNames names;

public:
	///	���O����I�u�W�F�N�g���擾
	NamedObject* FindObject(UTString name, UTString ns=""){ return names.Find(name, ns); }
	///	�^�Ɩ��O����I�u�W�F�N�g���擾
	template <class T> void FindObject(UTRef<T>& t, UTString name, UTString ns=""){
		T* p;
		FindObject(p, name, ns);
		t = p;
	}
	template <class T> void FindObject(T*& t, UTString name, UTString ns=""){
		NamedObject* p = names.Find(name, GETCLASSNAMES(T));
		t = DCAST(T, p);
	}
	virtual NamedObjectIf* FindObject(UTString name, const char* cls){
		NamedObject* p = names.Find(name, cls);
		return p;
	}

	typedef ObjectNames::iterator SetIt;
	typedef std::pair<SetIt, SetIt> SetRange;
	SetRange RangeObject(UTString n){ return names.Range(n); }
	
	ObjectNames::TNameMap& GetNameMap(){ return names.nameMap; }

	///	���O�\����C�Q�Ƃ���Ă��Ȃ��I�u�W�F�N�g���폜����D
	void ClearName();

	///	�f�o�b�O�p
	void Print(std::ostream& os) const { names.Print(os); }
	friend class NamedObject;
};
#define BASEIMP_NAMEMANAGER(base)	BASEIMP_NAMEDOBJECT(base)	\
	virtual NamedObjectIf* FindObject(UTString name, const char* cls){	\
		return base::FindObject(name, cls);								\
	}																	\


/**	�V�[���O���t�̃g�b�v�m�[�h�D�����E���_�����D
	�����_���ƃV�[���O���t�̊֌W���[�����߁C
	�����_�����Ⴄ�ƃV�[���O���t�͂��̂܂܎g�p�ł��Ȃ��D
	�V�[���O���t�́C���Ƃ��΃����_����OpenGL�Ȃ�CdisplayList
	�������Ă��邵�CD3D�Ȃ�� ID3DXMesh�������Ă���D
	OpenGL�̃V�[���O���t��D3D�ɕϊ����邽�߂ɂ́C��xDocument��
	�Z�[�u���āCD3D�`���Ń��[�h���Ȃ���΂Ȃ�Ȃ��D	*/
class SPR_DLL Scene:public NameManager, public SceneIf{
	OBJECTDEF(Scene);
	BASEIMP_NAMEMANAGER(NameManager);
public:
	///	�R���X�g���N�^
	Scene();
	virtual ~Scene(){Clear();}
	///	�V�[������ɂ���D
	void Clear();
	///
	void Print(std::ostream& os) const { NameManager::Print(os); }
};
#define	BASEIMP_SCENE(base) 	BASEIMP_NAMEMANAGER(base);

}
#endif
