
#include "..\..\..\include\EmbPython\Utility\SprEPvoid.h"



/////////////////////////////////////���\�b�h�o�^�p
///void�̃��\�b�h�̒�`
static PyMethodDef EPvoidObject_methods[] =
{
	{NULL}
};

/////////////////////////////////////////�����o�ϐ��o�^�p
/*������*/

//////////////////////////////////////////init
static int
EPvoidObject_init(EPvoidObject* self,PyObject *args, PyObject *kwds)
{
	return 0;
}
//////////////////////////////////////////new
static PyObject*
EPvoidObject_new(PyTypeObject *type,PyObject *args, PyObject *kwds)
{
	EPvoidObject *self;
	self = ( EPvoidObject*) type->tp_alloc(type,0);
	if ( self != NULL)
	{
		EPvoidObject_init(self,args,kwds);
	}
	return (PyObject *)self;
}
////////////////////////////////////////////dealloc
static void
EPvoidObject_dealloc(EPvoidObject* self)
{
	//Python�ł���Ȃ��Ă�Springhead�Ŏg���Ă邩������Ȃ�

	if ( self->mm == EP_MM_PY )
		delete(self->ptr);
	self->ob_base.ob_type->tp_free((PyObject*)self);
}

PyTypeObject EPvoidObjectType =
{
	PyVarObject_HEAD_INIT(NULL,0)
	"Test.void",             /*tp_name*/
	sizeof(EPvoidObject),             /*tp_basicsize*/
	0,                         /*tp_itemsize*/
	(destructor)EPvoidObject_dealloc,                        /*tp_dealloc*/
	0,                         /*tp_print*/
	0,                         /*tp_getattr*/
	0,                         /*tp_setattr*/
	0,                         /*tp_compare*/
	0,                         /*tp_repr*/
	0,                         /*tp_as_number*/
	0,                         /*tp_as_sequence*/
	0,                         /*tp_as_mapping*/
	0,                         /*tp_hash */
	0,                         /*tp_call*/
	0,                         /*tp_str*/
	0,                         /*tp_getattro*/
	0,                         /*tp_setattro*/
	0,                         /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE , /*tp_flags*/
	"void",           /* tp_doc */
	0,		               /* tp_traverse */
	0,		               /* tp_clear */
	0,		               /* tp_richcompare */
	0,		               /* tp_weaklistoffset */
	0,		               /* tp_iter */
	0,		               /* tp_iternext */
	EPvoidObject_methods,             /* tp_methods */
	0,//EPvoidObject_members,             /* tp_members */
	0,                         /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)EPvoidObject_init,      /* tp_init */
	0,                         /* tp_alloc */
	EPvoidObject_new,                 /* tp_new */

};

PyMODINIT_FUNC initEPvoidObject(void)
{
	PyObject* m;

	//Python�N���X�̍쐬
	if ( PyType_Ready( &EPvoidObjectType ) < 0 ) return NULL ;

	m = PyImport_AddModule("Utility");

	//���W���[���ɒǉ�
	Py_INCREF(&EPvoidObjectType);
	PyModule_AddObject(m,"void",(PyObject *)&EPvoidObjectType);
	return m;
}

PyObject* newEPvoid()
{
	
	EPvoidObject* obj = (EPvoidObject*)EPvoidObject_new(&EPvoidObjectType,NULL,NULL);
	obj->mm = EP_MM_PY;
	return (PyObject*)obj;
}

PyObject* newEPvoid(const void* ptr)
{
	PyObject* ret = EPvoidObject_new(&EPvoidObjectType,NULL,NULL);
	((EPvoidObject*)ret)->ptr = ptr;
	((EPvoidObject*)ret)->mm = EP_MM_SPR;

	return ret;
}