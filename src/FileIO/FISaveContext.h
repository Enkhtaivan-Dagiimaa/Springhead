#ifndef FISAVECONTEXT_H
#define FISAVECONTEXT_H

#include <Foundation/Object.h>
#include <FileIO/FITypeDesc.h>
#include <fstream>

namespace Spr{;

class FINodeHandlers;
class FISaveContext{
public:
	UTString fileName;
	std::fstream file;
	///	�m�[�h�n���h��
	FINodeHandlers* handlers;
	///	�^DB
	FITypeDescDb* typeDb;
	///	FITypeDesc�̃t�B�[���h�ւ̃C�^���[�^�̃X�^�b�N
	FIFieldIts fieldIts;
	///	�Z�[�u����f�B�X�N���v�^�̃X�^�b�N�D�l�X�g�����g�ݗ��Č^�ɔ����ăX�^�b�N�ɂȂ��Ă���D
	UTStack< UTRef<FINodeData> > datas;
	
	///	�Z�[�u���̃I�u�W�F�N�g�̃X�^�b�N
	ObjectIfs objects;
	///
	ObjectIfs rootObjects;
	///
	void Open(const char* fn);
};

}

#endif
