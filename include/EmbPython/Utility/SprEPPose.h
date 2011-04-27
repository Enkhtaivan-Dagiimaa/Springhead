#ifndef SPREPPOSE_H
#define SPREPPOSE_H

#include <Python/Python.h>
#include <Springhead.h>
#include "SprEPErrors.h"
using namespace Spr;



///�}�N��
#define EPPosed_Check(ob) (((PyObject*)ob)->ob_type == &EPPosedType)
#define EPPosed_ArgParseTuple( arg ) double wi,xi,yi,zi,pxi,pyi,pzi; PyArg_ParseTuple( arg , "ddddddd",&wi,&xi,&yi,&zi,&pxi,&pyi,&pzi )
#define EPPosed_BuildValue(self) Py_BuildValue("ddddddd",self->ptr->W(),self->ptr->X(),self->ptr->Y(),self->ptr->Z(),self->ptr->Px(),self->ptr->Py(),self->ptr->Pz())

//Python�̐V�����^�@PyObject�����

///////////////////////////////////�I�u�W�F�N�g�̐錾////////////
typedef struct
{
	PyObject_HEAD
	///�����ɃN���X�̕ϐ�������
	///�S��public?
	Posed *ptr;
} EPPosed;


/////////////////////////////////////���\�b�h�o�^�p
///Posed�̃��\�b�h�̒�`


EPPosed* newEPPosed();
EPPosed* newEPPosed(Posed);
EPPosed* newEPPosed(Posed*);

//////////////////////////////////////�I�u�W�F�N�g�̃^�C�v�錾
extern PyTypeObject EPPosedType;
//////////////////////////////////////�I�u�W�F�N�g�i���W���[���́j�������֐�
PyMODINIT_FUNC initEPPosed(void) ;

PyMODINIT_FUNC initEPPose(void) ;

#endif