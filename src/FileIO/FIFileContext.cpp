#include "FIFileContext.h"
#include "FITypeDesc.h"
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
//	FIFileContext::Data
FIFileContext::Data::Data(FITypeDesc* t, void* d):type(t), data(d){
	if (!data && type){
		data = type->Create();
		haveData = true;
	}else{
		haveData = false;
	}
}
FIFileContext::Data::~Data(){
	if (haveData) type->Delete(data);
}
//---------------------------------------------------------------------------
//	FIFileContext::FileInfo
// �t�@�C�� �}�b�s���O
//   �����̃t�@�C���̃A�N�Z�X���x������s�����߂ɁA���ۂ̃t�@�C������������Ƀ}�b�s���O����
bool FIFileContext::FileInfo::Map(std::string fn){
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
void FIFileContext::FileInfo::Unmap(){
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
FIFileContext::FileInfo::~FileInfo(){
	if (start) Unmap();
}
bool FIFileContext::FileInfo::IsGood(){
	return start && end && (end != (char*)-1);
}


//---------------------------------------------------------------------------
//	FIFileContext::FieldIt

FIFileContext::FieldIt::FieldIt(FITypeDesc* d){
	type = d;
	field = d->GetComposit().end();
	arrayPos = -1;
	arrayLength = 0;
	nextField=F_NONE;
}
bool FIFileContext::FieldIt::NextField(){
	if (!type->IsComposit()) return false;
	//	���̃t�B�[���h�֐i��
	if (field==type->GetComposit().end()){
		field=type->GetComposit().begin();
	}else{
		++field;
		if (field == type->GetComposit().end()){
			nextField = F_NONE;
			return false;
		}
	}
	//	�t�B�[���h�̔z��v�f����ݒ�
	if (field->varType==FITypeDesc::Field::SINGLE){
		arrayLength = 1;
	}else if(field->varType==FITypeDesc::Field::VECTOR){
		arrayLength = field->length;
	}else if(field->varType==FITypeDesc::Field::ARRAY){
		arrayLength = field->length;
	}
	//	�z��J�E���g��������
	arrayPos = -1;
	//	�t�B�[���h�̌^��ݒ�
	if (	field->type->GetTypeName().compare("BYTE")==0
		||	field->type->GetTypeName().compare("WORD")==0
		||	field->type->GetTypeName().compare("DWORD")==0
		||	field->type->GetTypeName().compare("char")==0
		||	field->type->GetTypeName().compare("short")==0
		||	field->type->GetTypeName().compare("int")==0
		||	field->type->GetTypeName().compare("enum")==0){
		nextField = F_INT;
	}else if (field->type->GetTypeName().compare("float")==0
		||	field->type->GetTypeName().compare("double")==0
		||	field->type->GetTypeName().compare("FLOAT")==0
		||	field->type->GetTypeName().compare("DOUBLE")==0){
		nextField = F_REAL;
	}else if (field->type->GetTypeName().compare("string")==0
		||  field->type->GetTypeName().compare("STRING")==0){
		nextField = F_STR;
	}else if (field->type->IsComposit()){
		nextField = F_BLOCK;
	}
	return true;
}
//---------------------------------------------------------------------------
//	FIFileContext::Link
FIFileContext::Link::Link(const IfStack& objs, const char* p, ObjectIf* o, std::string r):pos(p), object(o), ref(r){
	for(unsigned i=0; i<objs.size(); ++i){
		NameManagerIf* nm = ICAST(NameManagerIf, objs[i]);
		if (nm){
			nameManagers.push_back(nm);
		}
	}
}
//---------------------------------------------------------------------------
//	FIFileContext
void FIFileContext::WriteNumber(double v){
	FIFileContext::FieldIt& curField = fieldIts.back();
	curField.field->WriteNumber(datas.Top()->data, v, curField.arrayPos);
}
void FIFileContext::WriteString(std::string v){
	FIFileContext::FieldIt& curField = fieldIts.back();
	curField.field->WriteString(datas.Top()->data, v.c_str(), curField.arrayPos);
}
void FIFileContext::PushType(FITypeDesc* type){
	//	���[�h���ׂ�type�Ƃ��ăZ�b�g
	fieldIts.PushType(type);
	//	�ǂݏo�����f�[�^���\���̗̂p��
	datas.Push(new Data(type));
}
void FIFileContext::PopType(){
	datas.Pop();
	fieldIts.Pop();
}
bool FIFileContext::IsGood(){
	if (!fileInfo.size()) return false;
	return fileInfo.back().IsGood();
}
void FIFileContext::LoadNode(){
	if (datas.Top()->type->GetIfInfo()){
		//	���[�h�����f�[�^����I�u�W�F�N�g�����D
		objects.Push(Create(datas.Top()->type->GetIfInfo(), datas.Top()->data));
	}else{
		//	Create�ȊO�̎d��������D
		//	�Փ˔���̖����y�A�̐ݒ��d�͂̐ݒ�ȂǁD
	}
}
void FIFileContext::EnterBlock(){
	char* base = (char*)datas.Top()->data;
	void* ptr = fieldIts.back().field->GetAddressEx(base, fieldIts.ArrayPos());
	datas.Push(new Data(NULL, ptr));
	fieldIts.push_back(FieldIt(fieldIts.back().field->type));
}
void FIFileContext::LeaveBlock(){
	fieldIts.Pop();
	datas.Pop();
}
void FIFileContext::EndNode(){
	if (datas.Top()->type->GetIfInfo()){
		objects.Pop();
	}else{
		//	Create�ȊO�̏I�������D
	}
}
ObjectIf* FIFileContext::Create(const IfInfo* ifInfo, const void* data){
	for(UTStack<ObjectIf*>::reverse_iterator it = objects.rbegin(); it != objects.rend(); ++it){
		if (*it){
			ObjectIf* obj = (*it)->CreateObject(ifInfo, data);
			if (obj) return obj;
		}
	}
	return NULL;
}
void FIFileContext::AddLink(std::string ref, const char* pos){
	links.Push(Link(objects, pos, objects.back(), ref));
}
void FIFileContext::DoLink(){
	for(unsigned i=0; i<links.size(); ++i){
		Link& link = links[i];
		ObjectIf* refObj = NULL;
		for(unsigned i=0; i<link.nameManagers.size(); ++i){
			link.nameManagers[i]->FindObject(refObj, link.ref);
			if (refObj) break;
		}
		if (refObj){
			link.object->AddChildObject(refObj);
		}else{
			std::string err("Referenced object '");
			err.append(link.ref);
			err.append("' not found.");
			ErrorMessage(link.pos, err.c_str());
		}
	}
}

void FIFileContext::ErrorMessage(const char* pos, const char* msg){
	std::string m("error: ");
	m.append(msg);
	Message(pos, m.c_str());
}
void FIFileContext::Message(const char* pos, const char* msg){
	const char* ptr = fileInfo.back().start;
	int lines=0;
	int returns=0;
	const char* line=NULL;
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
	std::ostream& os = *errorStream;
	os << fileInfo.back().name << "(" << lines+1 << ") : ";
	os << msg << std::endl;
	os << std::string(line, ptr) << std::endl;
}

};
