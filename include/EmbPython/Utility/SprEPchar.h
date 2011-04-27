#ifndef SPREPCHAR_H
#define SPREPCHAR_H

#include <Python/Python.h>
#include "SprEPErrors.h"




/********************************EPchar*************************************/
#define EPchar_Check(ob) (((PyObject*)ob)->ob_type == &EPcharType)
///////////////////////////////////�I�u�W�F�N�g�̐錾////////////
typedef struct
{
	PyObject_HEAD
	///�����ɃN���X�̕ϐ�������
	///�S��public?
	char *ptr;
} EPchar;

/////////////////////////////////////���\�b�h�o�^�p
///void�̃��\�b�h�̒�`

PyObject* EPchar_new(PyTypeObject *type,PyObject *args, PyObject *kwds);
EPchar* newEPchar();
EPchar* newEPchar(const char*);

//////////////////////////////////////�I�u�W�F�N�g�̃^�C�v�錾
extern PyTypeObject EPcharType;
//////////////////////////////////////�I�u�W�F�N�g�i���W���[���́j�������֐�
PyMODINIT_FUNC initEPchar(void) ;


#endif