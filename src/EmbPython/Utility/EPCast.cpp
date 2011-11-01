#include "..\..\..\include\EmbPython\Utility\SprEPCast.h"
using namespace std;
#include <locale>
#include "EmbPython/SprEPBase.h"

double PyObject_asDouble(PyObject* obj)
{
	if(PyFloat_Check(obj))
		return PyFloat_AsDouble(obj);
	else if (PyLong_Check(obj))
		return PyLong_AsDouble(obj);
	else 
		assert(0);
	
	return 0;
}

long PyObject_asLong(PyObject* obj)
{
	if(PyFloat_Check(obj))
		return (long)PyFloat_AsDouble(obj);
	else if (PyLong_Check(obj))
		return PyLong_AsLong(obj);
	else 
		assert(0);

	return 0;
}

#define PyObject_as3(N,S,T)									\
N##S##T PyObject_as##N##S##T##(PyObject* obj){				\
if( obj->ob_type == &EP##N##S##dType)						\
	return ((##N##S##T##)*EPObject_Cast(obj,N##S##d));		\
	if( obj->ob_type == &EP##N##S##fType)					\
	return ((##N##S##T##)*EPObject_Cast(obj,N##S##f));		\
	DSTR << "CastError at PyObject_as3(" #N #S #T ")\n";	\
assert(0);													\
return N##S##T##();											\
}

PyObject_as3(Vec,2,d)
PyObject_as3(Vec,2,f)
PyObject_as3(Vec,3,d)
PyObject_as3(Vec,3,f)
PyObject_as3(Vec,4,d)
PyObject_as3(Vec,4,f)


PyObject_as3(Matrix,2,d)
PyObject_as3(Matrix,2,f)
PyObject_as3(Matrix,3,d)
PyObject_as3(Matrix,3,f)


PyObject_as3(Affine,2,d)
PyObject_as3(Affine,2,f)

//Matrix3d PyObject_asMatrix3d(PyObject* obj)
//{
//	if( obj->ob_type == &EPMatrix3dType )
//		return *EPObject_Cast(obj,Matrix3d);
//	if( obj->ob_type == &EPMatrix3fType )
//		return (Matrix3d)*EPObject_Cast(obj,Matrix3f);
//	
//	DSTR << "PyObject_asMatrix3d�ɂ����ăL���X�g������ɍs���܂���ł���";
//	assert(0);
//	return Matrix3d();
//}

#define PyObject_as2(N,T)									\
N##T PyObject_as##N##T##(PyObject* obj){					\
	if( obj->ob_type == &EP##N##dType )						\
	return (##N##T##)*EPObject_Cast(obj,##N##d);			\
	if( obj->ob_type == &EP##N##fType )						\
	return (##N##T##)*EPObject_Cast(obj,##N##f);			\
	DSTR << "CastError at PyObject_as2(" #N #T ")\n";		\
	assert(0);												\
	return N##T##();									\
}

PyObject_as2(Affine,d)
PyObject_as2(Affine,f)
PyObject_as2(Quaternion,d)
PyObject_as2(Quaternion,f)
PyObject_as2(Pose,d)
PyObject_as2(Pose,f)

//char* PyObject_asChar(PyObject* obj)
//{
//	PyObject* tmp = ((*((PyTupleObject*)obj)).ob_item[0]);
//	bool b = PyUnicode_Check(tmp);
//
//	//���C�h������(WCHAR*)���}���`�o�C�g������(char*)�ɕϊ�
//	//errno_t wcstombs_s(
//	//   size_t *pReturnValue,	//�ϊ����ꂽ������
//	//   char *mbstr,		//�ϊ����ʂ̃}���`�o�C�g������p�̃o�b�t�@�̃A�h���X(�ϊ���)
//	//   size_t sizeInBytes,	//mbstr �o�b�t�@�̃T�C�Y 
//	//   const wchar_t *wcstr,	//�ϊ�����郏�C�h������̃A�h���X(�ϊ���)
//	//   size_t count 		//wcstr �Ɋi�[���郏�C�h�����̍ő吔
//	//);
//	char buff[256];
//	wcstombs(buff,((const wchar_t *)((PyUnicodeObject *)(tmp))->str),sizeof(buff));
//
//	//basic_string<wchar_t> test(
//	//PyUnicode_Check(op)),
//	EPObject_Cast(self,CRBodyControllerIf)->SetTargetPos(buff,(*EPObject_Cast((PyTuple_GetItem(arg,1)),Vec3d)));
//	Py_RETURN_NONE;
//	Return_ArgError;
//}

ConvertStr::ConvertStr( PyObject *obj )
{
	buff = NULL;

	if(PyUnicode_Check(obj))
	{
		Py_ssize_t size = PyObject_Length( obj );
		buff = new char[size+1];
		setlocale( LC_CTYPE, "japanese" );
		int ret = wcstombs(buff,(const wchar_t*)PyUnicode_AS_DATA(obj),size);
		if (ret == -1) assert(0);//�ϊ��G���[ 
		buff[ret] = '\0';
	}
}

ConvertStr::~ConvertStr( )
{
	delete [] buff;
}

string ConvertStr::GetString()
{
	return string(buff);
}

const char* ConvertStr::GetBuffer()
{
	return buff;
}


EPCastfuncMap EPObject_CastMap;

void EPObject_RegistCastfunc(const IfInfo* ifinfo,EPCastfunc func)
{
	EPObject_CastMap.insert(pair<const IfInfo*,EPCastfunc>(ifinfo,func));
}

void __PYDECL EPObject_RuntimeDCast(EPObject* obj,const IfInfo* ifinfo)
{
	EPCastfuncMap::iterator it = EPObject_CastMap.find(ifinfo);
	if(it != EPObject_CastMap.end()) 
		it->second(obj);
}

void __PYDECL EPObject_RuntimeDCast(EPObject* obj,const char* name)
{
	for ( EPCastfuncMap::iterator it = EPObject_CastMap.begin(); it != EPObject_CastMap.end() ; it++)
	{
		if(strcmp(it->first->className,name) == 0 ) 
		{
			it->second(obj);
			break;
		}
	}
}