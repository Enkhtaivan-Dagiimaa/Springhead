/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "FileIO.h"
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

using namespace std;

namespace Spr{;

ImportHandler::ImportHandler(){
	type = "ImportDesc";
}

void ImportHandler::AfterLoadData(UTLoadedData* ld, UTLoadContext* lc){
	FILoadContext* fc = (FILoadContext*)lc;
	
	// �C���|�[�g����t�@�C���̃p�X���擾
	ImportDesc* imDesc = (ImportDesc*)ld->data;
	UTPath path(imDesc->path);

	// �ꎞ�I��LoadContext���DOM�����[�h
	FIFile* file = fc->sdk->CreateFileFromExt(path.Ext())->Cast();
	fc->PushFileMap(path.FullPath(), true);
	if(fc->IsGood())
		file->LoadImp(fc);
	fc->fileMaps.Pop();
	
}

void ImportHandler::AfterCreateObject(UTLoadedData* ld, UTLoadContext* lc){
	FILoadContext* fc = (FILoadContext*)lc;
	
	// Import���쐬�C�p�X��ݒ�C�e�I�u�W�F�N�g��o�^
	ImportDesc* imDesc = (ImportDesc*)ld->data;
	Import* childImp = DBG_NEW Import;
	childImp->path = imDesc->path;
	childImp->ownerObj = lc->objects.Top();
	fc->importStack.Top()->AddChild(childImp);
	
	fc->importStack.Push(childImp);

}

void ImportHandler::AfterCreateChild(UTLoadedData* ld, ObjectIf* child, UTLoadContext* lc){
	FILoadContext* fc = (FILoadContext*)lc;
	
	// Import�̎q�I�u�W�F�N�g��o�^
	fc->importStack.Top()->childObjs.push_back(child);

}

void ImportHandler::AfterCreateChildren(UTLoadedData* ld, UTLoadContext* lc){
	FILoadContext* fc = (FILoadContext*)lc;
	if(fc->importStack.empty())
		return;

	fc->importStack.Pop();
}

//-------------------------------------------------------------------------------------------------

void Import::Clear(){
	ClearChildren();
	ownerObj = NULL;
	childObjs.clear();
	path = UTPath();
}

/*void Import::EnumByData(std::vector<ObjectDataPair*>& pairs, UTLoadedData* data){
	if(ownerObj.data == data)
		pairs.push_back(&ownerObj);
	for(ObjectDataPairs::iterator it = childObjs.begin(); it != childObjs.end(); it++){
		if(it->data == data)
			pairs.push_back(&*it);
	}

	for(container_t::iterator it = Children().begin(); it != Children().end(); it++)
		(*it)->EnumByData(pairs, data);
}*/

}
