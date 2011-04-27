#ifndef SPREPUTSTRING_H
#define SPREPUTSTRING_H

#include <Python/Python.h>
#include <Springhead.h>
#include <iosfwd>
#include <iostream>
#include <fstream>
#include "SprEPErrors.h"

using namespace Spr;



/********************************EPUTString*************************************/
#define EPUTString_Check(ob) (((PyObject*)ob)->ob_type == &EPUTStringType)
///////////////////////////////////�I�u�W�F�N�g�̐錾////////////
typedef struct
{
	PyObject_HEAD
	///�����ɃN���X�̕ϐ�������
	///�S��public?
	UTString *ptr;
} EPUTString;

/////////////////////////////////////���\�b�h�o�^�p
///void�̃��\�b�h�̒�`

PyObject* EPUTString_new(PyTypeObject *type,PyObject *args, PyObject *kwds);
EPUTString* newEPUTString();
EPUTString* newEPUTString(UTString);
EPUTString* newEPUTString(UTString*);

//////////////////////////////////////�I�u�W�F�N�g�̃^�C�v�錾
extern PyTypeObject EPUTStringType;
//////////////////////////////////////�I�u�W�F�N�g�i���W���[���́j�������֐�
PyMODINIT_FUNC initEPUTString(void) ;

#endif