/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHCONTACTSURFACE_H
#define PHCONTACTSURFACE_H

#include <Physics/PHContactPoint.h>

namespace Spr{;

class PHShapePairForLCP;

class PHContactSurface : public PHContactPoint{
public:
	/// ZMP
	Vec3d	fpoint;

	/// �����f�ʂ̒��_�i�ڐG���W�n�ł̒l�j�D�g���N�̐����̂��߂ɕێ�
	std::vector<Vec3d>	section;

	///�O��A�ŋߓ_���������ӂ̒��_��\��section�ԍ�
	int lastL,lastR;
	
	/// �R���X�g���N�^
	PHContactSurface(){}
	PHContactSurface(PHShapePairForLCP* sp, Vec3d p, PHSolid* s0, PHSolid* s1, std::vector<Vec3d> sec);
	PHContactSurface(const Matrix3d& local, PHShapePairForLCP* sp, Vec3d p, PHSolid* s0, PHSolid* s1, std::vector<Vec3d> sec);

	// ----- PHConstraint�̋@�\���I�[�o�[���C�h
	
	virtual void IterateLCP();

	// ----- PHConstraint�̔h���N���X�Ŏ�������@�\

	virtual void CompBias();
	virtual void Projection(double& f_, int k);

	// ----- ���̃N���X�Ŏ�������@�\

	void	ProjectionTorque(SpatialVector& fnew);	///< �g���N�pProjection
	double	CalcSignedArea(Vec3d p, Vec3d q, Vec3d r);	///< �O�p�`pqr�̕����t���ʐώZ�o
	bool	CheckInside(int& L, int& R);		///< ��p�_���ʓ��ɂ��邩����
	void	SearchClosestPoint(int L, int R);	///< ��p�_�̖ʓ��̍ŋߓ_��������
	
	// ��]���C�p
	void	IntegrateArea();//�ڐG�ʂ�ϕ�
	double	rotateFriction;//��]���C�ɖ��C�W���Ɛ����R�͂�������O�̒l������
	int		segmentationNum;//�ʂ̕�����
	double  interval;//�ڐG�ʂ���؂鍏�ݕ�
};

}

#endif
