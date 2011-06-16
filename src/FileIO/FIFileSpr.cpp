/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <Foundation/UTLoadHandler.h>
#include <FileIO/FIFileSpr.h>
#include <FileIO/FILoadContext.h>
#include <FileIO/FISaveContext.h>
#include <fstream>
#include <sstream>
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

namespace Spr{;

#ifdef _DEBUG
#define TRACE_PARSE
#endif

#ifdef TRACE_PARSE
# define PDEBUG(x)	x
#else 
# define PDEBUG(x)
#endif

namespace FileSpr{
static UTStack<FILoadContext*> fileContexts;
static UTStack<FIFileSpr*> fileSprs;
static FILoadContext* fileContext;
static FIFileSpr* fileSpr;

struct MSG{
	const char* msg;
	MSG(const char* m):msg(m){
	}
	void operator() (const char* b, const char* e) const {
		DSTR << msg << std::endl;
	}
};


static UTString idTypeId;
enum IdType{ IT_NODE, IT_FIELD, IT_UNKNOWN} idType;
///	Id���A�^���Ȃ̂��A�\���̂̃����o���Ȃ̂�
static void SetIdType(const char* b, const char* e){
	idTypeId.assign(b,e);
	PDEBUG( DSTR << "SetIdType(" << idTypeId << ":" ; )
	if (fileContext->fieldIts.size() && fileContext->fieldIts.Top().type && fileContext->fieldIts.Top().HaveField(idTypeId)){
		idType = IT_FIELD;
		PDEBUG( DSTR << "field)" << std::endl ; )
	}else if (fileContext->typeDbs.Top()->Find(idTypeId) || fileContext->typeDbs.Top()->Find(idTypeId + "Desc")){
		idType = IT_NODE;
		PDEBUG( DSTR << "node)" << std::endl ; )
	}else{
		idType = IT_UNKNOWN;
		PDEBUG( DSTR << "unknown)" << std::endl ; )
	}
}
static bool IsIdNode(){
	return idType == IT_NODE;
}
static bool IsIdField(){
	return idType == IT_FIELD;
}


///	�m�[�h�̎n�܂�D�^�������ăZ�b�g
static void NodeStart(const char* b, const char* e){
	PDEBUG( DSTR << "NodeStart " << idTypeId << std::endl );
	fileContext->NodeStart(idTypeId);
}
///	�m�[�h�̖��O�̐ݒ�
static void NameSet(const char* b, const char* e){
	UTString n(b,e);
	fileContext->datas.back()->SetName(UTString(b,e));
}
///	�m�[�h�̎n�܂�D�^�������ăZ�b�g
static void NodeStartFromId(const char c){
	PDEBUG( DSTR << "NodeStartFromId " << idTypeId << std::endl );
	assert(idType == IT_FIELD);
	fileContext->fieldIts.PushType(fileContext->fieldIts.Top().type);
	fileContext->FindField(idTypeId);
	UTString typeName = fileContext->fieldIts.Top().field->typeName;
	fileContext->fieldIts.Pop();
	fileContext->NodeStart(typeName);
	fileContext->datas.back()->SetName(idTypeId);
}

///	�m�[�h�̏I���
static void NodeEnd(const char* b, const char* e){
	PDEBUG(DSTR << "NodeEnd " << fileContext->fieldIts.back().type->GetTypeName() << std::endl);
	fileContext->NodeEnd();
}

///	�u���b�N�^�̃X�L�b�v
static void NodeSkip(const char* b, const char* e){
	fileContext->datas.Top()->str = UTString(b,e);
}

static size_t letStartDepth;
static void LetStart(const char* b, const char* e){
	fileContext->FindField(idTypeId);
	letStartDepth = fileContext->fieldIts.size()+1;	//	���K�w��Next���Ă͂����Ȃ��̂ŁA+1�B
}
static void LetEnd(const char* b, const char* e){
	letStartDepth = 0;
	while (fileContext->fieldIts.size() > fileContext->nodeStartDepthes.Top()) 
		fileContext->CompositEnd();
}
/**	�u���b�N�ǂݏo�����C�t�B�[���h��ǂޑO�ɌĂ΂��D
	TypeDesc�����Ď��ɓǂݏo���ׂ��t�B�[���h���Z�b�g����D
	�ǂݏo���ׂ��t�B�[���h������� true ��Ԃ��D	*/
static bool NextField(){
	if (letStartDepth > fileContext->fieldIts.size()){
		PDEBUG( DSTR << "NextField:failed. letStartDepth=" << letStartDepth << "." << std::endl);
		return false;
	}

	char* base = (char*)fileContext->datas.Top()->data;
	bool rv = fileContext->fieldIts.NextField(base);
	PDEBUG(
		if (rv){
			DSTR << "NextField:";
			fileContext->fieldIts.Top().field->Print(DSTR);
			DSTR << std::endl;
		}else {
			DSTR << "NextField failed. end of composition." << std::endl;
		}
	)
	return rv;
}
static bool IsFieldValid(){
	return fileContext->fieldIts.IsValid();
}
///	�z��̃J�E���g�D�܂��ǂݏo���ׂ��f�[�^���c���Ă���� true ��Ԃ��D
static bool ArrayCount(){
	return fileContext->fieldIts.IncArrayPos();
}
static void VectorStart(const char c){
	fileContext->fieldIts.Top().arrayPos = -2;
}
static bool InVector(){
	return fileContext->fieldIts.Top().arrayPos == -2;
}
static bool IsBlock(){
	return !fileContext->fieldIts.Top().field->type->IsPrimitive();
}
static void BlockStart(const char* b, const char* e){
	fileContext->CompositStart();
	NextField();
}
static void BlockEnd(const char* b, const char* e){
	fileContext->CompositEnd();
}

static bool IsFieldInt(){ return fileContext->fieldIts.back().fieldType==UTTypeDescFieldIt::F_INT; }
static bool IsFieldReal(){ return fileContext->fieldIts.back().fieldType==UTTypeDescFieldIt::F_REAL; }
static bool IsFieldStr(){ return fileContext->fieldIts.back().fieldType==UTTypeDescFieldIt::F_STR; }
static bool IsFieldBlock(){ return fileContext->fieldIts.back().fieldType==UTTypeDescFieldIt::F_BLOCK; }
static bool IsFieldBool(){ return fileContext->fieldIts.back().fieldType==UTTypeDescFieldIt::F_BOOL; }

static double numValue;
static std::string strValue;
static bool boolValue;
static void NumSet(double v){
	numValue = v;
}
static void EnumSet(const char* b, const char* e){
	UTTypeDescFieldIt& curField = fileContext->fieldIts.back();
	for(unsigned i=0; i< curField.field->enums.size(); ++i){
		if (curField.field->enums[i].first.compare(UTString(b, e)) == 0){
			numValue = curField.field->enums[i].second;
			return;
		}
	}
	UTString str = "enum (";
	for(unsigned i=0; i< curField.field->enums.size(); ++i){
		str += curField.field->enums[i].first;
		str += " ";
	}
	str += ") expected.";
	fileContext->ErrorMessage(NULL, b, str.c_str());
	numValue = -1;
}


static void BoolSet(const char* b, const char* e){
	UTString v(b,e);
	boolValue = (v.compare("true")==0) || (v.compare("TRUE")==0) || (v.compare("1")==0);
}
static void StrSet(const char* b, const char* e){
	strValue.assign(b+1,e-1);
}

///	ObjectDesc�ɓǂݏo�����l����������
static void SetVal(const char* b, const char* e){
	UTTypeDescFieldIt& curField = fileContext->fieldIts.back();
	//	debug �o��
#ifdef TRACE_PARSE
	if (curField.fieldType!=UTTypeDescFieldIt::F_NONE){
		if (curField.fieldType==UTTypeDescFieldIt::F_BLOCK){
			DSTR << " => (" << curField.field->typeName << ") " << curField.field->name << std::endl;
		}else{
			if (curField.arrayPos==0){
				DSTR << "(" << curField.field->typeName << ") " << curField.field->name << " = " ;
			}
		}
		if (curField.fieldType == UTTypeDescFieldIt::F_REAL || curField.fieldType == UTTypeDescFieldIt::F_INT){
			DSTR << " " << numValue;
		}else if (curField.fieldType == UTTypeDescFieldIt::F_STR){
			DSTR << " " << strValue;
		}
	}
	DSTR << std::endl;
#endif
	//	�����܂�
	
	if (fileContext->fieldIts.IsBool()){
		fileContext->WriteBool(boolValue);
	}else if (fileContext->fieldIts.IsNumber()){
		fileContext->WriteNumber(numValue);
	}else if (fileContext->fieldIts.IsString()){
		fileContext->WriteString(strValue);
	}
}
static void StopArray(const char c){
	UTTypeDescFieldIt& curField = fileContext->fieldIts.back();
	curField.arrayPos=UTTypeDesc::BIGVALUE;
}

///	�Q�Ƃ�ǉ�����D
static void RefSet(const char* b, const char* e){
	std::string ref(b,e);
	fileContext->AddDataLink(ref, b);
}

static UTTypeDesc* tdesc;
//	XFile��template�̓ǂݏo���̊֐�
static void TempStart(const char* b, const char* e){
	tdesc = DBG_NEW UTTypeDesc(std::string(b,e));
}
static void DefType(const char* b, const char* e){
	tdesc->GetComposit().push_back(UTTypeDesc::Field());
	tdesc->GetComposit().back().typeName.assign(b, e);
}
static void DefId(const char* b, const char* e){
	tdesc->GetComposit().back().name.assign(b, e);
}
static void ArrayId(const char* b, const char* e){
	tdesc->GetComposit().back().varType = UTTypeDesc::Field::VECTOR;
	tdesc->GetComposit().back().lengthFieldName.assign(b, e);
}
static void ArrayNum(int n){
	tdesc->GetComposit().back().varType = UTTypeDesc::Field::ARRAY;
	tdesc->GetComposit().back().length = n;
}
static void TempEnd(char c){
	tdesc->Link(fileContext->typeDbs.Top());
	PDEBUG(DSTR << "load template:" << std::endl);
	PDEBUG(tdesc->Print(DSTR));
	fileContext->typeDbs.Top()->RegisterDesc(tdesc);
}

///	XXX expected. �̃G���[���b�Z�[�W���o���p�[�T
class ExpectParser {
	std::string msg; 
public:
    ExpectParser(const char *m) : msg(m) {}
	static std::ostream* errorStr;

	typedef boost::spirit::classic::nil_t result_t;
    
    template <typename ScannerT>
	int operator()(ScannerT const& scan, result_t& /*result*/) const {
		if (!scan.at_end()){
			std::string str = msg + std::string(" expected");
			fileContext->ErrorMessage(NULL, scan.first, str.c_str());
		}
		return -1;
    }
}; 
typedef boost::spirit::classic::functor_parser<ExpectParser> ExpP;
}	//	namespace FileSpr
using namespace FileSpr;


FIFileSpr::FIFileSpr(const FIFileSprDesc& desc):depthFromField(-1){
	Init();
}

void FIFileSpr::Init(){
	using namespace std;
	using namespace boost::spirit::classic;
	//	�p�[�T�̒�`
	//	�{���p�p�[�T
	start		=	*node;
	node		=	id[&SetIdType] >> if_p(&IsIdNode)[ 
						eps_p[&NodeStart] >> !id[&NameSet] >> (ch_p('{') >> block | ExpP("'{'"))
					].else_p[ nothing_p ] | ExpP("node type");
	block		=	*data >> (ch_p('}') | ExpP("'}'"))[&NodeEnd];
	data		=	ch_p('*') >> id[&RefSet] |
					id[&SetIdType] >> 
					if_p(&IsIdNode)[	//	�^���̏ꍇ�͎q�m�[�h
						eps_p[&NodeStart] >> !id[&NameSet] >> (ch_p('{') >> block | ExpP("'{'")) 
					].else_p[
						if_p(&IsIdField)[	//	�t�B�[���h���Ȃ���
							ch_p('=') >> (
								ch_p('{')[&NodeStartFromId] >> block	//	�u���b�N�`���ɂ��邩
								| eps_p[&LetStart] >> right [&LetEnd]	//	�l����ׂď�����
							)
						].else_p [ ExpP("field or type name") ]
					];
	right		=	do_p[
						!ch_p('[')[&VectorStart] >>
						while_p(&ArrayCount)[
							if_p(&IsBlock)[
								eps_p[&BlockStart] >> right >> eps_p[&BlockEnd]
							].else_p[
								value[&SetVal]
							]
							>> if_p(&InVector)[ !ch_p(']')[&StopArray] ]
						]
					].while_p(&NextField);
	value		=	if_p(&IsFieldBool)[ boolVal | ExpP("bool value") ].else_p[
					if_p(&IsFieldInt)[ iNum | ExpP("int value") ].else_p[
					if_p(&IsFieldReal)[ rNum | ExpP("numeric value") ].else_p[
					if_p(&IsFieldStr)[ str | ExpP("string") ].else_p[ nothing_p ]]]];

	id			=	lexeme_d[ (alpha_p|'_') >> *(alnum_p|'_'|'-') ];
	boolVal		=	(str_p("true") | "TRUE" | "false" | "FALSE" | "1" | "0")[&BoolSet];
	iNum		=	id[&EnumSet] | int_p[&NumSet];
	rNum		=	real_p[&NumSet];
	str			=	lexeme_d[ 
						ch_p('"') >> *( (ch_p('\\')>>anychar_p) | 
						~ch_p('"') ) >> ch_p('"') 
					][&StrSet];

	//	�X�L�b�v�p�[�T(�X�y�[�X�ƃR�����g��ǂݏo���p�[�T)�̒�`
	cmt		=	space_p
				|	"/*" >> *(~ch_p('*') | '*'>>~ch_p('/')) >> !ch_p('*') >> '/'
				|	"//" >> *~ch_p('\n') >> '\n'
				|	"#" >> *~ch_p('\n') >> '\n';
}



//------------------------------------------------------------------------------

void FIFileSpr::PushLoaderContext(FILoadContext* fc){
	fc->RegisterGroupToDb("Foundation Physics Graphics FileIO Framework Creature OldSpringhead");
	fc->typeDbs.Top()->RegisterAlias("Vec3f", "Vector");
	fc->typeDbs.Top()->RegisterAlias("Vec2f", "Coords2d");
	fc->typeDbs.Top()->RegisterAlias("Affinef", "Matrix3x3");
	fc->typeDbs.Top()->RegisterAlias("Affined", "Matrix4x4");

	fileContexts.Push(fc);
	fileSprs.Push(this);
	fileContext = fileContexts.Top();
	fileSpr = fileSprs.Top();
}
void FIFileSpr::PopLoaderContext(){
	fileContexts.Pop();
	fileSprs.Pop();
	if (fileContexts.size()){
		fileContext = fileContexts.Top();
		fileSpr = fileSprs.Top();
	}else{
		fileContext = NULL;
		fileSpr = NULL;
	}
}
void FIFileSpr::LoadImp(FILoadContext* fc){
	using namespace std;
	using namespace boost::spirit::classic;
	PushLoaderContext(fc);
	parse_info<const char*> info = parse(
		fileContext->fileMaps.Top()->start, 
		fileContext->fileMaps.Top()->end, start, cmt);
	PopLoaderContext();
}

//----------------------------------------------------
//	�Z�[�u���̃n���h��
#define INDENT(x)	UTPadding((sc->objects.size()+x)*2)
//<< (sc->objects.size()+x)
void FIFileSpr::OnSaveFileStart(FISaveContext* sc){
	sc->RegisterGroupToDb("Foundation Physics Graphics FileIO Framework Creature OldSpringhead");
	sc->typeDbs.Top()->RegisterAlias("Vec3f", "Vector");
	sc->typeDbs.Top()->RegisterAlias("Vec2f", "Coords2d");
	sc->typeDbs.Top()->RegisterAlias("Affinef", "Matrix3x3");
	sc->typeDbs.Top()->RegisterAlias("Affined", "Matrix4x4");
}
static bool cont;
static void* defaultData;
void FIFileSpr::OnSaveNodeStart(FISaveContext* sc){
	sc->Stream() << INDENT(-1) << sc->GetNodeTypeName();
	UTString name = sc->GetNodeName();
	if (name.length()) sc->Stream() << " " << name;
	sc->Stream() << "{" << std::endl;
	cont = false;
}
void FIFileSpr::OnSaveDataStart(FISaveContext* sc){
	assert(defaultData==NULL);
	defaultData = sc->fieldIts.Top().type->Create();
}
void FIFileSpr::OnSaveDataEnd(FISaveContext* sc){
	if (defaultData) sc->fieldIts.Top().type->Delete(defaultData);
	defaultData = NULL;
}
void FIFileSpr::OnSaveNodeEnd(FISaveContext* sc){
	sc->Stream() << INDENT(-1) << "}" << std::endl;
}

bool FIFileSpr::OnSaveFieldStart(FISaveContext* sc, int nElements){
	UTTypeDesc::Field* field = &*(sc->fieldIts.back().field);
	if (depthFromField==-1 && sc->fieldIts.Top().field->name.length()){
		//	�����l�ƍ������邩�m�F
		bool bNoChange = true;
		void* data = sc->datas.Top()->data;
		int dataLen=-1, defaultDataLen=-1;
		if (field->varType == UTTypeDesc::Field::VECTOR){
			dataLen = field->VectorSize(data);
			defaultDataLen = field->VectorSize(defaultData);
		}else if(field->varType == UTTypeDesc::Field::ARRAY){
			dataLen = defaultDataLen = field->length;
		}
		if (dataLen>=0){
			if (dataLen != defaultDataLen) bNoChange = false;
			for(int i=0; bNoChange && i<dataLen; ++i){
				if (memcmp(field->GetAddress(data, i), field->GetAddress(defaultData, i), field->type->GetSize())) bNoChange = false;
			}
		}else{
			if (memcmp(field->GetAddress(data, 0), field->GetAddress(defaultData, 0), field->type->GetSize())) bNoChange = false;
		}
		if (bNoChange) return false;

		sc->Stream() << INDENT(0);
		sc->Stream() << sc->fieldIts.Top().field->name << " = ";
		depthFromField=0;			
	}else if(depthFromField >= 0){
		depthFromField ++;
	}
	if (field->varType == UTTypeDesc::Field::VECTOR){
		sc->Stream() << "[";
	}
	return true;
}
void FIFileSpr::OnSaveFieldEnd(FISaveContext* sc, int nElements){
	UTTypeDesc::Field* field = &*(sc->fieldIts.back().field);
	if (field->varType == UTTypeDesc::Field::VECTOR){
		sc->Stream() << "]";
	}
	if (depthFromField == 0){
		sc->Stream() << std::endl;
	}
	if (depthFromField >= 0) depthFromField--;
}
void FIFileSpr::OnSaveElementEnd(FISaveContext* sc, int nElements, bool last){
	if (depthFromField > 0 || (!last && depthFromField==0) ) sc->Stream() << " ";
}
void FIFileSpr::OnSaveBool(FISaveContext* sc, bool val){
	sc->Stream() << (val ? "TRUE" : "FALSE");
}
void FIFileSpr::OnSaveInt(FISaveContext* sc, int val){
	sc->Stream() << val;
}
///	real�l�̕ۑ�
void FIFileSpr::OnSaveReal(FISaveContext* sc, double val){
	sc->Stream() << val;
}
///	string�l�̕ۑ�
void FIFileSpr::OnSaveString(FISaveContext* sc, UTString val){
	sc->Stream() << '"' << val << '"';
}
void FIFileSpr::OnSaveRef(FISaveContext* sc){
	NamedObjectIf* n = DCAST(NamedObjectIf, sc->objects.Top());
	sc->Stream() << INDENT(-1) << "*" << n->GetName() << std::endl;
}


};
