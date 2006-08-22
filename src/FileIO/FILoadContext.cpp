/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "FileIO.h"
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

#include "FILoadContext.h"
#include "FINodeHandler.h"
#include <fstream>
#include <sstream>
#ifdef _WIN32
#include <WinBasis/WinBasis.h>
#else
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

namespace Spr{;
//---------------------------------------------------------------------------
//	FINodeData
FINodeData::FINodeData(UTTypeDesc* t, void* d):type(t), data(d){
	if (!data && type){
		data = type->Create();
		haveData = true;
	}else{
		haveData = false;
	}
}
FINodeData::~FINodeData(){
	if (haveData) type->Delete(data);
}
//---------------------------------------------------------------------------
//	FILoadContext::FileInfo
// �t�@�C�� �}�b�s���O
//   �����̃t�@�C���̃A�N�Z�X���x������s�����߂ɁA���ۂ̃t�@�C������������Ƀ}�b�s���O����
bool FILoadContext::FileInfo::Map(std::string fn){
	name = fn;
#ifdef _WIN32
	// �t�@�C���I�[�v��
	hFile = CreateFile(fn.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);	
	if (!hFile){
		DSTR << "Cannot open input file: " << fn.c_str() << std::endl;
		return false;	
	}		
	// �t�@�C���T�C�Y�̎擾
	DWORD len = GetFileSize(hFile,NULL);	
	// �t�@�C���}�b�s���O�I�u�W�F�N�g�쐬
	hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	// �t�@�C��fn��ǂݑ����Ń}�b�v���A���̐擪�A�h���X���擾
	start = (const char*)MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);
	end = start + len;
	return true;	
#else	
	/*
	hFile = fopen(fn.c_str(), "rb");
	if (!hFile) {
		DSTR << "Cannot open input file: " << fn.c_str() << std::endl;
		return false;	
	}		
	fseek(hFile, 0, SEEK_END);
	int const len = ftell(hFile);
	fseek(hFile, 0, SEEK_SET);
	buffer = DBG_NEW char[len];
	fread(buffer, 1, len, hFile);
	start = buffer;
	end = start + len;*/
	fd = open(fn.c_str(), O_RDONLY); 
    if( fd < 0 ) {
		DSTR << "Cannot open input file: " << fn.c_str() << std::endl;
		return false;	
	}		
	if( fstat( fd, &filestat ) == 0 ) {
		// �ǂݍ��ݐ�p�Ńt�@�C���}�b�s���O
        sourceptr = mmap( NULL, filestat.st_size, PROT_READ, MAP_SHARED, fd, 0 );
		if( sourceptr != MAP_FAILED ) {
			start = (char*)sourceptr;
			end = start + filestat.st_size;
			return true;
		} 
	}
	return false;
#endif

}
// �t�@�C�� �A���}�b�s���O
void FILoadContext::FileInfo::Unmap(){
#ifdef _WIN32
	UnmapViewOfFile(start);		// �}�b�v�����t�@�C�����A���}�b�v
	CloseHandle(hFileMap);		// �t�@�C���}�b�s���O�I�u�W�F�N�g���N���[�Y
	CloseHandle(hFile);			// �t�@�C���̃n���h�����N���[�Y
#else
	//fclose(hFile);
	//delete[] buffer;
	munmap(sourceptr, filestat.st_size);
#endif
	start = end = NULL;
}
FILoadContext::FileInfo::~FileInfo(){
	if (start) Unmap();
}
bool FILoadContext::FileInfo::IsGood(){
	return start && end && (end != (char*)-1);
}


//---------------------------------------------------------------------------
//	FILoadContext::Tasks
void FILoadContext::Tasks::Execute(FILoadContext* ctx){
	for(iterator it = begin(); it!=end(); ++it){
		(*it)->Execute(ctx);
	}
	clear();
}

//---------------------------------------------------------------------------
//	FILoadContext::LinkTask
FILoadContext::LinkTask::LinkTask(const ObjectIfs& objs, FILoadContext::FileInfo* fi, const char* p, ObjectIf* o, std::string r):info(fi), pos(p), object(o), ref(r){
	for(int i=objs.size()-1; i>=0; --i){
		NameManagerIf* nm = DCAST(NameManagerIf, objs[i]);
		if (nm){
			nameManagers.push_back(nm);
		}
	}
}
void FILoadContext::LinkTask::Execute(FILoadContext* ctx){
	Spr::ObjectIf* refObj = NULL;
	for(unsigned i=0; i<nameManagers.size(); ++i){
		refObj = nameManagers[i]->FindObject(ref);
		if (refObj) break;
	}
	if (refObj){
		if (!object->AddChildObject(refObj)){
			std::string err("Can not link referenced object '");
			err.append(ref);
			err.append("' to '");
			err.append(object->GetIfInfo()->ClassName());
			err.append("'.");
			ctx->ErrorMessage(info, pos, err.c_str());
		}
	}else{
		std::string err("Referenced object '");
		err.append(ref);
		err.append("' not found.");
		ctx->ErrorMessage(info, pos, err.c_str());
	}
}

//---------------------------------------------------------------------------
//	FILoadContext
void FILoadContext::WriteBool(bool v){
	UTTypeDescFieldIt& curField = fieldIts.back();
	curField.field->WriteBool(datas.Top()->data, v, curField.arrayPos);
}
void FILoadContext::WriteNumber(double v){
	UTTypeDescFieldIt& curField = fieldIts.back();
	curField.field->WriteNumber(datas.Top()->data, v, curField.arrayPos);
}
void FILoadContext::WriteString(std::string v){
	UTTypeDescFieldIt& curField = fieldIts.back();
	curField.field->WriteString(datas.Top()->data, v.c_str(), curField.arrayPos);
}
void FILoadContext::PushType(UTTypeDesc* type){
	//	���[�h���ׂ�type�Ƃ��ăZ�b�g
	fieldIts.PushType(type);
	//	�ǂݏo�����f�[�^���\���̗̂p��
	datas.Push(DBG_NEW FINodeData(type));
}
void FILoadContext::PopType(){
	datas.Pop();
	fieldIts.Pop();
}
bool FILoadContext::IsGood(){
	if (!fileInfo.size()) return false;
	return fileInfo.Top()->IsGood();
}
void FILoadContext::AddLink(std::string ref, const char* pos){
	links.push_back(DBG_NEW LinkTask(objects, fileInfo.Top(), pos, objects.back(), ref));
}
void FILoadContext::Link(){
	links.Execute(this);
	links.clear();
}
void FILoadContext::PostTask(){
	postTasks.Execute(this);
	postTasks.clear();
}

void FILoadContext::ErrorMessage(FileInfo* info, const char* pos, const char* msg){
	std::string m("error: ");
	m.append(msg);
	Message(info, pos, m.c_str());
}
void FILoadContext::Message(FileInfo* info, const char* pos, const char* msg){
	int lines=0;
	int returns=0;
	if (!info) info = fileInfo.Top();
	const char* ptr = info->start;
	const char* line=ptr;

	if (!pos) pos = info->parsingPos;
	if (pos){
		for(;ptr < pos; ++ptr){
			if (*ptr == '\n'){
				++lines;
				line = ptr+1;
			}
			if (*ptr == '\r'){
				++returns;
				line = ptr+1;
			}
		}
		for(;ptr < info->end; ++ptr){
			if (*ptr == '\n' || *ptr == '\r'){
				break;
			}
		}
		lines = std::max(lines, returns);
	}
	std::ostream& os = *errorStream;
	os << info->name << "(" << lines+1 << ") : ";
	os << msg << std::endl;
	os << std::string(line, ptr) << std::endl;
}
void FILoadContext::PushCreateNode(const IfInfo* info, const void* data){
	ObjectIf* obj = NULL;
	for(UTStack<ObjectIf*>::reverse_iterator it = objects.rbegin(); it != objects.rend(); ++it){
		if (*it) obj = (*it)->CreateObject(info, data);
		if (obj) break;
	}
	if (!obj) obj = CreateSdk(info, data);

	//	�I�u�W�F�N�g�ɖ��O��ݒ�
	if (obj){
		NamedObjectIf* n = DCAST(NamedObjectIf, obj);
		if (datas.Top()->name.length()){
			if (n){
				n->SetName(datas.Top()->name.c_str());
			}else{
				UTString err("Can not give name to an object of '");
				err.append(obj->GetIfInfo()->ClassName());
				const IfInfo* i = obj->GetIfInfo();
				const IfInfo* b = NamedObjectIf::GetIfInfoStatic();
				if (i->Inherit(b)){
					DSTR << "i Inherits b.\n";
				}
				err.append("'.");
				ErrorMessage(NULL, NULL, err.c_str());
			}
		}
	}else{
		UTString err("Can not create '");
		err.append(info->ClassName());
		err.append("'. Ancestor objects don't know how to make it.");
		ErrorMessage(NULL, NULL, err.c_str());
	}
	//	�e�I�u�W�F�N�g�ɒǉ�
	if (objects.size() && objects.Top()){
		objects.Top()->AddChildObject(obj);
	}
	//	�I�u�W�F�N�g�X�^�b�N�ɐς�
	objects.Push(obj);
	if (obj && objects.size() == 1) rootObjects.push_back(objects.Top());
}

};
