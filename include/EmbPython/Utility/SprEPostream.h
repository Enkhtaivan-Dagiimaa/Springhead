#ifndef SPREPOSTREAM_H
#define SPREPOSTREAM_H

#include <Python/Python.h>
#include <Springhead.h>
#include <iosfwd>
#include <iostream>
#include <fstream>
#include "SprEPErrors.h"

using namespace Spr;



/********************************EPostream*************************************/
#define EPostream_Check(ob) (((PyObject*)ob)->ob_type == &EPostreamType)
///////////////////////////////////�I�u�W�F�N�g�̐錾////////////
typedef struct
{
	PyObject_HEAD
	///�����ɃN���X�̕ϐ�������
	///�S��public?
	std::ostream *ptr;
} EPostream;

/////////////////////////////////////���\�b�h�o�^�p
///void�̃��\�b�h�̒�`

PyObject* EPostream_new(PyTypeObject *type,PyObject *args, PyObject *kwds);
EPostream* newEPostream();
EPostream* newEPostream(std::ostream*);

//////////////////////////////////////�I�u�W�F�N�g�̃^�C�v�錾
extern PyTypeObject EPostreamType;
//////////////////////////////////////�I�u�W�F�N�g�i���W���[���́j�������֐�
PyMODINIT_FUNC initEPostream(void) ;


#endif