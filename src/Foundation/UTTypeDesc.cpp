/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "UTTypeDesc.h"
#include "Scene.h"
#pragma hdrstop
#include <sstream>


namespace Spr{;

//----------------------------------------------------------------------------
//	UTTypeDesc::Field
UTTypeDesc::Field::~Field(){
	if (type){
		UTRef<UTTypeDesc> t = type;
		type = NULL;
	}
}


size_t UTTypeDesc::Field::GetSize(){
	if (varType==VECTOR){
		return type->SizeOfVector();
	}else{
		size_t sz = 0;
		if (isReference){
			sz = sizeof(void*);
		}else{
			sz = type->GetSize();
		}
		if (varType==SINGLE){
			return sz; 
		}else if (varType==ARRAY){
			return sz * length;
		}
	}
	assert(0);
	return 0;
}
void UTTypeDesc::Field::AddEnumConst(std::string name, int val){
	enums.push_back(std::make_pair(name, val));
}
///
void UTTypeDesc::Field::AddEnumConst(std::string name){
	int val = 0;
	if (enums.size()) val = enums.back().second+1;
	enums.push_back(std::make_pair(name, val));
}
const void* UTTypeDesc::Field::GetAddress(const void* base, int pos){
	const void* ptr = (const char*)base + offset;
	if (varType == VECTOR){
		ptr = type->VectorAt(ptr, pos);
	}else if (varType == ARRAY){
		ptr = (const char*)ptr + type->GetSize()*pos;
	}
	return ptr;
}
void* UTTypeDesc::Field::GetAddressEx(void* base, int pos){
	void* ptr = (char*)base + offset;
	if (varType == VECTOR){
		while((int)type->VectorSize(ptr)<=pos) type->VectorPush(ptr);
		ptr = type->VectorAt(ptr, pos);
	}else if (varType == ARRAY){
		ptr = (char*)ptr + type->GetSize()*pos;
	}
	return ptr;
}

void UTTypeDesc::Field::Print(std::ostream& os) const{
	int w = os.width();
	os.width(0);
	os << UTPadding(w) << name.c_str() << "(+" << offset << ") = ";
	if (varType==VECTOR || varType==ARRAY){
		os << "Vector<";
		if (isReference){
			os << "UTRef<" << type->GetTypeName().c_str() << ">";
		}else{
			if (type) os << type->GetTypeName().c_str();
			else os << "(null)";
		}
		os << ">";
		if (length>1) os << " [" << length << "]";
		os << " ";
		if (type) os << (unsigned int)type->GetSize() * length;
		else os << "?";
	}else if (isReference){
		os << "UTRef<" << (type ? type->GetTypeName().c_str() : "(null)") << ">";
		if (length>1) os << " [" << length << "]";
		os << " " << sizeof(UTRef<UTTypeDesc::Field>) * length;
	}else{
		os.width(w);
		if (type) type->Print(os);
		else os << "(null)";
		if (length>1) os << " [" << length << "]";
		os.width(0);
	}
	os.width(w);
}

//----------------------------------------------------------------------------
//	UTTypeDesc::Composit
int UTTypeDesc::Composit::Size(UTString id){
	int rv = 0;
	for(iterator it = begin(); it != end(); ++it){
		if (id.length()==0 || id.compare(it->name)){
			rv += (int)it->GetSize();
		}
	}
	return rv;
}
void UTTypeDesc::Composit::Print(std::ostream& os) const{
	for(const_iterator it = begin(); it != end(); ++it){
		it->Print(os);
	}
	if (size()){
		if (!back().type || !back().type->GetComposit().size()) DSTR << std::endl;
	}
}
void UTTypeDesc::Composit::Link(UTTypeDescDb* db) {
	for(iterator it = begin(); it != end(); ++it){
		it->type = db->Find(it->typeName);
	}
}

//----------------------------------------------------------------------------
//	UTTypeDesc
UTTypeDesc::Field* UTTypeDesc::AddField(std::string pre, std::string tn, 
	std::string n, std::string suf){
	composit.push_back(Field());
	if (pre.compare("vector") == 0){
		composit.back().varType = UTTypeDesc::Field::VECTOR;
		composit.back().length = UTTypeDesc::BIGVALUE;
	}
	if (pre.compare("UTRef") == 0) composit.back().isReference = true;
	if (pre.compare("pointer") == 0) composit.back().isReference = true;
	if (suf.size()){
		std::istringstream is(suf);
		is >> composit.back().length;
		if (!is.good()){
			composit.back().lengthFieldName = suf;
		}
		if (composit.back().varType == UTTypeDesc::Field::SINGLE){
			composit.back().varType = UTTypeDesc::Field::ARRAY;
		}
	}

	composit.back().typeName = tn;
	composit.back().name = n;

	return &composit.back();
};

UTTypeDesc::Field* UTTypeDesc::AddBase(std::string tn){
	Composit::iterator it;
	if (composit.size()){
		for(it = composit.begin(); it!=composit.end(); ++it){
			if (it->name.size()) break;
		}
		--it;
		it = composit.insert(it, Field());
	}else{
		composit.push_back(Field());
		it = composit.begin();
	}
	it->typeName = tn;
	return &*it;
}
void UTTypeDesc::Link(UTTypeDescDb* db) {
	composit.Link(db);
}
void UTTypeDesc::Print(std::ostream& os) const{
	int w = os.width();
	os.width(0);
	os << typeName << " " << (int)size;
	if (composit.size()){
		os << "{" << std::endl;
		os.width(w+2);
		composit.Print(os);
		os.width(0);
		os << UTPadding(w) << "}" << std::endl;
	}
	os.width(w);
}

//----------------------------------------------------------------------------
//	UTTypeDescDb
UTTypeDescDb::~UTTypeDescDb(){
	db.clear();
}
UTTypeDesc* UTTypeDescDb::Find(UTString tn){
	UTRef<UTTypeDesc> key = new UTTypeDesc;
	if (prefix.length() && tn.compare(0, prefix.length(), prefix)==0){
		tn = tn.substr(prefix.length());
	}
	key->typeName = tn;
	Db::iterator it = db.find(key);
	if (it != db.end()) return *it;
	return NULL;
}
void UTTypeDescDb::SetPrefix(UTString p){
	prefix = p;
	typedef std::vector< UTRef< UTTypeDesc > > Descs;
	Descs descs;
	for(Db::iterator it = db.begin(); it != db.end(); ++it){
		descs.push_back(*it);
	}
	db.clear();
	for(Descs::iterator it = descs.begin(); it != descs.end(); ++it){
		if (prefix.length() && (*it)->typeName.compare(0, prefix.length(), prefix) == 0){
			(*it)->typeName = (*it)->typeName.substr(prefix.length());
		}
	}
	db.insert(descs.begin(), descs.end());
}
void UTTypeDescDb::Link() {
	for(Db::iterator it=db.begin(); it!=db.end(); ++it){
		(*it)->Link(this);
	}
}
void UTTypeDescDb::Print(std::ostream& os) const{
	int w = os.width();
	os.width(0);
	DSTR << UTPadding(w) << "Group: " << group << "   Prefix: " << prefix << std::endl;
	os.width(w);
	for(Db::const_iterator it = db.begin(); it != db.end(); ++it){
		(*it)->Print(os);
		os << std::endl;
	}
}

//---------------------------------------------------------------------------
//	UTTypeDescDbPool
UTRef<UTTypeDescDbPool> UTTypeDescDbPool::pool;
UTTypeDescDbPool* SPR_CDECL UTTypeDescDbPool::GetPool(){
	if (!pool) pool = new UTTypeDescDbPool();
	return pool;
}
UTTypeDescDb* SPR_CDECL UTTypeDescDbPool::Get(std::string gp){
	UTRef<UTTypeDescDb> key = DBG_NEW UTTypeDescDb(gp);
	std::pair<UTTypeDescDbPool::iterator, bool> r = GetPool()->insert(key);
	return *r.first;
}
void SPR_CDECL UTTypeDescDbPool::Print(std::ostream& os){
	for(const_iterator it = GetPool()->begin(); it != GetPool()->end(); ++it){
		(*it)->Print(os);
		os << std::endl;
	}
}

//---------------------------------------------------------------------------
//	UTTypeDescFieldIt

UTTypeDescFieldIt::UTTypeDescFieldIt(UTTypeDesc* d){
	type = d;
	if (type){
		field = type->GetComposit().end();
		if (type->IsPrimitive()){
			//	�g�ݗ��Č^�łȂ��ꍇ
			DSTR << "Not a composit type" << std::endl;
			arrayPos = -1;
			arrayLength = 1;
			fieldType=GetTypeId(type);
		}else{
			arrayPos = -1;
			arrayLength = 0;
			fieldType=F_NONE;
		}
	}
}
bool UTTypeDescFieldIt::NextField(void* base){
	if (!type || !type->GetComposit().size()) return false;
	//	���̃t�B�[���h�֐i��
	if (field == type->GetComposit().end()){
		field = type->GetComposit().begin();
	}else{
		++field;
		if (field == type->GetComposit().end()){
			fieldType = F_NONE;
			return false;
		}
	}
	SetFieldInfo(base);
	return true;
}
	
bool UTTypeDescFieldIt::FindField(UTString name, void* base){
	if (!type || !type->GetComposit().size()) return false;
	//	���̃t�B�[���h�֐i��
	for(field = type->GetComposit().begin(); field != type->GetComposit().end(); ++field){
		if (field->name.compare(name) == 0){
			break;
		}
	}
	if (field == type->GetComposit().end()){
		fieldType = F_NONE;
		return false;
	}
	SetFieldInfo(base);
	return true;
}

void UTTypeDescFieldIt::SetFieldInfo(void* base){
	//	�t�B�[���h�̔z��v�f����ݒ�
	if (field->varType==UTTypeDesc::Field::SINGLE){
		arrayLength = 1;
	}else if(field->varType==UTTypeDesc::Field::VECTOR || field->varType==UTTypeDesc::Field::ARRAY){
		arrayLength = field->length;
		if (field->lengthFieldName.length()){
			for(UTTypeDesc::Composit::iterator it = type->composit.begin(); it != field && it != type->composit.end(); ++it){
				if (it->name.compare(field->lengthFieldName) == 0){
					arrayLength = it->ReadNumber(base, 0);
					break;
				}
			}
		}
	}
	//	�z��J�E���g��������
	arrayPos = -1;
	//	�t�B�[���h�̌^��ݒ�
	if (!field->type){
		DSTR << type->GetTypeName() << " has NULL field type" << std::endl;
		type->Print(DSTR);
		assert(0);
	}
	fieldType = GetTypeId(field->type);
}

UTTypeDescFieldIt::FieldType UTTypeDescFieldIt::GetTypeId(UTTypeDesc* type){
	UTTypeDescFieldIt::FieldType fieldType = F_NONE;
	if(type->IsPrimitive()){
		if(		type->GetTypeName().compare("BYTE")==0
			||	type->GetTypeName().compare("WORD")==0
			||	type->GetTypeName().compare("DWORD")==0
			||	type->GetTypeName().compare("char")==0
			||	type->GetTypeName().compare("short")==0
			||	type->GetTypeName().compare("int")==0
			||	type->GetTypeName().compare("unsigned")==0
			||	type->GetTypeName().compare("size_t")==0
			||	type->GetTypeName().compare("enum")==0){
			fieldType = F_INT;
		}else if (type->GetTypeName().compare("bool")==0
			||	type->GetTypeName().compare("BOOL")==0){
			fieldType = F_BOOL;
		}else if (type->GetTypeName().compare("float")==0
			||	type->GetTypeName().compare("double")==0
			||	type->GetTypeName().compare("FLOAT")==0
			||	type->GetTypeName().compare("DOUBLE")==0){
			fieldType = F_REAL;
		}else if (type->GetTypeName().compare("string")==0
			||  type->GetTypeName().compare("STRING")==0){
			fieldType = F_STR;
		}
	}else{
		fieldType = F_BLOCK;
	}
	return fieldType;
}

}
