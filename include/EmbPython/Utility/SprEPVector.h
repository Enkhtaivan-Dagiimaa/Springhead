#ifndef SPREPVECTOR_H
#define SPREPVECTOR_H

#include <Python/Python.h>
#include <Springhead.h>
#include "SprEPErrors.h"
using namespace Spr;


/**********************************Vec3d***********************/
///�}�N��
#define EPVec3d_Check(ob) (((PyObject*)ob)->ob_type == &EPVec3dType)

//Python�̐V�����^�@PyObject�����

///////////////////////////////////�I�u�W�F�N�g�̐錾////////////
typedef struct
{
	PyObject_HEAD
	///�����ɃN���X�̕ϐ�������
	///�S��public?
	Vec3d *ptr;
} EPVec3d;


/////////////////////////////////////���\�b�h�o�^�p
///Vec3d�̃��\�b�h�̒�`

PyObject* EPVec3d_new(PyTypeObject *type,PyObject *args, PyObject *kwds);
EPVec3d* newEPVec3d();
EPVec3d* newEPVec3d(Vec3d);
EPVec3d* newEPVec3d(Vec3d*);

//////////////////////////////////////�I�u�W�F�N�g�̃^�C�v�錾
extern PyTypeObject EPVec3dType;
//////////////////////////////////////�I�u�W�F�N�g�i���W���[���́j�������֐�
PyMODINIT_FUNC initEPVec3d(void) ;

/**********************************Vec2d***********************/
///�}�N��
#define EPVec2d_Check(ob) (((PyObject*)ob)->ob_type == &EPVec2dType)

//Python�̐V�����^�@PyObject�����

///////////////////////////////////�I�u�W�F�N�g�̐錾////////////
typedef struct
{
	PyObject_HEAD
	///�����ɃN���X�̕ϐ�������
	///�S��public?
	Vec2d *ptr;
} EPVec2d;


/////////////////////////////////////���\�b�h�o�^�p
///Vec2d�̃��\�b�h�̒�`

PyObject* EPVec2d_new(PyTypeObject *type,PyObject *args, PyObject *kwds);
EPVec2d* newEPVec2d();
EPVec2d* newEPVec2d(Vec2d);
EPVec2d* newEPVec2d(Vec2d*);

//////////////////////////////////////�I�u�W�F�N�g�̃^�C�v�錾
extern PyTypeObject EPVec2dType;
//////////////////////////////////////�I�u�W�F�N�g�i���W���[���́j�������֐�
PyMODINIT_FUNC initEPVec2d(void) ;

/**********************************Vec3f***********************/
///�}�N��
#define EPVec3f_Check(ob) (((PyObject*)ob)->ob_type == &EPVec3fType)

//Python�̐V�����^�@PyObject�����

///////////////////////////////////�I�u�W�F�N�g�̐錾////////////
typedef struct
{
	PyObject_HEAD
	///�����ɃN���X�̕ϐ�������
	///�S��public?
	Vec3f *ptr;
} EPVec3f;


/////////////////////////////////////���\�b�h�o�^�p
///Vec3f�̃��\�b�h�̒�`

PyObject* EPVec3f_new(PyTypeObject *type,PyObject *args, PyObject *kwds);
EPVec3f* newEPVec3f();
EPVec3f* newEPVec3f(Vec3f);
EPVec3f* newEPVec3f(Vec3f*);

//////////////////////////////////////�I�u�W�F�N�g�̃^�C�v�錾
extern PyTypeObject EPVec3fType;
//////////////////////////////////////�I�u�W�F�N�g�i���W���[���́j�������֐�
PyMODINIT_FUNC initEPVec3f(void) ;

/**********************************Vec2f***********************/
///�}�N��
#define EPVec2f_Check(ob) (((PyObject*)ob)->ob_type == &EPVec2fType)

//Python�̐V�����^�@PyObject�����

///////////////////////////////////�I�u�W�F�N�g�̐錾////////////
typedef struct
{
	PyObject_HEAD
	///�����ɃN���X�̕ϐ�������
	///�S��public?
	Vec2f *ptr;
} EPVec2f;


/////////////////////////////////////���\�b�h�o�^�p
///Vec2f�̃��\�b�h�̒�`

PyObject* EPVec2f_new(PyTypeObject *type,PyObject *args, PyObject *kwds);
EPVec2f* newEPVec2f();
EPVec2f* newEPVec2f(Vec2f);
EPVec2f* newEPVec2f(Vec2f*);


//////////////////////////////////////�I�u�W�F�N�g�̃^�C�v�錾
extern PyTypeObject EPVec2fType;
//////////////////////////////////////�I�u�W�F�N�g�i���W���[���́j�������֐�
PyMODINIT_FUNC initEPVec2f(void) ;

/**********************************Vec4f***********************/
///�}�N��
#define EPVec4f_Check(ob) (((PyObject*)ob)->ob_type == &EPVec4fType)

//Python�̐V�����^�@PyObject�����

///////////////////////////////////�I�u�W�F�N�g�̐錾////////////
typedef struct
{
	PyObject_HEAD
	///�����ɃN���X�̕ϐ�������
	///�S��public?
	Vec4f *ptr;
} EPVec4f;


/////////////////////////////////////���\�b�h�o�^�p
///Vec4f�̃��\�b�h�̒�`

PyObject* EPVec4f_new(PyTypeObject *type,PyObject *args, PyObject *kwds);
EPVec4f* newEPVec4f();
EPVec4f* newEPVec4f(Vec4f);
EPVec4f* newEPVec4f(Vec4f*);

//////////////////////////////////////�I�u�W�F�N�g�̃^�C�v�錾
extern PyTypeObject EPVec4fType;
//////////////////////////////////////�I�u�W�F�N�g�i���W���[���́j�������֐�
PyMODINIT_FUNC initEPVec4f(void) ;


PyMODINIT_FUNC initEPVector(void);

#endif