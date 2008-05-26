#ifndef SYNCHRONIZATION_H
#define SYNCHRONIZATION_H

#include <Springhead.h>
#include <Physics/PHSolid.h>
#include <Base/TMatrix.h>

using namespace Spr;
using namespace PTM;

/// �ߖT���̂̏�����ꂽ�\����
struct PHNeighborObject {
	PHSolidIf* phSolidIf;	///< �V�[�������ߖT���̂ւ̃|�C���^
	PHSolid phSolid;			///< phSolidIf*�̒��g�̃R�s�[
	bool blocal;				///< �͊o�v���Z�X�ŃV�~�����[�V�������Ă��邩�ǂ���
	bool bfirstlocal;			///< ���߂ċߖT���̂ɂȂ������ǂ���
	Vec3d closestPoint;		///< �ߖT���̍ŋߓ_�i���[�J�����W�j
	Vec3d pointerPoint;		///< �͊o�|�C���^�ŋߓ_�i���[�J�����W�j
	Vec3d face_normal;		///< �ߖT���̖̂ʂ̖@���i�̗\��j
	Vec3d direction;			///< �ߖT���̂���͊o�|�C���^�ւ̃x�N�g�� // ���܂͂����ʂ̖@���Ƃ��Ă���Ă� 
	TMatrixRow<6, 3, double> A;	///< LocalDynamics�Ŏg���^���W���i���[�U�̓��͂ɂ��ω��j
	SpatialVector b;						///< LocalDynamics�Ŏg���^���W���i�d�͓��̒萔���j
};

#endif