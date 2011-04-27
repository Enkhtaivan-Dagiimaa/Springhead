#ifndef SPREPQUATERNION_H
#define SPREPQUATERNION_H

#include <Python/Python.h>
#include <Springhead.h>
#include "SprEPErrors.h"
using namespace Spr;



/****************************************************Quaterniond***********************************************************************/
///�}�N��
#define EPQuaterniond_Check(ob) (((PyObject*)ob)->ob_type == &EPQuaterniondType)
#define EPQuaterniond_ArgParseTuple( arg ) double wi,xi,yi,zi,pxi,pyi,pzi; PyArg_ParseTuple( arg , "dddd",&wi,&xi,&yi,&zi )
#define EPQuaterniond_BuildValue(self) Py_BuildValue("dddd",self->ptr->W(),self->ptr->X(),self->ptr->Y(),self->ptr->Z())

//Python�̐V�����^�@PyObject�����

///////////////////////////////////�I�u�W�F�N�g�̐錾////////////
typedef struct
{
	PyObject_HEAD
	///�����ɃN���X�̕ϐ�������
	///�S��public?
	Quaterniond *ptr;
} EPQuaterniond;


/////////////////////////////////////���\�b�h�o�^�p
///Quaterniond�̃��\�b�h�̒�`



EPQuaterniond* newEPQuaterniond();
EPQuaterniond* newEPQuaterniond(Quaterniond);
EPQuaterniond* newEPQuaterniond(Quaterniond*);

//////////////////////////////////////�I�u�W�F�N�g�̃^�C�v�錾
extern PyTypeObject EPQuaterniondType;
//////////////////////////////////////�I�u�W�F�N�g�i���W���[���́j�������֐�
PyMODINIT_FUNC initEPQuaterniond(void) ;

/****************************************************Quaternionf***********************************************************************/
///�}�N��
#define EPQuaternionf_Check(ob) (((PyObject*)ob)->ob_type == &EPQuaternionfType)
#define EPQuaternionf_ArgParseTuple( arg ) float wi,xi,yi,zi,pxi,pyi,pzi; PyArg_ParseTuple( arg , "ffff",&wi,&xi,&yi,&zi )
#define EPQuaternionf_BuildValue(self) Py_BuildValue("ffff",self->ptr->W(),self->ptr->X(),self->ptr->Y(),self->ptr->Z())

//Python�̐V�����^�@PyObject�����

///////////////////////////////////�I�u�W�F�N�g�̐錾////////////
typedef struct
{
	PyObject_HEAD
	///�����ɃN���X�̕ϐ�������
	///�S��public?
	Quaternionf *ptr;
} EPQuaternionf;


/////////////////////////////////////���\�b�h�o�^�p
///Quaternionf�̃��\�b�h�̒�`



EPQuaternionf* newEPQuaternionf();
EPQuaternionf* newEPQuaternionf(Quaternionf);
EPQuaternionf* newEPQuaternionf(Quaternionf*);

//////////////////////////////////////�I�u�W�F�N�g�̃^�C�v�錾
extern PyTypeObject EPQuaternionfType;
//////////////////////////////////////�I�u�W�F�N�g�i���W���[���́j�������֐�
PyMODINIT_FUNC initEPQuaternionf(void) ;



PyMODINIT_FUNC initEPQuaternion(void) ;

#endif