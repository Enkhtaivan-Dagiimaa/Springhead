#ifndef FIFILE_H
#define FIFILE_H

#include <SprFileIO.h>
#include <FileIO/FITypeDesc.h>
#include "FINodeHandler.h"

namespace Spr{;
class FIFileContext;
class FISaveContext;
class FIFile:public InheritObject<FIFileIf, Object>{
public:
	OBJECTDEFABST(FIFile);
	virtual bool Load(ObjectIfs& objs, const char* fn);
	virtual void Load(FIFileContext* fc);
	virtual bool Save(const ObjectIfs& objs, const char* fn);
	virtual void Save(FISaveContext* sc);
protected:
	///	�t�@�C���ɃZ�[�u�E�t�@�C�����烍�[�h����^�̏��(FITypeDesc)
	FITypeDescDb typeDb;
	///	���[�h���Ɏ������[�h�Ƃ͕ʂ̏������K�v�ȃm�[�h�̃n���h��
	FINodeHandlers handlers;
	///	���[�h�̎���
	virtual void LoadImp(FIFileContext* fc)=0;
};

template <class intf, class base>
struct InheritFIFile:public InheritObject<intf, base>{
	virtual bool Load(ObjectIfs& objs, const char* fn){ return base::Load(objs, fn); }
	virtual bool Save(const ObjectIfs& objs, const char* fn){ return base::Save(objs, fn); }
};


}
#endif
