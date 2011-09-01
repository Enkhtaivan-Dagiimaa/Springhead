#include "..\..\..\include\EmbPython\Utility\SprEPCast.h"
using namespace std;
#include <locale>
#include "EmbPython/SprEPUtility.h"

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

//�œK����off�ɂ��Ȃ��ƃL���X�g������Ȃ�
#pragma optimize("",off)
Vec3d* PyObject_asVec3d(PyObject* obj)
{
	if( obj->ob_type == &EPVec3dType)
		return EPObject_Cast(obj,Vec3d);
	if( obj->ob_type == &EPVec3fType)
		return &((Vec3d)*EPObject_Cast(obj,Vec3f));
	if( obj->ob_type == &EPVec3fType)
		return &((Vec3d)*EPObject_Cast(obj,Vec3i));

	DSTR << "PyObject_asVec3d�ɂ����ăL���X�g������ɍs���܂���ł���";
	assert(0);
}

Vec3f* PyObject_asVec3f(PyObject* obj)
{
	if( obj->ob_type == &EPVec3fType)
		return EPObject_Cast(obj,Vec3f);
	if( obj->ob_type == &EPVec3dType)
		return &((Vec3f)*EPObject_Cast(obj,Vec3d));
	if( obj->ob_type == &EPVec3fType)
		return &((Vec3f)*EPObject_Cast(obj,Vec3i));

	DSTR << "PyObject_asVec3f�ɂ����ăL���X�g������ɍs���܂���ł���";
	assert(0);
}
#pragma optimize("",on)

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
		//setlocale( LC_CTYPE, "jpn" );
		int ret = wcstombs(buff,(const wchar_t*)PyUnicode_AS_DATA(obj),size);
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

void EPObject_RuntimeDCast(EPObject* obj,const IfInfo* ifinfo)
{
	EPCastfuncMap::iterator it = EPObject_CastMap.find(ifinfo);
	if(it != EPObject_CastMap.end()) 
		it->second(obj);
}