#ifndef FOUNDATION_OBJECT_H
#define FOUNDATION_OBJECT_H

#include <SprFoundation.h>
#include <Base/Base.h>

namespace Spr{;


#define IF_IMP_COMMON(cls)															\
	IfInfoImp<cls##If> cls##If::ifInfo = IfInfoImp<cls##If>(#cls, cls##_BASEIF);	\
	template <> \
	void* IfInfoImp<cls##If>::GetSprObject(ObjectIf* i)const{							\
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

///	Object�h���N���X�̎��s���^���

#define OBJECTDEF(cls)					DEF_UTTYPEINFODEF(cls)
#define OBJECTDEFABST(cls)				DEF_UTTYPEINFOABSTDEF(cls)
#define OBJECTIMPBASE(cls)				DEF_UTTYPEINFO(cls)
#define OBJECTIMPBASEABST(cls)			DEF_UTTYPEINFOABST(cls)
#define OBJECTIMP(cls, base)			DEF_UTTYPEINFO1(cls, base)
#define OBJECTIMP2(cls, b1, b2)			DEF_UTTYPEINFO2(cls, b1, b2)
#define OBJECTIMP3(cls, b1, b2, b3)		DEF_UTTYPEINFO3(cls, b1, b2, b3)
#define OBJECTIMPABST(cls, base)		DEF_UTTYPEINFOABST1(cls, base)
#define OBJECTIMPABST2(cls, b1, b2)		DEF_UTTYPEINFOABST2(cls, b1,b2)
#define OBJECTIMPABST3(cls, b1, b2, b3)	DEF_UTTYPEINFOABST3(cls, b1,b2,b3)

class Object;	
///	�C���^�t�F�[�X->�I�u�W�F�N�g�ւ̃L���X�g
#define OCAST(T, i)	OcastImp<T>(i)
template <class T, class I> T* OcastImp(const I* i){
	ObjectIf* oi = (ObjectIf*)i;
	void* obj = i ? i->GetIfInfo()->GetSprObject(oi) : NULL;
	return (T*)(Object*)obj;
}
	
/**	�SObject�̊�{�^	*/
class Object:public ObjectIf, public UTTypeInfoBase, public UTRefCount{
public:
	OBJECTDEF(Object);		///<	�N���X���̎擾�Ȃǂ̊�{�@�\�̎���

	virtual int AddRef(){return UTRefCount::AddRef();}
	virtual int DelRef(){return UTRefCount::DelRef();}
	virtual int RefCount(){return UTRefCount::RefCount();}

	///	�f�o�b�O�p�̕\��
	virtual void Print(std::ostream& os) const;
	///	�I�u�W�F�N�g�̍쐬
	virtual ObjectIf* CreateObject(const IfInfo* info, const void* desc){ return NULL; }
	///	�q�I�u�W�F�N�g�̐�
	virtual size_t NChildObject() const { return 0; }
	///	�q�I�u�W�F�N�g�̎擾
	virtual ObjectIf* GetChildObject(size_t pos) { return NULL; }
	virtual const ObjectIf* GetChildObject(size_t pos) const { 
		return ((Object*) this)->GetChildObject(pos);
	}
	///	�q�I�u�W�F�N�g�̒ǉ�
	virtual bool AddChildObject(ObjectIf* o){ return false; }
	///	�f�[�^�̓ǂݏo��
	virtual bool GetDesc(void* desc){ return false; }
	///	�f�[�^�̓ǂݏo��
	virtual void* GetDescAddress(){ return NULL; }
};
template <class intf, class base>
struct InheritObject:public intf, base{
	virtual int AddRef(){return base::AddRef();}
	virtual int DelRef(){return base::DelRef();}
	virtual int RefCount(){return base::RefCount();}				
	virtual ObjectIf* CreateObject(const IfInfo* i, const void* d){
		return base::CreateObject(i,d);
	}
	virtual void Print(std::ostream& os) const{ base::Print(os); }
	virtual size_t NChildObject() const { return base::NChildObject(); }
	virtual ObjectIf* GetChildObject(size_t pos){ return base::GetChildObject(pos); }
	virtual const ObjectIf* GetChildObject(size_t pos) const{
		return base::GetChildObject(pos);
	}
	virtual bool AddChildObject(ObjectIf* o){
		return base::AddChildObject(o);		
	}
	virtual bool GetDesc(void* desc){ return base::GetDesc(desc); }
	virtual void* GetDescAddress(){ return base::GetDescAddress(); }
};

class NameManager;
/**	���O������Object�^�D
	SDK��Scene�ɏ��L�����D	*/
class NamedObject: public InheritObject<NamedObjectIf, Object>{
	OBJECTDEF(NamedObject);		///<	�N���X���̎擾�Ȃǂ̊�{�@�\�̎���
protected:
	friend class ObjectNames;
	UTString name;				///<	���O
	NameManager* nameManager;	///<	���O�̌�����d���Ǘ���������́DScene��SDK�ȂǁD
public:
	NamedObject():nameManager(NULL){}
	///	���O�̎擾
	const char* GetName() const { return name.c_str(); }
	///	���O�̐ݒ�
	void SetName(const char* n);
	///	�f�o�b�O�p�̕\��
	virtual void Print(std::ostream& os) const;
	virtual void SetNameManager(NameManager* s){ nameManager = s; }
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
	OBJECTDEF(SceneObject);		///<	�N���X���̎擾�Ȃǂ̊�{�@�\�̎���
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


}
#endif
