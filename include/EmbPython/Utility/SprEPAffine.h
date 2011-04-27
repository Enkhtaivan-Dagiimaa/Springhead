#ifndef SPREPAFFINE_H
#define SPREPAFFINE_H

#include <Python/Python.h>
#include <Springhead.h>
#include "SprEPErrors.h"
using namespace Spr;




/*********************************************Affinef**************************/
///�}�N��
#define EPAffinef_Check(ob) (((PyObject*)ob)->ob_type == &EPAffinefType)

#define asAffinef(ob) (*(((EPAffinef*)ob)->ptr))

//debug

//Python�̐V�����^�@PyObject�����

///////////////////////////////////�I�u�W�F�N�g�̐錾////////////
typedef struct
{
	PyObject_HEAD
	///�����ɃN���X�̕ϐ�������
	///�S��public?
	Affinef *ptr;
} EPAffinef;


/////////////////////////////////////���\�b�h�o�^�p
///Affinef�̃��\�b�h�̒�`

PyObject* EPAffinef_new(PyTypeObject *type,PyObject *args, PyObject *kwds);
EPAffinef* newEPAffinef();
EPAffinef* newEPAffinef(Affinef);
EPAffinef* newEPAffinef(Affinef*);

//////////////////////////////////////�I�u�W�F�N�g�̃^�C�v�錾
extern PyTypeObject EPAffinefType;
//////////////////////////////////////�I�u�W�F�N�g�i���W���[���́j�������֐�
PyMODINIT_FUNC initEPAffinef(void) ;


PyMODINIT_FUNC initEPAffine(void);
#endif