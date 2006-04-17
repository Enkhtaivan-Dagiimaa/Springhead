#ifndef SPR_OBJECT_H
#define SPR_OBJECT_H
#include "../SprBase.h"
#include <iosfwd>

namespace Spr{;

struct ObjectIf;

/**	�C���^�t�F�[�X�̌^���N���X�̊�{�N���X�D�N���X����p���֌W�������Ă��āCICAST()�Ȃǂ����p����D
1�̃C���^�t�F�[�X�N���X�ɕt��1�̃C���X�^���X���ł���D	*/
class IfInfo{
public:
//	static int maxId;
//	int id;
	const char* className;
	IfInfo** base;
	IfInfo(const char* cn, IfInfo** b): className(cn), base(b){}
	virtual const char* ClassName() const =0;
	virtual void* GetSprObject(ObjectIf* intf)const =0;
	virtual ObjectIf* GetIf(void* obj)const =0;
	bool Inherit(const char* key) const;
	bool Inherit(const IfInfo* key) const;
protected:
	virtual ~IfInfo() {};
};
///	IfInfo�̎����D1�N���X1�C���X�^���X
template <class T>
class IfInfoImp: public IfInfo{
public:
	IfInfoImp(const char* cn, IfInfo** b): IfInfo(cn, b){}
	virtual const char* ClassName() const { return className; }
	virtual void* CreateInstance() const{ return 0;}
	virtual void* GetSprObject(ObjectIf* intf)const;
	virtual ObjectIf* GetIf(void* obj)const;
};

///	���s���^�������N���X�����ׂ������o�̐錾���D
#define IF_DEF(cls)										\
public:													\
	static IfInfoImp<cls##If> ifInfo;					\
	virtual const IfInfo* GetIfInfo() const {			\
		return &ifInfo;									\
	}													\
	static const IfInfo* GetIfInfoStatic(){				\
		return &ifInfo;									\
	}													\

///	�C���^�t�F�[�X�N���X�̃L���X�g
#define ICAST(T, p)	UTIcastImp<T>(p)
template <class T, class P> T* UTIcastImp(P p){
	void* obj = p ? p->GetIfInfo()->GetSprObject((ObjectIf*)p) : NULL;
	return (T*)T::GetIfInfoStatic()->GetIf(obj);
}

///	���ׂẴC���^�t�F�[�X�N���X�̊�{�N���X
struct ObjectIf{
	IF_DEF(Object);
	virtual int AddRef()=0;
	virtual int DelRef()=0;
	virtual int RefCount()=0;

	virtual ~ObjectIf(){}
	///	�f�o�b�O�p�̕\��
	virtual void Print(std::ostream& os) const =0;	
	///	�I�u�W�F�N�g�̍쐬
	virtual ObjectIf* CreateObject(const IfInfo* info, const void* desc)=0;
	///	�q�I�u�W�F�N�g�̐�
	virtual size_t NChildObject() const =0;
	///	�q�I�u�W�F�N�g�̎擾
	virtual ObjectIf* GetChildObject(size_t pos) = 0;
	virtual const ObjectIf* GetChildObject(size_t pos) const = 0;
	///	�q�I�u�W�F�N�g�̒ǉ�
	virtual bool AddChildObject(ObjectIf* o)=0;
	///	�f�[�^�̓ǂݏo��(�Q�Ɣ� NULL��Ԃ����Ƃ�����)
	virtual void* GetDescAddress() = 0;
	///	�f�[�^�̓ǂݏo��(�R�s�[�� �K����������Ă���(�͂�))
	virtual void GetDesc(void* desc) = 0;
};

///	�C���^�t�F�[�X�N���X�ւ̃|�C���^�̔z��
struct ObjectIfs: public UTStack<ObjectIf*>{
};

///	���O�����I�u�W�F�N�g�̃C���^�t�F�[�X
struct NamedObjectIf: public ObjectIf{
	IF_DEF(NamedObject);
	///	���O�̎擾
	virtual const char* GetName() const =0;
	///	���O�̐ݒ�
	virtual void SetName(const char* n) =0;
};

struct SceneIf;
///	�V�[���O���t���\������m�[�h�̃C���^�t�F�[�X
struct SceneObjectIf: NamedObjectIf{
	IF_DEF(SceneObject);
	///	����Scene�̎擾
	virtual SceneIf* GetScene() =0;
};

}

#endif
