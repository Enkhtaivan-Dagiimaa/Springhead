#ifndef SPREPUNSIGNEDLONG_H
#define SPREPUNSIGNEDLONG_H

#include <Python/Python.h>
#include "SprEPErrors.h"




/********************************EPunsignedlong*************************************/
#define EPunsignedlong_Check(ob) (((PyObject*)ob)->ob_type == &EPunsignedlongType)
///////////////////////////////////�I�u�W�F�N�g�̐錾////////////
typedef struct
{
	PyObject_HEAD
	///�����ɃN���X�̕ϐ�������
	///�S��public?
	unsigned long *ptr;
} EPunsignedlong;

/////////////////////////////////////���\�b�h�o�^�p
///void�̃��\�b�h�̒�`
PyObject* EPunsignedlong_new(PyTypeObject *type,PyObject *args, PyObject *kwds);
EPunsignedlong* newEPunsignedlong();
EPunsignedlong* newEPunsignedlong(unsigned long);
EPunsignedlong* newEPunsignedlong(unsigned long*);

//////////////////////////////////////�I�u�W�F�N�g�̃^�C�v�錾
extern PyTypeObject EPunsignedlongType;
//////////////////////////////////////�I�u�W�F�N�g�i���W���[���́j�������֐�
PyMODINIT_FUNC initEPunsignedlong(void) ;

#endif