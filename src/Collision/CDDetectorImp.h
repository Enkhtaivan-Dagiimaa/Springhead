/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CDDETECTORIMP_H
#define CDDETECTORIMP_H
#include "CDQuickHull3D.h"
#include "CDConvexMesh.h"
#include "CDSphere.h"
#include "CDBox.h"

namespace Spr {;

#define CD_EPSILON	HULL_EPSILON
#define CD_INFINITE	HULL_INFINITE

class CDFace;
class CDConvex;

/// Shape�̑g�̏��
struct CDShapePairState{
	unsigned lastContactCount;
	Vec3d normal;				///<	�Փ˂̖@��(0����1��) (Global)
	double depth;				///<	�Փ˂̐[���F�ŋߖT�_�����߂邽�߂ɁC2���̂𓮂��������D
	//CDShapePairState(const CDShapePair& s):
	//	lastContactCount(s.lastContactCount),normal(s.normal), depth(s.depth){}
};
class CDShapePair: public CDShapePairState, public UTRefCount{
public:
	CDConvex* shape[2];			// ����Ώۂ̌`��
	Posed shapePoseW[2];		// World�n�ł̌`��̎p���̃L���b�V��

	//	�ڐG���茋��
	Vec3d	closestPoint[2];		///< �ŋߖT�_�i���[�J�����W�n�j
	Vec3d	commonPoint;			///< ���������̓��_�iWorld���W�n�j
	bool	bActive;				///< ���ݐڐG�������Ă��邩
	unsigned lastContactCount;
	enum State{
		NEW,
		CONTINUE,
	} state;
	Vec3d iNormal;				///<	�ϕ��ɂ��@��
	Vec3d center;				///<	2�̍ŐN���_�̒��Ԃ̓_�CCDContactAnalysis::CalcNormal ���X�V����D

public:
	//virtual void Clear();
	void SetState(const CDShapePairState& s){
		(CDShapePairState&)*this = s;
	}

	///	�`��̎p��(shapePoseW)�̍X�V�D�eShape�̐eSolid�̎p����n���D
	///void UpdateShapePose(Posed pose0, Posed pose1);
	
	///	�ڐG����
	bool Detect(unsigned ct, CDConvex* s0, CDConvex* s1, const Posed& pose0, const Posed& pose1);
	
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

class CDFace;


///	ContactAnalysis���g�p����ʑ��ʑ̖̂ʂ�\���D
class CDContactAnalysisFace{
public:
	class DualPlanes: public std::vector<CDQHPlane<CDContactAnalysisFace>*>{};

	CDFace* face;	///<	�ʂ�o�Εϊ��������_��QuickHull������̂ŁCCDFace��Vtxs.
	int id;			///<	�ǂ����Solid�̖ʂ����\��ID�D

	//@group CDContactAnalysis �����������̌`������߂�ۂɎg����Ɨ̈�
	//@{
	Vec3f normal;	///<	�ʂ̖@���x�N�g��
	float dist;		///<	���_����̋���
	/**	QuickHull�A���S���Y���p���[�N�G���A�D
		��ԉ������_���猩����ʂ��폜�������Ǝc�����`��̃G�b�W����
		������ڑ����Ă������߂̃|�C���^�D
		���_���ʂ̐ڑ��D	�ʁ����_�͒��_�̕��я����番����D	*/
	CDQHPlane<CDContactAnalysisFace>* horizon;
	//@}
	
	///	QuickHull�ɂƂ��Ă̒��_�D���̖ʂ�o�Εϊ����Ăł��钸�_
	Vec3f GetPos() const { return normal / dist; }
	/**	�o�Εϊ����s���Dbase�ɓn�����_�o�b�t�@�́C�o�Εϊ����\�Ȃ悤��
		�o�Εϊ��̒��S�����_�ƂȂ�悤�ȍ��W�n�łȂ���΂Ȃ�Ȃ��D	*/
	bool CalcDualVtx(Vec3f* base);

	/**	���̖ʂ�o�Εϊ����Ăł��钸�_���܂ޖʁD
		�܂�C���������̌`����\�����钸�_�̂����C
		���̖ʂɂ������	*/
	DualPlanes dualPlanes;
	///	���������̌`����\�����钸�_�̂������̖ʂɂ�����̂̐�.
	size_t NCommonVtx(){ return dualPlanes.size(); }
	///	���������̌`����\�����钸�_�̂������̖ʂɂ������.
	Vec3f CommonVtx(int i);
	///	�f�o�b�O�p�\��
	void Print(std::ostream& os) const;
};
inline std::ostream& operator << (std::ostream& os, const CDContactAnalysisFace& f){
	f.Print(os);
	return os;
}

/**	���������̉�͂�����N���X�D(���������̌`������߂�/����̖@����ϕ��ŋ��߂�)	*/
class CDContactAnalysis{
public:
	/// \defgroup quickHull QuickHull�̂��߂̒��_�ƕ���
	//@{
	
	typedef std::vector<CDContactAnalysisFace*> Vtxs;	
	static Vtxs vtxs;					///<	QuickHull�̒��_
	typedef std::vector<CDContactAnalysisFace> VtxBuffer;	
	static VtxBuffer vtxBuffer;

	static CDQHPlanes<CDContactAnalysisFace> planes;	///<	��
	bool isValid;						///<	���������̃|���S���͗L���H
	//@}
	std::vector<Vec3f> tvtxs[2];		///<	�Ώۂ�2�̓ʑ��ʑ̂�CommonPoint�n�ł̒��_�̍��W
	/**	���ʕ����̌`������߂�D
		���ʂ́C���ʕ������\������ʂ� vtxs.begin() ����Ԃ�l�܂łɁC
		���ʕ������\�����钸�_���C planes.begin ���� planes.end �̂�����
		deleted==false �̂��̂ɓ���ĕԂ��D
		cp �� shapePoseW �� shape[0], shape[1]�̒��_��World�n�ɕϊ�����
		�ϊ��s�񂪓����Ă��Ȃ���΂Ȃ�Ȃ��D	*/
	CDContactAnalysisFace** FindIntersection(CDShapePair* cp);
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
