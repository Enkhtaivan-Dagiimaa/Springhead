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
FILoadContext::LinkTask::LinkTask(const ObjectIfs& objs, const char* p, ObjectIf* o, std::string r):pos(p), object(o), ref(r){
	for(int i=objs.size()-1; i>=0; --i){
		NameManagerIf* nm = DCAST(NameManagerIf, objs[i]);
		if (nm){
			nameManagers.push_back(nm);
		}
	}
}
void FILoadContext::LinkTask::Execute(FILoadContext* ctx){
	ObjectIf* refObj = NULL;
	for(unsigned i=0; i<nameManagers.size(); ++i){
		nameManagers[i]->FindObject(refObj, ref);
		if (refObj) break;
	}
	if (refObj){
		if (!object->AddChildObject(refObj)){
			std::string err("Can not link referenced object '");
			err.append(ref);
			err.append("' to '");
			err.append(object->GetIfInfo()->ClassName());
			err.append("'.");
			ctx->ErrorMessage(pos, err.c_str());
		}
	}else{
		std::string err("Referenced object '");
		err.append(ref);
		err.append("' not found.");
		ctx->ErrorMessage(pos, err.c_str());
	}
}

//---------------------------------------------------------------------------
//	FILoadContext
void FILoadContext::WriteBool(bool v){
	FIFieldIt& curField = fieldIts.back();
	curField.field->WriteBool(datas.Top()->data, v, curField.arrayPos);
}
void FILoadContext::WriteNumber(double v){
	FIFieldIt& curField = fieldIts.back();
	curField.field->WriteNumber(datas.Top()->data, v, curField.arrayPos);
}
void FILoadContext::WriteString(std::string v){
	FIFieldIt& curField = fieldIts.back();
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
	return fileInfo.back().IsGood();
}
void FILoadContext::LoadNode(){
	if (datas.Top()->type->GetIfInfo()){
		//	���[�h�����f�[�^����I�u�W�F�N�g�����D
		ObjectIf* obj = NULL;
		ObjectIf* creator = NULL;
		for(UTStack<ObjectIf*>::reverse_iterator it = objects.rbegin(); it != objects.rend(); ++it){
			if (*it) obj = (*it)->CreateObject(datas.Top()->type->GetIfInfo(), datas.Top()->data);
			if (obj){
				creator = *it;
				break;
			}
		}
		if (!obj) obj = CreateSdk(datas.Top()->type->GetIfInfo(), datas.Top()->data);
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
					ErrorMessage(NULL, err.c_str());
				}
			}
		}else{
			UTString err("Can not create '");
			err.append(datas.Top()->type->GetIfInfo()->ClassName());
			err.append("'. Ancestor objects don't know how to make it.");
			ErrorMessage(NULL, err.c_str());
		}
		//	�e�I�u�W�F�N�g�ɒǉ�
		if (objects.size() && objects.Top()){
			objects.Top()->AddChildObject(obj);
		}
		//	�I�u�W�F�N�g�X�^�b�N�ɐς�
		objects.Push(obj);
		if (obj && objects.size() == 1) rootObjects.push_back(objects.Top());
	}else{
		static FINodeHandler key;
		key.AddRef();
		key.type = datas.Top()->type->GetTypeName();
		FINodeHandlers::iterator it = handlers->lower_bound(&key);
		FINodeHandlers::iterator end = handlers->upper_bound(&key);
		if (end != handlers->end()) ++end;
		for(; it != end; ++it){
			(*it)->Load(this);
		}
		key.DelRef();
		//	Create�ȊO�̎d��������D
		//	�Փ˔���̖����y�A�̐ݒ��d�͂̐ݒ�ȂǁD
	}
}
void FILoadContext::EnterBlock(){
	char* base = (char*)datas.Top()->data;
	void* ptr = fieldIts.back().field->GetAddressEx(base, fieldIts.ArrayPos());
	datas.Push(DBG_NEW FINodeData(NULL, ptr));
	fieldIts.push_back(FIFieldIt(fieldIts.back().field->type));
}
void FILoadContext::LeaveBlock(){
	fieldIts.Pop();
	datas.Pop();
}
void FILoadContext::EndNode(){
	if (datas.Top()->type && datas.Top()->type->GetIfInfo()){
		objects.Pop();
	}else{
		//	Create�ȊO�̏I�������D
	}
}
void FILoadContext::AddLink(std::string ref, const char* pos){
	links.push_back(DBG_NEW LinkTask(objects, pos, objects.back(), ref));
}
void FILoadContext::Link(){
	links.Execute(this);
}
void FILoadContext::PostTask(){
	postTasks.Execute(this);
}

void FILoadContext::ErrorMessage(const char* pos, const char* msg){
	std::string m("error: ");
	m.append(msg);
	Message(pos, m.c_str());
}
void FILoadContext::Message(const char* pos, const char* msg){
	const char* ptr = fileInfo.back().start;
	int lines=0;
	int returns=0;
	const char* line=ptr;
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
		for(;ptr < fileInfo.back().end; ++ptr){
			if (*ptr == '\n' || *ptr == '\r'){
				break;
			}
		}
		lines = std::max(lines, returns);
	}
	std::ostream& os = *errorStream;
	os << fileInfo.back().name << "(" << lines+1 << ") : ";
	os << msg << std::endl;
	os << std::string(line, ptr) << std::endl;
}

};
