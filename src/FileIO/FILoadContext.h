#ifndef FILoadContext_H
#define FILoadContext_H

#include <Foundation/Object.h>
#include <Foundation/UTTypeDesc.h>
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
class FILoadContext{
public:
	//--------------------------------------------------------------------------
	//	�N���X��`
	///
	struct FileInfo{
		~FileInfo();
		std::string name;	///<	�t�@�C����
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
		FileInfo():start(NULL), end(NULL){}
		/// �t�@�C�� �}�b�s���O		
		bool Map(std::string fn);
		/// �t�@�C�� �A���}�b�s���O
		void Unmap();
		///	���[�h�ł����ԂȂ�true
		bool IsGood();
	};
	///	�^�X�N�N���X�D���[�h��ɂ܂Ƃ߂Ďd���������邽�߂̂��́D
	class Task:public UTRefCount{
	public:
		virtual ~Task(){}
		virtual void Execute(FILoadContext* ctx)=0;	
	};
	///	�^�X�N���X�g
	class Tasks:public std::vector< UTRef<Task> >{
	public:
		void Execute(FILoadContext* ctx);
	};
	///	�m�[�h�ւ̎Q�Ƃ��L�^���Ă����N���X�D�S�����[�h�ł��Ă��烊���N����D
	class LinkTask: public Task{
	public:
		std::vector<NameManagerIf*> nameManagers;
		std::string ref;
		ObjectIf* object;
		const char* pos;
		LinkTask(const ObjectIfs& objs, const char* p, ObjectIf* o, std::string r);
		void Execute(FILoadContext* ctx);
	};
	
	//--------------------------------------------------------------------------
	//	�ϐ�
	///	���[�h���̃t�@�C���̖��O�ƒ��g�D�t�@�C��include�ɔ�����stack�ɂȂ��Ă���D
	UTStack<FileInfo> fileInfo;	
	///	���݃��[�h���̃I�u�W�F�N�g�D�l�X�g�����I�u�W�F�N�g�ɔ����ăX�^�b�N�ɂȂ��Ă���D
	ObjectIfs objects;
	///	�X�^�b�N�ɍŏ��ɋl�܂ꂽ�I�u�W�F�N�g���t�@�C���̈�ԊO�������[�g�̃I�u�W�F�N�g�̋L�^�D
	ObjectIfs rootObjects;
	///	���[�h�����f�B�X�N���v�^�̃X�^�b�N�D�l�X�g�����g�ݗ��Č^�ɔ����ăX�^�b�N�ɂȂ��Ă���D
	UTStack< UTRef<FINodeData> > datas;
	///	���[�h����FITypedesc�̃t�B�[���h�̈ʒu�D�g�ݗ��Č^�̃t�B�[���h�ɔ����ăX�^�b�N�ɂȂ��Ă���D
	FIFieldIts fieldIts;
	///	�G���[���b�Z�[�W�o�͗p�̃X�g���[�� cout �Ƃ� DSTR ���w�肷��D
	std::ostream* errorStream;
	///	���t�@�����X����Ń����N���邽�߂̋L�^�D
	Tasks links;
	///	���[�h�ƃ����N���I����Ă��珈������^�X�N
	Tasks postTasks;
	///	�^DB
	UTTypeDescDb* typeDb;
	///	�m�[�h�n���h��
	FINodeHandlers* handlers;

	//---------------------------------------------------------------------------
	//	�֐�
	///
	FILoadContext():errorStream(NULL){
		errorStream=&DSTR;
	}
	/**	�m�[�h�̃��[�h�D
		���[�h����Desc����I�u�W�F�N�g���쐬����D
		�쐬��->AddChildObject(�V�I�u�W�F�N�g)���Ăяo���D
		�I�u�W�F�N�g�̍쐬�́C�e�I�u�W�F�N�g��CreateObject()�C�e�̐e��CreateObject()��
		��c��CreateObject()�����ɌĂ�ŁC�쐬�ł����Ƃ���Ŏ~�߂�D
		�ǂ̃I�u�W�F�N�g���쐬�ł��Ȃ��ꍇ�́CCreateSdk()���Ăяo���D

		�쐬�҂Ɛe���قȂ�ꍇ�C�e��AddChildObject()���Ăяo���D	*/
	void LoadNode();
	///	�m�[�h�̃��[�h�̏I��
	void EndNode();
	///	�u���b�N(�g�ݗ��Č^)�ɓ���
	void EnterBlock();
	///	�u���b�N(�g�ݗ��Č^)����o��
	void LeaveBlock();
	///	�G���[���b�Z�[�W�̏o�́Dpos���t�@�C�����ƍs�ԍ��ɕϊ�����D
	void ErrorMessage(const char* pos, const char* msg);
	///	���b�Z�[�W�̍쐬�Dpos���t�@�C�����ƍs�ԍ��ɕϊ�����D
	void Message(const char* pos, const char* msg);
	///	���[�h�ł����ԂȂ�true
	bool IsGood();
	///	type���������鏀��������(type���Z�b�g���CXXDesc��p�ӂ���)
	void PushType(UTTypeDesc* type);
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
	void Link();
	///
	void PostTask();
};


}

#endif	// FILoadContext_H
