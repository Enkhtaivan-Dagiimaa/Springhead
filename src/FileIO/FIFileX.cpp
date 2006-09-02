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

#include "FIFileX.h"
#include <Foundation/UTLoadHandler.h>
#include <fstream>
#include <sstream>
#include <Springhead.h>

namespace Spr{;

IF_OBJECT_IMP(FIFileX, FIFile);


//#define TRACE_PARSE
#ifdef TRACE_PARSE
# define PDEBUG(x)	x
#else 
# define PDEBUG(x)
#endif

namespace FileX{
static UTStack<FILoadContext*> fileContexts;
static UTStack<FIFileX*> fileXs;
static FILoadContext* fileContext;
static FIFileX* fileX;

///	�e���v���[�g��TypeDesc�����邩�ǂ����D
static bool TypeAvail(){
	return fileContext->fieldIts.size() && fileContext->fieldIts.back().type;
}

///	�m�[�h�f�[�^�̓ǂݏo������
static void NodeStart(const char* b, const char* e){
	std::string tn(b,e);
	PDEBUG( DSTR << "NodeStart " << tn << std::endl );

	//	�^���̎擾
	UTTypeDesc* type = fileX->GetDb()->Find(tn);
	if (!type) type = fileX->GetDb()->Find(tn + "Desc");
	
	if (type){
		fileContext->PushType(type);	//	���ꂩ�烍�[�h����^�Ƃ���Push
	}else{
		tn.append(" not defined.");
		fileContext->ErrorMessage(NULL, b, tn.c_str());
		fileContext->PushType(NULL);	//	Pop�ɔ����āCPush���Ă���
	}
}
///	�m�[�h�̖��O�̐ݒ�
static void NameSet(const char* b, const char* e){
	fileContext->datas.back()->name = UTString(b,e);
}
///	�ǂݏo�����f�[�^(ObjectDesc)����C�I�u�W�F�N�g���쐬����D
static void LoadNodeStub(const char* b, const char* e){
	fileContext->fileMaps.Top()->curr = b;
	fileX->LoadNode(fileContext);
}

///	�m�[�h�ǂݏo���̌㏈��
static void NodeEnd(const char* b, const char* e){
	PDEBUG(DSTR << "NodeEnd " << fileContext->fieldIts.back().type->GetTypeName() << std::endl);
	fileX->LoadEndNode(fileContext);
	fileContext->PopType();
}

///	�u���b�N�^�̓ǂݏo������
static void BlockStart(const char* b, const char* e){
	PDEBUG(DSTR << "blockStart" << std::endl);
	fileX->LoadEnterBlock(fileContext);
}

///	�u���b�N�^�̏I��
static void BlockEnd(const char* b, const char* e){
	PDEBUG(DSTR << "blockEnd" << std::endl);
	fileX->LoadLeaveBlock(fileContext);
}

/**	�u���b�N�ǂݏo�����C�t�B�[���h��ǂޑO�ɌĂ΂��D
	TypeDesc�����Ď��ɓǂݏo���ׂ��t�B�[���h���Z�b�g����D
	�ǂݏo���ׂ��t�B�[���h������� true ��Ԃ��D	*/
static bool NextField(){
	bool rv = fileContext->fieldIts.NextField();
	PDEBUG(
		if (rv){
			DSTR << "NextField:";
			fileContext->fieldIts.Top().field->Print(DSTR);
			DSTR << std::endl;
		}else {
			DSTR << "NextField failed." << std::endl;
		}
	)
	return rv;
}
///	�z��̃J�E���g�D�܂��ǂݏo���ׂ��f�[�^���c���Ă���� true ��Ԃ��D
static bool ArrayCount(){
	return fileContext->fieldIts.IncArrayPos();
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
static void BoolSet(const char* b, const char* e){
	UTString v(b,e);
	boolValue = (v.compare("true")==0) || (v.compare("TRUE")==0) || (v.compare("1")==0);
}
static void StrSet(const char* b, const char* e){
	strValue.assign(b+1,e-1);
}

///	ObjectDesc�ɓǂݏo�����l����������
static void SetVal(const char* b, const char* e){
	char ch = *b;

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
		if (ch == ';') DSTR << std::endl;
	}
#endif
	//	�����܂�
	
	if (fileContext->fieldIts.IsBool()){
		fileContext->WriteBool(boolValue);
	}else if (fileContext->fieldIts.IsNumber()){
		fileContext->WriteNumber(numValue);
	}else if (fileContext->fieldIts.IsString()){
		fileContext->WriteString(strValue);
	}
	if (ch == ';'){
		curField.arrayPos=UTTypeDesc::BIGVALUE;
	}
}
static void StopArray(const char c){
	UTTypeDescFieldIt& curField = fileContext->fieldIts.back();
	curField.arrayPos=UTTypeDesc::BIGVALUE;
}

///	�Q�ƌ^���������ށD(������)
static void RefSet(const char* b, const char* e){
	//DSTR << "ref(" << std::string(b,e) << ") not yet implemented." << std::endl;
	std::string ref(b,e);
	fileContext->AddLink(ref, b);
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
	tdesc->Link(fileX->GetDb());
	PDEBUG(DSTR << "load template:" << std::endl);
	tdesc->Print(DSTR);
	fileX->GetDb()->RegisterDesc(tdesc);
}
}
using namespace FileX;


class ExpectParser {
	std::string msg; 
public:
    ExpectParser(const char *m) : msg(m) {}
	static std::ostream* errorStr;

	typedef boost::spirit::nil_t result_t;
    
    template <typename ScannerT>
	int operator()(ScannerT const& scan, result_t& /*result*/) const {
		if (!scan.at_end()){
			std::string str = msg + std::string(" is expected");
			fileContext->ErrorMessage(NULL, scan.first, str.c_str());
		}
		return -1;
    }
}; 

typedef boost::spirit::functor_parser<ExpectParser> ExpP;


FIFileX::FIFileX(){
	Init();
}
void FIFileX::Init(){
	handlers.clear();
	typeDb.Clear();
	RegisterGroup("Foundation Physics Graphics Framework OldSpringhead");
	typeDb.RegisterAlias("Vec3f", "Vector");
	typeDb.RegisterAlias("Vec2f", "Coords2d");
	typeDb.RegisterAlias("Affinef", "Matrix3x3");
	typeDb.RegisterAlias("Affined", "Matrix4x4");

	using namespace std;
	using namespace boost::spirit;
	using namespace Spr;
	//	�p�[�T�̒�`
	//	�{���p�p�[�T
	start		= (str_p("xof 0302txt 0064") | str_p("xof 0303txt 0032") | ExpP("'xof 0303txt 0032'")) 
					>> *(temp | data | ExpP("template or data"));

	temp		= str_p("template") >> id[&TempStart] >> ch_p('{') >> !uuid
					>> *define >> ch_p('}')[&TempEnd];
	uuid		= ch_p('<') >> *~ch_p('>') >> ch_p('>');
	define		= defArray | defNormal | defOpen | defRestrict;
	defNormal	= id[&DefType] >> id[&DefId] >> ";";
	defArray	= str_p("array") >> id[&DefType] >> id[&DefId] 
					>> '[' >> arraySuffix >> ']' >> ';';
	defOpen		= str_p("[...]");
	defRestrict	= ch_p('[') >> *(id >> ',') >> id >> ']';
	arraySuffix	= id[&ArrayId] | int_p[&ArrayNum] | ExpP("id or int value");

	data		= id[&NodeStart] >> !id[&NameSet] >> (ch_p('{') | ExpP("'{'")) >>
				  if_p(&TypeAvail)[ block[&LoadNodeStub] >> !ch_p(';') >> *(data|ref) ].
				  else_p[ *blockSkip ]		//<	�m��Ȃ��^���̏ꍇ�X�L�b�v
				  >> (ch_p('}') | ExpP("'}'"))[&NodeEnd];
	blockSkip	= ch_p('{') >> *(blockSkip|~ch_p('}')) >> ch_p('}');
	ref			= ch_p('{') >> (id[&RefSet] | ExpP("id")) >> (ch_p('}')|ExpP("'}'"));
	block		= while_p(&NextField)[
					while_p(&ArrayCount)[
						ch_p(';')[&StopArray] |
						(exp >> ((ch_p(',')|!ch_p(';'))[&SetVal] | ExpP("',' or ';'")))
					]
				  ];
	exp			= if_p(&IsFieldBool)[ boolVal | ExpP("bool value") ] >>
				  if_p(&IsFieldInt)[ iNum | ExpP("int value") ] >>
				  if_p(&IsFieldReal)[ rNum | ExpP("numeric value") ] >>
				  if_p(&IsFieldStr)[ str | ExpP("string") ] >>
				  if_p(&IsFieldBlock)[ eps_p[&BlockStart] >> block[&BlockEnd] ];
	id			= lexeme_d[ (alpha_p|'_') >> *(alnum_p|'_'|'-') ];
	boolVal		= (str_p("true") | "TRUE" | "false" | "FALSE" | "1" | "0")[&BoolSet];
	iNum		= int_p[&NumSet];
	rNum		= real_p[&NumSet];
	str			= lexeme_d[ 
					ch_p('"') >> *( (ch_p('\\')>>anychar_p) | 
						~ch_p('"') ) >> ch_p('"') ][&StrSet];

	//	�X�L�b�v�p�[�T(�X�y�[�X�ƃR�����g��ǂݏo���p�[�T)�̒�`
	cmt		=	space_p
				|	"/*" >> *(~ch_p('*') | '*'>>~ch_p('/')) >> !ch_p('*') >> '/'
				|	"//" >> *~ch_p('\n') >> '\n'
				|	"#" >> *~ch_p('\n') >> '\n';
}



//------------------------------------------------------------------------------
void FIFileX::PushLoaderContext(FILoadContext* fc){
	fileContexts.Push(fc);
	fileXs.Push(this);
	fileContext = fileContexts.Top();
	fileX = fileXs.Top();
}
void FIFileX::PopLoaderContext(){
	fileContexts.Pop();
	fileXs.Pop();
	if (fileContexts.size()){
		fileContext = fileContexts.Top();
		fileX = fileXs.Top();
	}else{
		fileContext = NULL;
		fileX = NULL;
	}
}
void FIFileX::LoadImp(FILoadContext* fc){
	using namespace std;
	using namespace boost::spirit;
	using namespace Spr;
	PushLoaderContext(fc);
	parse_info<const char*> info = parse(
		fileContext->fileMaps.Top()->start, 
		fileContext->fileMaps.Top()->end, start, cmt);
	PopLoaderContext();
}
#define INDENT(x)	UTPadding((sc->objects.size()+x)*2)
//<< (sc->objects.size()+x)
void FIFileX::OnSaveFileStart(FISaveContext* sc){
	sc->file << "xof 0302txt 0064" << std::endl;
}
static bool cont;
void FIFileX::OnSaveNodeStart(FISaveContext* sc){
	sc->file << INDENT(-1) << sc->GetNodeTypeName();
	UTString name = sc->GetNodeName();
	if (name.length()) sc->file << " " << name;
	sc->file << "{" << std::endl;
	cont = false;
}
void FIFileX::OnSaveNodeEnd(FISaveContext* sc){
	sc->file << INDENT(-1) << "}" << std::endl;
}
void FIFileX::OnSaveDataEnd(FISaveContext* sc){
	if (cont) sc->file << std::endl;
}

void FIFileX::OnSaveFieldStart(FISaveContext* sc, int nElements){
	if (!cont){
		sc->file << INDENT(0);
		cont = true;
	}
}
void FIFileX::OnSaveFieldEnd(FISaveContext* sc, int nElements){
	if (!cont) sc->file << INDENT(0);
	sc->file << ";";
	cont = true;
	if (sc->fieldIts.Top().fieldType == UTTypeDescFieldIt::F_BLOCK){
		sc->file << std::endl;
		cont = false;
	}
}
void FIFileX::OnSaveElementEnd(FISaveContext* sc, int nElements, bool last){
	if (!last) sc->file << ",";
}
void FIFileX::OnSaveBool(FISaveContext* sc, bool val){
	sc->file << (val ? "TRUE" : "FALSE");
}
void FIFileX::OnSaveInt(FISaveContext* sc, int val){
	sc->file << val;
}
///	real�l�̕ۑ�
void FIFileX::OnSaveReal(FISaveContext* sc, double val){
	sc->file << val;
}
///	string�l�̕ۑ�
void FIFileX::OnSaveString(FISaveContext* sc, UTString val){
	sc->file << '"' << val << '"' << std::endl;
}
void FIFileX::OnSaveRef(FISaveContext* sc){
	NamedObjectIf* n = DCAST(NamedObjectIf, sc->objects.Top());
	sc->file << INDENT(-1) << "{" << n->GetName() << "}" << std::endl;
}


};
