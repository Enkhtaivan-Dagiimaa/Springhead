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

//#define PDEBUG_EVAL(x)	x
#define PDEBUG_EVAL(x)

//---------------------------------------------------------------------------
//	FIFile
FIFile::FIFile(){
	import = NULL;
	DSTRFlag = true;
}
void FIFile::SetImport(ImportIf* im){
	import = im->Cast();
}
ImportIf* FIFile::GetImport(){
	return import->Cast();
}
bool FIFile::Load(ObjectIfs& objs, const char* fn){
	DSTR << "Loading " << fn << " ...." << std::endl;

	FILoadContext fc;
	fc.sdk = sdk;
	fc.objects.insert(fc.objects.end(), objs.begin(), objs.end());

	// �C���|�[�g���ݒ肳��Ă��Ȃ��ꍇ�͍쐬����
	if(!import)
		import = sdk->CreateImport()->Cast();
	import->Clear();
	fc.importStack.Push(import->Cast());

	fc.PushFileMap(fn, true);

	bool ok = false;
	if(fc.IsGood()){
		Load(&fc);
		// ���[�h�����ŏ�ʃI�u�W�F�N�g���X�^�b�N�ɐςށi���[�U���A�N�Z�X�ł���悤�Ɂj
		objs.insert(objs.end(), fc.rootObjects.begin(), fc.rootObjects.end());
		ok = true;
	}

	fc.PopFileMap();

	return ok;
}
void FIFile::Load(FILoadContext* fc){
	LoadImp(fc);
	
	fc->SetDSTR(DSTRFlag);

	fc->LinkData();
	if(DSTRFlag) fc->loadedDatas.Print(DSTR);

	//	�f�[�^����m�[�h���쐬
	fc->CreateScene();
	//	�m�[�h�������N
	fc->LinkNode();
	//	�����N��̏���
	fc->PostTask();
}

bool FIFile::Save(const ObjectIfs& objs, const char* fn){
	// �ۑ���f�B���N�g���ֈړ�(�Ȃ���΍쐬)
	UTPath path(fn);
	UTPath::CreateDir(path.Dir());
	UTString oldCwd = UTPath::GetCwd();
	UTPath::SetCwd(path.Dir());

	FISaveContext sc;
	if(import)
		sc.importStack.Push(import->Cast());
	sc.sdk = sdk;

	bool ok = false;
	sc.PushFileMap(path.File().c_str(), IsBinary());
	if(sc.IsGood()){
		Save(objs, &sc);
		ok = true;
	}
	sc.PopFileMap();

	// ���f�B���N�g���֕��A
	UTPath::SetCwd(oldCwd);

	return ok;
}
void FIFile::Save(const ObjectIfs& objs, FISaveContext* sc){
	OnSaveFileStart(sc);
	for(ObjectIfs::const_iterator it = objs.begin(); it != objs.end(); ++it){
		SaveNode(sc, *it);
	}
	OnSaveFileEnd(sc);
}
void FIFile::SaveNode(FISaveContext* sc, ObjectIf* obj){
	// ����I�u�W�F�N�g��2�x�ڈȍ~�̕ۑ��͎Q�ƈ����ɂȂ�
	// �����L�҂����[�h���ƃZ�[�u���ŕς��\��������	tazz
	if (!sc->savedObjects.insert(obj).second){
		sc->objects.Push(obj);
		OnSaveRef(sc);
		sc->objects.Pop();
		return;
	}
	
	//	�Z�[�u���̃m�[�h���L�^
	sc->objects.Push(obj);

	UTString tn = sc->GetNodeTypeName();
	tn.append("Desc");
	UTTypeDesc* type = sc->typeDbs.Top()->Find(tn);
	if(type){
		//	�Z�[�u�ʒu��ݒ�
		sc->fieldIts.Push(UTTypeDescFieldIt(type));
		//	�I�u�W�F�N�g����f�[�^�����o���D
		void* data = (void*)obj->GetDescAddress();
		if (data){
			sc->datas.Push(DBG_NEW UTLoadedData(NULL, type, data));
		}else{
			sc->datas.Push(DBG_NEW UTLoadedData(NULL, type));
			data = sc->datas.back()->data;
			obj->GetDesc(data);
		}
		OnSaveNodeStart(sc);
		
		//	�f�[�^�̃Z�[�u
		OnSaveDataStart(sc);
		SaveBlock(sc);
		sc->datas.Pop();
		sc->fieldIts.Pop();
		OnSaveDataEnd(sc);
	}
	else{
		UTString err("Node '");
		err.append(tn);
		err.append("' not found. can not save data.");
		sc->ErrorMessage(err.c_str());
	}

	ObjectIfs exportedObjs;
	
	// �G�N�X�|�[�g�Ώۂ̎q�I�u�W�F�N�g��ʃt�@�C���ɕۑ�
	if(!sc->importStack.empty()){

		// obj���I�[�i�[�I�u�W�F�N�g�ł���G�N�X�|�[�g�G���g�����
		vector<Import*>	imports;
		Import* im = sc->importStack.Top();
		for(int i = 0; i < (int)im->Children().size(); i++){
			Import* imChild = im->Children()[i];
			if(imChild->ownerObj == obj)
				imports.push_back(imChild);
		}

		// �G���g�����Ƃɕʃt�@�C���ɕۑ�
		for(vector<Import*>::iterator it = imports.begin(); it != imports.end(); it++){
			Import* imChild = *it;

			if(!imChild->loadOnly){
				// �ۑ���f�B���N�g�����쐬���Ĉړ�
				UTPath path(imChild->path);
				bool changeDir = (path.Dir().compare("") != 0);
				UTString oldCwd;
				if(changeDir){
					UTPath::CreateDir(path.Dir());
					oldCwd = UTPath::GetCwd();
					UTPath::SetCwd(path.Dir());
				}

				// �G�N�X�|�[�g�Ώۂ̃I�u�W�F�N�g��ʃt�@�C���֕ۑ�
				FIFile* file = sc->sdk->CreateFileFromExt(path.Ext())->Cast();
				sc->PushFileMap(path.File(), IsBinary());
				if(sc->IsGood()){
					OnSaveFileStart(sc);
					for(int i = 0; i < (int)imChild->childObjs.size(); i++){
						ObjectIf* childObj = imChild->childObjs[i];
						file->SaveNode(sc, childObj);
						exportedObjs.push_back(childObj);
					}
					OnSaveFileEnd(sc);
				}
				sc->PopFileMap();
				
				if(changeDir)
					UTPath::SetCwd(oldCwd);
			}

			// �ۑ����̃t�@�C���ɂ�Import�m�[�h����������
			SaveNode(sc, imChild->Cast());
		}
	}

	// �G�N�X�|�[�g�ΏۊO�̎q�I�u�W�F�N�g��ۑ�
	size_t nChild = obj->NChildObject();
	if (nChild){
		OnSaveChildStart(sc);
		for(size_t i=0; i<nChild; ++i){
			ObjectIf* child = obj->GetChildObject(i);
			assert(child);
			// �G�N�X�|�[�g�ς݂Ȃ�X�L�b�v
			if(find(exportedObjs.begin(), exportedObjs.end(), child) != exportedObjs.end())
				continue;
			SaveNode(sc, child);
		}
		OnSaveChildEnd(sc);
	}

	if(type)
		OnSaveNodeEnd(sc);
	
	//	�L�^��POP
	sc->objects.Pop();
}
void FIFile::SaveBlock(FISaveContext* sc){
	OnSaveBlockStart(sc);
	void* base = sc->datas.Top()->data;
	while(sc->fieldIts.back().NextField(base)){
		//UTTypeDesc::Composit::iterator field = sc->fieldIts.back().field;	//	���݂̃t�B�[���h�^
		UTTypeDesc::Field* field = &*(sc->fieldIts.back().field);
		//	�v�f���̎擾
		int nElements = 1;
		if (field->varType == UTTypeDesc::Field::VECTOR){
			nElements = field->VectorSize(base);
		}else if (field->varType == UTTypeDesc::Field::ARRAY){
			nElements = field->length;
		}
		OnSaveFieldStart(sc, nElements);
		PDEBUG_EVAL( DSTR << "Save field '" << field->name << "' : " << field->typeName << " = "; )
		for(int pos=0; pos<nElements; ++pos){
			OnSaveElementStart(sc, pos, (pos==nElements-1));
			switch(sc->fieldIts.back().fieldType){
				case UTTypeDescFieldIt::F_BLOCK:{
					PDEBUG_EVAL( DSTR << "=" << std::endl; )
					void* blockData = field->GetAddress(base, pos);
					sc->datas.Push(DBG_NEW UTLoadedData(NULL, field->type, blockData));
					sc->fieldIts.Push(UTTypeDescFieldIt(field->type));
					SaveBlock(sc);
					sc->fieldIts.Pop();
					sc->datas.Pop();
					}break;
				case UTTypeDescFieldIt::F_BOOL:{
					bool val = field->ReadBool(base, pos);
					PDEBUG_EVAL( DSTR << val ? "true" : "false"; )
					OnSaveBool(sc, val);
					}break;
				case UTTypeDescFieldIt::F_INT:{
					int val = (int)field->ReadNumber(base, pos);
					PDEBUG_EVAL( DSTR << val; )
					OnSaveInt(sc, val);
					}break;
				case UTTypeDescFieldIt::F_REAL:{
					double val = field->ReadNumber(base, pos);
					PDEBUG_EVAL( DSTR << val; )
					OnSaveReal(sc, val);
					}break;
				case UTTypeDescFieldIt::F_STR:{
					UTString val = field->ReadString(base, pos);
					PDEBUG_EVAL( DSTR << val; )
					OnSaveString(sc, val);
					}break;
				default:;
			}
			PDEBUG_EVAL( if (pos<nElements-1) DSTR << ", "; )
			OnSaveElementEnd(sc, pos, (pos==nElements-1));
		}
		PDEBUG_EVAL( DSTR << ";" << std::endl; )
		OnSaveFieldEnd(sc, nElements);
	}
	OnSaveBlockEnd(sc);
}


}	//	namespace Spr
