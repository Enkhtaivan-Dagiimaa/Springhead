#ifndef SPR_TYPE_H
#define SPR_TYPE_H
#include <iosfwd>

namespace Spr{;

struct ObjectIf{
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
