#ifndef SYNCHRONIZATION_H
#define SYNCHRONIZATION_H

#include <Springhead.h>
#include <Physics/PHSolid.h>

using namespace Spr;

/// �ߖT���̂̏�����ꂽ�\����
struct PHNeighborObject {
	PHSolidIf* phSolidIf;	///< �V�[�������ߖT���̂ւ̃|�C���^
	PHSolid phSolid;			///< phSolidIf*�̒��g�̃R�s�[
	bool lastDynamical;		///< ������dynamical�@// ���͎g���ĂȂ��D���łɋߖT���̂�������false�ɂȂ��Ă��܂����߁D
	bool lastFrozen;			///< ������frozen		// ���͎g���ĂȂ�
	bool blocal;				///< �͊o�v���Z�X�ŃV�~�����[�V�������Ă��邩�ǂ���
	bool bfirstlocal;
	Vec3d closestPoint;		///< �ߖT���̍ŋߓ_�i���[�J�����W�j
	Vec3d pointerPoint;		///< �͊o�|�C���^�ŋߓ_�i���[�J�����W�j
	Vec3d face_normal;		///< �ߖT���̖̂ʂ̖@���i�̗\��j
	Vec3d direction;			///< �ߖT���̂���͊o�|�C���^�ւ̃x�N�g�� // ���܂͂����ʂ̖@���Ƃ��Ă���Ă� 
	
};

#endif