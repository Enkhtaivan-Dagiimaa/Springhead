#ifndef SPREPISTREAM_H
#define SPREPISTREAM_H

#include <Python/Python.h>
#include <iostream>
#include "SprEPErrors.h"



/********************************EPistream*************************************/
#define EPistream_Check(ob) (((PyObject*)ob)->ob_type == &EPistreamType)
///////////////////////////////////�I�u�W�F�N�g�̐錾////////////
typedef struct
{
	PyObject_HEAD
	///�����ɃN���X�̕ϐ�������
	///�S��public?
	std::istream *ptr;
} EPistream;

/////////////////////////////////////���\�b�h�o�^�p
///void�̃��\�b�h�̒�`

PyObject* EPistream_new(PyTypeObject *type,PyObject *args, PyObject *kwds);
EPistream* newEPistream();
EPistream* newEPistream(std::istream*);

//////////////////////////////////////�I�u�W�F�N�g�̃^�C�v�錾
extern PyTypeObject EPistreamType;
//////////////////////////////////////�I�u�W�F�N�g�i���W���[���́j�������֐�
PyMODINIT_FUNC initEPistream(void) ;


#endif