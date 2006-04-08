#ifndef FIFILECONTEXT_H
#define FIFILECONTEXT_H

#include <Foundation/Object.h>
#include <FileIO/FITypeDesc.h>
#include <string>
#ifdef _WIN32
#include <WinBasis/WinBasis.h>
#else 
#include <sys/stat.h>
#endif


namespace Spr{;

class FINodeHandlers;
/**	�t�@�C�����[�h���Ɏg�p����R���e�L�X�g
	�t�@�C�������[�h����ۂ́C�f�[�^���m�[�h���ƂɃ��[�h���āC
	�I�u�W�F�N�g����邽�߂̃f�B�X�N���v�^�\����(PHSolidDesc�Ȃ�)��
	�܂����[�h����D
	���̂��ƁC�I�u�W�F�N�g�𐶐�����D	*/
class FIFileContext{
public:
	//--------------------------------------------------------------------------
	//	�N���X��`
	///
	typedef UTStack<ObjectIf*> IfStack;
	///
	struct FileInfo{
		~FileInfo();
		std::string name;	///<	�t�@�C����
		int line;			///<	�s�ԍ�
		const char* start;	///<	�������}�b�v���ꂽ�t�@�C���̐擪
		const char* end;	///<	�������}�b�v���ꂽ�t�@�C���̏I�[
#ifdef _WIN32
		HANDLE hFile, hFileMap;		///<	�t�@�C���n���h���A�t�@�C���}�b�s���O�I�u�W�F�N�g
#else 
		//FILE *hFile;
		//char *buffer;
		int fd;					///<	�t�@�C���f�B�X�N���v�^
		struct stat filestat;	///<	�t�@�C���T�C�Y�𓾂�̂Ɏg��
		void *sourceptr;
#endif
		/// �R���X�g���N�^
		FileInfo():line(0),start(NULL), end(NULL){}
		/// �t�@�C�� �}�b�s���O		
		bool Map(std::string fn);
		/// �t�@�C�� �A���}�b�s���O
		void Unmap();
		///	���[�h�ł����ԂȂ�true
		bool IsGood();
	};
	/**	�t�@�C�����玩���I�ɓǂݏo������C�t�@�C���Ɏ����I�ɏ������񂾂肷��
	�f�[�^�D�������̊Ǘ����s���D	*/
	struct Data: UTRefCount{
		FITypeDesc* type;	///<	�f�[�^�̌^ 
		UTString name;		///<	���O
		void* data;			///<	���[�h�����f�[�^
		bool haveData;		///<	data��delete���ׂ����ǂ����D
		Data(FITypeDesc* t=NULL, void* d=NULL);
		~Data();
	};
	/**	�t�B�[���h�̎�ނ������t���O�D
		�قƂ�ǂ̃t�@�C���t�H�[�}�b�g�ŁC�����C�����C������ŁC�قȂ�p�[�T���K�v�ɂȂ�D
		�����ŁC�����ŕ��ށD
		�g�ݗ��Č^�́CFITypeDesc���Q�Ƃ��ēǂݏo���̂ŁCF_BLOCK��p�ӂ����D
	*/
	enum FieldType{
		F_NONE, F_BOOL, F_INT, F_REAL, F_STR, F_BLOCK
	};
	/**	TypeDesc�̃t�B�[���h�̃C�^���[�^
		�o�C�i���t�@�C����X�t�@�C������C����^�̃f�[�^�����ɓǂݏo���Ă����ꍇ�C
		�ǂݏo�����̃f�[�^��FITypeDesc�̃c���[�̂ǂ��ɑΉ����邩��ێ����Ă����K�v������D
	*/
	struct FieldIt{
		FITypeDesc* type;						///<	�ǂݏo������FITypeDesc
		FITypeDesc::Composit::iterator field;	///<	�g�ݗ��Č^�̏ꍇ�C���̒��̂ǂ̃t�B�[���h��
		int arrayPos;							///<	�z��̏ꍇ�C�ǂݏo�����̓Y����
		int arrayLength;						///<	�Œ蒷�̏ꍇ�̔z��̒���
		FieldType nextField;					///<	�ǂݏo���t�B�[���h�̌^

		FieldIt(FITypeDesc* d);					///<	�R���X�g���N�^
		bool NextField();						///<	���̃t�B�[���h�ɐi��
	};
	class FieldItStack:public UTStack<FieldIt>{
	public:
		///
		void PushType(FITypeDesc* t){
			Push(FIFileContext::FieldIt(t));
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
			return back().nextField==F_BOOL;
		}
		bool IsNumber(){
			return back().nextField==F_INT || back().nextField==F_REAL;
		}
		bool IsString(){
			return back().nextField==F_STR;
		}
	};
	///	�m�[�h�ւ̎Q�Ƃ��L�^���Ă����N���X�D�S�����[�h�ł��Ă��烊���N����D
	struct Link{
		std::vector<NameManagerIf*> nameManagers;
		std::string ref;
		ObjectIf* object;
		const char* pos;
		Link(const IfStack& objs, const char* p, ObjectIf* o, std::string r);
	};
	
	//--------------------------------------------------------------------------
	//	�ϐ�
	///	���[�h���̃t�@�C���̖��O�ƒ��g�D�t�@�C��include�ɔ�����stack�ɂȂ��Ă���D
	UTStack<FileInfo> fileInfo;	
	///	���݃��[�h���̃I�u�W�F�N�g�D�l�X�g�����I�u�W�F�N�g�ɔ����ăX�^�b�N�ɂȂ��Ă���D
	IfStack objects;
	///	���[�h�����f�B�X�N���v�^�̃X�^�b�N�D�l�X�g�����g�ݗ��Č^�ɔ����ăX�^�b�N�ɂȂ��Ă���D
	UTStack< UTRef<Data> > datas;
	///	���[�h����FITypedesc�̃t�B�[���h�̈ʒu�D�g�ݗ��Č^�̃t�B�[���h�ɔ����ăX�^�b�N�ɂȂ��Ă���D
	FieldItStack fieldIts;
	///	�G���[���b�Z�[�W�o�͗p�̃X�g���[�� cout �Ƃ� DSTR ���w�肷��D
	std::ostream* errorStream;
	///	���t�@�����X����Ń����N���邽�߂̋L�^�D
	UTStack<Link> links;
	///	�^DB
	FITypeDescDb* typeDb;
	///	�m�[�h�n���h��
	FINodeHandlers* handlers;

	//---------------------------------------------------------------------------
	//	�֐�
	///
	FIFileContext():errorStream(NULL){
		errorStream=&DSTR;
	}
	///	�m�[�h�̃��[�h
	void LoadNode();
	///	�m�[�h�̃��[�h�̏I��
	void EndNode();
	///	�u���b�N(�g�ݗ��Č^)�ɓ���
	void EnterBlock();
	///	�u���b�N(�g�ݗ��Č^)����o��
	void LeaveBlock();
	///	�I�u�W�F�N�g�̐���
	ObjectIf* Create(const IfInfo* ifInfo, const void* desc);
	///	�G���[���b�Z�[�W�̏o�́Dpos���t�@�C�����ƍs�ԍ��ɕϊ�����D
	void ErrorMessage(const char* pos, const char* msg);
	///	���b�Z�[�W�̍쐬�Dpos���t�@�C�����ƍs�ԍ��ɕϊ�����D
	void Message(const char* pos, const char* msg);
	///	���[�h�ł����ԂȂ�true
	bool IsGood();
	///	type���������鏀��������(type���Z�b�g���CXXDesc��p�ӂ���)
	void PushType(FITypeDesc* type);
	////
	void PopType();
	///
	void WriteString(std::string v);
	///
	void WriteNumber(double d);
	///
	void WriteBool(bool b);
	///
	void AddLink(std::string ref, const char* pos);
	///
	void DoLink();
};


}

#endif	// FIFILECONTEXT_H
