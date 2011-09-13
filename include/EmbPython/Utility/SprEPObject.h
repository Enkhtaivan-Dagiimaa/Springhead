#ifndef SPREPOBJECT_H
#define SPREPOBJECT_H

#include <Python/Python.h>
#include <Springhead.h>

using namespace Spr;



enum MemoryManager{
	EP_MM_SPR = 0,
	EP_MM_PY
};



/********************************EPObject*************************************/
#define EPObject_Check(ob) PyObject_TypeCheck(ob,&EPObjectType)
#define EPObject_Ptr(ob) (((EPObject*)ob)->ptr)
#define EPObject_Cast(ob,dest) ((dest*)((EPObject*)ob)->ptr)
///////////////////////////////////�I�u�W�F�N�g�̐錾////////////
typedef struct
{
	PyObject_HEAD
	const void *ptr;
	MemoryManager mm;
	
} EPObject;

/////////////////////////////////////���\�b�h�o�^�p
///void�̃��\�b�h�̒�`

PyObject* __PYDECL EPObject_new(PyTypeObject *type,PyObject *args, PyObject *kwds);
PyObject* newEPObject();
PyObject* newEPObject(const void*);

//////////////////////////////////////�I�u�W�F�N�g�̃^�C�v�錾
extern PyTypeObject EPObjectType;
//////////////////////////////////////�I�u�W�F�N�g�i���W���[���́j�������֐�
PyMODINIT_FUNC initEPObject(void) ;

#endif