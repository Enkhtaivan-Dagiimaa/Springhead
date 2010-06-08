/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "FileIO.h"
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

using namespace std;

namespace Spr{;

//---------------------------------------------------------------------------
//	FISaveFileMap

class FISaveFileMap : public UTFileMap{
public:
	///	�Z�[�u�t�@�C���̃t�@�C���X�g���[��
	std::ofstream file;
	
	virtual bool Map(const UTString fn, bool binary){
		ios_base::openmode mode = ios_base::out;
		// �o�C�i���t�H�[�}�b�g�̏ꍇbinary�t���O���K�v
		if(binary)
			mode |= ios_base::binary;
		
		file.open(fn.c_str(), mode);
		return file.is_open();
	}
	virtual void Unmap(){
		file.close();
	}
	virtual bool IsGood(){
		return file.good();
	}
};


//---------------------------------------------------------------------------
//	FISaveContext

FISaveContext::FISaveContext(){
	errorStream = &DSTR;
	typeDbs.Push(DBG_NEW UTTypeDescDb);
	handlerDbs.Push(DBG_NEW UTLoadHandlerDb);
}
void FISaveContext::PushFileMap(const UTString fn, bool binary){
	fileMaps.Push(DBG_NEW FISaveFileMap);
	fileMaps.Top()->Map(fn, binary);
}
void FISaveContext::PopFileMap(){
	fileMaps.Top()->Unmap();
	fileMaps.Pop();
}
ostream& FISaveContext::Stream(){
	return ((FISaveFileMap*)&*fileMaps.Top())->file;
}
void FISaveContext::Message(const char* msg){
	*errorStream << msg << std::endl;
}
void FISaveContext::ErrorMessage(const char* msg){
	*errorStream << "error: " << msg << std::endl;
}
UTString FISaveContext::GetNodeTypeName(){
	UTString rv(DCAST(Object, objects.back())->GetTypeInfo()->ClassName());
	return rv;
}
UTString FISaveContext::GetNodeName(){
	NamedObject* n = objects.back()->Cast();
	UTString rv;
	if (n && n->GetName()) rv = n->GetName();
	return rv;
}

void FISaveContext::RegisterGroupToDb(const char* gp){
	const char* p = gp;
	while(1){
		const char* end = strchr(p, ' ');
		if (!end) end = gp + strlen(gp);
		if (p < end){
			UTString group(p, end);
			p = end+1;
			*handlerDbs.Top() += *UTLoadHandlerDbPool::Get(group.c_str());
			*typeDbs.Top() += *UTTypeDescDbPool::Get(group.c_str());
		}else{
			break;
		}
	}
}

};

