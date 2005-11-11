#ifndef FOUNDATION_OBJECT_H
#define FOUNDATION_OBJECT_H

#include <Springhead.h>
#include <Base/Base.h>


namespace Spr{;


///	Object ���p�������N���X�̓����o�Ƃ��Ă��̃}�N�������D
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

class Scene;
/**	�V�[���O���t�̃I�u�W�F�N�g�^�D
	�N���X����Ԃ��֐������D	*/
class Object:public ObjectIf, public UTTypeInfoBase, public UTRefCount{
	friend class ObjectNames;
	UTString name;				///<	���O
	UTString nameSpace;			///<	���O
protected:
	Scene* scene;
public:
	Object():scene(NULL){}
	///	�N���X���̎擾�Ȃǂ̊�{�@�\�̎���
	OBJECTDEF(Object);

	virtual void SetScene(Scene*s){ scene = s; }
	///	���O�̎擾
	const char* GetName() const { return name.c_str(); }
	const char* GetNameSpace() const { return nameSpace.c_str(); }
	///	���O�̐ݒ�
	void SetName(const char* n);
	int AddRef(){return UTRefCount::AddRef();}
	int DelRef(){return UTRefCount::DelRef();}
	int RefCount(){return UTRefCount::RefCount();}

#define BASEIMP_OBJECT(base)											\
	const char* GetName() const { return base::GetName(); }				\
	const char* GetNameSpace() const { return base::GetNameSpace(); }	\
	void SetName(const char* n) { base::SetName(n); }					\
	int AddRef(){return base::AddRef();}								\
	int DelRef(){return base::AddRef();}								\
	int RefCount(){return base::AddRef();}								\


	///	�f�o�b�O�p�̕\��
	virtual void Print(std::ostream& os) const;

/*
	///	���L���Ă���I�u�W�F�N�g�̐�
	virtual size_t NChildObjects(){ return 0; }
	///	���L���Ă���I�u�W�F�N�g
	virtual Object* ChildObject(size_t i){ return NULL; }
	///	�Q�Ƃ��Ă���I�u�W�F�N�g�̐�
	virtual size_t NReferenceObjects(){ return 0; }
	///	�Q�Ƃ��Ă���I�u�W�F�N�g
	virtual Object* ReferenceObject(size_t i){ return NULL; };
	///	�q�I�u�W�F�N�g�̒ǉ�(���L�E�Q�Ƃ���Ȃ�)
	virtual bool AddChildObject(Object* o){ return false; }
	///	�q�I�u�W�F�N�g�̍폜
	virtual bool DelChildObject(Object* o){ return false; }
	///	�q�ɂȂ肦��I�u�W�F�N�g�̌^���̔z��
	virtual const UTTypeInfo** ChildCandidates(){ return NULL; }
*/
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
