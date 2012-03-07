#ifndef PH_HAPTICRENDERBASE_H
#define PH_HAPTICRENDERBASE_H

#include <Physics/PHHapticPointer.h>

using namespace PTM;
namespace Spr{;

class PHSolidsForHaptic;
class PHSolidPairForHaptic;
class PHSolidPairsForHaptic;
class PHShapePairForHaptic;

struct PHIntermediateRepresentation :public UTRefCount{
	int solidID;				///< ���̂�ID(PHSolidsForHapitc��PHSolidPairForHaptic)�ƐN�����Ă��邩
	PHSolidPairForHaptic* solidPair;	///< PHSolidPairForHaptic�ւ̃|�C���^
	double depth;				///< �N����
	Vec3d normal;				///< �ʖ@��
	Vec3d r;					///< �͊o�|�C���^�̏d�S����̐ڐG�_�܂ł̃x�N�g��
	Vec3d pointerPointW;		///< �͊o�|���^�̐N���_(world)
	Vec3d contactPointW;		///< ���ԕ\���ʂ�vertex�𓊉e�����ʒu(world)
	Vec3d pointerPointVel;		///< �͊o�|�C���^���̐ڐG�_�̑��x(world)
	Vec3d contactPointVel;		///< ���̑��̐ڐG�_�̑��x(world)
	Posed interpolation_pose;	///< �`��̕�Ԏp��
	double f;					///< �N�������v�Z�ŐڐG�_�����i�ړ��ɍ�p����傫��
	float springK;					///< �o�l�W��
	float damperD;					///< �_���p�W��
	float mu;					///< �����C�W��
	float mu0;					///< �ő�Î~���C�W��(�ő�Î~���C�͖�����)
	PHIntermediateRepresentation(){
		solidID = -1;
		f = 0.0;
		solidPair = NULL;
	}
};
typedef PHIntermediateRepresentation PHIr;
typedef std::vector< PHIr* > PHIrs;

struct PHHapticRenderInfo : public PHHapticRenderDesc{
	PHHapticPointers*		pointers;
	PHSolidsForHaptic*		hsolids;
	PHSolidPairsForHaptic*	sps;
	int loopCount;
	double pdt;
	double hdt;
	bool bInterpolatePose;
};

class PHHapticRender : public PHHapticRenderInfo, public SceneObject{
public:
	SPR_OBJECTDEF(PHHapticRender);
	ACCESS_DESC(PHHapticRender);
	HapticRenderMode mode;
	bool bMultiPoints;

	PHHapticRender();
	// API
	virtual void SetHapticRenderMode(HapticRenderMode m);
	virtual void EnableMultiPoints(bool b);

	// Implementation
	virtual void HapticRendering(PHHapticRenderInfo info);
	virtual void DisplayHapticForce();
	virtual PHIrs CompIntermediateRepresentation(PHHapticPointer* pointer);
	virtual void PenaltyBasedRendering();
	virtual void ConstraintBasedRendering();
	virtual void VibrationRendering();
	virtual void VirtualCoupling();

	// �K�E�X�U�C�f���@���g��Ax+b>0������
	template <class AD, class XD, class BD>
	void GaussSeidel(MatrixImp<AD>& a, VectorImp<XD>& x, const VectorImp<BD>& b){
		int nIter = 15;					// �����񐔂̏��
		double error = 0.0;
		double errorRange = 10e-8;		// ���e�덷
		int n = (int)a.height();		// �A���������̐�(�s��a�̍s��)
		std::vector< double > lastx;
		for(int i = 0; i < n; i++){
			lastx.push_back(x[i]);
			x[i] = 0;
		}

		for(int k = 0; k < nIter; k++){		
			for(int i = 0; i < n; i++){
				double term1 = 0.0;
				double term2 = 0.0;
				for(int j = 0; j < i; j++){
					term1 += a[i][j] * x[j];
				}
				for(int j = i+1; j < n; j++){
					term2 += a[i][j] * lastx[j];
				}
				// x�̍X�V(�J��Ԃ��v�Z�̎����g�p)
				x[i] =  (-b[i] - term1 - term2) / a[i][i];
				if(x[i] < 0) x[i] = 0.0;
			}

			// (lastx - x)��2��̑��a�ƌ덷�͈͂��r
			error = 0.0;
			for(int i = 0; i < n; i++){
				error += pow(x[i] - lastx[i], 2);
				//DSTR << "iterete" << i << "," << x[i] << std::endl;
			}
			if(error < errorRange){
				//DSTR << "Finish the iteration in admissible error. " << std::endl;
				//DSTR << k << std::endl;
				return;
			}

			// �J��Ԃ��v�Z�̂��߂ɍX�V���x��lastx�ɕۑ�
			for(int i = 0; i < n; i++) lastx[i] = x[i];
		}
		//nIter�Ōv�Z���I���Ȃ������̂őł��؂�
		//static int iterError = 0;
		//iterError += 1;
		//DSTR << iterError << "Could not converge in iteration steps. Error = " << error << std::endl;
		//CSVOUT << error << std::endl;
	}
};

}

#endif