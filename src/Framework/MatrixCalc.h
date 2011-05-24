#ifndef __MATRIXCALC_H_INCLUDED__
#define __MATRIXCALC_H_INCLUDED__


#include <cmath>
#include "tetgen.h"



class MatrixCalc{
private:
	void swapLines(REAL mat[],int size, int lineNum1, int lineNum2);
public:
	MatrixCalc();
	REAL	determinant(REAL mat[],int size);		//det|mat|�̌��ʂ�Ԃ�
	void VecMultiplyTVec(REAL vec1[], REAL vec2[], int size ,REAL ret[]);	//size*1�̃x�N�g��vec1��1*size�̃x�N�g��vec2�̊|���Z������
	void MatrixPlusMatrix(REAL mat1[], REAL mat2[], int size);	//mat1��mat2�𑫂��Bmat1=mat1+mat2;
	void MatrixMultiplyVec(REAL mat[], REAL vec[], int size, REAL temp[] );//�}�g���b�N�X�ƍs��̊|���Z

	void CopyMatrix(REAL original[],REAL copy[], int size);//�R�s�[�Boriginal�F���{, copy:�R�s�[��
	void CopyVector(REAL original[], REAL copy[], int size);//�R�s�[�Boriginal�F���{, copy:�R�s�[��
	void CopyVector(int original[], int copy[], int size);//�R�s�[�Boriginal�F���{, copy:�R�s�[��
	void SubstructVec2FromVec1(REAL vec1[], REAL vec2[],int size);//�����Z�Bvec1=vec1-vec2
	void AddVec2ToVec1(REAL vec1[] , REAL vec2[] , int size);//�����Z

	void GaussJordan(REAL mat[],REAL vec[],int size);//�s�{�b�g�Ȃ��K�E�X�W�����_���@
	void GaussJordanUsePivot(REAL mat[],REAL vec[],int size);//�s�{�b�g����K�E�X�W�����_���@
	void LUsolve(REAL mat[], REAL vec[],REAL ret[],int size);//LU������p�����v�Z

	void ExteriorProduct(REAL v1[],REAL v2[],REAL ret[]);//�O�ρBv1�~v2
	void ExteriorProduct(REAL xa, REAL ya, REAL za, REAL xb, REAL yb,REAL zb,REAL ret[]); 
	REAL GetVectorLength(REAL v[]);//�x�N�g���̐�Βl���擾����

	REAL CreateJudgeCoefficient(REAL mat[]);
	REAL CreateJudgeD(REAL a,REAL b,REAL c,REAL xyz[]);
	bool judgeInOout(REAL a, REAL b, REAL c, REAL d,REAL target[]);

	REAL MatrixCalc::createJudgeCoefficient(REAL mat[]);
	REAL MatrixCalc::createJudgeD(REAL a, REAL b, REAL c, REAL xyz[]);

	REAL CalcTriangleAreas(REAL p1[],REAL p2[],REAL p3[]);

};


#endif