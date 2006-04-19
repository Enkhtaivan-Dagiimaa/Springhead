/** \page pageMoreTutorial	�`���[�g���A���i�ڍוҁj

<br>

\section contentsMoreTutorial	�� Contents ��
	&nbsp;&nbsp;&nbsp; \ref moreTutUTRef <br>
	&nbsp;&nbsp;&nbsp; \ref moreTutVec2 <br>
	&nbsp;&nbsp;&nbsp; \ref moreTutVec3 <br>
	&nbsp;&nbsp;&nbsp; \ref moreTutVec4 <br>
	&nbsp;&nbsp;&nbsp; \ref moreTutOri <br>
	&nbsp;&nbsp;&nbsp; \ref moreTutPose <br>
	&nbsp;&nbsp;&nbsp; \ref moreTutPose <br>
	&nbsp;&nbsp;&nbsp; \ref moreTutForce <br>
<br><br>
<hr>


<!-------------------- 1�D�@UTRef<T> �Q�ƃJ�E���^�ƎQ�ƃ|�C���^ -------------------->

\subsection moreTutUTRef	1�D�@UTRef<T> �Q�ƃJ�E���^�ƎQ�ƃ|�C���^
���������[�N�΍�Ƃ��āA�Q�ƃJ�E���^�������N���X�ƃJ�E���^���Ǘ�����X�}�[�g�|�C���^�̃e���v���[�g���������Ă��܂��B<br>
<a href="../../src/html/classSpr_1_1UTRef.html">UTRef<�N���X��></a>���Q�ƃ|�C���^�A
<a href="../../src/html/classSpr_1_1UTRefCount.html">UTRefCount</a>���Q�ƃJ�E���^�ƂȂ�܂��B<br>
<a href="../../src/html/classSpr_1_1UTRefCount.html">UTRefCount</a>����{�N���X�Ɏ��N���XA������ꍇ�C���̂悤�Ɏg�����Ƃ��ł��܂��B<br>
for���[�v�ɂ�10��̃������m�ۂ�new�ōs���Adelete�ł̃������J�����s���Ă��܂��񂪁A���������[�N�͔������܂���B
\verbatim	
    UTRef<A> p;
    for(int i=0; i10; ++i){
        p = new A;
        p->A�̃��\�b�h()
    }
\endverbatim
<br><br><br>
<hr>

<!-------------------- 2�D�@2�����x�N�g�� ------------------------------------------>
\subsection moreTutVec2		2�D�@2�����x�N�g��
2�̒P���xfloat�^�f�[�^�����x�N�g���^��Vec2f�^�A2�̔{���xdouble�^�f�[�^�����x�N�g���^��Vec2d�^�ƂȂ�܂��B
\verbatim	
    Vec2f vec(a, b);                            // a�Ab �ŏ�����
    Vec2f vec = Vec2f(a, b);                    // a�Ab �ŏ�����
    vec[0] = a;                                 // vec.x = a;
    vec[1] = b;                                 // vec.y = b;
    Vec2f vec2= vec + Vec2f(x, y);              // ���Z
\endverbatim
<br><br><br>
<hr>

<!-------------------- 3�D�@3�����x�N�g�� ------------------------------------------>
\subsection moreTutVec3		3�D�@3�����x�N�g��
3�̒P���xfloat�^�f�[�^�����x�N�g���^��Vec3f�^�A3�̔{���xdouble�^�f�[�^�����x�N�g���^��Vec3d�^�ƂȂ�܂��B
\verbatim	
    Vec3f vec(a, b, c);                         // a�Ab�Ac �ŏ�����
    Vec3f vec = Vec3f(a, b, c);                 // a�Ab�Ac �ŏ�����
    vec[0] = a;                                 // vec.x = a;
    vec[1] = b;                                 // vec.y = b;
    vec[2] = c;                                 // vec.z = c;
    Vec3f vec2= vec + Vec3f(x, y, z);           // ���Z
\endverbatim
<br><br><br>
<hr>

<!-------------------- 4�D�@4�����x�N�g�� ------------------------------------------>
\subsection moreTutVec4		4�D�@4�����x�N�g��
4�̒P���xfloat�^�f�[�^�����x�N�g���^��Vec4f�^�A4�̔{���xdouble�^�f�[�^�����x�N�g���^��Vec4d�^�ƂȂ�܂��B
\verbatim	
    Vec4f vec(a, b, c);                         // a�Ab�Ac �ŏ�����
    Vec4f vec = Vec4f(a, b, c);                 // a�Ab�Ac �ŏ�����
    vec[0] = a;                                 // vec.x = a;
    vec[1] = b;                                 // vec.y = b;
    vec[2] = c;                                 // vec.z = c;
    Vec4f vec2= vec + Vec4f(x, y, z);           // ���Z
\endverbatim
<br><br><br>
<hr>

<!-------------------- 5�D�@���̂̌��� --------------------------------------------->
\subsection moreTutOri	5�D�@���̂̌���
���͎̂��_�ƈႢ�A�傫���������Ă���̂ŁA���̂̌������l�����Ȃ��Ă͂Ȃ�܂���B<br>
���̂̌����͈ȉ��̂悤�ɐݒ肵�܂��B
\verbatim	
    solid->SetOrientation(Quaternionf::Rot(Rad(30), 'z'));      // z������30�x��]
\endverbatim
�܂��A���̂悤�ɂ��āA���̂̌������擾���邱�Ƃ��ł��܂��B
\verbatim	
    Quaterniond ori = solid->solid1-GetOrientation();           // ���̂̌������擾
\endverbatim
<br><br><br>
<hr>

<!-------------------- 6�D�@���̂̈ʒu --------------------------------------------->
\subsection moreTutPose	6�D�@���̂̈ʒu
�ʏ�A�V�~�����[�V�������s���ɂ́A���̕��̂��ǂ��ɂ��邩�Ƃ�����񂪕K�v�ɂȂ�܂��B
���̏��Ƃ����̂��A���W�ł��B���W�ɂ��A���̂���Ԃ̂ǂ̈ʒu�ɂ��邩���������Ƃ��ł��܂��B
���̂̈ʒu�͈ȉ��̂悤�ɐݒ肵�܂��B
\verbatim	
    solid->SetFramePosition(Vec3f(0,-3,0));                     // ���̂̈ʒu��(0,-3,0)�ɐݒ�
\endverbatim
�܂��A���̂悤�ɂ��āA���̂̈ʒu���擾���邱�Ƃ��ł��܂��B
\verbatim	
    Vec3d framepos = solid->solid1->>GetFramePosition();        // ���̂̈ʒu���擾
\endverbatim
<br><br><br>
<hr>

<!-------------------- 7�D�@���̂̎p�� --------------------------------------------->
\subsection moreTutPose		7�D�@���̂̎p��
���̂̎p���́APosed�^�ŕ\����܂��B <br>
Posed�^�́AAffine�Ɠ����@�\������7�����x�N�g���ŁA�����ƈʒu�����킹���x�N�g���ƂȂ��Ă��܂��B
���̂̎p���͈ȉ��̂悤�ɂ��Đݒ肵�܂��B
\verbatim
    Posed p = Posed::Rot(Rad(30.0), 'z');                       // z������30�x��]������Posed�^p��p�ӂ���
    solid->SetPose(p);                                          // Posed�^p�����̂̎p���Ƃ��Đݒ肷��
\endverbatim
�܂��A���̂悤�ɂ��āA���̂̎p�����擾���邱�Ƃ��ł��܂��B
\verbatim
    Posed pose = soBlock->GetPose();                            // ���̂̎p�����擾
\endverbatim
<br><br><br>
<hr>

<!-------------------- 8�D�@���̂ɗ͂������� --------------------------------------->
\subsection moreTutForce	8�D�@���̂ɗ͂�������
���̂ɗ͂���p����ꍇ���l���A���ʒ��S�ɗ͂�������ꍇ�A�ȉ��̂悤�ɐݒ肷�邱�Ƃ��ł��܂��B
<img src="../../include/docsrc/02Tutorial/force.jpg" width="682" height="146">
\verbatim
    solid->AddForce(Vec3f(10, 0, 0);    // ���ʒ��S�ɉE������10N�̗͂�������
\endverbatim
�܂��A���ʒ��S�ł͂Ȃ��A�ʒu���w�肵�āA�͂������邱�Ƃ��\�ł��B
\verbatim    
    solid->AddForce(f, r);              �@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@            // �͂� �ʒur(World�n) �ɉ�����
    solid->AddForce(Vec3f(10,0,0), Vec3f(0,2,0)+solid1->GetCenterPosition());       // �d�S��2m����E������10N�̗͂�������
\endverbatim
<br><br><br>

<br>
<div align="right">
<a href="#pageTutorial">top of page</a>
</div>

*/
