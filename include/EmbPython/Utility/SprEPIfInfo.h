#ifndef SPREPIFINFO_H
#define SPREPIFINFO_H

#include <Python/Python.h>
#include <Springhead.h>
#include "SprEPErrors.h"

using namespace Spr;



/********************************EPIfInfo*************************************/
#define EPIfInfo_Check(ob) (((PyObject*)ob)->ob_type == &EPIfInfoType)
///////////////////////////////////�I�u�W�F�N�g�̐錾////////////
typedef struct
{
	PyObject_HEAD
	///�����ɃN���X�̕ϐ�������
	///�S��public?
	IfInfo *ptr;
} EPIfInfo;

/////////////////////////////////////���\�b�h�o�^�p
///void�̃��\�b�h�̒�`

PyObject* EPIfInfo_new(PyTypeObject *type,PyObject *args, PyObject *kwds);
EPIfInfo* newEPIfInfo();
EPIfInfo* newEPIfInfo(IfInfo*);
//EPIfInfo* newEPIfInfo(IfInfo); ���ۃN���X�̓C���X�^���X���ł��Ȃ�

//////////////////////////////////////�I�u�W�F�N�g�̃^�C�v�錾
extern PyTypeObject EPIfInfoType;
//////////////////////////////////////�I�u�W�F�N�g�i���W���[���́j�������֐�
PyMODINIT_FUNC initEPIfInfo(void) ;


#endif