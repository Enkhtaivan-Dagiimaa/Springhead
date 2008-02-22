/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "UTLoadContext.h"
#include "UTLoadHandler.h"
#include "UTTypeDesc.h"
#include "Scene.h"
#include <Base/Affine.h>
#include <fstream>
#include <sstream>
#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

namespace Spr{;

//---------------------------------------------------------------------------
//	UTFileMap
bool UTFileMap::IsGood(){
	return start && end && (end != (char*)-1);
}


//---------------------------------------------------------------------------
//	UTLoadedData
UTLoadedData::UTLoadedData(UTLoadContext* fc, UTTypeDesc* t, void* d): parent(NULL), 
	type(t),data(d), haveData(false), man(NULL){
	if (!data) SetType(t);
	if (fc && fc->fileMaps.size()){
		fileInfo = fc->fileMaps.Top();
		filePos = fc->fileMaps.Top()->curr;
	}else{
		fileInfo = NULL;
		filePos = NULL;
	}
}
UTLoadedData::~UTLoadedData(){
	if (haveData){
		type->Delete(data);
		data = NULL;
	}
}
UTString UTLoadedData::GetName() const {
	return GetAttribute("name");
}
bool UTLoadedData::HasAttribute(UTString key) const {
	std::map<UTString, UTString>::const_iterator it;
	it = attributes.find(key);
	return it != attributes.end();
}
UTString UTLoadedData::GetAttribute(UTString key) const {
	std::map<UTString, UTString>::const_iterator it;
	it = attributes.find(key);
	if (it != attributes.end()) return it->second;
	return "";
}
void UTLoadedData::SetAttribute(UTString key, UTString val){
	attributes[key] = val;
}
void UTLoadedData::SetName(UTString n){
	attributes["name"] = n;
	UTLoadedData* ld = this;
	while(ld && !ld->nameMan) ld = ld->parent;
	if (ld){
		ld->nameMan->AddData(this);
		man = ld;
	}
}
void UTLoadedData::SetType(UTTypeDesc* t){
	if (data && type){
		type->Delete(data);
		data = NULL;
		haveData = false;
	}
	type = t;
	if (type){
		data = type->Create();
		haveData = true;
	}
}
void UTLoadedData::SetupNameManager(){
	if (type && type->ifInfo && type->ifInfo->Inherit(NameManagerIf::GetIfInfoStatic())){
		nameMan = DBG_NEW UTNameManagerForData;
		nameMan->data = this;
		UTLoadedData* ld = parent;
		while(ld && !ld->nameMan) ld = ld->parent;
		if (ld){
			nameMan->parent = ld->nameMan;
			ld->nameMan->childManagers.push_back(nameMan);
		}
	}
}
void UTLoadedData::AddLink(UTLoadedData* to){
	linkTo.push_back(to);
	to->linkFrom.push_back(this);
}
void UTLoadedData::AddChild(UTLoadedData* c){
	children.push_back(c);
	c->parent = this;
}
void UTLoadContext::LoadedDatas::Print(std::ostream& os){
	int w = os.width();
	os.width(0);
	os << UTPadding(w) << "--- Loaded data (desc) tree ------------------------" << std::endl;
	for(iterator it = begin(); it!=end(); ++it) (*it)->Print(os);
	os << "----------------------------------------------------" << std::endl;
	os.width(w);
}
void UTLoadedData::Print(std::ostream& os){
	UTString typeStr;
	if (type) typeStr = type->GetTypeName();
	else typeStr = UTString("(") + GetAttribute("type") + UTString(")");

	int w = os.width();
	os.width(0);
	os << UTPadding(w) << "<" << typeStr;
	for(Attributes::iterator it = attributes.begin(); it!=attributes.end(); ++it){
		if (it->first.compare("type")!=0){
			os << " " << it->first << "=" << it->second;
		}
	}
	os << (haveData ? " haveData" : "") << ">" << std::endl;
	if (linkTo.size()){
		os << UTPadding(w+2) << "linkTo = ";
		for(unsigned i=0; i<linkTo.size(); ++i) os << " " << linkTo[i]->GetName();
		os << std::endl;
	}
	if (linkFrom.size()){
		os << UTPadding(w+2) << "linkFrom = ";
		for(unsigned i=0; i<linkFrom.size(); ++i) os << " " << linkFrom[i]->GetName();
		os << std::endl;
	}
	for(unsigned i=0; i<children.size(); ++i){
		os.width(w+2);
		children[i]->Print(os);
		os.width(0);
	}
	if (str.length()){
		os << UTPadding(w+2) << "str=\"" << str << "\"" << std::endl;
	}
	os << UTPadding(w) << "</" << typeStr << ">" << std::endl;
	os.width(w);
}

UTLoadedData* UTLoadedData::FindAncestor(UTString tn){
	UTLoadedData* rv = parent;
	while(rv && rv->type->GetTypeName().compare(tn)!=0) rv = rv->parent;
	return rv;
}
UTLoadedData* UTLoadedData::FindDescendant(UTString tn){
	for(unsigned i=0; i<children.size(); ++i){
		if (children[i]->type->GetTypeName().compare(tn) == 0) return children[i];
	}
	for(unsigned i=0; i<children.size(); ++i){
		UTLoadedData* rv = children[i]->FindDescendant(tn);
		if (rv) return rv;
	}
	return NULL;
}
UTLoadedData* UTLoadedData::FindLinkAncestor(UTString tn){
	for(unsigned i=0; i<linkFrom.size(); ++i){
		if (linkFrom[i]->type->GetTypeName().compare(tn) == 0) return linkFrom[i];
	}
	for(unsigned i=0; i<linkFrom.size(); ++i){
		UTLoadedData* rv = linkFrom[i]->FindLinkAncestor(tn);
		if (rv) return rv;
	}
	return NULL;
}
UTLoadedData* UTLoadedData::FindLinkDescendant(UTString tn){
	for(unsigned i=0; i<linkTo.size(); ++i){
		if (linkTo[i]->type->GetTypeName().compare(tn) == 0) return linkTo[i];
	}
	for(unsigned i=0; i<linkTo.size(); ++i){
		UTLoadedData* rv = linkTo[i]->FindLinkDescendant(tn);
		if (rv) return rv;
	}
	return NULL;
}

void UTLoadedData::EnumLinkAncestor(std::vector<UTLoadedDatas>& res, UTString tn){
	if (type->GetTypeName().compare(tn) == 0){
		res.push_back(UTLoadedDatas());
		res.back().push_back(this);
		return;
	}
	int nRes = res.size();
	if (parent){
		parent->EnumLinkAncestor(res, tn);
		for(unsigned r=nRes; r<res.size(); ++r){
			res[r].push_back(this);
		}
	}
	for(unsigned i=0; i<linkFrom.size(); ++i){
		unsigned nRes = res.size();
		linkFrom[i]->EnumLinkAncestor(res, tn);
		for(unsigned r=nRes; r<res.size(); ++r){
			res[r].push_back(this);
		}
	}
}
void UTLoadedData::EnumLinkDescendant(std::vector<UTLoadedDatas>& res, UTString tn){
	if (type->GetTypeName().compare(tn) == 0){
		res.push_back(UTLoadedDatas());
		res.back().push_back(this);
		return;
	}
	for(unsigned i=0; i<children.size(); ++i){
		unsigned nRes = res.size();
		children[i]->EnumLinkDescendant(res, tn);
		for(unsigned r=nRes; r<res.size(); ++r){
			res[r].push_back(this);
		}
	}
	for(unsigned i=0; i<linkTo.size(); ++i){
		unsigned nRes = res.size();
		linkTo[i]->EnumLinkDescendant(res, tn);
		for(unsigned r=nRes; r<res.size(); ++r){
			res[r].push_back(this);
		}
	}
}
/*
bool UTLoadedData::FindRoute(UTLoadedDatas& res, UTLoadedData* to, bool upward=true){
	if (to == this){
		res.push_back(this);
		return true;
	}
	if (upward){
		if (parent->FindRoute(res, to, upward)){
			res.push_back(this);
			return true;
		}
		for(UTLoadedDatas::iterator it = linkFrom.begin(); it != linkFrom.end(); ++it){
			if ((*it)->FindRoute(res, to, upward)){
				res.push_back(this);
				return true;
			}
		}
	}else{
		for(UTLoadedDataRefs::iterator it = children.begin(); it != children.end(); ++it){
			if ((*it)->FindRoute(res, to, upward)){
				res.push_back(this);
				return true;
			}
		}
		for(UTLoadedDatas::iterator it = linkTo.begin(); it != linkTo.end(); ++it){
			if ((*it)->FindRoute(res, to, upward)){
				res.push_back(this);
				return true;
			}
		}
	}
	return false;
}
*/

//---------------------------------------------------------------------------
//	UTLoadTasks
OBJECT_IMP(UTLoadTask, NamedObject);

void UTLoadTasks::Execute(UTLoadContext* ctx){
	for(iterator it = begin(); it!=end(); ++it){
		(*it)->Execute(ctx);
	}
	clear();
}

//---------------------------------------------------------------------------
//	UTDataLinkTask
///	�m�[�h�ւ̎Q�Ƃ��L�^���Ă����N���X�D�S�����[�h�ł��Ă��烊���N����D
class UTDataLinkTask: public UTLoadTask{
public:
	//@name	�f�[�^�t�@�C���̂ǂ��̃f�[�^��
	//@{
	UTRef<UTFileMap> info;	///<	�t�@�C��
	const char* pos;		///<	�ʒu
	//@}

	UTLoadedData* linkFrom;	///<	�����N���f�[�^
	std::string linkTo;		///<	�����N��f�[�^�̖��O

	UTDataLinkTask(UTLoadedData* from, std::string to, UTFileMap* info, const char* p);
	void Execute(UTLoadContext* ctx);
};

UTDataLinkTask::UTDataLinkTask(UTLoadedData* from, std::string to, UTFileMap* fm, const char* p):
	info(fm), pos(p), linkFrom(from), linkTo(to){
}

void UTDataLinkTask::Execute(UTLoadContext* ctx){
	UTLoadedData* refData = NULL;
	UTLoadedData* ld = linkFrom;
	while (ld && !ld->nameMan){
		ld = ld->parent;
	}
	if (ld) refData = ld->nameMan->FindData(linkTo);
	if (refData){
		linkFrom->AddLink(refData);
	}else{
		std::string err("Referenced data '");
		err.append(linkTo);
		err.append("' not found.");
		ctx->ErrorMessage(info, pos, err.c_str());
	}
}



UTLinkTask::UTLinkTask(ObjectIf* from, UTString to, NameManagerIf* nm):
	linkFrom(from), linkTo(to){
		SetNameManager(nm);
		assert(nameManager);
}
void UTLinkTask::Execute(UTLoadContext* ctx){
	assert(nameManager);
	ObjectIf* to = nameManager->FindObject(linkTo);
	assert(to);
	linkFrom->AddChildObject(to);
};

//---------------------------------------------------------------------------
//	UTNameMangerForData
UTNameManagerForData::UTNameManagerForData():data(NULL), parent(NULL){
}

bool UTNameManagerForData::AddData(UTLoadedData* data){
	if (!data->GetName().length()) return false;
	DataSet::iterator it = dataSet.find(data);
	if (it == dataSet.end()){
		dataSet.insert(data);
		return true;
	}else{
		if (data == *it) return false;
	}
	UTString base = data->GetName();
	int i=1;
	while(1){
		std::ostringstream ss;
		ss << "_" << i << '\0';
		data->attributes["name"] = base + ss.str();
		DataSet::iterator it = dataSet.find(data);
		if (it == dataSet.end()){
			dataSet.insert(data);
			nameMap[base] = data->GetName();
			return true;
		}
		i++;
/*
		DSTR << base << ss.str() << std::endl;
		DSTR << "set:";
		for(DataSet::iterator it = dataSet.begin();it!= dataSet.end(); ++it){
			DSTR << " " << (*it)->GetName();
		}
		DSTR << std::endl;
*/
	}
	return false;
}
UTLoadedData* UTNameManagerForData::FindData(UTString name, UTString cls){
	//	�����Ǝq����T���B
	UTLoadedData* rv = FindDataFromDescendant(name, cls);
	if (rv) return rv;
	//	��c��T���D
	rv = FindDataFromAncestor(name, cls);
	if (rv) return rv;

	//	����ł��Ȃ��Ȃ�΁Anamespace������āA������x����
	int pos = name.find('/');
	if (pos != (int)UTString::npos){	//	 ���O��Ԃ̎w�肪����ꍇ
		UTString n = name.substr(pos+1);
		rv = FindData(n, cls);
	}
	return rv;
}
//	��c��T��
UTLoadedData* UTNameManagerForData::FindDataFromAncestor(UTString name, UTString cls){
	//	�Ȃ���Αc���T���B
	UTNameManagerForData* nm = parent;
	while(nm){
		UTLoadedData* rv = nm->SearchSet(name, cls);	//	�܂��e��T���A
		if (rv) return rv;
		//	�Z���T���A
		for(UTNameManagerForData::NameManagers::iterator it = nm->childManagers.begin(); 
			it!=nm->childManagers.end(); ++it){
			if (*it != this){
				rv = (*it)->FindDataFromDescendant(name, cls);
				if (rv) return rv;
			}
		}
		//	�Ȃ���΁A�e�̐e��T���B
		nm = nm->parent;
	}
	return NULL;
}

//	�����Ǝq����T��
UTLoadedData* UTNameManagerForData::FindDataFromDescendant(UTString name, UTString cls){
//	DSTR << "UTNameManagerForData(" << GetName() << ")::FindDescendant search " << name << std::endl;
	//	�҂�����̂��̂�T��
	UTLoadedData* rv = FindDataExact(name, cls);
	if (rv) return rv;
	//	�Ȃ���΁C�q���ɂ��ĒT��
	for(UTNameManagerForData::NameManagers::iterator it = childManagers.begin();
		it != childManagers.end(); ++it){
		rv = (*it)->FindDataFromDescendant(name, cls);
		if (rv) return rv;
	}
	return rv;
}
//	�l�[���X�y�[�X���݂Ŗ��O����������B�����ꏊ�ɂ��Ă͍ċA�Ȃ��B
UTLoadedData* UTNameManagerForData::FindDataExact(UTString name, UTString cls){
	UTLoadedData* rv = NULL;
	int pos = name.find('/');
	if (pos != (int)UTString::npos){	//	 ���O��Ԃ̎w�肪����ꍇ
		UTString n = name.substr(pos+1);
		UTString ns = name.substr(0, pos);
		//	�҂�����̂��̂�T���D
		for(UTNameManagerForData::NameManagers::iterator it = childManagers.begin();
			it != childManagers.end(); ++it){
			if (ns.compare((*it)->data->GetName()) == 0){
				rv = (*it)->FindDataExact(n, cls);
				if (rv) return rv;
			}
		}
	}else{	//	���O��Ԃ������ꍇ
		//	�҂�����̂��̂�T��
		rv = SearchSet(name, cls);
	}
	return rv;
}
UTLoadedData* UTNameManagerForData::SearchSet(UTString name, UTString cls){
	static UTLoadedData key(NULL, 0, NULL);
	key.SetName(name.c_str());
	UTNameManagerForData::DataSet::iterator it = dataSet.find(&key);
	if (it != dataSet.end()) return *it;
	return NULL;
}

//---------------------------------------------------------------------------
//	UTFileContext
UTFileContext::UTFileContext():errorStream(NULL){
}
//---------------------------------------------------------------------------
//	UTLoadContext
UTLoadContext::UTLoadContext(){
	errorStream=&DSTR;
	rootNameManagerForData = DBG_NEW UTLoadedData(NULL, NULL);
	rootNameManagerForData->nameMan = DBG_NEW UTNameManagerForData;
	rootNameManagerForData->nameMan->data = rootNameManagerForData;
	typeDbs.Push(DBG_NEW UTTypeDescDb);
	handlerDbs.Push(DBG_NEW UTLoadHandlerDb);
}
void UTLoadContext::WriteBool(bool v){
	UTTypeDescFieldIt& curField = fieldIts.back();
	if (curField.type->IsPrimitive()){
		curField.type->WriteBool(v, datas.Top()->data);
	}else{
		curField.field->WriteBool(datas.Top()->data, v, curField.arrayPos);
	}
}
void UTLoadContext::WriteNumber(double v){
	UTTypeDescFieldIt& curField = fieldIts.back();
	if (curField.type->IsPrimitive()){
		curField.type->WriteNumber(v, datas.Top()->data);
	}else{
		curField.field->WriteNumber(datas.Top()->data, v, curField.arrayPos);
	}
}
void UTLoadContext::WriteString(std::string v){
	UTTypeDescFieldIt& curField = fieldIts.back();
	if (curField.type->IsPrimitive()){
		curField.type->WriteString(v.c_str(), datas.Top()->data);
	}else{
		curField.field->WriteString(datas.Top()->data, v.c_str(), curField.arrayPos);
	}
}
void UTLoadContext::NodeStart(UTString tn, UTLoadedData::Attributes* attrs){
	//	�m�[�h�̌^��������
	UTTypeDesc* type = typeDbs.Top()->Find(tn);
	if (!type) type = typeDbs.Top()->Find(tn + "Desc");	
/*	TODOhase	
	//	�f�[�^�t�@�C���ɒm��Ȃ��^���������Ƃ��C�G���[���o������.
	//	XML�̏ꍇ�ȂǁC���邳���̂ŏ������D
	if (!type){
		UTString msg = tn;
		msg.append(" not defined.");
		Message(NULL, NULL, msg.c_str());
	}
*/
	//	�^�������[�h�p�C�^���[�^�ɃZ�b�g
	fieldIts.PushType(type);

	//	type�ɂ�����Desc�̃m�[�h(DOM�m�[�h)��p��
	UTLoadedData* data = DBG_NEW UTLoadedData(this, type);
	data->SetAttribute("type", type ? type->GetTypeName() : tn);	//	DOM�m�[�h�Ɍ^����ݒ�
	if (attrs){
		for(UTLoadedData::Attributes::iterator it = attrs->begin(); it!=attrs->end(); ++it){
			data->SetAttribute(it->first, it->second);
		}
	}
	//	DOM�c���[�ɒǉ�
	if (datas.size()){
		datas.Top()->AddChild(data);	//	�q�m�[�h�Ƃ��Ēǉ�
	}else{
		loadedDatas.push_back(data);	//	Top�m�[�h�Ƃ��ċL�^
		rootNameManagerForData->AddChild(data);
	}
	data->SetupNameManager();			//	���O�Ǘ��c���[�ɒǉ�

	//	�q�m�[�h�̃��[�h�p�ɁCDOM�m�[�h���X�^�b�N�ɐςށD
	datas.Push(data);

	//	DOM���[�h�O�n���h���̌Ăяo��
	static UTRef<UTLoadHandler> key = DBG_NEW UTLoadHandler;
	key->type = datas.Top()->GetAttribute("type");
	std::pair<UTLoadHandlerDb::iterator, UTLoadHandlerDb::iterator> range 
		= handlerDbs.Top()->equal_range(key);
	for(UTLoadHandlerDb::iterator it = range.first; it != range.second; ++it){
		(*it)->BeforeLoadData(datas.Top(), this);
	}
}
void UTLoadContext::NodeEnd(){
	//	�f�[�^���[�h��n���h���̌Ăяo��
	static UTRef<UTLoadHandler> key = DBG_NEW UTLoadHandler;
	key->type = datas.Top()->GetAttribute("type");
	std::pair<UTLoadHandlerDb::iterator, UTLoadHandlerDb::iterator> range 
		= handlerDbs.Top()->equal_range(key);
	for(UTLoadHandlerDb::iterator it = range.first; it != range.second; ++it){
		(*it)->AfterLoadData(datas.Top(), this);
	}

	//	�X�^�b�N�̕Еt��
	datas.Pop();
	fieldIts.Pop();
}
bool UTLoadContext::IsGood(){
	if (!fileMaps.size()) return false;
	return fileMaps.Top()->IsGood();
}
void UTLoadContext::AddDataLink(std::string ref, const char* pos){
	dataLinks.push_back(DBG_NEW UTDataLinkTask(datas.Top(), ref, fileMaps.Top(), pos));
}
void UTLoadContext::LinkData(){
	dataLinks.Execute(this);
	dataLinks.clear();
}
void UTLoadContext::LinkNode(UTLoadedData* ld){
	for(UTLoadedDatas::iterator ld2 = ld->linkTo.begin(); ld2 != ld->linkTo.end(); ++ld2){
		for (ObjectIfs::iterator o2 = (*ld2)->loadedObjects.begin(); o2!=(*ld2)->loadedObjects.end(); ++o2){
			bool rv = false;
			for (ObjectIfs::iterator o1 = ld->loadedObjects.begin(); o1!=ld->loadedObjects.end(); ++o1){
				DSTR << DCAST(NamedObject, *o1)->GetName() << "->" 
				<< DCAST(NamedObject, *o2)->GetName() << std::endl; 
				rv |= (*o1)->AddChildObject( (*o2)->Cast() );
			}
			if (!rv){
				std::string err("Can not add referenced object '");
				err.append((*ld2)->GetName());
				NamedObjectIf* no = (*o2)->Cast();
				if (no){
					err.append("=");
					err.append(no->GetName());
				}
				err.append(":");
				err.append((*o2)->GetIfInfo()->ClassName());
				err.append("' into '");
				for (ObjectIfs::iterator o1 = ld->loadedObjects.begin(); o1!=ld->loadedObjects.end(); ++o1){
					err.append(ld->GetName());
					no = (*o1)->Cast();
					if (no){
						err.append("=");
						err.append(no->GetName());
					}
					err.append(":");
					err.append((*o1)->GetIfInfo()->ClassName());
					err.append("'");
				}
				err.append(".");
				Message(ld->fileInfo, ld->filePos, err.c_str());
				lostAndFound.push_back(*o2);
			}
		}
	}
	for(UTLoadedDataRefs::iterator it = ld->children.begin(); it != ld->children.end(); ++it){
		LinkNode(*it);
	}
}

void UTLoadContext::CreateScene(){
	for(UTLoadedDataRefs::iterator it = loadedDatas.begin(); it!=loadedDatas.end(); ++it){
		datas.Push(*it);
		CreateSceneRecursive();
		datas.Pop();
	}
}
void UTLoadContext::CreateSceneRecursive(){
	UTLoadedData* ld = datas.Top();

	//	�n���h���[�̏���
	static UTRef<UTLoadHandler> key = DBG_NEW UTLoadHandler;
	key->type = ld->GetAttribute("type");
	std::pair<UTLoadHandlerDb::iterator, UTLoadHandlerDb::iterator> range 
		= handlerDbs.Top()->equal_range(key);
	typedef std::vector<UTLoadHandler*> Handlers;
	for(UTLoadHandlerDb::iterator it = range.first; it != range.second; ++it){
		(*it)->BeforeCreateObject(ld, this);
	}

	//	��c�I�u�W�F�N�g�ɍ���Ă��炤
	ObjectIf* obj = NULL;
	const IfInfo* info = NULL;
	if (ld->type) info = ld->type->GetIfInfo();
	if (info){
		obj = CreateObject(info, ld->data, ld->GetName());	//	�쐬���āC
		if (obj){
			ld->loadedObjects.Push(obj);
			objects.Push(obj);								//	�X�^�b�N�ɐς�
			if (objects.size() == 1){
				rootObjects.push_back(objects.Top());	//	���[�g�I�u�W�F�N�g�Ƃ��ċL�^
			}
		}
	}
	for(UTLoadHandlerDb::iterator it = range.first; it != range.second; ++it){
		(*it)->AfterCreateObject(ld, this);
	}

	//	�q�m�[�h�̍쐬
	for(UTLoadedDataRefs::iterator it = ld->children.begin(); it!= ld->children.end(); ++it){
		datas.Push(*it);
		CreateSceneRecursive();	//	�q���f�[�^�ɑΉ�����I�u�W�F�N�g�̍쐬
		datas.Pop();
	}
	//	�n���h���[�̏���
	for(UTLoadHandlerDb::iterator it = range.first; it != range.second; ++it){
		(*it)->AfterCreateChildren(ld, this);
	}
	
	//	�I������
	if(obj){
		objects.Pop();										//	�X�^�b�N��Pop
	}
}

void UTLoadContext::LinkNode(){
	for(UTLoadedDataRefs::iterator it = loadedDatas.begin(); it!=loadedDatas.end(); ++it){
		LinkNode(*it);
	}
	links.Execute(this);
	links.clear();
	lostAndFound.clear();
}

void UTLoadContext::PostTask(){	
	postTasks.Execute(this);
	postTasks.clear();
}
void UTLoadContext::ErrorMessage(UTFileMap* info, const char* pos, const char* msg){
	std::string m("error: ");
	m.append(msg);
	Message(info, pos, m.c_str());
}
void UTLoadContext::Message(UTFileMap* info, const char* pos, const char* msg){
	int lines=0;
	int returns=0;
	if (!info) info = fileMaps.Top();
	const char* ptr = info->start;
	const char* line=ptr;

	if (!pos) pos = info->curr;
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
		lines = lines > returns ? lines : returns;
	}
	std::ostream& os = *errorStream;
	os << info->name << "(" << lines+1 << ") : ";
	os << msg << std::endl;
	os << std::string(line, ptr) << std::endl;
}
ObjectIf* UTLoadContext::CreateObject(const IfInfo* info,  const void* data, UTString name){
	ObjectIf* obj=NULL;
	for(UTStack<ObjectIf*>::reverse_iterator it = objects.rbegin(); 
		it != objects.rend(); ++it){
		if (*it) obj = (*it)->CreateObject(info, data);
		if (obj) break;
	}
	//	��c�����Ȃ��ꍇ�CSdk�̍쐬�����Ă݂�D
	if (!obj) obj = SdkIf::CreateSdk(info, data);

	if (obj){
		//	�I�u�W�F�N�g�ɖ��O��ݒ�
		NamedObjectIf* n = DCAST(NamedObjectIf, obj);
		if (name.length()){
			if (n){
				n->SetName(name.c_str());
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
	return obj;
}

void UTLoadContext::RegisterGroupToDb(const char* gp){
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
