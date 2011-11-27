/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
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
#include <Physics/PHIKEngine.h>
#include <Physics/PHIKActuator.h>
#include <Physics/PHIKEndEffector.h>
#include <Physics/PHFemEngine.h>

namespace Spr {;

class CDShape;
struct CDShapeDesc;
class PHSolidContainer;
class PHPenaltyEngine;
class PHConstraintEngine;
class PHGravityEngine;
struct PHConstraintDesc;
struct PHJointDesc;
class PHScene;
class PHFemEngine;

class SPR_DLL PHRay : public SceneObject, public PHRayDesc{
	SPR_OBJECTDEF(PHRay);
protected:
	std::vector<PHRaycastHit>	hits;
public:
	Vec3d	GetOrigin(){ return origin; }
	void	SetOrigin(const Vec3d& ori){ origin = ori; }
	Vec3d	GetDirection(){ return direction; }
	void	SetDirection(const Vec3d& dir){ direction = dir; }
	void	Apply();
	int		NHits(){ return (int)hits.size(); }
	PHRaycastHit* GetHits(){ return &hits[0]; }
	PHRaycastHit* GetNearest();
	PHRaycastHit* GetDynamicalNearest();
};
typedef std::vector< UTRef<PHRay> > PHRays;

class SPR_DLL PHScene : public Scene, public PHSceneDesc{
	SPR_OBJECTDEF(PHScene);
	friend class PHConstraint;
public:
	PHEngines				engines;
protected:
	PHSolidContainer*		solids;
	PHPenaltyEngine*		penaltyEngine;
	PHConstraintEngine*		constraintEngine;
	PHGravityEngine*		gravityEngine;
	PHIKEngine*				ikEngine;
	PHRays					rays;
	PHFemEngine*			femEngine;
	double					timeStepInv;	///< timeStep�̋t���D�������p
public:
	
	friend class			PHSolid;
	friend class			PHFrame;
	friend class			Object;


	//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	///	�R���X�g���N�^
	PHScene(const PHSceneDesc& desc = PHSceneDesc());
	void Init();
	///	�f�X�g���N�^
	~PHScene(){}

	//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	//���̃N���X���ł̋@�\.
	PHEngines*				GetEngines();
	CDShapeIf*				CreateShape(const IfInfo* ii, const CDShapeDesc& desc);	
	double					GetTimeStepInv(){ return timeStepInv; }
	
	//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	//�C���^�t�F�[�X(PHSceneIf)�̎���
	PHSdkIf*				GetSdk();
	PHSolidIf*				CreateSolid(const PHSolidDesc& desc = PHSolidDesc());
	int						NSolids() const;
	PHSolidIf**				GetSolids();
	void					SetContactMode(PHSolidIf* lhs, PHSolidIf* rhs, PHSceneDesc::ContactMode = PHSceneDesc::MODE_LCP);
	void					SetContactMode(PHSolidIf** group ,size_t length, PHSceneDesc::ContactMode mode = PHSceneDesc::MODE_LCP);
	void					SetContactMode(PHSolidIf* solid, PHSceneDesc::ContactMode = PHSceneDesc::MODE_LCP);
	void					SetContactMode(PHSceneDesc::ContactMode mode = PHSceneDesc::MODE_LCP);
	virtual int				GetNumIteration();
	virtual void			SetNumIteration(int n);
	PHJointIf*				CreateJoint(PHSolidIf* lhs, PHSolidIf* rhs, const IfInfo* ii, const PHJointDesc& desc);
	int						NJoints() const;
	PHJointIf*				GetJoint(int i);
	int						NContacts() const;
	PHContactPointIf*		GetContact(int i);
	int						NSolidPairs() const;
	PHSolidPairForLCPIf*	GetSolidPair(int i, int j);
	PHSolidPairForLCPIf*	GetSolidPair(PHSolidIf* lhs, PHSolidIf* rhs, bool& bSwap);
//	UTCombination<UTRef<PHSolidPairForLCPIf>>	GetSolidPair();
	PHRootNodeIf*			CreateRootNode(PHSolidIf* root, const PHRootNodeDesc& desc = PHRootNodeDesc());
	int						NRootNodes() const;
	PHRootNodeIf*			GetRootNode(int i);
	PHTreeNodeIf*			CreateTreeNode(PHTreeNodeIf* parent, PHSolidIf* child, const PHTreeNodeDesc& desc = PHTreeNodeDesc());
	PHGearIf*				CreateGear(PHJoint1DIf* lhs, PHJoint1DIf* rhs, const PHGearDesc& desc = PHGearDesc());
	int						NGears() const;
	PHGearIf*				GetGear(int i);
	PHPathIf*				CreatePath(const PHPathDesc& desc = PHPathDesc());
	int						NPaths() const;
	PHPathIf*				GetPath(int i);
	PHRayIf*				CreateRay(const PHRayDesc& desc = PHRayDesc());
	int						NRays() const;
	PHRayIf*				GetRay(int i);
	double					GetAirResistanceRate(){return airResistanceRate;}
	void					SetAirResistanceRate(double rate){airResistanceRate =rate;}

	PHIKActuatorIf*				CreateIKActuator(const IfInfo* ii, const PHIKActuatorDesc& desc = PHIKActuatorDesc());
	int						NIKActuators();
	PHIKActuatorIf*				GetIKActuator(int i);
	PHIKEndEffectorIf*		CreateIKEndEffector(const IfInfo* ii, const PHIKEndEffectorDesc& desc = PHIKEndEffectorDesc());
	int						NIKEndEffectors();
	PHIKEndEffectorIf*		GetIKEndEffector(int i);
	int						NFemMeshes()const;
	PHFemMeshIf*			GetFemMesh(int i);

	void 					FindNeighboringSolids(PHSolidIf* solid, double range, PHSolidIfs& nsolids);

	/// �ϕ��X�e�b�v��Ԃ�
	double					GetTimeStep()const{return timeStep;}
	/// �ϕ��X�e�b�v��ݒ肷��
	void					SetTimeStep(double dt);
	/// �J�E���g����Ԃ�
	unsigned				GetCount()const{return count;}
	/// �J�E���g����ݒ肷��
	void					SetCount(unsigned c){count = c;}
	///	�V�[���̎�����i�߂� ClearForce(); GenerateForce(); Integrate(); �Ɠ���
	void					Step();
	///	�V�[���̎�����i�߂�i�͂̃N���A�j
	void					ClearForce();
	///	�V�[���̎�����i�߂�i�͂̐����j
	void					GenerateForce();
	///	�V�[���̎�����i�߂�i�͂Ƒ��x��ϕ����āC���x�ƈʒu���X�V�j
	void					Integrate();
	void					IntegratePart1();
	void					IntegratePart2();
	///	�V�[������ɂ���D
	void					Clear();
	virtual void			SetGravity(const Vec3d& accel);
	virtual Vec3d			GetGravity();
	int						NEngines();
	PHEngineIf*				GetEngine(int i);
	PHConstraintEngineIf*	GetConstraintEngine();
	PHGravityEngineIf*		GetGravityEngine();
	PHPenaltyEngineIf*		GetPenaltyEngine();
	PHIKEngineIf*			GetIKEngine();
	ObjectIf*				CreateObject(const IfInfo* info, const void* desc);
	virtual size_t			NChildObject() const;
	virtual ObjectIf*		GetChildObject(size_t pos);
	virtual bool			AddChildObject(ObjectIf* o);
	virtual bool			DelChildObject(ObjectIf* o);	
	virtual void			SetStateMode(bool bConstraints);

	ACCESS_DESC(PHScene);
	virtual size_t GetStateSize() const;
	virtual void ConstructState(void* m) const;
	virtual void DestructState(void* m) const;
	virtual const void* GetStateAddress() const { return NULL; } // not supported.
	virtual bool GetState(void* s) const;
	virtual void SetState(const void* s);
	virtual void GetStateR(char*& s);
	virtual void SetStateR(const char*& state);
	virtual bool WriteStateR(std::ostream& fout);
	virtual bool ReadStateR(std::istream& fin);
	virtual void DumpObjectR(std::ostream& os, int level=0) const;
protected:
	virtual void AfterSetDesc();
	virtual void BeforeGetDesc() const;
};

}
#endif
