/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHTREENODE_H
#define PHTREENODE_H

#include <SprPhysics.h>
#include <Physics/PHSpatial.h>

namespace Spr{;

///	�c���[�m�[�h�̊�{�N���X
class PHRootNode;
class PHTreeNode : public SceneObject, public PHTreeNodeIfInit, public UTTreeNode<PHTreeNode>{
public:
	OBJECTDEF_ABST(PHTreeNode, SceneObject);
	
	bool		Includes(PHTreeNode* node);		///< �����ȉ��Ƀm�[�hnode�����邩
	PHTreeNode*	FindBySolid(PHSolid* solid);	///< �����ȉ��̃c���[��solid���Q�Ƃ��Ă���m�[�h��T��
	PHTreeNode* FindByJoint(PHJoint* joint);	///< �����ȉ��̃c���[��joint���Q�Ƃ��Ă���m�[�h��T��
	void		RemoveGearNode();
	//int GetNumOfNodes();
	//int GetTotalDof();							///< �q�m�[�h���܂߂����R�x�̍��v
	//int AssignID(int id, std::vector<PHTreeNode*>& table);		///< �ċA�I��ID������U��

	PHTreeNode();

	virtual PHSolid*	GetSolid(){return joint->solid[1];}
	virtual PHRootNode*	GetRootNode(){return GetParent()->GetRootNode();}
	
	virtual void CompAccel(){}
	virtual void CompAccelDiff(bool bUpdate){}
	/// ���̃m�[�h�ɉ����S���͂̕ω��ʂ��瑼�̃m�[�h�̑��x�ω��ʂւ̉e�����v�Z�DLCP�Ŏg�p
	virtual void CompResponse(const SpatialVector& dF, bool bUpdate = true);
	virtual void CompBiasForceDiff(bool bUpdate){}
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
	SpatialVector			a;				///< �����x
	SpatialVector			da;				///< �S���͂̕ω��ɂ��a�̕ω���
	SpatialVector			ap, Ic, ZplusIc, XtrZplusIc;

	PHScene*	scene;
	PHConstraintEngine* engine;
};

class PHRootNode : public PHTreeNode, public PHRootNodeIfInit{
public:
	OBJECTDEF(PHRootNode, PHTreeNode);

	void SetupABA();
	
	virtual PHSolid* GetSolid(){return solid;}
	virtual PHRootNode* GetRootNode(){return this;}
	virtual int	 GetDof(){ return 6; }
	virtual void CompArticulatedInertia();
	virtual void CompArticulatedBiasForce();
	virtual void CompAccel();
	virtual void CompBiasForceDiff(bool Update);
	virtual void UpdateVelocity(double dt);
	virtual void UpdatePosition(double dt);
	
	PHSolid*	solid;		///< ���[�g�m�[�h�̍���

	PHRootNode(const PHRootNodeDesc& desc = PHRootNodeDesc()){
		solid = NULL;
	}
protected:
	SpatialMatrix		Iinv;
};

///	N���R�x�̊֐߂̊�{�N���X
template<int NDOF>
class PHTreeNodeND : public PHTreeNode{
public:
	PHJointND<NDOF>* GetJoint(){return (PHJointND<NDOF>*)DCAST(PHJoint , joint);}

	/// ABA�֌W�̊֐�
	virtual void AccumulateInertia();
	virtual void AccumulateBiasForce();
	virtual void CompJointJacobian();
	virtual void CompAccel();
	virtual void CompAccelDiff(bool bUpdate);
	virtual void CompBiasForceDiff(bool bUpdate);
	virtual void UpdateJointVelocity(double dt);
	virtual void UpdateJointPosition(double dt);

	/// LCP�֌W�̊֐�
			void CompResponse(const PTM::TVector<NDOF, double>& tau, bool bUpdate = true);
			void CompResponseMatrix();
	virtual void ModifyJacobian();
	virtual void SetupLCP();
	virtual void IterateLCP();

	virtual void CompBias()=0;
	virtual void Projection(double& f, int k)=0;

	PHTreeNodeND();
protected:
	bool			constr[NDOF];			///< ABA�Ƃ��Ċe���R�x���S�����邩
	SpatialVector	J[NDOF];				///< �֐ߍ��W���瑊�Α��x�ւ̃��R�r�A��
	SpatialVector	IJ[NDOF], XtrIJ[NDOF], J_JIJinv[NDOF], IJ_JIJinv[NDOF], XtrIJ_JIJinv[NDOF];
	PTM::TMatrixCol<NDOF, NDOF, double> JIJ, JIJinv;
	PTM::TVector<NDOF, double>	JtrZplusIc;
	PTM::TVector<NDOF, double>	accel, daccel, dtau;

	PTM::TVector<NDOF, double> A, Ainv, dA, b, db, f;	///< �֐ߍ��WLCP�̂��߂̕ϐ�
	PTM::TMatrixCol<NDOF, NDOF, double> Jq;	///< �֐ߍ��WLCP�ɂ�����֐ߑ��x����S���ϐ��ւ̃��R�r�A��
};

///	1���R�x�̊֐�
class PHTreeNode1D;
class PHTreeNode1D : public PHTreeNodeND<1>{
public:
	OBJECTDEF_ABST_NOIF(PHTreeNode1D, PHTreeNode);
	PHGear*			gear;				///< �M�A�ւ̎Q�ƁD���̃M�A�����gearedParent�m�[�h�ƘA������
	/// �M�A�m�[�h�ւ̎Q�ƁDNULL->�A���Ȃ�, this->�M�A�g���C���ŏ�i
	PHTreeNode1D*	gearNode;
	PHTreeNode1D*	parent1D;
	SpatialTransform	Xcg;			///< �M�A�g���C���̐e�m�[�h�Ƃ̊Ԃ̍��W�ϊ�
	SpatialMatrix	sumXIX;
	SpatialVector	sumXtrIJ, sumXtrIJ_sumJIJinv;
	SpatialVector	sumXtrZplusIc, sumXtrdZ;
	double			sumJIJ, sumJIJinv;
	double			sumJtrZplusIc, sumJtrdZ;
	double			sumtorque;
	
	void			AddGear(PHGear* gear, PHTreeNode1D* child);
	PHJoint1D*		GetJoint(){return DCAST(PHJoint1D, joint);}
	virtual void	CompSpatialTransform();
	virtual void	CompJointJacobian();
	virtual void	CompCoriolisAccel();
	virtual void	InitArticulatedInertia();
	virtual void	AccumulateInertia();
	virtual void	AccumulateBiasForce();
	virtual void	CompBiasForceDiff(bool bUpdate);
	virtual void	CompAccel();
	virtual void	CompAccelDiff(bool bUpdate);
	virtual void	Projection(double& f, int k);
	virtual void	UpdateJointVelocity(double dt);
	virtual void	UpdateJointPosition(double dt);
	
	PHTreeNode1D();
};

}

#endif
