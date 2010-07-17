/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef UTLoadContext_H
#define UTLoadContext_H
#ifndef SWIGSPR

#include <Foundation/Object.h>
#include <Foundation/UTTypeDesc.h>


namespace Spr{;

///	�t�@�C���}�b�v(���̂Ƃ���t�@�C���̃��[�h��p)
class UTFileMap: public UTRefCount{
public:
	std::string name;	///<		�t�@�C����
	const char* start;	///<		�������}�b�v���ꂽ�t�@�C���̐擪
	const char* end;	///<		�������}�b�v���ꂽ�t�@�C���̏I�[
	const char* curr;	///<		���݂̈ʒu
	/// �R���X�g���N�^
	UTFileMap():start(NULL), end(NULL), curr(NULL){}
	///	�f�X�g���N�^
	virtual ~UTFileMap(){}
	/// �t�@�C�� �}�b�s���O		
	virtual bool Map(const UTString fn, bool binary)=0;
	/// �t�@�C�� �A���}�b�s���O
	virtual void Unmap()=0;
	///	���[�h�ł����ԂȂ�true
	virtual bool IsGood()=0;
};


class UTLoadContext;
class UTLoadedData;
class UTLoadedDatas: public std::vector< UTLoadedData* >{
};
class UTLoadedDataRefs: public std::vector< UTRef<UTLoadedData> > {
};
class UTNameManagerForData;
/**	�t�@�C�����[�h�̍ۂ�DOM�m�[�h�̂悤�Ȗ���������m�[�h
	�t�@�C������ObjectDesc��ǂݏo������C�t�@�C���ɏ������񂾂肷�邽�߂̃f�[�^�D
	ObjectDesc �ւ̃|�C���^(data) �� �^��� (type) �����D
	�������̊Ǘ����s���D	*/
class UTLoadedData: public UTRefCount{
public:
	typedef std::map<UTString, UTString> Attributes;
protected:
	 Attributes attributes;			///<	����

public:
	UTRef<UTFileMap> fileInfo;		///<	�t�@�C�����
	const char* filePos;			///<	�t�@�C�����ł̈ʒu
	UTLoadedData* parent;			///<	�e�m�[�h
	UTLoadedDataRefs children;		///<	�q�m�[�h
	UTLoadedDatas linkFrom;			///<	�Q�ƌ��m�[�h
	UTLoadedDatas linkTo;			///<	�Q�Ɛ�m�[�h
	UTTypeDesc* type;				///<	�^
	void* data;						///<	���[�h�����f�B�X�N���v�^
	UTString str;					///<	���[�h���e�̕�����D�^��񂪂Ȃ��ꍇ�Ɏg���D
	bool haveData;					///<	data�����L���邩�ǂ���

	///	type��NameManager�̔h���N���X�̏ꍇ�����CData�pNameManger������
	UTRef<UTNameManagerForData> nameMan;	
	UTLoadedData* man;				///<	���̃f�[�^�̖��O���Ǘ�����NameManger�����f�[�^
	
	///	���̃f�[�^������ꂽ�I�u�W�F�N�g����
	ObjectIfs loadedObjects;

	UTLoadedData(UTLoadContext* fc, UTTypeDesc* t, void* data=NULL);
	~UTLoadedData();
	void AddLink(UTLoadedData* to);	///<	�Q�Ƃ̒ǉ�
	void AddChild(UTLoadedData* c);	///<	�q�m�[�h�̃f�[�^�̒ǉ�
	void SetupNameManager();
	void SetType(UTTypeDesc* t);
	void SetName(UTString n);
	UTString GetName() const;
	void SetAttribute(UTString key, UTString val);
	UTString GetAttribute(UTString key) const;
	bool HasAttribute(UTString key) const;
	friend class UTNameManagerForData;
	bool operator < (const UTLoadedData& d2) const {
		return GetName().compare(d2.GetName()) < 0;
	}
	UTLoadedData* FindAncestor(UTString tn);
	UTLoadedData* FindDescendant(UTString tn);
	UTLoadedData* FindLinkAncestor(UTString tn);
	UTLoadedData* FindLinkDescendant(UTString tn);
	void EnumLinkAncestor(std::vector<UTLoadedDatas>& res, UTString tn);
	void EnumLinkDescendant(std::vector<UTLoadedDatas>& res, UTString tn);
	void Print(std::ostream& os);
};

///	UTLoadedData �p�̖��O�Ǘ�
class UTNameManagerForData: public UTRefCount{
public:

	typedef std::set<UTLoadedData*, UTContentsLess<UTLoadedData*> > DataSet;
	typedef std::map<UTString, UTString> NameMap;
	///@name	���O��Ԃ��\�����邽�߂̃c���[
	//@{
	///
	typedef std::vector<UTNameManagerForData*> NameManagers;
	///	������̃f�[�^
	UTLoadedData* data;
	///	�q���O���
	NameManagers childManagers;
	///	�e���O���
	UTNameManagerForData* parent;
	//@}

	UTLoadedData* FindData(UTString name, UTString cls="");
protected:
	NameMap nameMap;
	DataSet dataSet;
public:
	UTNameManagerForData();

	///	UTLoadedData�𖼑O�\�ɒǉ�
	bool AddData(UTLoadedData* data);

	/**	���O�̕ϊ��i���O���Ԃ������ꍇ�C�m�[�h�̖��O��ϊ����邪�C
		���̊֐��́C���̖��O���ϊ���̖��O�̕ϊ�������D	
		@param n ���̖��O
		@return �ϊ���̖��O
	*/
	UTString MapName(UTString n);

protected:
	UTLoadedData* FindDataFromAncestor(UTString name, UTString cls);
	UTLoadedData* FindDataFromDescendant(UTString name, UTString cls);
	UTLoadedData* FindDataExact(UTString name, UTString cls);
	UTLoadedData* SearchSet(UTString name, UTString cls);
};

class UTLoadContext;
///	���[�h��ɏ����������邽�߂ɁA���[�h���ɐ�������A���[�h��Ɏ��s�����D
class UTLoadTask:public NamedObject{
public:
	SPR_OBJECTDEF_NOIF(UTLoadTask);
	virtual ~UTLoadTask(){}
	virtual void Execute(UTLoadContext* ctx){};
};

///	���[�h��Ƀ����N���邽�߂̃^�X�N�D���ʂ͎g��Ȃ��D
class UTLinkTask:public UTLoadTask{
public:
	ObjectIf* linkFrom;
	UTString linkTo;
	UTLinkTask(ObjectIf* from, UTString to, NameManagerIf* nm);
	virtual void Execute(UTLoadContext* ctx);
	virtual ~UTLinkTask(){}
};

///	���[�h��̏������s�����߂̃^�X�N���X�g
class UTLoadTasks:public std::vector< UTRef<UTLoadTask> >{
public:
	void Execute(UTLoadContext* ctx);
};

typedef std::pair<UTRef<ObjectIf>, UTRef<ObjectIf> > UTPairObject;
typedef std::map<UTRef<ObjectIf>, UTRef<ObjectIf> >  UTMapObject;


class UTLoadHandlerDb;

/**	�t�@�C�����[�h/�Z�[�u���Ɏg�p����R���e�L�X�g�̊�{�N���X	*/
class UTFileContext{
public:
	UTFileContext();
	///	���[�h/�Z�[�u���̃t�@�C���̖��O�ƒ��g�D�t�@�C��include�ɔ�����stack�ɂȂ��Ă���D
	UTStack< UTRef<UTFileMap> > fileMaps;
	///	�G���[���b�Z�[�W�o�͗p�̃X�g���[�� cout �Ƃ� DSTR ���w�肷��D
	std::ostream* errorStream;
	///	typeDb �̃X�^�b�N
	UTStack< UTRef<UTTypeDescDb> > typeDbs;
	///	handlerDb�̃X�^�b�N
	UTStack< UTRef<UTLoadHandlerDb> > handlerDbs;
	/**	���݃��[�h�� or �Z�[�u���̃I�u�W�F�N�g�D
		�l�X�g�����I�u�W�F�N�g�ɔ����ăX�^�b�N�ɂȂ��Ă���D*/
	ObjectIfs objects;

	///	���[�h�ł����ԂȂ�true
	bool IsGood();
	
	///	�t�@�C���}�b�v���쐬���ăX�^�b�N(fileMaps)�ɐς�
	virtual void PushFileMap(const UTString fn, bool binary)=0;
	virtual void PopFileMap()=0;
};

/**	�t�@�C�����[�h���Ɏg�p����R���e�L�X�g
	�t�@�C�������[�h����ۂ́C�f�[�^���m�[�h���ƂɃ��[�h���āC
	�I�u�W�F�N�g����邽�߂̃f�B�X�N���v�^�\����(PHSolidDesc�Ȃ�)��
	�܂����[�h����D
	���̂��ƁC�I�u�W�F�N�g�𐶐�����D	*/
class UTLoadContext: public UTFileContext{
protected:
	UTStack< UTRef<ObjectIf> > lostAndFound;
public:	
	//--------------------------------------------------------------------------
	//	�ϐ�

	///	�X�^�b�N�ɍŏ��ɋl�܂ꂽ�I�u�W�F�N�g���t�@�C���̈�ԊO�������[�g�̃I�u�W�F�N�g�̋L�^�D
	ObjectIfs rootObjects;
	///	���[�h�����f�B�X�N���v�^�̃X�^�b�N�D�l�X�g�����g�ݗ��Č^�ɔ����ăX�^�b�N�ɂȂ��Ă���D
	UTStack< UTRef<UTLoadedData> > datas;
	struct LoadedDatas:public std::vector< UTRef<UTLoadedData> >{
		void Print(std::ostream& os);
	};
	///	���[�h�����f�B�X�N���v�^�̋L�^�D
	LoadedDatas loadedDatas;
	///	���[�h����FITypedesc�̃t�B�[���h�̈ʒu�D�g�ݗ��Č^�̃t�B�[���h�ɔ����ăX�^�b�N�ɂȂ��Ă���D
	UTTypeDescFieldIts fieldIts;
	///	�m�[�h�X�^�[�g����fieldIts.size()
	size_t nodeStartDepth;

	///	�t���O�̃X�^�b�N
	UTStack<char> flags;

	///	�f�[�^�̃����N����ł��邽�߂̋L�^�D
	UTLoadTasks dataLinks;
	///	���t�@�����X����Ń����N���邽�߂̋L�^�D
	UTLoadTasks links;
	///	���[�h�ƃ����N���I����Ă��珈������^�X�N
	UTLoadTasks postTasks;
	// �}�b�v�I�u�W�F�N�g�D�m�[�h�ԂŊ֌W���������������ꍇ�Ɏg�p�D
	UTMapObject mapObj;	
	//	�f�[�^�p�̃��[�gNameManager�̂��߂̃I�u�W�F�N�g
	UTRef<UTLoadedData> rootNameManagerForData;
	//	DSTR�ɏ���\�����邩�ǂ����̃t���O
	bool DSTRFlag;

	//---------------------------------------------------------------------------
	///	�R���X�g���N�^
	UTLoadContext();
	virtual ~UTLoadContext(){}
	//@name ���[�_�C�n���h�����������߂̃��[�e�B���e�B
	//@{
	///	�G���[���b�Z�[�W�̏o�́Dpos���t�@�C�����ƍs�ԍ��ɕϊ�����D
	void ErrorMessage(UTFileMap* info, const char* pos, const char* msg);
	///	���b�Z�[�W�̍쐬�Dpos���t�@�C�����ƍs�ԍ��ɕϊ�����D
	void Message(UTFileMap* info, const char* pos, const char* msg);
	///	type���������鏀��������(type���Z�b�g���CXXDesc��p�ӂ���)
	void NodeStart(UTString tn, UTLoadedData::Attributes* attrs=NULL);
	////
	void NodeEnd();
	///	�g���^�ɓ���(fieldIts��datas�Ɍ��݂̃t�B�[���h�̌^��Push����j
	void CompositStart();
	///	�g���^���甲��(fieldIts��datas��Pop����j
	void CompositEnd();
	///	���݃��[�h���̃m�[�h����w��̃t�B�[���h���������B
	bool UTLoadContext::FindField(UTString name);
	///
	void WriteString(std::string v);
	///
	void WriteNumber(double d);
	///
	void WriteBool(bool b);
	///
	void AddDataLink(std::string ref, const char* pos);	
	//@}
	//@name �f�[�^����I�u�W�F�N�g�����Ƃ��̃��[�e�B���e�B
	//@{
	///	�f�[�^����I�u�W�F�N�g���쐬
	ObjectIf* CreateObject(const IfInfo* info,  const void* data, UTString name="");

	//@}
	void LinkData();
	///
	void CreateScene();
	///
	void LinkNode();
	///
	void PostTask();
	/**	���[�h����m�[�h�̃O���[�v��o�^�B�O���[�v�����X�y�[�X�ŋ�؂��Ďw��B
		��FResisterGroupToDb("Foundation Physics Graphics Framework OldSpringhead");
	*/
	void RegisterGroupToDb(const char* gp);

	void SetDSTR(bool f){ DSTRFlag = f ;}
protected:
	void LinkNode(UTLoadedData* ld);
	ObjectIf* CreateSceneRecursive();
};

}

#endif
#endif	// UTLoadContext_H
