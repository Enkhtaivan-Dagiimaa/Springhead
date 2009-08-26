#ifndef FWINTERACT_INFO_H
#define FWINTERACT_INFO_H

#include <Springhead.h>
#include <Physics/PHSolid.h>
#include <Base/TMatrix.h>
#include <vector>

using namespace Spr;
using namespace PTM;


/** Local Dynamics�ɕK�v�ȕϐ� */
struct Mobility{
	Vec3d force;
	TMatrixRow<6, 3, double> A;		///< LocalDynamics�Ŏg�����r���e�B�i���[�U�̓��͂ɂ��ω��j
};
/** �|�C���^�ƃ\���b�h�̋ߖT�_�y�і@�� */
struct NeighborInfo{
	Vec3d closest_point;			///< �ߖT���̍ŋߓ_(���[�J�����W)
	Posed last_pose;				///< �ߖT���̂̑O���Pose
	Vec3d last_closest_point;		///< �O��̋ߖT���̂̍ŋߓ_(���[�J�����W)
	Vec3d pointer_point;			///< �͊o�|�C���^�ŋߓ_(���[�J�����W)
	Vec3d face_normal;				///< �ߖT���̂̒񎦖ʂ̖@��(�ߖT���̂���͊o�|�C���^�ւ̃x�N�g��)
	Vec3d last_face_normal;			///< �O��̋ߖT���̂̒񎦖ʂ̖@��
	double test_force_norm;			///< �\���V�~�����[�V�����ŒP�ʗ͂̑傫��
	Vec3d test_force;				///< �\���V�~�����[�V�����Ŏg���e�X�g��
	Vec3d impulse;					///< �����v���Z�X��1�X�e�b�v�I���܂łɗ͊o�|�C���^���������͐�
	NeighborInfo(){
		closest_point = Vec3d(0.0, 0.0, 0.0);					
		pointer_point = Vec3d(0.0, 0.0, 0.0);						
		face_normal = Vec3d(0.0, 0.0, 0.0);			
		last_face_normal = Vec3d(0.0, 0.0, 0.0);
		test_force_norm = 0.0;
		impulse = Vec3d();
	}
};
struct SyncInfo{
};
/** �|�C���^�ߖT���ǂ����̃t���O */
struct Flag{
	bool bneighbor;
	bool blocal;			///< �͊o�v���Z�X�ŃV�~�����[�V�������Ă��邩�ǂ���
	bool bfirstlocal;		///< ���߂ċߖT���̂ɂȂ������ǂ���
	Flag(){
		bneighbor = false;
		blocal = false;							
		bfirstlocal = false;	
	}
};

struct FWInteractInfo{
	Flag flag;
	struct Mobility mobility;
	struct NeighborInfo neighborInfo;

};

typedef std::vector<FWInteractInfo> AllInteractInfo;

#endif
