#ifndef SPREPMATRIX_H
#define SPREPMATRIX_H

#include <Python/Python.h>
#include <Springhead.h>
#include "SprEPErrors.h"

using namespace Spr;



/*********************************************Matrix3d**************************/
///�}�N��
#define EPMatrix3d_Check(ob) (((PyObject*)ob)->ob_type == &EPMatrix3dType)

#define asMatrix3d(ob) (*(((EPMatrix3d*)ob)->ptr))

//debug

//Python�̐V�����^�@PyObject�����

///////////////////////////////////�I�u�W�F�N�g�̐錾////////////
typedef struct
{
	PyObject_HEAD
	///�����ɃN���X�̕ϐ�������
	///�S��public?
	Matrix3d *ptr;
} EPMatrix3d;


/////////////////////////////////////���\�b�h�o�^�p
///Matrix3d�̃��\�b�h�̒�`

PyObject* EPMatrix3d_new(PyTypeObject *type,PyObject *args, PyObject *kwds);
EPMatrix3d* newEPMatrix3d();
EPMatrix3d* newEPMatrix3d(Matrix3d);
EPMatrix3d* newEPMatrix3d(Matrix3d*);

//////////////////////////////////////�I�u�W�F�N�g�̃^�C�v�錾
extern PyTypeObject EPMatrix3dType;
//////////////////////////////////////�I�u�W�F�N�g�i���W���[���́j�������֐�
PyMODINIT_FUNC initEPMatrix3d(void) ;



/*********************************************Matrix3f**************************/
///�}�N��
#define EPMatrix3f_Check(ob) (((PyObject*)ob)->ob_type == &EPMatrix3fType)

#define asMatrix3f(ob) (*(((EPMatrix3f*)ob)->ptr))

//debug

//Python�̐V�����^�@PyObject�����

///////////////////////////////////�I�u�W�F�N�g�̐錾////////////
typedef struct
{
	PyObject_HEAD
	///�����ɃN���X�̕ϐ�������
	///�S��public?
	Matrix3f *ptr;
} EPMatrix3f;


/////////////////////////////////////���\�b�h�o�^�p
///Matrix3f�̃��\�b�h�̒�`

PyObject* EPMatrix3f_new(PyTypeObject *type,PyObject *args, PyObject *kwds);
EPMatrix3f* newEPMatrix3f();
EPMatrix3f* newEPMatrix3f(Matrix3f);
EPMatrix3f* newEPMatrix3f(Matrix3f*);

//////////////////////////////////////�I�u�W�F�N�g�̃^�C�v�錾
extern PyTypeObject EPMatrix3fType;
//////////////////////////////////////�I�u�W�F�N�g�i���W���[���́j�������֐�
PyMODINIT_FUNC initEPMatrix3f(void) ;



PyMODINIT_FUNC initEPMatrix(void) ;

#endif