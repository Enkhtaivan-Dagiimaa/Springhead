// EPObject�Ɋւ���Swig�}�N���̒�`

//�|�C���^�ł͂Ȃ����̂��󂯎��R���X�g���N�^��ǉ��i�f�t�H���g�ł̓|�C���^�݂̂��󂯎��
%define EXTEND_NEW(type)
%header%{
	PyObject* __EPDECL newEP##type##Object(type var1);
%}
%wrapper%{
	PyObject* newEP##type##Object(type var1)
	{
		PyObject *ret = EP##type##Object_new(&EP##type##Type,NULL,NULL);
		EPObject_Ptr(ret) = new type(var1);
		((EPObject*)ret)->mm = EP_MM_PY;
		return ret;
	}
%}
%enddef
