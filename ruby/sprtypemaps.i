// typemaps

// �X�J���[�ϐ��̃|�C���^���邢�͎Q��
%define OUTPUT_SCALER_TYPEMAP(type)
%typemap(in, numinputs=0) type*($*1_ltype temp), type&($*1_ltype temp) "$1 = &temp;";
%typemap(argout) type*, type& {
	rb_ary_push($result, rb_float_new(*$1));
}
%enddef

OUTPUT_SCALER_TYPEMAP(int);
OUTPUT_SCALER_TYPEMAP(float);
OUTPUT_SCALER_TYPEMAP(double);

#undef OUTPUT_SCALER_TYPEMAP

// �x�N�g���ϐ��̑��l�Cconst�|�C���^���邢��const�Q��
%define INPUT_VECTOR_TYPEMAP(type, size)
%typemap(typecheck) type %{
	Check_Type($input, T_ARRAY);
	$1 = RARRAY($input)->len == size ? 1 : 0;
%}
%typemap(in) type %{
	for(int i = 0; i < size; i++)
		$1[i] = NUM2DBL(rb_ary_entry($input, i));
%}
%typemap(in) const type&($*1_ltype temp), const type*($*1_ltype temp)%{
	for(int i = 0; i < size; i++)
		temp[i] = NUM2DBL(rb_ary_entry($input, i));
	$1 = &temp;
%}
%typemap(argout) const type&, const type* ""	// const������typemap���K�p����Ȃ����߂̃_�~�[
%enddef

INPUT_VECTOR_TYPEMAP(Vec3f, 3);
INPUT_VECTOR_TYPEMAP(Vec3d, 3);
INPUT_VECTOR_TYPEMAP(Quaterniond, 4);
INPUT_VECTOR_TYPEMAP(Posed, 7);

#undef INPUT_VECTOR_TYPEMAP

// �x�N�g���ϐ��̃|�C���^���邢�͎Q��
%define OUTPUT_VECTOR_TYPEMAP(type, size)
%typemap(in, numinputs=0) type*($*1_ltype temp), type&($*1_ltype temp) "$1 = &temp;";
%typemap(argout) type*, type& {
	VALUE v = rb_ary_new2(size);
	for(int i = 0; i < size; i++)
		rb_ary_store(v, i, rb_float_new((*$1)[i]));
	rb_ary_push($result, v);
}
%enddef

OUTPUT_VECTOR_TYPEMAP(Vec3f, 3);
OUTPUT_VECTOR_TYPEMAP(Vec3d, 3);
OUTPUT_VECTOR_TYPEMAP(Quaterniond, 4);
OUTPUT_VECTOR_TYPEMAP(Posed, 7);

#undef OUTPUT_VECTOR_TYPEMAP

// �߂�l�Ńx�N�g���^�̑��l
%define RETURN_VECTOR_TYPEMAP(type, size)
%typemap(out) type %{
	$result = rb_ary_new2(size);
	for(int i = 0; i < size; i++)
		rb_ary_store($result, i, rb_float_new($1[i]));
%}
%enddef

RETURN_VECTOR_TYPEMAP(Vec3f, 3);
RETURN_VECTOR_TYPEMAP(Vec3d, 3);
RETURN_VECTOR_TYPEMAP(Quaterniond, 4);
RETURN_VECTOR_TYPEMAP(Posed, 7);

#undef RETURN_VECTOR_TYPEMAP

//GetSolids�̖߂�l��PHSolid**
//setcontactmode�̈�����PHSolid**
//Vec3d�Ȃǂ̃����o�ϐ���get/set
