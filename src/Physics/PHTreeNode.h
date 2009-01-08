/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHTREENODE_H
#define PHTREENODE_H

#include <SprPhysics.h>
#include <Physics/PHJoint.h>
#include <Physics/PhysicsDecl.hpp>

namespace Spr{;

///	�c���[�m�[�h�̊�{�N���X
class PHRootNode;
class PHTreeNode : public SceneObject, public UTTreeNode<PHTreeNode>, public PHTreeNodeDesc{
public:
	SPR_OBJECTDEF_ABST(PHTreeNode);
	
	virtual bool AddChildObject(ObjectIf* o);
	virtual size_t NChildObject();
	virtual ObjectIf* GetChildObject(size_t i);

	void Enable(bool bEnable = true);
	bool IsEnabled(){ return bEnabled; }
	int  NChildren(){ return (int)Children().size(); }
	PHTreeNodeIf* GetChild(int i){ return Children()[i]->Cast(); }
	
	bool		Includes(PHTreeNode* node);		///< �����ȉ��Ƀm�[�hnode�����邩
	PHTreeNode*	FindBySolid(PHSolid* solid);	///< �����ȉ��̃c���[��solid���Q�Ƃ��Ă���m�[�h��T��
	PHTreeNode* FindByJoint(PHJoint* joint);	///< �����ȉ��̃c���[��joint���Q�Ƃ��Ă���m�[�h��T��
	void		RemoveGearNode();
	void		Prepare(PHScene* scene, PHConstraintEngine* engine);	///< �ċA�I��scene��engine���Z�b�g����

	PHTreeNode();

	virtual PHSolid*	GetSolid(){return joint->solid[1];}
	virtual PHRootNode*	GetRootNode(){return GetParent()->GetRootNode();}
	virtual void ResetGearNode();
	virtual void CompAccel()=0;
	virtual void CompAccelDiff(bool bUpdate, bool bImpulse)=0;
	/// ���̃m�[�h�ɉ����S���͂̕ω��ʂ��瑼�̃m�[�h�̑��x�ω��ʂւ̉e�����v�Z�DLCP�Ŏg�p
	virtual void CompResponse(const SpatialVector& df, bool bUpdate, bool bImpulse);
	virtual void CompBiasForceDiff(bool bUpdate, bool bImpulse)=0;
	virtual void SetupLCP();
	virtual void IterateLCP();
	virtual void InitArticulatedInertia();
			void InitArticulatedBiasForce();
	virtual void CompArticulatedInertia();			///< Articulated Inertia�̌v�Z
	virtual void CompArticulatedBiasForce();		///< Articulated Bias Force�̌v�Z
	virtual	void CompSpatialTransform();			///< �m�[�h�Ԃ̍��W�ϊ��̌v�Z
	virtual void CompCoriolisAccel();
	virtual void CompJointJacobian(){}				///< �֐߃��R�r�A�����v�Z
	virtual void CompJointCoriolisAccel(){}			///< �R���I���̉����x���v�Z	
	virtual void AccumulateInertia(){}				///< Ia��e�m�[�h��Ia�ɐςݏグ��
	virtual void AccumulateBiasForce(){}			///< Za��e�m�[�h��Za�ɐςݏグ��
			void ResetCorrectionVarsRecursive();
	virtual void ResetCorrectionVars();				///< Correction�̕ϐ��̏�����
	virtual void UpdateVelocity(double dt);			///< ���̂̑��x�̍X�V
	virtual void UpdatePosition(double dt);			///< ���̂̈ʒu�̍X�V
	virtual void UpdateJointVelocity(double dt){}	///< �֐ߑ��x�̍X�V
	virtual void UpdateJointPosition(double dt){}	///< �֐߈ʒu�̍X�V
	virtual void CompRelativeVelocity(){}			///< �֐ߑ��x���獄�̊ԑ��Α��x���v�Z
	virtual void CompRelativePosition(){}			///< �֐߈ʒu���獄�̊ԑ��Έʒu���v�Z
	
	//int			id;						///< ID�D���[�g�m�[�h��0��depth-first�Ŋ��蓖�Ă���D
	PHJoint*	joint;						///< �e�m�[�h�Ƃ��̃m�[�h�Ƃ��Ȃ��S��
	//bool		opposite;					///< �c���[�m�[�h�̐e�q�֌W�ƍS���̐e�q�֌W�i�\�P�b�g�����Ă�̂��e�j���t�Ȃ�true

	SpatialMatrix			I;				///< Articulated Inertia
	SpatialVector			Z;				///< Articulated Bias Force
	SpatialVector			dZ;				///< �S���͂̕ω��ɂ��Z�̕ω���
	SpatialVector			c;				///< �R���I�������x
	SpatialVector			cj;				///< �֐ߑ��x�ɂ��R���I�������x

	SpatialTransform		Xcp, Xcj;
	SpatialMatrix			XIX, XtrIJ_JIJinv_Jtr, XtrIJ_JIJinv_JtrIX;
	SpatialVector			a, da;			///< �����x�Ƃ��̕ω���
	//SpatialVector			v, dv;			///< ���x�Ƃ��̕ω���
	SpatialVector			ap, Ic, ZplusIc, XtrZplusIc;

	PHScene*	scene;
	PHConstraintEngine* engine;
};

class PHRootNode : public PHTreeNode{
public:
	SPR_OBJECTDEF(PHRootNode);
	SPR_DECLMEMBEROF_PHRootNodeDesc;

	virtual bool AddChildObject(ObjectIf* o);
	virtual size_t NChildObject();
	virtual ObjectIf* GetChildObject(size_t i);
	
	void SetupABA();
	void SetupCorrectionABA();
	
	virtual PHSolid* GetSolid(){return solid;}
	virtual PHRootNode* GetRootNode(){return this;}
	virtual int	 GetDof(){ return 6; }
	virtual void CompArticulatedInertia();
	virtual void CompArticulatedBiasForce();
	virtual void CompAccel();
	virtual void CompAccelDiff(bool bUpdate, bool bImpulse){}
	virtual void CompBiasForceDiff(bool bUpdate, bool bImpulse);
	virtual void UpdateVelocity(double dt);
	virtual void UpdatePosition(double dt);
	
	PHSolid*	solid;		///< ���[�g�m�[�h�̍���

	PHRootNode(const PHRootNodeDesc& desc = PHRootNodeDesc()){
		solid = NULL;
	}
protected:
	SpatialMatrix		Iinv;
};

class PHGear;
///	N���R�x�̊֐߂̊�{�N���X
template<int NDOF>
class PHTreeNodeND : public PHTreeNode{
public:
	typedef	PTM::TVector<NDOF, double>	VecNd;
	typedef PTM::TMatrixCol<6, NDOF, double> Matrix6Nd;
	typedef PTM::TMatrixCol<NDOF, NDOF, double> MatrixNd;
	
	PHJointND<NDOF>* GetJoint(){return (PHJointND<NDOF>*)DCAST(PHJoint , joint);}

	/// ABA�֌W�̊֐�
	virtual void ResetGearNode();
	virtual void CompCoriolisAccel();
	virtual void CompSpatialTransform();
	virtual void InitArticulatedInertia();
	virtual void AccumulateInertia();
	virtual void AccumulateBiasForce();
	virtual void CompJointJacobian();
	virtual void CompAccel();
	virtual void ResetCorrectionVars();
	virtual void CompAccelDiff(bool bUpdate, bool bImpulse);
	virtual void CompBiasForceDiff(bool bUpdate, bool bImpulse);
	virtual void UpdateJointVelocity(double dt);
	virtual void UpdateJointPosition(double dt);

	/// LCP�֌W�̊֐�
	virtual void CompResponse(const VecNd& tau, bool bUpdate);
			void CompResponseMatrix();
	virtual void ModifyJacobian();
	virtual void SetupLCP();
	virtual void IterateLCP();
	virtual void CompBias()=0;
	virtual void Projection(double& f, int k)=0;

	void AddGear(PHGear* gear, PHTreeNodeND<NDOF>* child);
	PHGear*			gear;				///< �M�A�ւ̎Q�ƁD���̃M�A����Đe�m�[�h�ƘA������
	PHTreeNodeND<NDOF>*	gearNode;			///< �M�A�m�[�h�ւ̎Q�ƁDNULL:�A���Ȃ�, this:�M�A�g���C���ŏ�i
	std::vector<PHTreeNodeND<NDOF>*> gearChildren;	///< ���g��gearNode�̏ꍇ�C�A�����Ă���m�[�h�̔z��
	PHTreeNodeND<NDOF>*	parentND;
	
	PHTreeNodeND();
//protected:
	bool			constr[NDOF];			///< ABA�Ƃ��Ċe���R�x���S�����邩
	Matrix6Nd		J;						///< �֐ߍ��W���瑊�Α��x�ւ̃��R�r�A��
	Matrix6Nd		IJ, XtrIJ, J_JIJinv, IJ_JIJinv, XtrIJ_JIJinv;
	MatrixNd		JIJ, JIJinv;
	VecNd	JtrZplusIc;
	VecNd	accel, daccel, dtau, vel;	///< �֐߉����x�i�ƕω��ʁj�C�֐߃g���N�̕ω��ʁC�֐ߑ��x

	SpatialTransform	Xcg;			///< �M�A�g���C���̐e�m�[�h�Ƃ̊Ԃ̍��W�ϊ�
	SpatialMatrix	sumXIX;
	SpatialVector	sumXtrZplusIc, sumXtrdZ;
	Matrix6Nd		sumXtrIJ, sumXtrIJ_sumJIJinv;
	MatrixNd		sumJIJ, sumJIJinv;
	VecNd			sumJtrZplusIc, sumJtrdZ, sumtorque, JtrdZ;
	

	VecNd A, Ainv, dA, b, db, f;	///< �֐ߍ��WLCP�̂��߂̕ϐ�
	MatrixNd Jq;					///< �֐ߍ��WLCP�ɂ�����֐ߑ��x����S���ϐ��ւ̃��R�r�A��
};

///	1���R�x�̊֐�
class PHTreeNode1D : public PHTreeNodeND<1>{
public:
	SPR_OBJECTDEF_ABST1(PHTreeNode1D, PHTreeNode);
	SPR_DECLMEMBEROF_PHTreeNode1DDesc;
	
	PHJoint1D*		GetJoint(){return DCAST(PHJoint1D, joint);}
	virtual void	CompBias();
	virtual void	Projection(double& f, int k);
	
	PHTreeNode1D();
};

}

#endif
