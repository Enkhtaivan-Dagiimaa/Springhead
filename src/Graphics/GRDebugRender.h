/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file GRDebugRender.h
 *	@brief �f�o�b�O��񃌃��_���[�i���́A�ʁj�@�@
*/
#ifndef GRDEBUGRENDER_H
#define GRDEBUGRENDER_H

#include "GRRender.h"

namespace Spr{;

/**	@class	GRDebugRender
    @brief	�f�o�b�O��񃌃��_���[�̎����@ */
class GRDebugRender:public GRRender, public GRDebugRenderIfInit{
	OBJECTDEF(GRDebugRender, GRRender);
protected:
	int matSampleCount;
	std::vector<GRMaterialDesc> matSample;		/// �����_���[�ŗp�ӂ��Ă���ގ�(24���)

	bool	modeSolid, modeWire;				/// �`�惂�[�h
	bool	renderAxis, renderForce;
	float	scaleAxis, scaleForce;
public:
	/**  �R���X�g���N�^  */
	GRDebugRender();
	virtual void DrawScene(PHSceneIf* scene);
	virtual void DrawSolid(PHSolidIf* so);
	virtual void DrawConstraint(PHConstraintIf* con);
	virtual void SetMaterialSample(GRDebugRenderIf::TMaterialSample matname);
	virtual void SetRenderMode(bool solid = true, bool wire = false){ modeSolid = solid; modeWire = wire; }
	virtual void EnableRenderAxis(bool enable = true, float scale = 1.0f){
		renderAxis = enable;
		scaleAxis = scale;
	}
	virtual void EnableRenderForce(bool enable = true, float scale = 1.0f){
		renderForce = enable;
		scaleForce = scale;
	}

	/// �J�v�Z���̕`��
	void DrawCapsule(CDCapsuleIf* cap, bool solid);
	/// ���b�V���̕`��
	void DrawMesh(CDConvexMeshIf* mesh, bool solid);
	/// �R�[���̕`��
	void DrawCone(float radius, float height, int slice, bool solid);
	/// �V�����_�̕`��
	void DrawCylinder(float radius, float height, int slice, bool solid);
	/// ���W���̕`��
	void DrawAxis(bool solid);
	void DrawCoordinateAxis(bool solid);
	/// �����̕`��
	void DrawLine(const Vec3d& p0, const Vec3d& p1);
	/// �ʂ̕`��
	void DrawFaceSolid(CDFaceIf* face, Vec3f * base);
	void DrawFaceWire(CDFaceIf* face, Vec3f * base);
	
	
};

}
#endif
