#ifndef SPREPSTRING_H
#define SPREPSTRING_H

#include <Python/Python.h>
#include <Springhead.h>
#include <string>
#include <fstream>
#include "SprEPErrors.h"

using namespace Spr;



/********************************EPstring*************************************/
#define EPstring_Check(ob) (((PyObject*)ob)->ob_type == &EPstringType)
///////////////////////////////////�I�u�W�F�N�g�̐錾////////////
typedef struct
{
	PyObject_HEAD
	///�����ɃN���X�̕ϐ�������
	///�S��public?
	std::string *ptr;
} EPstring;

/////////////////////////////////////���\�b�h�o�^�p
///void�̃��\�b�h�̒�`

PyObject* EPstring_new(PyTypeObject *type,PyObject *args, PyObject *kwds);
EPstring* newEPstring();
EPstring* newEPstring(std::string);
EPstring* newEPstring(std::string*);

//////////////////////////////////////�I�u�W�F�N�g�̃^�C�v�錾
extern PyTypeObject EPstringType;
//////////////////////////////////////�I�u�W�F�N�g�i���W���[���́j�������֐�
PyMODINIT_FUNC initEPstring(void) ;

#endif