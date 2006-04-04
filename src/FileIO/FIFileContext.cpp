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
//	FIFileContext::Primitive
FIFileContext::Primitive::Primitive(FITypeDesc* d, void* o):desc(d), obj(o){
	if (!obj && desc) obj = desc->Create();
}
FIFileContext::Primitive::~Primitive(){
	if (desc && obj) desc->Delete(obj);
}
//---------------------------------------------------------------------------
//	FIFileContext::FileInfo
// ファイル マッピング
//   既存のファイルのアクセス速度向上を行うために、実際のファイルをメモリ上にマッピングする
bool FIFileContext::FileInfo::Map(std::string fn){
	name = fn;
#ifdef _WIN32
	// ファイルオープン
	hFile = CreateFile(fn.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);	
	if (!hFile){
		DSTR << "Cannot open input file: " << fn.c_str() << std::endl;
		return false;	
	}		
	// ファイルサイズの取得
	DWORD len = GetFileSize(hFile,NULL);	
	// ファイルマッピングオブジェクト作成
	hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	// ファイルfnを読み属性でマップし、その先頭アドレスを取得
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
		// 読み込み専用でファイルマッピング
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
// ファイル アンマッピング
void FIFileContext::FileInfo::Unmap(){
#ifdef _WIN32
	UnmapViewOfFile(start);		// マップしたファイルをアンマップ
	CloseHandle(hFileMap);		// ファイルマッピングオブジェクトをクローズ
	CloseHandle(hFile);			// ファイルのハンドルをクローズ
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
//	FIFileContext
bool FIFileContext::IsGood(){
	if (!fileInfo.size()) return false;
	return fileInfo.back().IsGood();
}
ObjectIf* FIFileContext::Create(const IfInfo* ifInfo, const void* desc){
	for(IfStack::reverse_iterator it = objects.rbegin(); it != objects.rend(); ++it){
		ObjectIf* obj = (*it)->CreateObject(ifInfo, desc);
		if (obj) return obj;
	}
	return NULL;
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
