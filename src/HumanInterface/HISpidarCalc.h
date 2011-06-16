#ifndef HI_SPIDARCALC_H
#define HI_SPIDARCALC_H

#include <Base/TQuaternion.h>
#include <Base/QuadProgram.h>

namespace Spr{;

/**	SPIDAR(�����g�����͊o�񎦑��u)�̂��߂̎p���v�Z�Ɨ͊o�v�Z���s����{�N���X�D
	�C���^�t�F�[�X(���z�֐�)�ƁC2���v��@��J��Ԃ��@�ɂ��p���v�Z�̂��߂̍�Ɨ̈��p�ӂ���D
	void Spr::HISpidarCalcBase::MakeWireVec(), 
	void Spr::HISpidarCalcBase::UpdatePos(), 
	void Spr::HISpidarCalcBase::MeasureWire(), 
	��3�̊֐����I�[�o�[���C�h���邱�ƂŌX��SPIDAR�̂��߂̌v�Z���s���悤��
	�ł���D	*/

class SPR_DLL HISpidarCalcBase{
protected:
	/**@name	�p���Ɨ͊o���v�Z���邽�߂̃p�����[�^.
		�p���x�N�g�� = a(���C����) = p(w(���C����))
		a: ���C���� ���� �p���x�N�g�� �ւ̕ϊ��֐��D
		w: ���C���� ���� ���C���ڍ��ʒu�x�N�g�� �ւ̕ϊ��֐��D
		p: ���C���ڍ��ʒu�x�N�g�� ���� �p���x�N�g�� �ւ̕ϊ��֐�

		�p���x�N�g�� = a(���C����) = p(w(���C����)) ��Δ��������
		���p���x�N�g�� = A�E�����C���� = P�EW�E�����C����
	*/
	//@{
	PTM::VVector<float>		measuredLength;		///<	�v���������C����
	PTM::VVector<float>		calculatedLength;	///<	�O�̎p������v�Z����郏�C����
	PTM::VMatrixRow<float>	matA;				///<	�s�� A
	PTM::VMatrixRow<float>	matATA;				///<	A * A�̓]�u
	PTM::VVector<float>		lengthDiff;			///<	�����C���� = �v�����C���� - �Z�o���C����.		lengthDiff[i] is the measured SetLength of this time minus the computed legth of last time  
	PTM::VVector<float>		lengthDiffAve;		///<	�����C�����̕��ϒl
	PTM::VVector<float>		postureDiff;		///<	���߂��p���̕ω��� = A * lengthDiff
	//@}
	VQuadProgram<float>		quadpr;				///<	�Q���v����̌v�Z�N���X
public:
	HISpidarCalcBase(){}					///<	�R���X�g���N�^
	virtual ~HISpidarCalcBase(){}			///<	�f�X�g���N�^
protected:
	virtual void Update()=0;				///<	�p�����̍X�V
	PTM::VVector<float>& Tension() {		///<	����
		return quadpr.vecX;
	}
	/**	������
		@param minF	�ŏ�����
		@param maxF	�ő咣��
		@param dof	�G���h�G�t�F�N�^�̎��R�x	*/
	void Init(int dof, const VVector<float>& minF, const VVector<float>& maxF);
	virtual void MakeWireVec()=0;			///<	���C�������x�N�g���̌v�Z
	virtual void UpdatePos()=0;				///<	���C���ݒu�ʒu���W�̍X�V
	virtual void MeasureWire()=0;			///<	���C�����̌v��
};

/**	�G���h�G�t�F�N�^(�O���b�v)�����i3���R�x����������SPIDAR�p�̌v�Z�N���X�D
	���̐��͉ρD	*/
class HISpidarCalc3Dof: public HISpidarCalcBase{
protected:
	Vec3f pos;								///<	�O���b�v�̈ʒu
	int nRepeat;							///<	�J��Ԃ���
	std::vector<Vec3f> wireDirection;		///<	���C�������P�ʃx�N�g���i���i�j

	float sigma;							///<	�W���s��̑Ίp�����ɉ�����

	Vec3f trnForce;				///	���i��
	float smooth;				///	�񎟌`���@���͂̂Q�捀�̌W��
	float lambda_t;				///	�񎟌`���@���i���̌W��

public:
	HISpidarCalc3Dof();
	virtual ~HISpidarCalc3Dof();
	
	void Init(int dof, const VVector<float>& minF, const VVector<float>& maxF);

	virtual void Update();							///<	�p�����̍X�V
	virtual void SetForce(const Vec3f&);			///<	�񎦗͂̐ݒ�i���i�D��]�͂͒񎦂��Ȃ��j
	///	�͂̎擾
	Vec3f GetForce(){ return trnForce; }
	///	�ʒu�̎擾
	Vec3f GetPosition(){ return pos; }
};


/**	6���R�x��SPIDAR�p�̌v�Z�N���X�D���̖{���͉�	*/
class HISpidarCalc6Dof : public HISpidarCalc3Dof {
protected:
	Quaternionf ori;
	std::vector<Vec3f> wireMoment;				///<	���C�������P�ʃx�N�g���i��]���[�����g�j
	Vec3f rotForce;								///<	��]��
	float lambda_r;								///<	�񎟌`���@��]���̌W��
	
public:
	HISpidarCalc6Dof();
	virtual ~HISpidarCalc6Dof();

	///	������
	void Init(int dof, const VVector<float>& minF, const VVector<float>& maxF);
	/// �p�����̍X�V
	void Update();								
	/// �񎦗͂̐ݒ�i���i�E��]�́j
	void SetForce(const Vec3f&, const Vec3f& = Vec3f());
	///
	Vec3f GetTorque(){ return rotForce; }
	///	�p�x�̎擾
	Quaternionf GetOrientation(){ return ori; }
	
	/// �e�X�g�p���͊֐�
	float* InputForTest(float,float,float,float*,float);
};

}	//	namespace Spr

#endif
