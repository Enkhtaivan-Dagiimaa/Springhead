#ifndef __THERMALCONDACTION_H_INCLUDED__
#define __THERMALCONDACTION_H_INCLUDED__

#include "Tetrahedron.h"
#include "MatrixCalc.h"
#include <iostream>
#include <vector>
#include "tetgen.h"


struct ElemAreas	//�v�f�̊e�ʂ̖ʐϒl�̊i�[
{
	REAL facet1;
	REAL facet2;
	REAL facet3;
	REAL facet4;
};

struct OptionalPoint{
	REAL x;
	REAL y;
	REAL z;
	REAL Tp;
	REAL N1;
	REAL N2;
	REAL N3;
	REAL N4;
	
	int node1;
	int node2;
	int node3;
	int node4;
};
struct Belong{
	std::vector<int> element;
};

class ThermalFEM
{
private:
	//int elemNum;						//�v�f����ۑ�
	//int nodeNum;						//�ߓ_����ۑ�
	//int *elems;							//�v�f�̍\����ۑ�����
	//REAL *nodeCoordinates;			//�ߓ_���W��ۑ�����
	REAL *mat;						//�S�̂̃}�g���b�N�X��ۑ�����Ƃ���
	REAL *K1;							//�M�`���}�g���b�N�X�̑�ꍀ(�M�`���Ɋւ��鍀)
	REAL *K2;							//�M�`���}�g���b�N�X�̑��(�M�`�B���Ɋւ��鍀)
	REAL *K3;							//�M�`���}�g���b�N�X�̑�O��(�t�˂Ɋւ��鍀)
	REAL *CapaMat;					//�M�e�ʃ}�g���b�N�X
	REAL *CGlobal;					//�M�e�ʃ}�g���b�N�X�~dt

	REAL *FluxMat;					//�M�����}�g���b�N�X
	REAL *F1;							//�M�����̎��̂����������M�Ɋւ��鍀��ۑ�
	REAL *F2;							//�M�����̎��̂����M����q0�Ɋւ��鍀��ۑ�
	REAL *F3;							//�M�����̎��̂����M�`�B���Ǝ��͗��̉��x�Ɋւ��鍀��ۑ�
	REAL *F4;							//�M�����̎��̂����t�˂Ɋւ��鍀��ۑ�

	REAL conductivity;				//�M�`����
	REAL *Q;							//�������M��
	REAL density;						//���x
	REAL c;							//��M
	REAL dt;							//���ԍ��ݕ�
	REAL q0;							//�M����
	REAL h;							//�M�t�˗�
	REAL alpha;						//�M�`�B��

	REAL Tfluid;						//���͗��̉��x
	REAL Tout;						//���͊����x
	int qdirection;						//
	

	Tetrahedron tet;
	MatrixCalc mc;

	bool usedRadiantHeat;
	bool usedGenerationOfHeat;
	bool usedElem;
	bool usedNode;

	REAL eta0ds(int node1,int node2,int node3,int node4);
	REAL eta0ds(int nodes[]);

	void CreateK1Local(int elemNumber, REAL ktemp[]);		//K1�̃}�g���b�N�X���쐬����BelemNumber:�v�f�ԍ�, ktemp:����16�̔z��

	void CreateK2Local(int node1, int node2, int node3, int node4, REAL ktemp[]);	//k2�̋Ǐ����W�n�ł̍s����쐬����B
																					//ktemp:����16�̔z��
	void CreateK3Local(int node1, int node2, int node3, int node4, REAL ktemp[]); //k3�̋Ǐ����W�n�ł̍s����쐬����B
																					//ktemp:����16�̔z��
	void CreateCapacityLocal(int elemNumber,REAL ktemp[]);						//�Ǐ����W�n�ł̔M�e�ʃ}�g���b�N�X
																					//ktemp:����16�̔z��
	void CreateQVecLocal(int elemNumber, REAL temp[],int n[]);					//�Ǐ����W�n�ł̓������M�̃x�N�g�����쐬����
	void CreateHeatFluxVecLocalDS(int nodeNums[],REAL temp[]);					//�Ǐ����W�n�ł�
	void checkSurface(int elemNumber,int node1,int node2,int node3,int n[]);		//�ǂ̖ʂ���M�����������Ă��邩�m�F
	int  checkSurface(int elemNumber, int node1, int node2, int node3);				//�߂�lk=0,1,2,3��(0,1,2),(0,1,3),(0,2,3),(1,2,3)�̂ǂ̐ߓ_�̑g�ݍ��킹��������

	void GetNodeNumbers(int elemNumber,int n[]);									//�v�f���\������ߓ_�̔ԍ����擾����
	void sortInt(int n[],int num);

	//�C�ӂ̓_�̒l���擾���鎞�Ɏg��
	Belong *belong;		//�������Ă���v�f��ۑ�����B
	OptionalPoint *optional;	//�C�ӂ̓_�̒l���Ƃ�ۂɗp����
	bool usedOptional;			//���̂�����̊֐����g�������ǂ���
	int totalOP;				//�C�ӓ_�̑���
	bool InOrOut(REAL target[],REAL v1[],REAL v2[],REAL v3[],REAL v4[]);			//�v�f���b�V���̒��ɓ����Ă���̂��ǂ���
	void CreateShapeFunction(int elemNumber, REAL a[],REAL b[],REAL c[],REAL d[]);	//�`��֐��쐬
	
	
	//�ʐόv�Z
	void initializeElemAreas();
	bool usedBorderCode;
	bool usedElemAreas;

	//�d�l�ύX�ɂ��ǉ��ɂȂ�������
	void SetInit_relateNode(int totalNode);

	//20090515�C��
	bool useTETGEN;

public:
	ThermalFEM();					//�R���X�g���N�^
	~ThermalFEM();					//�f�X�g���N�^

	

	REAL *temperature;					//���x��ۑ�����

	//�g�����ǂ����̔���p
	bool useRadiantHeat;					//�t�˂̍����g�����ǂ����̐ݒ�Btrue:�g��, false:�g��Ȃ�
	bool useGenerationOfHeat;					//�������M���g�����ǂ����̐ݒ�Btrue:�g��, false:�g��Ȃ�

	//�����̐ݒ�֌W
	void SetTotalNodes(int totalNode);									//���ߓ_����ۑ�����
	void SetTotalElems(int totalElem);									//���v�f����ۑ�����
	void SetElemAndNodeNum(int totalElem,int TotalNode);				//�v�f���Ɛߓ_����ۑ�����
	void SetInitialTemperature(REAL temp[]);							//�����̉��x��ݒ肷��
	void SetConductivity(REAL conduct);								//�M�`������ݒ肷��
	void SetQ(REAL *qi);												//�������M����ݒ肷��
	void SetDensity(REAL dens);										//���x��ݒ肷��
	void SetSpecificTemp(REAL st);									//��M��ݒ肷��
	void SetDT(REAL dt0);												//���ݕ���ݒ肷��
	void SetHeatflux(REAL flux);										//�M�����̒l��ݒ肷��B
	void SetHeatflux(REAL flux,int direction);						//�M�����̒l��ݒ肷��Bdirection:-1 �O�����͑Ώۓ����֌���������,+1:��͑Ώۓ�������O�֌���������
	void SetTransRatio(REAL ratio);									//�M�`�B���̒l��ݒ肷��
	void SetRadiantHeat(REAL radiantHeat);							//�M�t�˗����t�˂̂���v�f�̐ݒ�
	void SetTfluid(REAL tfluid);										//���͗��̉��x�̐ݒ�
	void SetTout(REAL tout);											//���͊����x�̐ݒ�
	void SetTemperature(REAL temp,int nodeNumber);					//nodeNumber�Ԃ̐ߓ_�̉��x��ݒ肷��
	void SetHeatfluxDirection(int direction);							//�����̕�����ݒ肷�� direction:-1 �O�����͑Ώۓ����֌���������,+1:��͑Ώۓ�������O�֌���������

	//�W���p
	void showGlobalMatrix();						//�S�̃}�g���b�N�X��\������
	void showTemperature();				//���x�x�N�g����\������
	void showK1();
	void showK2();
	void showK3();
	void showCapacityMat();
	void showF1();
	void showF2();
	void showF3();
	void showF4();
	void showGlobalFluxVector();

	void initializeK1();
	void initializeK2();												//�M�`���}�g���b�N�X��2��(�M�`�B���Ɋւ��鍀)������������
	void CreateK2(int elemNumber, int node1, int node2, int node3);		//�M�`���}�g���b�N�X��2��(�M�`�B���Ɋւ��鍀)���쐬����
	void initializeK3();												//�M�`���}�g���b�N�X��3��(�t�˂Ɋւ��鍀)������������
	void CreateK3(int elemNumber, int node1, int node2, int node3);		//�M�`���}�g���b�N�X��3��(�t�˂Ɋւ��鍀)���쐬����
	void CreateK1();												//�M�`���}�g���b�N�X��1��(�M�`�����Ɋւ��鍀)���쐬����
	void CreateCapacityMat();										//�S�̍��W�n�ł̔M�e�ʃ}�g���b�N�X���쐬����
	void initializeCapacityMat();
	void CreateHeatFluxVecNo1();										//�M�����x�N�g���̎���1�����쐬����
	void CreateHeatFluxVecNo1(int elem);								//�M�����x�N�g���̎���1�����쐬����
	void CreateHeatFluxVecNo1(int elem, REAL genHeat);				//�M�����x�N�g���̎���1�����쐬����. 
	void CreateHeatFluxVecNo2(int elem,int node1,int node2,int node3);	//�M�����x�N�g���̎���2�����쐬����
	void CreateHeatFluxVecNo2(int elem,int node1,int node2,int node3,REAL qelem);	//�M�����x�N�g���̎���2�����쐬����
	void CreateHeatFluxVecNo3(int elem,int node1,int node2,int node3);	//�M�����x�N�g���̎���3�����쐬����
	void CreateHeatFluxVecNo3(int elem,int node1,int node2,int node3,REAL tfruid);	//�M�����x�N�g���̎���3�����쐬����
	void CreateHeatFluxVecNo4(int elem,int node1,int node2,int node3);	//�M�����x�N�g���̎���4�����쐬����
	void CreateHeatFluxVecNo4(int elem,int node1,int node2,int node3,REAL tout);	//�M�����x�N�g���̎���4�����쐬����
	void initializeFluxVecNo1();
	void initializeFluxVecNo2();
	void initializeFluxVecNo3();
	void initializeFluxVecNo4();

	void CreateGlobalMatrixAndVec();									//�S�̍s��ƑS�̃x�N�g�����쐬����
	void CalcStartGaussJordan();										//�K�E�X�W�����_���@�Ōv�Z����
	void CalcStartGaussJordanUsePivot();								//�s�{�b�g�t���K�E�X�W�����_���@�Ōv�Z����
	void CalcStartLUSolve();											//LU���������Čv�Z����
	void FixTemperature(REAL temp, int nodeNumber);					//���x���Œ肵�Čv�Z����

	//���E����
	void detectBorder(int s[]);		//�v�f���̒����̃|�C���^���i�[
	void detectBorder();

	//�l��Ԃ�
	void GetGlobalMatrix(REAL ret[]);
	void GetK1Matrix(REAL ret[]);
	void GetK2Matrix(REAL ret[]);
	void GetK3Matrix(REAL ret[]);
	void GetCapacityMatrix(REAL ret[]);
	void GetGlobalFluxVector(REAL ret[]);
	void GetHeatFlux1Vector(REAL ret[]);
	void GetHeatFlux2Vector(REAL ret[]);
	void GetHeatFlux3Vector(REAL ret[]);
	void GetHeatFlux4Vector(REAL ret[]);
	void GetTemperature(REAL ret[]);
	void GetAnElementMember(int elementNumber,int n[]);
	REAL GetElemVolume(int elemNumber);				//elemnumber�͂P�ȏ�.�ړ_�̐��Ɨv�f�̍\���A�ړ_�̍��W�Ɨv�f�̍\���̐ݒ肪�I�������Ɏg�p�ł���B�S�̗̂���̇A�Ԃ܂ŏI�������.

	//�ߓ_���W�A�v�f�\���̊i�[
	void SetANodeCordinates(int nodeNumber, REAL x,REAL y,REAL z);
	void SetAnElementMember(int elementNumber,int node1,int node2,int node3, int node4);
	void SetTetrahedronlistStartWithZero(int tetrahedronlist[]);		//���ׂĂ̗v�f�\����ۑ�����
	void SetTetrahedronlist(int tetrahedronlist[]);						//���ׂĂ̗v�f�\����ۑ�����
	void SetPointlist(REAL pointlist[]);								//���ׂĂ̐ߓ_���W��ۑ�����
	
	//���̑��̋@�\
	void GetExteriorProduct(int elementNumber,int node1,int node2,int node3, REAL ret[]);//�v�f���̂���ʂ̊O�ς����߂�
	void GetElementFace(int elemNumber,int faceNumber,int n[]);//�v�f��4�̖ʂ�faceNumber�Ԗڂ̖ʂ̍\����Ԃ��BfaceNumber��1�`4�Bn[]�͒���3�̔z��B

	//�C�ӂ̓_�̒l���Ƃ邽�߂̊֐��B	//TetGen()�ō�������b�V�����_����
	int SerchNearestPoint(REAL x,REAL y,REAL z);
	void PrepareforGetOptionalPoint(int totalofOptionalPoint);						//1)
	void SetOptionalPointCoordinates(int number,REAL x,REAL y,REAL z);		//2)�C�ӂ̓_�̍��W���w�肷��֐��Q�R�̂����ǂ�ł��ǂ�.number�͂O�ȏ�ŁA�ꑱ���̔ԍ�
	void SetOptionalPointCoordinates(int number,REAL xyz[]);						//[]
	void SetOptionalPointCoordinates(REAL AllCoodinates[]);						//[]
	void SerchAnOptionalPointBelong(int optionalPointNumber);						//
	void CreateOptionalPointParameters(int elemNumber,int optionalPointNumber);		//
	void SerchAllOptionalPointBelong();												//
	REAL GetOptionalPointTemperature(int OptionalPointNumber);					//OptionalPointNumber:����̔C�ӂ̓_���P�����ɓ����
	void GetOptionalPointTemperature(REAL optionalPointTemperatures[]);			//
	
	//�ʐόv�Z
	int *borderCode;//�ʂ̏���ۑ�
	ElemAreas *elemFaceAreas;	//������
	void calcElemFaceArea(int elemNumber, int code);	//�O�k�̓���̗v�f�����v�Z�@elemNumber 1�ȏ�̗v�f�ԍ�	//code 0x(4���̐���) �O���̋��E�ʂ̖ʐς��v�Z
	void calcElemFaceArea(int code[]);					//�O�k�̑S���̗v�f�Ōv�Z
	void calcElemFaceArea();							//�S�v�f�̑S�ʂ��v�Z

	tetgenio out,in;
	void TFEMTetrahedralize(char *switches);
	void TFEMTetrahedralize(char *switches,tetgenio in);
	void TFEMTetrahedralize(char *switches,tetgenio in,tetgenio out);
	
};


#endif

