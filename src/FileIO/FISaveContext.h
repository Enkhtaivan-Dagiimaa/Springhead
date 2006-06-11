#ifndef FISAVECONTEXT_H
#define FISAVECONTEXT_H

#include <Foundation/Object.h>
#include <Foundation/UTTypeDesc.h>
#include <fstream>

namespace Spr{;

class FINodeHandlers;
class FISaveContext{
public:
	///	�G���[���b�Z�[�W�o�͗p�̃X�g���[��
	std::ostream* errorStream;
	///	�Z�[�u����t�@�C���̖��O
	UTString fileName;
	///	�Z�[�u�t�@�C���̃t�@�C���X�g���[��
	std::ofstream file;
	///	�m�[�h�n���h��
	FINodeHandlers* handlers;
	///	�^DB
	UTTypeDescDb* typeDb;
	///	UTTypeDesc�̃t�B�[���h�ւ̃C�^���[�^�̃X�^�b�N
	UTTypeDescFieldIts fieldIts;
	///	�Z�[�u����f�B�X�N���v�^�̃X�^�b�N�D�l�X�g�����g�ݗ��Č^�ɔ����ăX�^�b�N�ɂȂ��Ă���D
	UTStack< UTRef<FINodeData> > datas;
	///
	std::set<ObjectIf*> savedObjects;
	
	FISaveContext();
	///	�Z�[�u���̃I�u�W�F�N�g�̃X�^�b�N
	ObjectIfs objects;
	///	�Z�[�u�p�Ƀt�@�C�����J��
	void Open(const char* fn);
	///	���b�Z�[�W�̏o��
	void Message(const char* msg);
	///	�G���[���b�Z�[�W�̏o��
	void ErrorMessage(const char* msg);

	UTString GetNodeTypeName();
	UTString GetNodeName();
};

}

#endif
