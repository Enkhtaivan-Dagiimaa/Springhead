#ifndef SYNCHRONIZATION_H
#define SYNCHRONIZATION_H

#include <Springhead.h>
#include <Base/TMatrix.h>
#include <Physics/PHSolid.h>

using namespace Spr;
using namespace PTM;

/// �ߖT���̂̏�����ꂽ�\����
struct PHNeighborObject {
	PHSolidIf* phSolidIf;				///< �V�[�������ߖT���̂ւ̃|�C���^
	PHSolid phSolid;					///< phSolidIf*�̒��g�̃R�s�[
	bool bneighbor;
	bool blocal;						///< �͊o�v���Z�X�ŃV�~�����[�V�������Ă��邩�ǂ���
	bool bfirstlocal;					///< ���߂ċߖT���̂ɂȂ������ǂ���
	Vec3d closestPoint;					///< �ߖT���̍ŋߓ_(���[�J�����W)
	Vec3d pointerPoint;					///< �͊o�|�C���^�ŋߓ_(���[�J�����W)
	Vec3d face_normal;					///< �ߖT���̂̒񎦖ʂ̖@��(�ߖT���̂���͊o�|�C���^�ւ̃x�N�g��)
	Vec3d last_face_normal;				///< �O��̋ߖT���̂̒񎦖ʂ̖@��
	double test_force_norm;				///< �\���V�~�����[�V�����ŒP�ʗ͂̑傫��
	TMatrixRow<6, 3, double> A;			///< LocalDynamics�Ŏg���^���W���i���[�U�̓��͂ɂ��ω��j
	SpatialVector b;					///< �\���V�~�����[�V�����ŋ��߂��^���W���i�d�͓��̒萔���j
	SpatialVector curb;					///< �ʏ�V�~�����[�V�����łł��萔��
	SpatialVector lastb;				///< �O��̗\���V�~�����[�V�����ŋ��߂��^���W���̒萔��
	SpatialVector lastvel;

	PHNeighborObject(){
		phSolidIf = NULL;	
		bneighbor = false;
		blocal = false;							
		bfirstlocal = false;						
		closestPoint = Vec3d(0.0, 0.0, 0.0);					
		pointerPoint = Vec3d(0.0, 0.0, 0.0);						
		face_normal = Vec3d(0.0, 0.0, 0.0);			
		last_face_normal = Vec3d(0.0, 0.0, 0.0);
		test_force_norm = 0.0;		
	}; 
};

#endif