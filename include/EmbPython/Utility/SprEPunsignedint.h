#ifndef SPREPUNSIGNEDINT_H
#define SPREPUNSIGNEDINT_H

#include <Python/Python.h>
#include "SprEPErrors.h"




/********************************EPunsignedint*************************************/
#define EPunsignedint_Check(ob) (((PyObject*)ob)->ob_type == &EPunsignedintType)
///////////////////////////////////�I�u�W�F�N�g�̐錾////////////
typedef struct
{
	PyObject_HEAD
	///�����ɃN���X�̕ϐ�������
	///�S��public?
	unsigned int *ptr;
} EPunsignedint;

/////////////////////////////////////���\�b�h�o�^�p
///void�̃��\�b�h�̒�`
PyObject* EPunsignedint_new(PyTypeObject *type,PyObject *args, PyObject *kwds);
EPunsignedint* newEPunsignedint();
EPunsignedint* newEPunsignedint(unsigned int);
EPunsignedint* newEPunsignedint(unsigned int*);

//////////////////////////////////////�I�u�W�F�N�g�̃^�C�v�錾
extern PyTypeObject EPunsignedintType;
//////////////////////////////////////�I�u�W�F�N�g�i���W���[���́j�������֐�
PyMODINIT_FUNC initEPunsignedint(void) ;

#endif