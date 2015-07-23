/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file SprPHSolid.h
 *	@brief ����
*/
#ifndef SPR_PHOPOBJ_H
#define SPR_PHOPOBJ_H

#include <Foundation/SprObject.h>



/**	\addtogroup gpPhysics	*/
//@{
namespace Spr{;


///	���̂̃X�e�[�g
struct PHOpObjState{
	Vec3d		velocity;		///<	���ʒ��S�̑��x		(World�n)
	Vec3d		angVelocity;	///<	�p���x				(World�n)
//	Vec3d		lastVelocity;	///<	�O��̑��x			(World�n)
//	Vec3d		lastAngVelocity;///<	�O��̊p���x			(World�n)
	Posed		pose;			///<	���W���_�̈ʒu�ƌ���	(World�n)
//	Posed		lastPose;		///<	�O��̈ʒu�ƌ���		(World�n)
};


struct PHOpObjDesc : public PHOpObjState{
	double		mass;			///<	����
	Matrix3d	inertia;		///<	�����e���\��	(Local�n)
	Vec3d		center;			///<	���ʒ��S�̈ʒu	(Local�n)
	bool		dynamical;		///<	�����@���ɏ]����(���x�͐ϕ������)

	PHOpObjDesc(){ Init(); }
	void Init(){
		mass = 1.0f;
		inertia = Matrix3d::Unit();
		dynamical = true;
	}
};

struct CDShapeIf; 
struct PHTreeNodeIf;

///	���̂̃C���^�t�F�[�X
struct PHOpObjIf : public SceneObjectIf{
	SPR_IFDEF(PHOpObj);
	
	
	bool initialDeformObject(Vec3f *vts, int vtsNum, float pSize);
	void		SetGravity(bool bOn);
	void SimpleSimulationStep();

	void AddVertextoLocalBuffer(Vec3f v);
	bool InitialObjUsingLocalBuffer(float pSize);

	//�B�����ق��������֐�����
	void positionPredict();
	void groupStep();
	void integrationStep();
	void ReducedPositionProject();
	void positionProject();

	void BuildBlendWeight();
	

};

//@}

}	//	namespace Spr
#endif
