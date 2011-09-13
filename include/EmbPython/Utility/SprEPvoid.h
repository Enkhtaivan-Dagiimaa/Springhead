#ifndef SPREPVOID_H
#define SPREPVOID_H

#include <Python/Python.h>
#include <Springhead.h>
#include "SprEPObject.h"

using namespace Spr;




/********************************EPvoid*************************************/
#define EPvoid_Check(ob) (((PyObject*)ob)->ob_type == &EPvoidType)
///////////////////////////////////�I�u�W�F�N�g�̐錾////////////
typedef struct
{
	PyObject_HEAD
	///�����ɃN���X�̕ϐ�������
	///�S��public?
	const void *ptr;
	MemoryManager mm;
} EPvoid;

/////////////////////////////////////���\�b�h�o�^�p
///void�̃��\�b�h�̒�`

PyObject* EPvoid_new(PyTypeObject *type,PyObject *args, PyObject *kwds);
PyObject* newEPvoid();
PyObject* newEPvoid(const void*);

//////////////////////////////////////�I�u�W�F�N�g�̃^�C�v�錾
extern PyTypeObject EPvoidType;
//////////////////////////////////////�I�u�W�F�N�g�i���W���[���́j�������֐�
PyMODINIT_FUNC initEPvoid(void) ;

#endif