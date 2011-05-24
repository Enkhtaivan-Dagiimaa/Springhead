#ifndef __TETRAHEDRON_H_INCLUDED__
#define __TETRAHEDRON_H_INCLUDED__

/************************************

�l�ʑ̗v�f�֌W

*************************************/
#include <iostream>
#include "MatrixCalc.h"

class Tetrahedron
{
public:
	//REAL LocalRoundN[12];	//�Ǐ����W�n�ɂ�����N����,��,�ĂŕΔ����������̂��i�[���Ă���B
							//0~3:��N/�݃�, 4~7:��N/�݃�,8~11��N/�݃�,

	Tetrahedron();
	REAL LocalDetJ(REAL x1,REAL x2,REAL x3, REAL x4,
				REAL y1,REAL y2,REAL y3, REAL y4,
				REAL z1,REAL z2,REAL z3, REAL z4);					//���R�r�A��J�̍s�񎮂̌��ʂ�Ԃ�

	void LocalJ(REAL x1,REAL x2,REAL x3, REAL x4,
				REAL y1,REAL y2,REAL y3, REAL y4,
				REAL z1,REAL z2,REAL z3, REAL z4,
				REAL matJ[]);												//���R�r�A����Ԃ�

	void LocalInvJMultiplyDetJ(REAL x1,REAL x2,REAL x3, REAL x4,
				REAL y1,REAL y2,REAL y3, REAL y4,
				REAL z1,REAL z2,REAL z3, REAL z4,
				REAL invj[]);												//���R�r�A��J�̋t�s���det|J|�����������̂�Ԃ�

	void LocalInvJ(REAL x1,REAL x2,REAL x3, REAL x4,
				REAL y1,REAL y2,REAL y3, REAL y4,
				REAL z1,REAL z2,REAL z3, REAL z4,
				REAL invj[]);												//���R�r�A���̋t�s���Ԃ�

	REAL CalcVolume(REAL x1,REAL x2,REAL x3, REAL x4,
					REAL y1,REAL y2,REAL y3, REAL y4,
					REAL z1,REAL z2,REAL z3, REAL z4);			//�l�ʑ̂̑̐ς�Ԃ�

	void retlocalRoundN(REAL N[]);
	
};



#endif