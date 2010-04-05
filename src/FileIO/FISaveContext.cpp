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

#include "FISaveContext.h"
#include <fstream>
#include <sstream>

using namespace std;

namespace Spr{;

//---------------------------------------------------------------------------
//	FISaveContext
FISaveContext::FISaveContext(){
	errorStream = &DSTR;
	typeDbs.Push(DBG_NEW UTTypeDescDb);
	handlerDbs.Push(DBG_NEW UTLoadHandlerDb);
}

bool FISaveContext::Open(const char* fn, bool binary){
	ios_base::openmode mode = ios_base::out;
	// �o�C�i���t�H�[�}�b�g�̏ꍇbinary�t���O���K�v
	if(binary)
		mode |= ios_base::binary;
	
	file.open(fn, mode);
	return file.is_open();
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

