/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef FWOLDSPRINGHEADNODEHANDLER_H
#define FWOLDSPRINGHEADNODEHANDLER_H

namespace SprOldSpringhead{;
using namespace Spr;

///	\defgroup gpFWOldSpringheadNode ��(Springhead1)�t�@�C�����[�h�p�݊��m�[�h
//@{
/// X�t�@�C���`���̃o�[�W�������
struct Header{
	unsigned int major;		
	unsigned int minor;		
	unsigned int flags;
};
	
///	DirectX��Frame�D GRFrame�ɑΉ�
struct Frame{
};
	
///	DirectX��FrameTransformMatrix�D�t���[���̕ϊ��s���\���DGRFrame::transform�ɑΉ��D
struct FrameTransformMatrix{
	Affinef matrix;
};

///	Springehead1�̌����DGRLight�ɑΉ��D
struct Light8{
	enum XLightType{
		XLIGHT_POINT = 1,			///< �_����
		XLIGHT_SPOT = 2,			/// �X�|�b�g���C�g
		XLIGHT_DIRECTIONAL = 3,		///< ���s����
	} type;
    Vec4f diffuse;		///<	�g�U��
    Vec4f specular;		///<	���ʌ�
    Vec4f ambient;		///<	����
    Vec3f position;		///<	�����̈ʒu
    Vec3f direction;	///<	�����̕���
    float range;		///<	�����͂��͈�
    float falloff;		///<	�����̑���(�傫���قǋ}�s)		0..��
    ///@name	�������DAtten = 1/( att0 + att1 * d + att2 * d^2)
	//@{
	float attenuation0;	///<	att0	0..��
    float attenuation1;	///<	att1	0..��
    float attenuation2;	///<	att2	0..��
	//@}
	float spotInner;	///<	�X�|�b�g���C�g�̒��S����		0..spotCutoff
	float spotCutoff;	///<	�X�|�b�g���C�g�̌���������͈�	0..��
};

///	DirectX�̃}�e���A���DGRMateiral�ɑΉ��D
struct Material{
	Vec4f face;				// face color
	float power;			// shininess power
	Vec3f specular;			// specular color
	Vec3f emissive;			// emissive color
};

///	DirectX��Mesh�̖ʁD
struct MeshFace{
	int nFaceVertexIndices;
	std::vector<int> faceVertexIndices;
};

///	DirectX��Mesh�DGRMesh�ɑΉ��D
struct Mesh{
	int nVertices;
	std::vector<Vec3f> vertices;
	int nFaces;
	std::vector<MeshFace> faces;
};

///	DirectX��Mesh�̖@���x�N�g��
struct MeshNormals{
	int nNormals;
	std::vector<Vec3f> normals;
	int nFaceNormals;
	std::vector<MeshFace> faceNormals;
};

///	DirectX��Mesh�̃}�e���A�����X�g�D
struct MeshMaterialList{
	int nMaterials;
	int nFaceIndexes;
	std::vector<int> faceIndexes;
};

/// DirectX�̃e�N�X�`���t�@�C�����iMaterial�̓����^�O)
struct TextureFilename{
	std::string filename;
};

///	DirectX��Mesh�̃e�N�X�`�����W�D
struct MeshTextureCoords{
	int nTextureCoords;
	std::vector<Vec2f> textureCoords;
};

///	Springhead1��Scene
struct Scene{
};

///	Springhead1��Simulator
struct Simulator{
	double timeStep;	///< �ϕ��X�e�b�v
	double decay;		///< ���̂̑��x�̌�����	
};
	
///	Springhead1��Camera
struct Camera{
	Affinef view;
	float width;
	float height;
	float offsetX;
	float offsetY;
	float front;
	float back;
};

///	Springhead1��Solid
struct Solid{
	float mass;				///<	����
	Matrix3f inertia;		///<	�������[�����g
	Vec3f velocity;			///<	���x
	Vec3f angularVelocity;	///<	�p���x
	Vec3f center;			///<	���S�ʒu
};

/// Springhead1�̕����}�e���A��
struct PhysicalMaterial{				
	float ns;				///< �R�͂̂΂ˌW��
	float nd;				///< �R�͂̃_���p�W��
	float fs;				///< ���C�͂̂΂ˌW��
	float fd;				///< ���C�͂̃_���p�W��
	float s;				///< �ő�Î~���C�W��
	float d;				///< �����C�W��
};

/// Springhead1��SolidContainer
struct SolidContainer{
};
	
///	Springhead1��GravityEngine
struct GravityEngine{
	Vec3f gravity;
};

///	Springhead1��ContactEngine
struct ContactEngine{
};

/// Springhead1��JointEngine
struct JointEngine{
};
	
/// Springhead1��Joint
struct Joint{
	int			nType;				///<	�֐ߎ�� 0:����, 1:�X���C�h
	Vec3f		prj;				///<	�e���̂̃t���[�����猩���֐߈ʒu
	Matrix3f	pRj;				///<	�e���̂̃t���[�����猩���֐ߎp��
	Vec3f		crj;				///<	�q���̂̃t���[�����猩���֐߈ʒu
	Matrix3f	cRj;				///<	�q���̂̃t���[�����猩���֐ߎp��

	float		fPosition;			///<	�ψ�
	float		fVelocity;			///<	���x
	float		fMaxTorque;			///<	�ő�g���N
	float		fMinPosition;		///<	���͈�(�ő�l)
	float		fMaxPosition;		///<	���͈�(�ŏ��l)
	int			nInputType;			///<	���͂̎�� @see PHJointInputType
	float		fInput;				///<	���͒l�̏����l
	float		fPValue;			///<	PID�����P�̒l
	float		fIValue;			///<	PID�����I�̒l
	float		fDValue;			///<	PID�����D�̒l
};

/// Springhead1��Import	
struct Import{
	std::string file;	
};
//@}

}
#endif
