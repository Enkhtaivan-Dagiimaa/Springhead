/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <Foundation/UTLoadHandler.h>
#include <FileIO/FIFileCOLLADA.h>
#include <FileIO/FIFileCOLLADAHandler.h>
#include <FileIO/FILoadContext.h>
#include <FileIO/FISaveContext.h>
#include <fstream>
#include <sstream>
#include <boost/bind.hpp>
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

namespace Spr{;

#define TRACE_PARSE
#ifdef TRACE_PARSE
# define PDEBUG(x)	x
#else 
# define PDEBUG(x)
#endif

namespace FileCOLLADA{
static UTStack<FILoadContext*> fileContexts;
static UTStack<FIFileCOLLADA*> fileCOLLADAs;
static FILoadContext* fileContext;
static FIFileCOLLADA* fileCOLLADA;

///	�e���v���[�g��TypeDesc�����邩�ǂ����D
static bool TypeAvail(){
	return fileContext->fieldIts.size() && fileContext->fieldIts.back().type;
//	return fileContext->datas.size() && fileContext->datas.Top()->type;
}

UTString tagName;
UTLoadedData::Attributes attrs;
UTStack<char> tagStack;
///	XML�v�f(�^�O)�̎n�܂�
static void TagStart(const char* b, const char* e){
	attrs.clear();
	tagName = UTString(b,e);
	if (fileContext->fieldIts.size() 
		&& fileContext->fieldIts.Top().type
		&& fileContext->fieldIts.Top().HaveField(tagName)){	
		//	�g�ݗ��Č^�̃��[�h���ŁC�����o�����������ꍇ
		tagStack.Push(false);
	}else{
		//	������Ȃ��ꍇ�C�q�m�[�h�Ƃ��ă��[�h
		fileContext->NodeStart(tagName);
		tagStack.Push(true);
	}
	PDEBUG( DSTR << "TagStart " << tagName << std::endl );
}

///	�m�[�h�ǂݏo���̌㏈��
static void TagEnd(const char* b, const char* e){
	PDEBUG(DSTR << "TagEnd " << std::endl);
	if(tagStack.Top()) fileContext->NodeEnd();
	tagStack.Pop();
}

///	�m�[�h�̃f�[�^���X�L�b�v
static void SkipData(const char* b, const char* e){
	fileContext->datas.Top()->str = UTString(b,e);
}

UTString propKey, propValue;
static void SetPropertyKey(const char* b, const char* e){
	propKey = UTString(b, e);
}
static void SetPropertyValue(const char* b, const char* e){
	propValue = UTString(b, e);
}
static void SetProperty(const char* b, const char* e){
	attrs[propKey] = propValue;
}

///	�u���b�N�^�̓ǂݏo������
static void BlockStart(const char* b, const char* e){
	PDEBUG(DSTR << "blockStart" << std::endl);
	fileContext->CompositStart();
}

///	�u���b�N�^�̏I��
static void BlockEnd(const char* b, const char* e){
	PDEBUG(DSTR << "blockEnd" << std::endl);
	fileContext->CompositEnd();
}

/**	�u���b�N�ǂݏo�����C�t�B�[���h��ǂޑO�ɌĂ΂��D
	TypeDesc�����Ď��ɓǂݏo���ׂ��t�B�[���h���Z�b�g����D
	�ǂݏo���ׂ��t�B�[���h������� true ��Ԃ��D	*/
static bool NextField(){
	char* base = (char*)fileContext->datas.Top()->data;
	bool rv = fileContext->fieldIts.NextField(base);
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
	bool rv = fileContext->fieldIts.IncArrayPos();
//	DSTR << "ArrayCount=" << (rv ? "true" : "false") << std::endl;
	return rv;
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
	char ch = *b;

	UTTypeDescFieldIt& curField = fileContext->fieldIts.back();
	//	debug �o��
#ifdef TRACE_PARSE
	if (curField.fieldType!=UTTypeDescFieldIt::F_NONE){
		if (curField.fieldType==UTTypeDescFieldIt::F_BLOCK){
			DSTR << " => (" << curField.field->typeName << ") " << curField.field->name << std::endl;
		}else{
			if (curField.arrayPos==0 && curField.type->GetComposit().size()){
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
static void StopArray(const char* b, const char* e){
	UTTypeDescFieldIt& curField = fileContext->fieldIts.back();
	curField.arrayPos=UTTypeDesc::BIGVALUE;
}

///	�Q�ƌ^���������ށD(������)
static void RefSet(const char* b, const char* e){
	//DSTR << "ref(" << std::string(b,e) << ") not yet implemented." << std::endl;
	std::string ref(b,e);
	fileContext->AddDataLink(ref, b);
}


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


}
using namespace FileCOLLADA;


FIFileCOLLADA::FIFileCOLLADA(const FIFileCOLLADADesc& desc){
	Init();
}


/*
---------------------------------------------------------------------------------
COLLADA�̃m�[�h�̗�F
<physics_material	id="pmat2_0-PhysicsMaterial"	name="pmat2_0-PhysicsMaterial">
	<technique_common>
		<dynamic_friction>0</dynamic_friction>
		<restitution>0.800000012</restitution>
		<static_friction>0</static_friction>
	</technique_common>
</physics_material>
---------------------------------------------------------------------------------
�Ή����������\���́F
struct physics_materialCommon{
	float dynamic_friction;
	float restitution;
	float static_friction;
};
struct physics_material{
	physics_materialCommon technique_common;
};
---------------------------------------------------------------------------------
*/

static void FieldDump(const char* b, const char* e){
	DSTR << "Field: " << UTString(b,e) << std::endl; 
}
void FIFileCOLLADA::Init(){

	using namespace std;
	using namespace boost::spirit::classic;
	using namespace Spr;
	//	�p�[�T�̒�`
	//	�{���p�p�[�T
	start	= head >> body;
	head	= str_p("<?") >> "xml" >> "version" >> "=" >> "\"1.0\"" 
				>> "encoding" >> "=" >> "\"utf-8\"" >> "?>";
	body	= element;
	element = ch_p('<') >> id[&TagStart] >> *(property[&SetProperty]) >> (
				str_p("/>") | (ch_p('>') >> *field >> str_p("</") >> id >> ch_p('>') )
			  )[ &TagEnd ];
	property= id[&SetPropertyKey] >> '=' >> (id | string)[&SetPropertyValue];

	field	= element | data;
	data	= if_p(&TypeAvail)[
				if_p(&ArrayCount)[
					exp[&SetVal] | eps_p[&StopArray] 
				].else_p[
					nothing_p
				] >> 
				while_p(&ArrayCount)[ exp[&SetVal] | eps_p[&StopArray] ]
			  ].else_p[
				(+ ~ch_p('<'))[&SkipData]
			  ];
	exp		= if_p(&IsFieldBool)[ boolVal | ExpP("bool value") ] >>
			  if_p(&IsFieldInt)[ iNum | ExpP("int value") ] >>
			  if_p(&IsFieldReal)[ rNum | ExpP("numeric value") ] >>
			  if_p(&IsFieldStr)[ str | ExpP("string") ] >> 
			  if_p(&IsFieldBlock)[ eps_p[&BlockStart] >>  *field >> eps_p[&BlockEnd] ];
	boolVal	= (str_p("true") | "TRUE" | "false" | "FALSE" | "1" | "0")[&BoolSet];
	iNum	= id[&EnumSet] | int_p[&NumSet];
	rNum	= real_p[&NumSet];
	str		= lexeme_d[ ch_p('"') >> *( (ch_p('\\')>>anychar_p) | 
						~ch_p('"') ) >> ch_p('"') ][&StrSet];

	// xml �̖��O
	id			= lexeme_d[ (alpha_p|'_') >> *(alnum_p|'_'|'-') ];
	string		= lexeme_d[ ch_p('"') >> *~ch_p('"') >> '"' ];

	//	�X�L�b�v�p�[�T(�X�y�[�X�ƃR�����g��ǂݏo���p�[�T)�̒�`
	cmt		=	space_p
		|	"<!--" >> *(~ch_p('-') | '-' >> ~ch_p('-') | "--" >> ~ch_p('>')) >> "-->";
}


//------------------------------------------------------------------------------
/*	�^�Z�b�g�̐؂�ւ��̃T���v���ɂȂ�\�肾�������ǁC�s�v����
class FINHC_library_physics_models: public UTLoadHandlerSetDb<library_physics_models>{
public:
	FINHC_library_physics_models():UTLoadHandlerSetDb<Desc>("library_physics_models"){
//	TODO:
//		handlerDb = ;
//		typeDb = ;
	}
};
*/

void FIFileCOLLADA::PushLoaderContext(FILoadContext* fc){
	fc->RegisterGroupToDb("Foundation Physics Graphics FileIO Framework COLLADA");

	fileContexts.Push(fc);
	fileCOLLADAs.Push(this);
	fileContext = fileContexts.Top();
	fileCOLLADA = fileCOLLADAs.Top();
}
void FIFileCOLLADA::PopLoaderContext(){
	fileContexts.Pop();
	fileCOLLADAs.Pop();
	if (fileContexts.size()){
		fileContext = fileContexts.Top();
		fileCOLLADA = fileCOLLADAs.Top();
	}else{
		fileContext = NULL;
		fileCOLLADA = NULL;
	}
}
void FIFileCOLLADA::LoadImp(FILoadContext* fc){
	using namespace std;
	using namespace boost::spirit::classic;
	using namespace Spr;
	PushLoaderContext(fc);
	parse_info<const char*> info = parse(
		fileContext->fileMaps.Top()->start, 
		fileContext->fileMaps.Top()->end, start, cmt);
	PopLoaderContext();
}
#define INDENT(x)	UTPadding((sc->objects.size()+x)*2)
//<< (sc->objects.size()+x)
void FIFileCOLLADA::OnSaveFileStart(FISaveContext* sc){
	sc->Stream() << "xof 0302txt 0064" << std::endl;
}
static bool cont;
void FIFileCOLLADA::OnSaveNodeStart(FISaveContext* sc){
	sc->Stream() << INDENT(-1) << sc->GetNodeTypeName();
	UTString name = sc->GetNodeName();
	if (name.length()) sc->Stream() << " " << name;
	sc->Stream() << "{" << std::endl;
	cont = false;
}
void FIFileCOLLADA::OnSaveNodeEnd(FISaveContext* sc){
	sc->Stream() << INDENT(-1) << "}" << std::endl;
}
void FIFileCOLLADA::OnSaveDataEnd(FISaveContext* sc){
	if (cont) sc->Stream() << std::endl;
}

bool FIFileCOLLADA::OnSaveFieldStart(FISaveContext* sc, int nElements){
	if (!cont){
		sc->Stream() << INDENT(0);
		cont = true;
	}
	return true;
}
void FIFileCOLLADA::OnSaveFieldEnd(FISaveContext* sc, int nElements){
	if (!cont) sc->Stream() << INDENT(0);
	sc->Stream() << ";";
	cont = true;
	if (sc->fieldIts.Top().fieldType == UTTypeDescFieldIt::F_BLOCK){
		sc->Stream() << std::endl;
		cont = false;
	}
}
void FIFileCOLLADA::OnSaveElementEnd(FISaveContext* sc, int nElements, bool last){
	if (!last) sc->Stream() << ",";
}
void FIFileCOLLADA::OnSaveBool(FISaveContext* sc, bool val){
	sc->Stream() << (val ? "TRUE" : "FALSE");
}
void FIFileCOLLADA::OnSaveInt(FISaveContext* sc, int val){
	sc->Stream() << val;
}
///	real�l�̕ۑ�
void FIFileCOLLADA::OnSaveReal(FISaveContext* sc, double val){
	sc->Stream() << val;
}
///	string�l�̕ۑ�
void FIFileCOLLADA::OnSaveString(FISaveContext* sc, UTString val){
	sc->Stream() << '"' << val << '"' << std::endl;
}
void FIFileCOLLADA::OnSaveRef(FISaveContext* sc){
	NamedObjectIf* n = DCAST(NamedObjectIf, sc->objects.Top());
	sc->Stream() << INDENT(-1) << "{" << n->GetName() << "}" << std::endl;
}


};
