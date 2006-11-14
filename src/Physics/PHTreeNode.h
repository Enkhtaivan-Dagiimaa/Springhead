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
class PHTreeNode : public InheritSceneObject<PHTreeNodeIf, SceneObject>, public UTTreeNode<PHTreeNode>{
public:
	OBJECT_DEF_ABST(PHTreeNode);
	
	void		SetScene(SceneIf* s){scene = DCAST(PHScene, s);}
	bool		Includes(PHTreeNode* node);		///< �����ȉ��Ƀm�[�hnode�����邩
	PHTreeNode*	FindBySolid(PHSolid* solid);	///< �����ȉ��̃c���[��solid���Q�Ƃ��Ă���m�[�h��T��
	//int GetNumOfNodes();
	//int GetTotalDof();							///< �q�m�[�h���܂߂����R�x�̍��v
	//int AssignID(int id, std::vector<PHTreeNode*>& table);		///< �ċA�I��ID������U��

	void CompIsolatedInertia();
	void CompIsolatedBiasForce();
	void CompSpatialTransform();					///< �m�[�h�Ԃ̍��W�ϊ��̌v�Z
	void CompCoriolisAccel();
	PHTreeNode();

	virtual PHSolid*	GetSolid(){return joint->solid[1];}
	virtual PHRootNode*	GetRootNode(){return GetParent()->GetRootNode();}
	
	virtual void CompAccel(){}
	virtual void CompAccelDiff(bool bUpdate){}
	/// ���̃m�[�h�ɉ����S���͂̕ω��ʂ��瑼�̃m�[�h�̑��x�ω��ʂւ̉e�����v�Z�DLCP�Ŏg�p
	virtual void CompResponse(const SpatialVector& dF, bool bUpdate = true);
	virtual void CompBiasForceDiff(bool bUpdate){}
	virtual void CompArticulatedInertia();			///< Articulated Inertia�̌v�Z
	virtual void CompArticulatedBiasForce();		///< Articulated Bias Force�̌v�Z
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
	SpatialVector			Z;				///< Articulated Offset Force
	SpatialVector			dZ;				///< �S���͂̕ω��ɂ��Z�̕ω���
	SpatialVector			c;				///< �R���I�������x
	SpatialVector			cj;				///< �֐ߑ��x�ɂ��R���I�������x

	SpatialTransform		Xcp, Xcj;
	SpatialMatrix			IJ_JIJinv_Jtr, IJ_JIJinv_JtrI;
	SpatialVector			a;				///< �����x
	SpatialVector			da;				///< �S���͂̕ω��ɂ��a�̕ω���
	SpatialVector			ap, Ic, ZplusIc;
protected:
	PHScene*	scene;
};

class PHRootNode : public InheritSceneObject<PHRootNodeIf, PHTreeNode>{
public:
	OBJECT_DEF(PHRootNode);

	void SetupDynamics();
	//void CompDefaultVelocity(double dt);
	//void CompEquationOfMotion();

	virtual PHSolid* GetSolid(){return solid;}
	virtual PHRootNode* GetRootNode(){return this;}
	virtual int	 GetDof(){ return 6; }
	virtual void CompArticulatedInertia();
	virtual void CompArticulatedBiasForce();
	//virtual void GetJointAccel();
	virtual void CompAccel();
	virtual void CompBiasForceDiff(bool Update);
	virtual void UpdateVelocity(double dt);
	virtual void UpdatePosition(double dt);
	
	PHSolid*	solid;		///< ���[�g�m�[�h�̍���
protected:
	//int		nNodes;								///< ���m�[�h��	
	//std::vecotr<PHTreeNode*>	nodeTable;		///< �e�m�[�h�ւ̃_�C���N�g�A�N�Z�X�p�e�[�u��
	//UTCombination< std::vector<Matrix6d> >	inverseInertia;	///< �����s��̋t�s��
	//std::vector<Vec6d>			offsetAccel;	///< �S�֐߃g���N��0�̏ꍇ�̊֐߉����x

	SpatialMatrix		Iinv;
};

///	1���R�x�̊֐�
class PHTreeNode1D : public PHTreeNode{
public:
	virtual void AccumulateInertia();
	virtual void AccumulateBiasForce();
	virtual void CompJointJacobian();
	virtual void CompAccel();
	virtual void CompAccelDiff(bool bUpdate);
	virtual void CompBiasForceDiff(bool bUpdate);
	virtual void UpdateJointVelocity(double dt);
	virtual void UpdateJointPosition(double dt);
protected:
	SpatialVector	J;							///< �֐߃��R�r�A���D1���Ȃ̂Ńx�N�g��
	SpatialVector	IJ, J_JIJinv, IJ_JIJinv;
	double			JIJ, JIJinv, J_ZplusIc;
	double			accel, daccel;				///< �֐߉����x
	PHJoint1D* GetJoint(){return DCAST(PHJoint1D, joint);}

};

///	�����R�x�̊֐߂̊�{�N���X
template<int NDOF>
class PHTreeNodeND : public PHTreeNode{
public:
	virtual void AccumulateInertia();
	virtual void AccumulateOffsetForce();
	virtual void CompJointJacobian();
	virtual void CompAccel();
	virtual void CompAccelDiff(bool bUpdate);
	virtual void CompBiasForceDiff(bool bUpdate);
	virtual void UpdateJointVelocity(double dt);
	virtual void UpdateJointPosition(double dt);	
protected:
	SpatialVector	J[NDOF];
	SpatialVector	IJ[NDOF], IJ_JIJinv[NDOF], J_JIJinv[NDOF];
	PTM::TMatrixCol<NDOF, NDOF, double> JIJ, JIJinv;
	PTM::TVector<NDOF, double>	J_ZplusIc;
	PTM::TVector<NDOF, double>	accel, daccel;		///< �֐ߑ��x�̕ω���
	PHJointND<NDOF>* GetJoint(){return DCAST(PHJointND<NDOF> , joint);}
};

}

#endif