#ifndef FIFILE_H
#define FIFILE_H

#include <SprFileIO.h>
#include <FileIO/FITypeDesc.h>
#include "FINodeHandler.h"

namespace Spr{;
class FILoadContext;
class FISaveContext;
class FIFile:public InheritObject<FIFileIf, Object>{
protected:
	///	�t�@�C���ɃZ�[�u�E�t�@�C�����烍�[�h����^�̏��(FITypeDesc)
	FITypeDescDb typeDb;
	///	���[�h���Ɏ������[�h�Ƃ͕ʂ̏������K�v�ȃm�[�h�̃n���h��
	FINodeHandlers handlers;
public:
	OBJECTDEFABST(FIFile);
	virtual bool Load(ObjectIfs& objs, const char* fn);
	virtual void Load(FILoadContext* fc);
	virtual bool Save(const ObjectIfs& objs, const char* fn);
	virtual void Save(const ObjectIfs& objs, FISaveContext* sc);
protected:
	///	���[�h�̎���
	virtual void LoadImp(FILoadContext* fc)=0;
	///
	void SaveNode(FISaveContext* sc, ObjectIf* obj);
	///
	void SaveBlock(FISaveContext* sc);

	///	�ۑ������̃n���h���[
	//@{
	///	�t�@�C���J�n���̏���
	virtual void OnSaveFileStart(FISaveContext* sc){}
	///	�t�@�C���I�����̏���
	virtual void OnSaveFileEnd(FISaveContext* sc){}
	///	�m�[�h�J�n���̏���
	virtual void OnSaveNodeStart(FISaveContext* sc){}
	///	�m�[�h�I�����̏���
	virtual void OnSaveNodeEnd(FISaveContext* sc){}
	///	�f�[�^�J�n���̏���
	virtual void OnSaveDataStart(FISaveContext* sc){}
	///	�f�[�^�I�����̏���
	virtual void OnSaveDataEnd(FISaveContext* sc){}
	///	�q�I�u�W�F�N�g�̕ۑ��J�n���̏���
	virtual void OnSaveChildStart(FISaveContext* sc){}
	///	�q�I�u�W�F�N�g�̕ۑ��I�����̏���
	virtual void OnSaveChildEnd(FISaveContext* sc){}

	///	�u���b�N�J�n���̏���
	virtual void OnSaveBlockStart(FISaveContext* sc){}
	///	�u���b�N�I�����̏���
	virtual void OnSaveBlockEnd(FISaveContext* sc){}
	///	�t�B�[���h�J�n���̏���
	virtual void OnSaveFieldStart(FISaveContext* sc, int nElements){}
	///	�t�B�[���h�I�����̏���
	virtual void OnSaveFieldEnd(FISaveContext* sc, int nElements){}
	///	�v�f�J�n���̏���
	virtual void OnSaveElementStart(FISaveContext* sc, int pos, bool last){}
	///	�v�f�I�����̏���
	virtual void OnSaveElementEnd(FISaveContext* sc, int pos, bool last){}
	///	bool�l�̕ۑ�
	virtual void OnSaveBool(FISaveContext* sc, bool val){}
	///	int�l�̕ۑ�
	virtual void OnSaveInt(FISaveContext* sc, int val){}
	///	real�l�̕ۑ�
	virtual void OnSaveReal(FISaveContext* sc, double val){}
	///	string�l�̕ۑ�
	virtual void OnSaveString(FISaveContext* sc, UTString val){}
	virtual void OnSaveRef(FISaveContext* sc){}
	//@}

};

template <class intf, class base>
struct InheritFIFile:public InheritObject<intf, base>{
	virtual bool Load(ObjectIfs& objs, const char* fn){ return base::Load(objs, fn); }
	virtual bool Save(const ObjectIfs& objs, const char* fn){ return base::Save(objs, fn); }
};


}
#endif
