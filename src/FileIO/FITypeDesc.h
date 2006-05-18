#ifndef FITYPEDESC_H
#define FITYPEDESC_H

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
	���\�b�h���\�[�X���玩�������HTypeDesc���d������΂���ōςށD

	������𐔒l�ɕϊ�����Ƃ���܂ł̓p�[�T�̎d���D
	double �� ���l�^�ɕϊ�����̂�TYpeDesc�̎d���D

	�p�[�T�������^�́C���l�C������C�|�C���^�C�g�ݗ���
	Typedesc�������̂́C���l��char int float double
	
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


namespace Spr{;
/**	@page FITypeDesc �h�L�������g�I�u�W�F�N�g�ƌ^�L�q
	C++�̍\���̂���f�[�^�������o���ꍇ�ȂǁC�\���̂�f�[�^�ɂ́C
	�ϐ�����ϐ��̌^�ƌ��������͂Ȃ��D
	�����ŁC�^�L�q�^�I�u�W�F�N�g(FITypeDesc)���g���ċL�q����D
	FITypeDesc�̃I�u�W�F�N�g����邽�߂̃\�[�X�̓w�b�_�t�@�C���� typedesc.exe ��
	�p�[�X���Ď�����������D*/

///	�Ώۂ̌^�ɃA�N�Z�X���邽�߂̃N���X
class FIAccessBase:public UTRefCount{
public:
	virtual ~FIAccessBase(){}
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
class FIAccess:public FIAccessBase{
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

class FITypeDescDb;
///	�^��\��
class SPR_DLL FITypeDesc:public UTRefCount{
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
		UTRef<FITypeDesc> type;
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
		const void* FITypeDesc::Field::GetAddress(const void* base, int pos);
		void* FITypeDesc::Field::GetAddress(void* base, int pos){
			return (void*)GetAddress((const void*)base, pos); 
		}
		///	�t�B�[���h�̃A�h���X���v�Z�Dvector���g������D
		void* FITypeDesc::Field::GetAddressEx(void* base, int pos);
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
			type->WriteBool(val, GetAddressEx(base, pos));
		}
		///	type�����l�̒P���^�̏ꍇ�ɁC���l��ǂݏo���֐�
		double ReadNumber(const void* base, int pos=0){
			return type->ReadNumber(GetAddress(base, pos));
		}
		///	type�����l�̒P���^�̏ꍇ�ɁC���l���������ފ֐�
		void WriteNumber(void* base, double val, int pos = 0){
			type->WriteNumber(val, GetAddressEx(base, pos));
		}
		///	������ǂݏo��
		std::string ReadString(const void* base, int pos=0){
			return type->ReadString(GetAddress(base, pos));
		}
		///	�����񏑂�����
		void WriteString(void* base, const char* val, int pos=0){
			type->WriteString(val, GetAddressEx(base, pos));
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
		void Link(FITypeDescDb* db);
		///
		void Print(std::ostream& os) const;
	};
protected:
	///	�^��
	std::string typeName;
	///	�f�[�^�̒���
	size_t size;
	///	�g�ݗ��Č^�̒��g�̋L�q�D�P���^�̏ꍇ�́Csize() == 0
	Composit composit;
	///	IfInfo
	const IfInfo* ifInfo;
	///
	UTRef<FIAccessBase> access;

	friend class FITypeDescDb;
	friend void RegisterTypes();
public:
	///	�R���X�g���N�^
	FITypeDesc():size(0), ifInfo(NULL){}
	///	�R���X�g���N�^
	FITypeDesc(std::string tn, int sz=0): typeName(tn), size(sz), 
		ifInfo(NULL){}
	///	
	virtual ~FITypeDesc(){}
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
	bool IsComposit(){ return composit.size()!=0; }
	///	�g�ݗ��Č^�̗v�f
	Composit& GetComposit(){ return composit; }
	///	�t�B�[���h�̌^���̃����N
	void Link(FITypeDescDb* db);
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
		return access->Create();
	}
	///	�I�u�W�F�N�g�̌�n��
	void Delete(void* ptr){ access->Delete(ptr); }
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
inline bool operator < (const FITypeDesc& d1, const FITypeDesc& d2){
	return d1.GetTypeName().compare(d2.GetTypeName()) < 0;
}

template <class N>
class SPR_DLL FITypeDescNumber:public FITypeDesc{
public:
	FITypeDescNumber(){
		access = DBG_NEW FIAccess<N>;
	}
	FITypeDescNumber(std::string tn): FITypeDesc(tn, sizeof(N)){
		access = DBG_NEW FIAccess<N>;
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
class SPR_DLL FITypeDescBool:public FITypeDesc{
public:
	FITypeDescBool(){
		access = DBG_NEW FIAccess<N>;
	}
	FITypeDescBool(std::string tn): FITypeDesc(tn, sizeof(N)){
		access = DBG_NEW FIAccess<N>;
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

class SPR_DLL FITypeDescString:public FITypeDesc{
public:
	FITypeDescString(){
		access = DBG_NEW FIAccess<std::string>;
	}
	FITypeDescString(std::string tn): FITypeDesc(tn, sizeof(std::string)){
		access = DBG_NEW FIAccess<std::string>;
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
class SPR_DLL FITypeDescDb: public UTRefCount{
public:
	///
	static UTRef<FITypeDescDb> theTypeDescDb;
	///	�R���e�i�̌^
	typedef std::set< UTRef<FITypeDesc>, UTContentsLess< UTRef<FITypeDesc> > > Db;
	struct ProtoDesc{
		std::string fileType;
		UTRef<FITypeDesc> desc;
	};
	typedef std::vector<ProtoDesc> ProtoDescs;
protected:
	std::string prefix;			///<	���O�̂����v���t�B�b�N�X�̕���
	Db db;						///<	FITypeDesc �����Ă����R���e�i
	static ProtoDescs protoDescs;
public:
	///	
	~FITypeDescDb();
	static const ProtoDescs& GetProtoDescs(){ return protoDescs; }
	/**	�^�����f�[�^�x�[�X�ɓo�^�D	*/
	void RegisterDesc(FITypeDesc* n){
		if (prefix.length() && n->typeName.compare(0, prefix.length(), prefix) == 0){
			n->typeName = n->typeName.substr(prefix.length());
		}
		db.insert(n);
	}
	/**	�^����Alias��o�^	*/
	void RegisterAlias(const char* src, const char* dest){
		FITypeDesc* srcDesc = Find(src);
		assert(srcDesc);
		FITypeDesc* destDesc =DBG_NEW FITypeDesc(*srcDesc);
		destDesc->typeName = dest;
		RegisterDesc(destDesc);
	}
	/**	�^�����v���g�^�C�v���X�g�ɓo�^	*/
	void RegisterProto(FITypeDesc* n);
	/**	�^����Prefix ��ݒ�D
		�^����Find�Ō�������ۂɁC�����L�[��Prefix�������L�[�ł��^������������D	*/
	void SetPrefix(std::string p);
	/**	�^���𖼑O���猟������D
		@param tn	�^���Dprefix �͏ȗ����Ă悢�D	*/
	FITypeDesc* Find(std::string tn);
	///	DB���̌^���������N�D
	void Link();
	///	DB���̌^���̕\��
	void Print(std::ostream& os) const ;
};


/**	�t�@�C������ObjectDesc��ǂݏo������C�t�@�C���ɏ������񂾂肷�邽�߂̃f�[�^�D
	ObjectDesc �ւ̃|�C���^(data) �� �^��� (type) �����D
	�������̊Ǘ����s���D	*/
class FINodeData: public UTRefCount{
public:
	FITypeDesc* type;	///<	�f�[�^�̌^ 
	UTString name;		///<	���O
	void* data;			///<	���[�h�����f�[�^
	bool haveData;		///<	data��delete���ׂ����ǂ����D
	FINodeData(FITypeDesc* t=NULL, void* d=NULL);
	~FINodeData();
};
/**	TypeDesc�̃t�B�[���h�̃C�^���[�^
	�o�C�i���t�@�C����X�t�@�C������C����^�̃f�[�^�����ɓǂݏo���Ă����ꍇ�C
	�ǂݏo�����̃f�[�^��FITypeDesc�̃c���[�̂ǂ��ɑΉ����邩��ێ����Ă����K�v������D
*/
class FIFieldIt{
public:
	/**	�t�B�[���h�̎�ނ������t���O�D
		�قƂ�ǂ̃t�@�C���t�H�[�}�b�g�ŁC�����C�����C������ŁC�قȂ�p�[�T���K�v�ɂȂ�D
		�����ŁC�����ŕ��ށD
		�g�ݗ��Č^�́CFITypeDesc���Q�Ƃ��ēǂݏo���̂ŁCF_BLOCK��p�ӂ����D
	*/
	enum FieldType{
		F_NONE, F_BOOL, F_INT, F_REAL, F_STR, F_BLOCK
	};
	FITypeDesc* type;						///<	�ǂݏo������FITypeDesc
//	FITypeDesc::Composit::iterator field;	///<	�g�ݗ��Č^�̏ꍇ�C���̒��̂ǂ̃t�B�[���h��
	FITypeDesc::Field* field;	///<	�g�ݗ��Č^�̏ꍇ�C���̒��̂ǂ̃t�B�[���h��
	int arrayPos;							///<	�z��̏ꍇ�C�ǂݏo�����̓Y����
	int arrayLength;						///<	�Œ蒷�̏ꍇ�̔z��̒���
	FieldType fieldType;					///<	�ǂݏo���t�B�[���h�̌^

	FIFieldIt(FITypeDesc* d);					///<	�R���X�g���N�^
	bool NextField();						///<	���̃t�B�[���h�ɐi��
};
class FIFieldIts:public UTStack<FIFieldIt>{
public:
	///
	void PushType(FITypeDesc* t){
		Push(FIFieldIt(t));
	}
	///	���̃t�B�[���h�ɐi��
	bool NextField(){
		if(size()) return back().NextField();
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
		return back().fieldType==FIFieldIt::F_BOOL;
	}
	bool IsNumber(){
		return back().fieldType==FIFieldIt::F_INT || back().fieldType==FIFieldIt::F_REAL;
	}
	bool IsString(){
		return back().fieldType==FIFieldIt::F_STR;
	}
};


///	�P���^��o�^����D
#define REG_FIELD(type)	RegisterDesc( DBG_NEW FITypeDesc(#type, sizeof(type)) )
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
