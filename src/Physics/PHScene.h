/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHSCENE_H
#define PHSCENE_H
#include <Physics/SprPHScene.h>
#include <Foundation/Scene.h>
#include <Physics/PHEngine.h>
#include <Physics/PHSolid.h>

namespace Spr {;

class CDShape;
struct CDShapeDesc;
class PHSolidContainer;
class PHPenaltyEngine;
class PHConstraintEngine;
class PHGravityEngine;
struct PHConstraintDesc;
typedef PHConstraintDesc PHJointDesc;

/**	�V�[���O���t�̃g�b�v�m�[�h�D�����E���_�����D
	�����_���ƃV�[���O���t�̊֌W���[�����߁C
	�����_�����Ⴄ�ƃV�[���O���t�͂��̂܂܎g�p�ł��Ȃ��D
	�V�[���O���t�́C���Ƃ��΃����_����OpenGL�Ȃ�CdisplayList
	�������Ă��邵�CD3D�Ȃ�� ID3DXMesh�������Ă���D
	OpenGL�̃V�[���O���t��D3D�ɕϊ����邽�߂ɂ́C��xDocument��
	�Z�[�u���āCD3D�`���Ń��[�h���Ȃ���΂Ȃ�Ȃ��D	*/
class SPR_DLL PHScene : public Scene, public PHSceneIfInit, public PHSceneDesc{
	OBJECTDEF(PHScene, Scene);
	friend class PHConstraint;
public:
	PHEngines engines;
protected:
	PHSolidContainer*	solids;
	PHPenaltyEngine*	penaltyEngine;
	PHConstraintEngine* constraintEngine;
	PHGravityEngine*	gravityEngine;
public:
	///	�R���X�g���N�^
	PHScene(const PHSceneDesc& desc=PHSceneDesc());
	void Init();
	///	�f�X�g���N�^
	~PHScene(){}

	PHConstraintEngine* GetConstraintEngine();
	PHGravityEngine*	GetGravityEngine();
	PHSolidIf* CreateSolid();
	PHSolidIf* CreateSolid(const PHSolidDesc& desc);
	CDShapeIf* CreateShape(const CDShapeDesc& desc);
	PHJointIf* CreateJoint(const PHJointDesc& desc);	///< �t�@�C�����[�_�p
	PHJointIf* CreateJoint(PHSolidIf* lhs, PHSolidIf* rhs, const PHJointDesc& desc);
	PHRootNodeIf* CreateRootNode(PHSolidIf* root);
	PHTreeNodeIf* CreateTreeNode(PHTreeNodeIf* parent, PHSolidIf* child);
	PHGearIf*	CreateGear(PHJoint1DIf* lhs, PHJoint1DIf* rhs, const PHGearDesc& desc);
	PHPathIf*  CreatePath(const PHPathDesc& desc);
	void SetContactMode(PHSolidIf* lhs, PHSolidIf* rhs, PHSceneDesc::ContactMode = PHSceneDesc::MODE_LCP);
	void SetContactMode(PHSolidIf** group ,size_t length, PHSceneDesc::ContactMode mode = PHSceneDesc::MODE_LCP);
	void SetContactMode(PHSolidIf* solid, PHSceneDesc::ContactMode = PHSceneDesc::MODE_LCP);
	void SetContactMode(PHSceneDesc::ContactMode mode = PHSceneDesc::MODE_LCP);
	virtual int GetNumIteration();
	virtual void SetNumIteration(int n);
	PHSdkIf* GetSdk();

	/// �ϕ��X�e�b�v��Ԃ�
	double GetTimeStep()const{return timeStep;}
	/// �ϕ��X�e�b�v��ݒ肷��
	void SetTimeStep(double dt);
	/// �J�E���g����Ԃ�
	unsigned GetCount()const{return count;}
	/// �J�E���g����ݒ肷��
	void SetCount(unsigned c){count = c;}
	///	�V�[���̎�����i�߂� ClearForce(); GenerateForce(); Integrate(); �Ɠ���
	void Step();
	///	�V�[���̎�����i�߂�i�͂̃N���A�j
	void ClearForce();
	///	�V�[���̎�����i�߂�i�͂̐����j
	void GenerateForce();
	///	�V�[���̎�����i�߂�i�͂Ƒ��x��ϕ����āC���x�ƈʒu���X�V�j
	void Integrate();
	///	�V�[������ɂ���D
	void Clear();

	virtual void SetGravity(const Vec3d& accel);
	virtual Vec3f GetGravity();

	virtual int NSolids();
	virtual PHSolidIf** GetSolids();
	ObjectIf* CreateObject(const IfInfo* info, const void* desc);
	virtual size_t NChildObject() const;
	virtual ObjectIf* GetChildObject(size_t pos);
	virtual bool AddChildObject(ObjectIf* o);
	virtual bool DelChildObject(ObjectIf* o);

	friend class PHSolid;
	friend class Object;
	ACCESS_DESC_STATE(PHScene);
};

}
#endif
