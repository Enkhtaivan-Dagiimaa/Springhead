//
// �֐��C�\���̃w�b�_�t�@�C��
//
// �ŏI�X�V���F2006/7/2
//

#include <math.h>

#define PI		3.1415926538

// �_�C�A���O�쐬�⎯�ʂ̂��߂̍\����
typedef struct{
	bool	flag;
	HWND	hDlg;
} DLG_INFO;

// �e�r�̑�����W�p�\����
typedef struct{
	double x;
	double y;
	double z;
} D3POINT;

// �e�r�̊֐ߊp�x�p�\����
typedef struct{
	double theta1;
	double theta2;
	double theta3;
} JointAngle;

// ���W�A����x�ɕϊ�
double Rad2Deg(double rad){

	return rad * 180.0 / PI;

}
// �x�����W�A���ɕϊ�
double Deg2Rad(double deg){

	return deg * PI / 180.0;

}

// ������W�����1���Z�o
double CalcTheta1(double x, double y){

	//return Rad2Deg( atan(y / x) );
	return atan(y / x);
}

// ������W�����2���Z�o
double CalcTheta2(double x, double y, double z, double L1, double L2, double L3){

	double	m = sqrt(x*x + y*y) - L1;

	//double	theta2 = atan(z / m) - acos( (L2*L2 + m*m + z*z - L3*L3) / (2 * L2 * sqrt(m*m + z*z)) );
	double	theta2 = atan(z / m) + acos( (L2*L2 + m*m + z*z - L3*L3) / (2 * L2 * sqrt(m*m + z*z)) );

	//return	Rad2Deg( theta2 );
	return	theta2;

}

// ������W�����3���Z�o
double CalcTheta3(double x, double y, double z, double L1, double L2, double L3){

	double	m = sqrt(x*x + y*y) - L1;

	//double	theta3 = PI - acos( (L2*L2 + L3*L3 - m*m - z*z) / (2 * L2 * L3) );
	double	theta3 = - PI + acos( (L2*L2 + L3*L3 - m*m - z*z) / (2 * L2 * L3) );

	//return	Rad2Deg( theta3 );
	return	theta3;

}