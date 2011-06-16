/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef FIIMPORT_H
#define FIIMPORT_H

#include <SprFileIO.h>
#include <Foundation/UTLoadHandler.h>
#include <Foundation/UTPath.h>

namespace Spr{;

/// Import�̃n���h��
//  �n���h���̓o�^��FISdk���s��
class ImportHandler : public UTLoadHandler{
public:	
	virtual void AfterLoadData(UTLoadedData* ld, UTLoadContext* lc);
	virtual void AfterCreateObject(UTLoadedData* ld, UTLoadContext* lc);
	virtual void AfterCreateChild(UTLoadedData* ld, ObjectIf* child, UTLoadContext* lc);
	virtual void AfterCreateChildren(UTLoadedData* ld, UTLoadContext* lc);
	ImportHandler();
};

/// �C���|�[�g�֌W��ێ�����DB
class Import : public Object, public UTTreeNode<Import>{
public:
	SPR_OBJECTDEF(Import);

	/// DOM�m�[�h�ƃI�u�W�F�N�g�̑�
	/*struct ObjectDataPair{
		UTLoadedData*	data;
		ObjectIf*		obj;

		ObjectDataPair():data(NULL), obj(NULL){}
	};
	typedef std::vector<ObjectDataPair>	ObjectDataPairs;*/

	//ObjectDataPair		ownerObj;		///< �C���|�[�g����I�u�W�F�N�g�̐e
	//ObjectDataPairs		childObjs;		///< �C���|�[�g����I�u�W�F�N�g
	ObjectIf*			ownerObj;
	ObjectIfs			childObjs;
	UTPath				path;			///< �C���|�[�g����t�@�C���̃p�X�i���΁j
	bool				loadOnly;

	virtual void Clear();
	virtual bool GetDesc(void* desc) const { ((ImportDesc*)desc)->path = path; return true; }
	
	void SetLoadOnly(bool on = true){ loadOnly = on; }

	// �q�C���|�[�g�G���g���̒�����I�[�i�[�I�u�W�F�N�g��obj�ł�����̂��
	void EnumByOwner(std::vector<Import*>& imports, ObjectIf* obj);

	// �q�C���|�[�g�G���g���̒�����DOM�m�[�h��data�ł���ObjectDataPair���
	//void EnumByData(ObjectIfs& objs, UTLoadedData* data);

	Import(const ImportDesc& desc = ImportDesc()){ loadOnly = false; }
};

}

#endif
