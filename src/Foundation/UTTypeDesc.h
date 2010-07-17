/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef UTTYPEDESC_H
#define UTTYPEDESC_H

#include <Base/BaseUtility.h>
#include <Base/BaseTypeInfo.h>
#include <Foundation/Object.h>
#include <set>
#include <map>
#include <algorithm>

/*
	�t�@�C�����[�_�́C�^���f�[�^�������Ă���D
	�|�C���^�͂����Ă����āC��Ń����N����D
	����I�u�W�F�N�g�̂ǂ��Ƀ|�C���^�𑫂�/�㏑������̂��w������d�g�݂��K�v�D


	�z��́C�ϒ��f�[�^�D���������vector�ȂǂȂ̂ŁC������ƈႤ���ǁD
	vector �̂܂܂��ƁC�����ȃL���X�g���K�v�ɂȂ�D�ǂ�����H
	��vector�A�N�Z�X���\�b�h���\�[�X���玩�������D

	������𐔒l�ɕϊ�����Ƃ���܂ł̓p�[�T�̎d���D
	double �� ���l�^�ɕϊ�����̂�TypeDesc�̎d���D
	�p�[�T�������^�́C���l�C������C�|�C���^�C�g�ݗ���
	typedesc�������̂́C���l��char int float double
	
	typedesc->CreateInstance();
	typedesc->SetData("�����o��", �f�[�^);
		:
		:
	typedesc->GetData("�����o��", �f�[�^);


	�f�[�^��Node�̃c���[�D
	Node�́C�t�B�[���h�̗�
	�t�B�[���h�́C���l�C������C�|�C���^�C�g�ݗ���
	�g�ݗ��Ă�TypeDesc�����D
*/
/*
	Field�ƍ\���̂̊֌W�̌�������Field = C�̌^�ɂ������D

	C�̌^�́C�P���^��struct (�̔z��)
	�� Field = �P���^ or struct (�̔z��)
		TypeDesc �͒P���^��struct
*/

namespace Spr{;
/**	@page UTTypeDesc �h�L�������g�I�u�W�F�N�g�ƌ^�L�q
	C++�̍\���̂���f�[�^�������o���ꍇ�ȂǁC�\���̂�f�[�^�ɂ́C
	�ϐ�����ϐ��̌^�ƌ��������͂Ȃ��D
	�����ŁC�^�L�q�^�I�u�W�F�N�g(UTTypeDesc)���g���ċL�q����D
	UTTypeDesc�̃I�u�W�F�N�g����邽�߂̃\�[�X�̓w�b�_�t�@�C���� typedesc.exe ��
	�p�[�X���Ď�����������D*/

///	�Ώۂ̌^�ɃA�N�Z�X���邽�߂̃N���X
class UTAccessBase:public UTRefCount{
public:
	virtual ~UTAccessBase(){}
	///	�I�u�W�F�N�g�̍\�z
	virtual void* Create()=0;
	///	�I�u�W�F�N�g�̔j��
	virtual void Delete(void* ptr)=0;
	///	vector<T>::push_back(); return &back();
	virtual void* VectorPush(void* v)=0;
	///	vector<T>::pop_back();
	virtual void VectorPop(void* v)=0;
	///	vector<T>::at(pos);
	virtual void* VectorAt(void* v, int pos)=0;
	///	vector<T>::size();
	virtual size_t VectorSize(const void* v)=0;
	///
	virtual size_t SizeOfVector()=0;
};
template <class T>
class UTAccess:public UTAccessBase{
	virtual void* Create(){ return DBG_NEW T; }
	virtual void Delete(void* ptr){delete (T*)ptr; }
	virtual void* VectorPush(void* v){
		((std::vector<T>*)v)->push_back(T());
		return &((std::vector<T>*)v)->back();
	}
	virtual void VectorPop(void* v){
		((std::vector<T>*)v)->pop_back();
	}
	virtual void* VectorAt(void* v, int pos){
		return &((std::vector<T>*)v)->at(pos);
	}
	size_t VectorSize(const void* v){
		return ((const std::vector<T>*)v)->size();
	}
	virtual size_t SizeOfVector(){
		return sizeof(std::vector<T>);
	}
};

class UTTypeDescDb;
///	�^��\��
class SPR_DLL UTTypeDesc:public UTRefCount{
public:
	enum { BIGVALUE= 0x40000000 };
	///	���R�[�h�̃t�B�[���h��\��
	class SPR_DLL Field{
	public:
		typedef std::vector<std::pair<std::string, int> > Enums;
		Enums enums;
		///	�����o��
		std::string name;
		///	�^��
		std::string typeName;
		///	�^
		UTRef<UTTypeDesc> type;
		///	�I�t�Z�b�g
		int offset;
		///	�z��̗v�f���D
		int length;
		///	�v�f����ʂ̃t�B�[���h����Ƃ�ꍇ�̃t�B�[���h��
		std::string lengthFieldName;
		///	vector/�z�񂩂ǂ���
		enum VarType{
			SINGLE, ARRAY, VECTOR
		} varType;
		///	�Q�Ƃ��ǂ���
		bool isReference;

		Field(): offset(-1), length(1), varType(SINGLE), isReference(false){}
		~Field();
		///	�f�[�^�̃T�C�Y
		size_t GetSize();
		///
		void Print(std::ostream& os) const;
		///
		void AddEnumConst(std::string name, int val);
		void AddEnumConst(std::string name);
		///	�t�B�[���h�̃A�h���X���v�Z
		const void* /*UTTypeDesc::Field::*/GetAddress(const void* base, int pos);
		void* /*UTTypeDesc::Field::*/GetAddress(void* base, int pos){
			return (void*)GetAddress((const void*)base, pos); 
		}
		///	�t�B�[���h�̃A�h���X���v�Z�Dvector���g������D
		void* /*UTTypeDesc::Field::*/GetAddressEx(void* base, int pos);
		///	�t�B�[���h��std::vector�̏ꍇ�Cvector::size() ���Ăяo���D
		size_t VectorSize(const void * base){ 
			return type->access->VectorSize(((char*)base)+offset); 
		}
		///	type��bool�̒P���^�̏ꍇ�ɁCbool��ǂݏo���֐�
		bool ReadBool(const void* base, int pos=0){
			return type->ReadBool(GetAddress(base, pos));
		}
		///	type��bool�̒P���^�̏ꍇ�ɁCbool���������ފ֐�
		void WriteBool(void* base, bool val, int pos = 0){
			if (base) type->WriteBool(val, GetAddressEx(base, pos));
		}
		///	type�����l�̒P���^�̏ꍇ�ɁC���l��ǂݏo���֐�
		double ReadNumber(const void* base, int pos=0){
			return type->ReadNumber(GetAddress(base, pos));
		}
		///	type�����l�̒P���^�̏ꍇ�ɁC���l���������ފ֐�
		void WriteNumber(void* base, double val, int pos = 0){
			if (base) type->WriteNumber(val, GetAddressEx(base, pos));
		}
		///	������ǂݏo��
		std::string ReadString(const void* base, int pos=0){
			return type->ReadString(GetAddress(base, pos));
		}
		///	�����񏑂�����
		void WriteString(void* base, const char* val, int pos=0){
			if (base) type->WriteString(val, GetAddressEx(base, pos));
		}
	};
	///	�g�ݗ��Č^������킷�ꍇ�Ɏg��
	class SPR_DLL Composit: public std::vector<Field>{
	public:
		///	�f�[�^�̃T�C�Y��Ԃ�
		int Size(){ return Size(""); }
		///	�f�[�^�̃T�C�Y��Ԃ�
		int Size(std::string id);
		///
		void Link(UTTypeDescDb* db);
		///
		void Print(std::ostream& os) const;
	};

	///	�^��
	std::string typeName;
	///	�f�[�^�̒���
	size_t size;
	///	�g�ݗ��Č^�̒��g�̋L�q�D
	Composit composit;
	///	�v���~�e�B�u���ǂ���
	bool bPrimitive;
	///	IfInfo
	const IfInfo* ifInfo;
	///
	UTRef<UTAccessBase> access;

	friend class UTTypeDescDb;
public:
	///	�R���X�g���N�^
	UTTypeDesc():size(0), bPrimitive(false), ifInfo(NULL){}
	///	�R���X�g���N�^
	UTTypeDesc(std::string tn, int sz=0): typeName(tn), size(sz), bPrimitive(false), ifInfo(NULL){}
	///	
	virtual ~UTTypeDesc(){}
	///
	void Print(std::ostream& os) const;
	///	�^��
	std::string GetTypeName() const { return typeName; }
	///	�^��
	void SetTypeName(const char* s) { typeName = s; }
	///	�^�̃T�C�Y
	size_t GetSize() { return size; }
	///	�t�B�[���h�̒ǉ�
	Field* AddField(std::string pre, std::string ty, std::string n, std::string post);
	///	base�̒ǉ�
	Field* AddBase(std::string tn);

	///	�g�ݗ��Č^���ǂ���
	bool IsPrimitive(){ return bPrimitive; }
	///	�g�ݗ��Č^�̗v�f
	Composit& GetComposit(){ return composit; }
	///	�t�B�[���h�̌^���̃����N
	void Link(UTTypeDescDb* db);
	///
	const IfInfo* GetIfInfo(){ return ifInfo; }

	//	���[�e�B���e�B�֐�
	///	TypeDesc��bool�̒P���^�̏ꍇ�ɁCbool��ǂݏo���֐�
	virtual bool ReadBool(const void* ptr){ assert(0); return 0;}
	///	TypeDesc�����l�̒P���^�̏ꍇ�ɁC���l���������ފ֐�
	virtual void WriteBool(bool val, void* ptr){ assert(0);}
	///	TypeDesc�����l�̒P���^�̏ꍇ�ɁC���l��ǂݏo���֐�
	virtual double ReadNumber(const void* ptr){ assert(0); return 0;}
	///	TypeDesc�����l�̒P���^�̏ꍇ�ɁC���l���������ފ֐�
	virtual void WriteNumber(double val, void* ptr){ assert(0);}
	///	������ǂݏo��
	virtual std::string ReadString(const void* ptr){ assert(0);  return 0;}
	///	�����񏑂�����
	virtual void WriteString(const char* val, void* ptr){ assert(0); }

	///	�I�u�W�F�N�g�̍\�z
	void* Create(){
		return access ? access->Create() : NULL;
	}
	///	�I�u�W�F�N�g�̌�n��
	void Delete(void* ptr){ 
		if (ptr){
			assert(access);
			access->Delete(ptr);
		}
	}
	///	vector::push_back() return &vector::back();
	void* VectorPush(void* v){ return access->VectorPush(v); }
	///	vector::pop_back();
	void VectorPop(void* v){ access->VectorPop(v); }
	///	return &vector::at(pos);
	void* VectorAt(void* v, int pos){ return access->VectorAt(v, pos); }
	const void* VectorAt(const void* v, int pos){ return VectorAt((void*)v, pos); }
	///	return vector::size();
	size_t VectorSize(const void * v){ return access->VectorSize(v); }
	///
	size_t SizeOfVector(){ return access->SizeOfVector(); }
};
inline bool operator < (const UTTypeDesc& d1, const UTTypeDesc& d2){
	return d1.GetTypeName().compare(d2.GetTypeName()) < 0;
}

template <class N>
class SPR_DLL UTTypeDescNumber:public UTTypeDesc{
public:
	UTTypeDescNumber(){
		access = DBG_NEW UTAccess<N>;
		bPrimitive = true;
	}
	UTTypeDescNumber(std::string tn): UTTypeDesc(tn, sizeof(N)){
		access = DBG_NEW UTAccess<N>;
		bPrimitive = true;
	}
protected:
	///	���l�ǂݏo��
	virtual double ReadNumber(const void* ptr){
		return *(const N*)ptr;
	}
	///	���l��������
	virtual void WriteNumber(double val, void* ptr){
		*(N*)ptr = (N)val;
	}
};

template <class N>
class SPR_DLL UTTypeDescBool:public UTTypeDesc{
public:
	UTTypeDescBool(){
		access = DBG_NEW UTAccess<N>;
		bPrimitive = true;
	}
	UTTypeDescBool(std::string tn): UTTypeDesc(tn, sizeof(N)){
		access = DBG_NEW UTAccess<N>;
		bPrimitive = true;
	}
protected:
	///	���l�ǂݏo��
	virtual bool ReadBool(const void* ptr){
		return *(const N*)ptr != 0;
	}
	///	���l��������
	virtual void WriteBool(bool val, void* ptr){
		*(N*)ptr = (N)val;
	}
};

class SPR_DLL UTTypeDescString:public UTTypeDesc{
public:
	UTTypeDescString(){
		access = DBG_NEW UTAccess<std::string>;
		bPrimitive = true;
	}
	UTTypeDescString(std::string tn): UTTypeDesc(tn, sizeof(std::string)){
		access = DBG_NEW UTAccess<std::string>;
		bPrimitive = true;
	}
protected:
	///	���l�ǂݏo��
	virtual std::string ReadString(const void* ptr){
		return *(std::string*)ptr;
	}
	///	���l��������
	virtual void WriteString(const char* val, void* ptr){
		*(std::string*)ptr = val;
	}
};

///	�^�̃f�[�^�x�[�X
class SPR_DLL UTTypeDescDb: public UTRefCount{
	std::set<UTString, UTStringLess> addedGroups;
public:
	///	�R���e�i�̌^
	typedef std::set< UTRef<UTTypeDesc>, UTContentsLess< UTRef<UTTypeDesc> > > Db;
protected:
	std::string group;			///<	�O���[�v��
	std::string prefix;			///<	���O�̂����v���t�B�b�N�X�̕���
	Db db;						///<	UTTypeDesc �����Ă����R���e�i
public:
	UTTypeDescDb(UTString gp=""):group(gp){}
	///	
	~UTTypeDescDb();
	/**	�^�����f�[�^�x�[�X�ɓo�^�D	*/
	void RegisterDesc(UTTypeDesc* n){
		if (prefix.length() && n->typeName.compare(0, prefix.length(), prefix) == 0){
			n->typeName = n->typeName.substr(prefix.length());
		}
		db.insert(n);
	}
	/**	�^����Alias��o�^	*/
	void RegisterAlias(const char* src, const char* dest){
		UTTypeDesc* srcDesc = Find(src);
		assert(srcDesc);
		UTTypeDesc* destDesc =DBG_NEW UTTypeDesc(*srcDesc);
		destDesc->typeName = dest;
		RegisterDesc(destDesc);
	}
	/**	�^�����v���g�^�C�v���X�g�ɓo�^	*/
	void RegisterProto(UTTypeDesc* n);
	/**	�^����Prefix ��ݒ�D
		�^����Find�Ō�������ۂɁC�����L�[��Prefix�������L�[�ł��^������������D	*/
	void SetPrefix(std::string p);
	/**	�^���𖼑O���猟������D
		@param tn	�^���Dprefix �͏ȗ����Ă悢�D	*/
	UTTypeDesc* Find(std::string tn);
	///	DB���̌^���������N�D
	void Link();
	///	DB���̌^���̕\��
	void Print(std::ostream& os) const;
	UTTypeDescDb& operator += (const UTTypeDescDb& b){
		if (b.group.compare(group)!=0 && 
			addedGroups.find(b.group) == addedGroups.end()){
			db.insert(b.db.begin(), b.db.end());
			Link();
			addedGroups.insert(b.group);
		}
		return *this;
	}
	UTString GetGroup() const {
		return group;
	}
	void Clear(){
		db.clear();
	}
	friend class UTTypeDescDbPool;
};
inline bool operator < (const UTTypeDescDb& d1, const UTTypeDescDb& d2){
	return d1.GetGroup() < d2.GetGroup();
}

class UTTypeDescDbPool: public UTRefCount, 
	public std::set< UTRef<UTTypeDescDb>, UTContentsLess< UTRef<UTTypeDescDb> > >{
protected:
	static UTRef<UTTypeDescDbPool> pool;
	static UTTypeDescDbPool* SPR_CDECL GetPool();
public:
	static UTTypeDescDb* SPR_CDECL Get(std::string gp);
	static void SPR_CDECL Print(std::ostream& os);
};

/**	TypeDesc�̃t�B�[���h�̃C�^���[�^
	�o�C�i���t�@�C����X�t�@�C������C����^�̃f�[�^�����ɓǂݏo���Ă����ꍇ�C
	�ǂݏo�����̃f�[�^��UTTypeDesc�̃c���[�̂ǂ��ɑΉ����邩��ێ����Ă����K�v������D
*/
class UTTypeDescFieldIt{
public:
	/**	�t�B�[���h�̎�ނ������t���O�D
		�قƂ�ǂ̃t�@�C���t�H�[�}�b�g�ŁC�����C�����C������ŁC�قȂ�p�[�T���K�v�ɂȂ�D
		�����ŁC�����ŕ��ށD
		�g�ݗ��Č^�́CUTTypeDesc���Q�Ƃ��ēǂݏo���̂ŁCF_BLOCK��p�ӂ����D
	*/
	enum FieldType{
		F_NONE, F_BOOL, F_INT, F_REAL, F_STR, F_BLOCK
	};
	UTTypeDesc* type;						///<	�ǂݏo������UTTypeDesc
	UTTypeDesc::Composit::iterator field;	///<	�g�ݗ��Č^�̏ꍇ�C���̒��̂ǂ̃t�B�[���h��
	int arrayPos;							///<	�z��̏ꍇ�C�ǂݏo�����̓Y����
	int arrayLength;						///<	�Œ蒷�̏ꍇ�̔z��̒���
	FieldType fieldType;					///<	�ǂݏo���t�B�[���h�̌^

	UTTypeDescFieldIt(UTTypeDesc* d);		///<	�R���X�g���N�^
	bool NextField(void* base);				///<	���̃t�B�[���h�ɐi��
	bool PrevField(void* base);				///<	�O�̃t�B�[���h�ɐi��
	bool HaveField(UTString name);			///<	�w��̖��O�̃t�B�[���h�������Ă���Ȃ�true�B�p�����̃t�B�[���h����������B
protected:
	void SetFieldInfo(void* base);			///<	NextField���g��
	FieldType GetTypeId(UTTypeDesc* t);
	friend class UTLoadContext;
};
class UTTypeDescFieldIts:public UTStack<UTTypeDescFieldIt>{
public:
	///
	void PushType(UTTypeDesc* t){
		Push(UTTypeDescFieldIt(t));
	}
	///	���̃t�B�[���h�ɐi��
	bool NextField(void* base){
		if(size()) return back().NextField(base);
		return false;
	}
	///	�z�񒆂ł̈ʒu
	int ArrayPos(){
		if(size()) return back().arrayPos;
		return -1;
	}
	///	�z��̒���
	int ArrayLength(){
		if(size()) return back().arrayLength;
		return 0;
	}
	bool IncArrayPos(){
		if(!size()) return false;
		++ back().arrayPos;
		return back().arrayPos < back().arrayLength;
	}
	bool IsArrayRest(){
		if (!size()) return false;
		return back().arrayPos < back().arrayLength;
	}
	bool IsBool(){
		return back().fieldType==UTTypeDescFieldIt::F_BOOL;
	}
	bool IsNumber(){
		return back().fieldType==UTTypeDescFieldIt::F_INT || back().fieldType==UTTypeDescFieldIt::F_REAL;
	}
	bool IsInteger(){
		return back().fieldType==UTTypeDescFieldIt::F_INT;
	}
	bool IsReal(){
		return back().fieldType==UTTypeDescFieldIt::F_REAL;
	}
	bool IsString(){
		return back().fieldType==UTTypeDescFieldIt::F_STR;
	}
	bool IsBlock(){
		return back().fieldType==UTTypeDescFieldIt::F_BLOCK;
	}
};

void SPR_CDECL UTRegisterTypeDescs();

///	�P���^��o�^����D
#define REG_FIELD(type)	RegisterDesc( DBG_NEW UTTypeDesc(#type, sizeof(type)) )
/**	�h�L�������g���烍�[�h�\�ȃN���X�̒�`�D
	�N���X����{�N���X�����ꍇ	*/
#define DEF_RECORDBASE(Type, Base, Def)					\
	struct Type: public Base Def;						\
	static const char* classNameOf##Type = #Type;		\
	static const char* classDefOf##Type = #Def;			\

/**	�h�L�������g���烍�[�h�\�ȃN���X�̒�`�D
	��{�N���X�������ꍇ	*/
#define DEF_RECORD(Type, Def)							\
	struct Type Def;									\
	static const char* classNameOf##Type = #Type;		\
	static const char* classDefOf##Type = #Def;			\

/**	DER_RECORD �Œ�`�����^��o�^����D*/
#define REG_RECORD(Type)	\
	Register(classNameOf##Type, classDefOf##Type)

/**	DER_RECORD �Œ�`�����^���v���g�^�C�v�Ƃ��Ă��o�^����D*/
#define REG_RECORD_PROTO(Type)	\
	Register(classNameOf##Type, classDefOf##Type, true)

/**	@page TypeDesc �^�����N���X
C++�̍\���̐錾�𕶎���Ƃ��Ď󂯎��C�^����͂���D
�g�����F�w�b�_�t�@�C���ŁC��`
DEF_RECORD( Test, {
	int x;
	char y;
}
)

�������āC�N���X�̒�`���s���C
.cpp�t�@�C���̉����̊֐��̒���
	REG_RECORD(Test);
�������ēo�^���܂��D
*/


}
#endif
