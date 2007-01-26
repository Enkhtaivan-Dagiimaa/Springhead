// typemaps

// $(RUBY_INCLUDE)/win32/win32.h, windows.h�̃}�N����`�Ƃ̏Փˉ��
%{
#undef write			// std
#undef read				// std
#undef CreateWindow		// FWAppGLUT
%}

// �^�L���X�g��Warning�𖳌���
%{
#ifdef _MSC_VER
# pragma warning(disable: 4311 4312)
#endif	
%}

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
INPUT_VECTOR_TYPEMAP(Vec2f, 2);
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

// UTString
%typemap(in) UTString %{
	$1 = UTString(STR2CSTR($input));
%}

// ������̔z��char*[]
%typemap(in) char*[] (std::vector<char*> temp)%{
	cout << RARRAY_LEN($input);
	temp.resize(RARRAY_LEN($input));
	for(int i = 0; i < RARRAY_LEN($input); i++)
		temp[i] = STR2CSTR(rb_ary_entry($input, i));
	$1 = &temp[0];
%}

// PHSceneIf::GetSolids�̖߂�l��PHSolid**
%typemap(out) Spr::PHSolidIf ** %{
	$result = rb_ary_new2(arg1->NSolids());
	for(int i = 0; i < arg1->NSolids(); i++)
		rb_ary_store($result, i, SWIG_NewPointerObj(SWIG_as_voidptr($1[i]), SWIGTYPE_p_Spr__PHSolidIf, 0 |  0 ));
%}

// GRFrameIf::GetChildren�̖߂�l��GRVisualIf**
%typemap(out) Spr::GRVisualIf ** %{
	$result = rb_ary_new2(arg1->NChildren());
	for(int i = 0; i < arg1->NChildren(); i++)
		rb_ary_store($result, i, SWIG_NewPointerObj(SWIG_as_voidptr($1[i]), SWIGTYPE_p_Spr__GRVisualIf, 0 |  0 ));
%}

//setcontactmode�̈�����PHSolid**
//Vec3d�Ȃǂ̃����o�ϐ���get/set
