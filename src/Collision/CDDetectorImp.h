#ifndef CDDETECTORIMP_H
#define CDDETECTORIMP_H
#include "CDQuickHull3D.h"

namespace Spr {;

#define CD_EPSILON	HULL_EPSILON
#define CD_INFINITE	HULL_INFINITE

class CDFace;
class CDConvex;

class CDShapePair: public UTRefCount{
public:
	//	����Ώۂ�Shape
	CDShape* shape[2];
	//	�ڐG���茋��
	Vec3d closestPoint[2]; // ���[�J�����W�n
	Vec3d commonPoint;     // World���W�n
	unsigned int lastContactCount;
	enum State{
		NEW,
		CONTINUE,
	} state;
	//	�@���v�Z����
	Vec3d normal;				///<	�Փ˂̖@��(0����1��) (Global)
	Vec3d iNormal;				///<	�ϕ��ɂ��@��
	Vec3d center;				///<	2�̍ŐN���_�̒��Ԃ̓_�CCDContactAnalysis::CalcNormal ���X�V����D
	double depth;				///<	�Փ˂̐[���F�ŋߖT�_�����߂邽�߂ɁC2���̂𓮂��������D

	//	World�n�ł̌`��̎p�����L���b�V��
	Posed shapePoseW[2];

	///	
	CDShapePair(CDShape* s0, CDShape* s1){
		shape[0] = s0;
		shape[1] = s1;
	}
	///	�`��̎p��(shapePoseW)�̍X�V�D�eShape�̐eSolid�̎p����n���D
	void UpdateShapePose(Posed pose0, Posed pose1);
	///	�ڐG����
	bool Detect(unsigned ct);
};


///	BBox���m�̌�������D�������Ă���� true�D
bool BBoxIntersection(Posed postureA, Vec3f centerA, Vec3f extentA,
					  Posed postureB, Vec3f centerB, Vec3f extentB);

///	GJK�ŋ��L�_��������
bool FindCommonPoint(const CDConvex* a, const CDConvex* b,
					 const Posed& a2w, const Posed& b2w,
					 Vec3d& v, Vec3d& pa, Vec3d& pb);

/// GJK�ōŋߖT�_�΂�������
void FindClosestPoints(const CDConvex* a, const CDConvex* b,
					   const Posed& a2w, const Posed& b2w,
					   Vec3d& pa, Vec3d& pb);

/**	�Փ˔��茋��(���������̌`������߂�@�\/�@�������߂�@�\�t��)	*/
class CDContactAnalysis{
public:
	/// \addtogroup quickHull QuickHull�̂��߂̒��_�ƕ���
	//@{
	///	�ʂ�o�Εϊ��������_��QuickHull������̂ŁCCDFace��Vtxs.
	typedef std::vector<CDFace*> Vtxs;	
	static Vtxs vtxs;					///<	QuickHull�̒��_
	static CDQHPlanes<CDFace> planes;	///<	��
	bool isValid;						///<	���������̃|���S���͗L���H
	//@}
	/**	���ʕ����̌`������߂�D
		���ʂ́C���ʕ������\������ʂ� vtxs.begin() ����Ԃ�l�܂łɁC
		���ʕ������\�����钸�_���C planes.begin ���� planes.end �̂�����
		deleted==false �̂��̂ɓ���ĕԂ��D
		cp �� shapePoseW �� shape[0], shape[1]�̒��_��World�n�ɕϊ�����
		�ϊ��s�񂪓����Ă��Ȃ���΂Ȃ�Ȃ��D	*/
	CDFace** FindIntersection(CDShapePair* cp);
	/**	���������̌`��̖@����ϕ����āC�Փ˂̖@�������߂�D
		����A��B�̏Փ˂̖@���́C���������̖ʂ̂����CA�̖ʂ̖@���̐ϕ�
		����B�̖ʂ̖@���̐ϕ������������̂ɂȂ�D	*/
	void IntegrateNormal(CDShapePair* cp);
	/**	�@���̌v�Z�D�O��̖@���̌����ɕ��̂𓮂����C
		���̂𗣂��čŋߖT�_�����߂�D	*/
	void CalcNormal(CDShapePair* cp);
};

}
#endif
