/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "UTLoadContext.h"
#include "UTLoadHandler.h"
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
	if (fc){
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
void UTLoadedData::SetName(UTString n){
	name = n;
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
		if (ld) nameMan->parent = ld->nameMan;
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
	os << UTPadding(w) << "Loaded data (desc) tree:" << std::endl;
	for(iterator it = begin(); it!=end(); ++it) (*it)->Print(os);
}
void UTLoadedData::Print(std::ostream& os){
	int w = os.width();
	os.width(0);
	os << UTPadding(w) << "<" << (type ? type->GetTypeName().c_str() : "(null)")
		<< " " << name << (haveData ? "node" : "") << ">" << std::endl;
	if (linkTo.size()){
		os << UTPadding(w+2) << "linkTo = ";
		for(unsigned i=0; i<linkTo.size(); ++i) os << " " << linkTo[i]->GetName();
		os << std::endl;
	}
	if (linkFrom.size()){
		os << UTPadding(w+2) << "linkFrom = ";
		for(unsigned i=0; i<linkTo.size(); ++i) os << " " << linkTo[i]->GetName();
		os << std::endl;
	}
	for(unsigned i=0; i<children.size(); ++i){
		os.width(w+2);
		children[i]->Print(os);
		os.width(0);
	}
	os << UTPadding(w) << "<" << (type ? type->GetTypeName().c_str() : "(null)")
		<< ">" << std::endl;
	os.width(w);
}



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
	linkFrom(from), linkTo(to), info(fm), pos(p){
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

//---------------------------------------------------------------------------
//	UTNameMangerForData
UTNameManagerForData::UTNameManagerForData():parent(NULL), data(NULL){
}

bool UTNameManagerForData::AddData(UTLoadedData* data){
	if (!data->name.length()) return false;
	std::pair<DataSet::iterator, bool> rv = dataSet.insert(data);
	if (rv.second){
		return true;
	}else if (*rv.first == data){
		return false;
	}
	UTString base = data->name;
	int i=1;
	do{
		std::ostringstream ss;
		ss << "_" << i << '\0';
		data->name = base + ss.str();
		rv = dataSet.insert(data);
		i++;
	} while(!rv.second);
	nameMap[base] = data->name;
	return true;
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
	if (pos != UTString::npos){	//	 ���O��Ԃ̎w�肪����ꍇ
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
	if (pos != UTString::npos){	//	 ���O��Ԃ̎w�肪����ꍇ
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
	key.name = name;
	UTNameManagerForData::DataSet::iterator it = dataSet.find(&key);
	if (it != dataSet.end()) return *it;
	return NULL;
}

//---------------------------------------------------------------------------
//	UTLoadContext
void UTLoadContext::WriteBool(bool v){
	UTTypeDescFieldIt& curField = fieldIts.back();
	curField.field->WriteBool(datas.Top()->data, v, curField.arrayPos);
}
void UTLoadContext::WriteNumber(double v){
	UTTypeDescFieldIt& curField = fieldIts.back();
	curField.field->WriteNumber(datas.Top()->data, v, curField.arrayPos);
}
void UTLoadContext::WriteString(std::string v){
	UTTypeDescFieldIt& curField = fieldIts.back();
	curField.field->WriteString(datas.Top()->data, v.c_str(), curField.arrayPos);
}
void UTLoadContext::PushType(UTTypeDesc* type){
	//	���[�h���ׂ�type�Ƃ��ăZ�b�g
	fieldIts.PushType(type);
	//	type�ɂ�����Desc�̃m�[�h(DOM�m�[�h)��p��
	UTLoadedData* data = DBG_NEW UTLoadedData(this, type);
	if (datas.size()){
		datas.Top()->AddChild(data);	//	�q�f�[�^�Ƃ��Ēǉ�
	}else{
		loadedDatas.push_back(data);	//	Top�̃f�[�^�Ƃ��ċL�^���Ă����D
	}
	data->SetupNameManager();
	//	�f�[�^�̃X�^�b�N�ɒǉ�
	datas.Push(data);
}
void UTLoadContext::PopType(){
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
	for (std::vector<Object*>::iterator o1 = ld->loadedObjects.begin(); o1!=ld->loadedObjects.end(); ++o1){
		for(UTLoadedDatas::iterator ld2 = ld->linkTo.begin(); ld2 != ld->linkTo.end(); ++ld2){
			for (std::vector<Object*>::iterator o2 = (*ld2)->loadedObjects.begin(); o2!=(*ld2)->loadedObjects.end(); ++o2){
				if (  !(*o1)->AddChildObject( (*o2)->Cast() )  ){
					std::string err("Can not add referenced object '");
					err.append((*ld2)->GetName());
					err.append("' into '");
					err.append(ld->GetName());
					err.append("'.");
					ErrorMessage(ld->fileInfo, ld->filePos, err.c_str());
				}				
			}
		}
	}
	for(UTLoadedDataRefs::iterator it = ld->children.begin(); it != ld->children.end(); ++it){
		LinkNode(*it);
	}
}
void UTLoadContext::LinkNode(){
	for(UTLoadedDataRefs::iterator it = loadedDatas.begin(); it!=loadedDatas.end(); ++it){
		LinkNode(*it);
	}
	links.Execute(this);
	links.clear();
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
UTRef<ObjectIf> UTLoadContext::CreateObject(const IfInfo* info,  const void* data, UTString name){
	UTRef<ObjectIf> obj;
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

};
