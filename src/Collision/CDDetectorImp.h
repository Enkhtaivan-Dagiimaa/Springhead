/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
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
	Vec3d normal;				///<	�Փ˂̖@��(0����1��) (Global)
	double depth;				///<	�Փ˂̐[���F�ŋߖT�_�����߂邽�߂ɁC2���̂𓮂��������D
	unsigned lastContactCount;	///<	�Ō�ɐڐG��������
	CDShapePairState():lastContactCount(-2), depth(0){}
};
class CDShapePair: public CDShapePairState, public UTRefCount{
public:
	CDConvex* shape[2];			// ����Ώۂ̌`��
	Posed shapePoseW[2];		// World�n�ł̌`��̎p���̃L���b�V��

	//	�ڐG���茋��
	Vec3d	closestPoint[2];		///< �ŋߖT�_�i���[�J�����W�n�j
	Vec3d	commonPoint;			///< ���������̓��_�iWorld���W�n�j
	Vec3d	center;					///< 2�̍ŐN���_�̒��Ԃ̓_
	enum State{
		NONE,		//	�ڐG�Ȃ�
		NEW,		//	�O��ڐG�Ȃ��ō���ڐG
		CONTINUE,	//	�O����ڐG
	} state;
	Vec3d iNormal;				///<	�ϕ��ɂ��@��

public:
	CDShapePair(){
	}
	void SetState(const CDShapePairState& s){
		(CDShapePairState&)*this = s;
	}	
	///	�ڐG����
	bool Detect(unsigned ct, const Posed& pose0, const Posed& pose1);
	///	�A���ڐG����D�����ɖ@��/���S�Ȃǂ��v�Z���Ă��܂��D
	bool DetectContinuously(unsigned ct, const Posed& pose0, const Vec3d& delta0, const Posed& pose1, const Vec3d& delta1);

	///	�@���̌v�Z
	void CalcNormal();
};
//	�f�o�b�O�p�c�[��
void CallDetectContinuously(std::istream& file, PHSdkIf* sdk);
void SaveDetectContinuously(CDShapePair* sp, unsigned ct, const Posed& pose0, const Vec3d& delta0, const Posed& pose1, const Vec3d& delta1);

///	BBox���m�̌�������D�������Ă���� true�D
bool FASTCALL BBoxIntersection(Posed postureA, Vec3f centerA, Vec3f extentA,
					  Posed postureB, Vec3f centerB, Vec3f extentB);

///	GJK�ŋ��L�_��������
bool FASTCALL FindCommonPoint(const CDConvex* a, const CDConvex* b,
					 const Posed& a2w, const Posed& b2w,
					 Vec3d& v, Vec3d& pa, Vec3d& pb);
/**	GJK�ŋ��L�_��������D�A����
	start����end�̊ԂɐڐG�_�����邩���ׂ�B
	@return 0:�܂������ڐG�Ȃ��B-2:start���O�ɐڐG����B-1: end����ɐڐG����B
	1: �ڐG����B
*/
int FASTCALL ContFindCommonPoint(const CDConvex* a, const CDConvex* b,
	const Posed& a2w, const Posed& b2w, const Vec3d& dir, double start, double end, 
	Vec3d& normal, Vec3d& pa, Vec3d& pb, double& dist);

///	�f�o�b�O�p�̃c�[���B�t�@�C���Ɉ�����ۑ�����B
void FASTCALL ContFindCommonPointSaveParam(const CDConvex* a, const CDConvex* b,
	const Posed& a2w, const Posed& b2w, const Vec3d& dir, double start, double end, 
	Vec3d& normal, Vec3d& pa, Vec3d& pb, double& dist);
///	�f�o�b�O�p�̃c�[���B
void ContFindCommonPointCall(std::istream& file, PHSdkIf* sdk);

#if 1
/// GJK�ōŋߖT�_�΂�������
void FASTCALL FindClosestPoints(const CDConvex* a, const CDConvex* b,
					   const Posed& a2w, const Posed& b2w,
					   Vec3d& pa, Vec3d& pb);
#endif

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
