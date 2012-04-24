/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHCONSTRAINTENGINE_H
#define PHCONSTRAINTENGINE_H

#include <Collision/CDDetectorImp.h>
#include <Physics/PHConstraint.h>
#include <Physics/PHGear.h>
#include <Physics/PHContactDetector.h>

namespace Spr{;

class PHSolid;
class PHJoint;
class PH1DJoint;
class PHRootNode;
class PHConstraintEngine;
class PHPath;

///	�`��̑g
class PHShapePairForLCP: public CDShapePair{
public:
	SPR_OBJECTDEF(PHShapePairForLCP);
	std::vector<Vec3d>	section;	///< �����f�ʂ̒��_�D�X��PHContactPoint�ƂȂ�D�����̂��߂ɕێ�
	///	�ڐG��́D�ڐG�����̐؂�������߂āC�؂�����\������ʑ��p�`�̒��_��engine�ɍS���Ƃ��Ēǉ�����D
	void EnumVertex(PHConstraintEngine* engine, unsigned ct, PHSolid* solid0, PHSolid* solid1);
	int NSectionVertexes(){return (int)section.size();}		//(section�̐���Ԃ��j
	Vec3d GetSectionVertex(int i){return section[i];}	//(i�Ԗڂ�section��Ԃ��j

	///	�ڐG�ʐρD�ڐG�`��̒��_���W����ʐς��v�Z
	double GetContactDimension(){
		if((int)section.size() < 2)	return 0.0;	// ���_��2�ȏ�Ȃ��ꍇ�͖ʐςȂ�
		Vec3d area;
		for(unsigned int i = 0; i < section.size()-2; i++){
			Vec3d vec1 = section[i+1] - section[0];
			Vec3d vec2 = section[i+2] - section[0];
			area += cross(vec1, vec2);
		}
		return area.norm() / 2;
	}

	/// �ڐG�ʂ̒P�ʖ@���x�N�g��
	Vec3d GetNormalVector(){
		Vec3d normal;
		if((int)section.size() < 3){
			// ���_��3�ȏ�Ȃ��ꍇ�͋ߖT�_�Ԃ̖@����Ԃ�
			normal = shapePoseW[1] * closestPoint[1] - shapePoseW[0] * closestPoint[0];
			return normal / normal.norm();
		}
		Vec3d vec1 = section[1] - section[0];
		Vec3d vec2 = section[2] - section[0];
		normal = cross(vec1, vec2);
		return normal / normal.norm();
	}

	void GetClosestPoints(Vec3d& pa, Vec3d& pb){
		pa = shapePoseW[0] * closestPoint[0];
		pb = shapePoseW[1] * closestPoint[1];
	}
};

/// Solid�̑g
class PHConstraintEngine;

class PHSolidPairForLCP : public PHSolidPair<PHShapePairForLCP, PHConstraintEngine>, public Object{
public:
	SPR_OBJECTDEF(PHSolidPairForLCP);
	virtual void OnDetect(PHShapePairForLCP* cp, PHConstraintEngine* engine, unsigned ct, double dt);
	virtual void OnContDetect(PHShapePairForLCP* cp, PHConstraintEngine* engine, unsigned ct, double dt);
	int	GetContactState(int i, int j){return shapePairs.item(i, j)->state;}
	unsigned GetLastContactCount(int i, int j){return shapePairs.item(i, j)->lastContactCount;}
	Vec3d GetCommonPoint(int i, int j){return shapePairs.item(i, j)->commonPoint;}
	double GetContactDepth(int i, int j){return shapePairs.item(i, j)->depth;}
	PHShapePairForLCPIf* GetShapePair(int i, int j){return shapePairs.item(i, j)->Cast();}
};

struct PHConstraintsSt{
	std::vector<PHConstraintState> joints;
	std::vector<PHConstraintState> gears;
};


class PHConstraintEngine : public PHConstraintEngineDesc, public PHContactDetector<PHShapePairForLCP, PHSolidPairForLCP, PHConstraintEngine>{
	friend class PHConstraint;
	friend class PHShapePairForLCP;
	SPR_OBJECTDEF1(PHConstraintEngine, PHEngine);
	ACCESS_DESC(PHConstraintEngine);
public:
	typedef PHContactDetector<PHShapePairForLCP, PHSolidPairForLCP, PHConstraintEngine> Detector;
	
	PHConstraintEngine();
	~PHConstraintEngine();
	
	PHJoint* CreateJoint(const IfInfo* ii, const PHJointDesc& desc, PHSolid* lhs = NULL, PHSolid* rhs = NULL);	///< �֐߂̒ǉ�����
	PHRootNode* CreateRootNode(const PHRootNodeDesc& desc, PHSolid* solid = NULL);	///< �c���[�\���̃��[�g�m�[�h���쐬
	PHTreeNode* CreateTreeNode(const PHTreeNodeDesc& desc, PHTreeNode* parent = NULL, PHSolid* solid = NULL);	///< �c���[�\���̒��ԃm�[�h���쐬
	PHGear*		CreateGear(const PHGearDesc& desc, PH1DJoint* lhs = NULL, PH1DJoint* rhs = NULL);	///< �M�A���쐬
	PHPath*		CreatePath(const PHPathDesc& desc);
	void		UpdateGearNode();
	virtual int GetPriority() const {return SGBP_CONSTRAINTENGINE;}
	virtual void Step();			///< 
	virtual void StepPart1();		///< 
	virtual void StepPart2();		///< 
	void UpdateSolids(bool bVelOnly);	///< ���ʂ�Solid�ɔ��f����. bVelOnly == true�Ȃ�Ό��ʂ̑��x�݂̂�Solid�ɔ��f�����C�ʒu�͂��̂܂܁D
	void UpdateOnlyVelocity();			///< obsolete. UpdateSolids(true)���g�p�̂���
	void Clear();

	typedef std::vector< UTRef<PHRootNode> >	PHRootNodes;
	typedef std::vector< UTRef<PHPath> >		PHPaths;
	//typedef std::vector< UTRef<PHJointLimit> >	PHJointLimits;
	//typedef std::vector< UTRef<PHMotor> >		PHMotors;
	
	PHConstraints	points;			///< �ڐG�_�̔z��
	PHConstraints	joints;			///< �֐߂̔z��
	
	PHRootNodes		trees;			///< Articulated Body System�̔z��
	PHGears			gears;			///< �M�A�̔z��
	PHPaths			paths;			///< �p�X�̔z��
	void SetupLCP();				///< ���x�X�VLCP�̏���
	void IterateLCP();				///< ���x�X�VLCP�̈�x�̔���
	void SetupCorrectionLCP();		///< �덷�C��LCP�̏���
	void IterateCorrectionLCP();	///< �덷�C��LCP�̈�x�̔���

	virtual PHConstraintsIf* GetContactPoints();
	virtual void	SetVelCorrectionRate(double value){velCorrectionRate = value;}
	virtual double	GetVelCorrectionRate(){return velCorrectionRate;}
	virtual void	SetPosCorrectionRate(double value){posCorrectionRate = value;}
	virtual double	GetPosCorrectionRate(){return posCorrectionRate;}
	virtual void	SetContactCorrectionRate(double value){contactCorrectionRate = value;}
	virtual double	GetContactCorrectionRate(){return contactCorrectionRate;}
	virtual void	SetBSaveConstraints(bool value){bSaveConstraints = value;}
	virtual void	SetUpdateAllSolidState(bool flag){bUpdateAllState = flag;}
	virtual void	SetUseContactSurface(bool flag){bUseContactSurface = flag;}

	virtual void	SetShrinkRate(double data){shrinkRate = data;}
	virtual double	GetShrinkRate(){return shrinkRate;}
	virtual void	SetShrinkRateCorrection(double data){shrinkRateCorrection = data;}
	virtual double	GetShrinkRateCorrection(){return shrinkRateCorrection;}
	virtual bool	AddChildObject(ObjectIf* o);
	virtual bool	DelChildObject(ObjectIf* o);

	virtual size_t	GetStateSize() const;
	virtual void	ConstructState(void* m) const;
	virtual void	DestructState(void* m) const ;
	virtual bool	GetState(void* s) const ;
	virtual void	SetState(const void* s);

	bool bContactDetectionEnabled; ///< �ڐG���肪�L�����D���ꂪfalse���ƐڐG���莩�̂��s��Ȃ�
	virtual void	EnableContactDetection(bool enable) { bContactDetectionEnabled = enable; }
};

}	//	namespace Spr
#endif
