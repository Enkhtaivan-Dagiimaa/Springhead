#ifndef SPR_TYPE_H
#define SPR_TYPE_H
#include <springhead.h>
#include <iosfwd>

namespace Spr{;

struct ObjectIf;

class IfInfo{
public:
	const char* className;
	IfInfo** base;
	IfInfo(const char* cn, IfInfo** b): className(cn), base(b){}
	virtual const char* ClassName() const =0;
	virtual void* GetObject(ObjectIf* intf)const =0;
	virtual ObjectIf* GetIf(void* obj)const =0;
};

template <class T>
class IfInfoImp: public IfInfo{
public:
	IfInfoImp(const char* cn, IfInfo** b): IfInfo(cn, b){}
	virtual const char* ClassName() const { return className; }
	virtual void* CreateInstance() const{ return 0;}
	virtual void* GetObject(ObjectIf* intf)const;
	virtual ObjectIf* GetIf(void* obj)const;
};

///	���s���^�������N���X�����ׂ������o�̐錾���D���ۃN���X��
#define IF_DEF(cls)										\
public:													\
	static IfInfoImp<cls##If> ifInfo;					\
	virtual const IfInfo* GetIfInfo() const {			\
		return &ifInfo;									\
	}													\
	static const IfInfo* GetIfInfoStatic(){				\
		return &ifInfo;									\
	}													\

///	�C���^�t�F�[�X�̃L���X�g
#define ICAST(T, p)	UTIcastImp<T>(p)
template <class T, class P> T* UTIcastImp(P p){
	void* obj = p->GetIfInfo()->GetObject((ObjectIf*)p);
	return (T*)T::GetIfInfoStatic()->GetIf(obj);
}

struct ObjectIf{
	IF_DEF(Object);
	virtual int AddRef()=0;
	virtual int DelRef()=0;
	virtual int RefCount()=0;

	virtual ~ObjectIf(){}
	///	���O�̎擾
	virtual const char* GetName() const =0;
	///	���O�̐ݒ�
	virtual void SetName(const char* n) =0;
	///	�f�o�b�O�p�̕\��
	virtual void Print(std::ostream& os) const =0;

/*	
//	�ȉ��͖{���ɕK�v���v����
	///	���L���Ă���I�u�W�F�N�g�̐�
	virtual size_t NChildObjectIfs(){ return 0; }
	///	���L���Ă���I�u�W�F�N�g
	virtual ObjectIf* ChildObjectIf(size_t i){ return NULL; }
	///	�Q�Ƃ��Ă���I�u�W�F�N�g�̐�
	virtual size_t NReferenceObjectIfs(){ return 0; }
	///	�Q�Ƃ��Ă���I�u�W�F�N�g
	virtual ObjectIf* ReferenceObjectIf(size_t i){ return NULL; }
	///	�q�I�u�W�F�N�g�̒ǉ�(���L�E�Q�Ƃ���Ȃ�)
	virtual bool AddChildObjectIf(ObjectIf* o){ return false; }
	///	�q�I�u�W�F�N�g�̍폜
	virtual bool DelChildObjectIf(ObjectIf* o){ return false; }
	///	�q�ɂȂ肦��I�u�W�F�N�g�̌^���̔z��
	virtual const UTTypeInfo** ChildCandidates(){ return NULL; }
*/
	
};
}

#endif
