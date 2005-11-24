#ifndef SPR_CDSHAPEIF_H
#define SPR_CDSHAPEIF_H
#include <Springhead.h>
#include <Foundation/Object.h>

namespace Spr{;
/*	memo
	shape�ɊK�w�\�����������邩�ǂ����D
	Novodex ODE�Ƃ� �K�w�\���͎����Ă��Ȃ��D
	�K�w�𓮂����Ƃ��ɂ́C�d�S�� �������[�����g�����������Ȃ���΂Ȃ�Ȃ��̂ŁC
	����ȂɎg��Ȃ��Ƃ������f���Ǝv���D
	���Ȃ��Ƃ��C�V�~�����[�V�������ɊK�w�œ������͍̂���Ǝv���D

	�Ȃ̂ŁC�K�w�͎����Ȃ��Ƃ������j�ō��D

	Solid �͕����� Shape �������Ƃ��ł���D
	Shape �͊K�w�\���͎����Ȃ��D

	�K�w�\���͕������C�u�����Ƃ͕ʂɁC�V�[���O���t���C�u����������
*/

struct CDShapeIf : public ObjectIf{
};

struct PHMaterial{
	PHMaterial(){
		mu = mu0 = e = 0.2f;
	}
	float mu, mu0;	///<	�����C�C�Î~���C�W��
	float e;		///<	���˕Ԃ�W��
};

struct CDShapeDesc{
	enum ShapeType{
		CONVEX,
		CONVEXMESH,
	} type;
	CDShapeDesc(ShapeType t): type(t){}
};
/**	�ʌ`��̃��b�V���̐����p
*/
struct CDConvexMeshDesc: public CDShapeDesc{
	CDConvexMeshDesc():CDShapeDesc(CONVEXMESH){}
	std::vector<Vec3f> vertices;	///<	���_�̍��W
	PHMaterial material;			///<	�ގ�
};
/**
	�ʌ`��̃��b�V��	���_��ǉ�
*/
struct CDConvexMeshIf: public CDShapeIf{
};

}

#endif
