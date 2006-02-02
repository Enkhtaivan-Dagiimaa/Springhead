#ifndef FITYPEDESC_H
#define FITYPEDESC_H

#include <Base/BaseUtility.h>
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
	�����ŁC�^�L�q�^�I�u�W�F�N�g(FITypeDesc)���g���ċL�q���Ă����D
*/

/**	FIVVector�̊�{�N���X�D
	�x�N�^�ւ̃��[�h�ƃZ�[�u�̃T�|�[�g�ɕK�v�ȉ��z�֐���p��	*/
class FIVVectorBase{
public:
	virtual size_t VItemSize() = 0;
	virtual size_t VSize() = 0;
	virtual void VResize(size_t s) = 0;
	virtual char* VBegin() = 0;
	virtual char* VEnd() = 0;
	virtual void VInsert(int pos, void* v) = 0;
	virtual void VPushBack(void* v) = 0;
	virtual void VPushBack() = 0;
	virtual void VPopBack() = 0;
};

/**	FITypeDesc �������ł���std::vector
	�x�N�^�ւ̃��[�h�ƃZ�[�u�̃T�|�[�g�ɕK�v�ȉ��z�֐������������D
	FITypeDesc �͕��ʂ�vector ���܂ރN���X�̓T�|�[�g���Ȃ��D*/
template <class T>
class FIVVector: public FIVVectorBase, public std::vector<T>{
	virtual size_t VItemSize(){ return sizeof(T); }
	virtual size_t VSize(){ return size(); }
	virtual void VResize(size_t s){ resize(s); }
	virtual char* VBegin(){ return (char*)&*begin(); }
	virtual char* VEnd(){ return (char*) &*end(); }
	virtual void VInsert(int pos, void* v){ insert(begin()+pos, *(T*)v); }
	virtual void VPushBack(void* v){ push_back(*(T*)v); }
	virtual void VPushBack(){ push_back(T()); }
	virtual void VPopBack(){ pop_back(); }
public:
	FIVVector(){}
	FIVVector(const std::vector<T>& v):std::vector<T>(v){}
	FIVVector& operator=(const std::vector<T>& v){ *(std::vector<T>*)this = v; return *this; }
};


class FITypeDescDb;
///	�^��\��
class SPR_DLL FITypeDesc:public UTRefCount{
public:
	///	���R�[�h�̃t�B�[���h��\��
	class SPR_DLL Field{
	public:
		typedef std::vector<std::pair<std::string, int> > Enums;
		Enums enums;
		Field(): length(1), bReference(false), bVector(false){}
		~Field();
		///	�����o��
		std::string name;
		///	�^��
		std::string typeName;
		///	�I�t�Z�b�g
		int offset;
		///	�T�C�Y
		size_t size;
		///	�^
		UTRef<FITypeDesc> type;
		///	�z��̗v�f���D
		int length;
		///	�v�f����ʂ̃t�B�[���h����Ƃ�ꍇ�̃t�B�[���h��
		std::string lengthFieldName;
		///	vector���ǂ���
		int bVector;
		///	�Q�Ƃ��ǂ���
		bool bReference;
		///	�f�[�^�̃T�C�Y
		size_t Size();
		///
		void Print(std::ostream& os) const;
		///
		void AddEnumConst(std::string name, int val);
		void AddEnumConst(std::string name);
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

	friend class FITypeDescDb;
public:
	///	�R���X�g���N�^
	FITypeDesc():size(0){}
	///	�R���X�g���N�^
	FITypeDesc(std::string tn, int sz=0): typeName(tn), size(sz){}
	///	
	~FITypeDesc(){}
	///
	void Print(std::ostream& os) const;
	///	�^��
	std::string GetTypeName() const { return typeName; }
	///	�^��
	std::string SetTypeName(const char* s) { typeName = s; }
	///	�^�̃T�C�Y
	size_t Size() { return size; }
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

	///	���l�ǂݏo��
	virtual double ReadNumber(const unsigned char* ptr){ assert(0); return 0;}
	///	���l��������
	virtual void WriteNumber(unsigned char* ptr, double val){ assert(0);}
	///	������ǂݏo��
	virtual std::string ReadString(const unsigned char* ptr){ assert(0);  return 0;}
	///	�����񏑂�����
	virtual void WriteString(const unsigned char* ptr, const char* val){ assert(0); }
};
inline bool operator < (const FITypeDesc& d1, const FITypeDesc& d2){
	return d1.GetTypeName().compare(d2.GetTypeName()) < 0;
}

template <class N>
class SPR_DLL FITypeDescNumber:public FITypeDesc{
public:
	FITypeDescNumber(){}
	FITypeDescNumber(std::string tn): FITypeDesc(tn, sizeof(N)){}
	///	���l�ǂݏo��
	virtual double ReadNumber(const unsigned char* ptr){
		return *(const N*)ptr;
	}
	///	���l��������
	virtual void WriteNumber(unsigned char* ptr, double val){
		*(N*)ptr = (N)val;
	}
};

///	�^�̃f�[�^�x�[�X
class SPR_DLL FITypeDescDb: public UTRefCount{
public:
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

///	FIVVector�̃T�C�Y�w��Dx �ɂ̓t�B�[���h��(�����o�ϐ���)���w�肷��D
#define VSIZE(x)

///	�P���^��o�^����D
#define REG_FIELD(type)	RegisterDesc( new FITypeDesc(#type, sizeof(type)) )
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
