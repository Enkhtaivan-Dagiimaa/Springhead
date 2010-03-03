/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

/** \addtogroup gpPhysics 	*/
//@{

/**
 *	@file SprPHScene.h
 *	@brief �V�[��
*/
#ifndef SPR_PHSCENEIF_H
#define SPR_PHSCENEIF_H
#include <Foundation/SprScene.h>
#include <Physics/SprPHSolid.h>
#include <Physics/SprPHJoint.h>
#include <Physics/SprPHIK.h>
#include <Physics/SprPHEngine.h>

namespace Spr{;

struct PHSdkIf;

struct CDShapeIf;
struct CDShapeDesc;
struct PHEngineIf;
struct PHConstraintEngineIf;
struct PHGravityEngineIf;
struct PHPenaltyEngineIf;

/// Ray cast�̌��ʕۑ��p
struct PHRaycastHit{
	PHSolidIf* solid;
	CDShapeIf* shape;
	Vec3f	 point;
	float	 distance;
//	PHRaycastResultEntry(){};
//	PHRaycastResultEntry(PHSolidIf* sld, CDShapeIf* shp, const Vec3f& p, float o):solid(sld), shape(shp), point(p), offset(o){}
};
/*struct PHRaycastResult{
	 std::vector<PHRaycastResultEntry> results;
	 PHRaycastResultEntry* Nearest();
};*/
struct PHRayDesc{
	Vec3d origin;
	Vec3d direction;
};
struct PHRayIf : SceneObjectIf{
public:
	SPR_IFDEF(PHRay);
	Vec3d	GetOrigin();
	void	SetOrigin(const Vec3d& ori);
	Vec3d	GetDirection();
	void	SetDirection(const Vec3d& dir);
	void	Apply();
	int		NHits();
	PHRaycastHit* GetHits();
	PHRaycastHit* GetNearest();
	PHRaycastHit* GetDynamicalNearest();
};

//struct PHJointDesc: public PHConstraintDesc{
//};

/// �����G���W���̃V�[���̏��
struct PHSceneState{
	/// �ϕ��X�e�b�v[s]
	double timeStep;
	/// �ϕ�������
	unsigned count;
	PHSceneState(){Init();}
	void Init(){
		timeStep = 0.005;
		count = 0;
	}
};
/// �����G���W���̃V�[���̃f�X�N���v�^
struct PHSceneDesc: public PHSceneState{
	///	�ڐG�̃��[�h
	enum ContactMode{
		MODE_NONE,		///< ��������
		MODE_PENALTY,	///< �y�i���e�B�@�ŉ���
		MODE_LCP		///< LCP�ŉ���
	};
	
	/** �ݒ�p�����[�^�D
		�eEngine��API����đS�p�����[�^���擾�E�ݒ�\�����C�ȉ��̃p�����[�^��
		�p�ɂɗp����̂�PHSceneIf������A�N�Z�X�ł���悤�ɂ��Ă���D�i�v�����j
	 */
	Vec3f	gravity;		///< �d�͉����x�x�N�g���D�f�t�H���g�l��(0.0f, -9.8f, 0.0f)�D
	double	airResistanceRate; ///<���X�e�b�v���̂̑��x�Ɋ|������{���@�f�t�H���g�l��1.0.
	int		numIteration;	///< LCP�̔�����

	PHSceneDesc(){Init();}
	void Init(){
		PHSceneState::Init();
		gravity			= Vec3f(0,-9.8f,0);
		airResistanceRate = 1.0;
		numIteration	= 15;
	}
};

///	�V�[��
struct PHSceneIf : public SceneIf{
public:
	SPR_IFDEF(PHScene);

	/** @brief ����Scene������SDK��Ԃ�
		@return SDK�̃C���^�t�F�[�X
	 */
	PHSdkIf* GetSdk();

	/** @brief Solid���쐬����
		@param desc �쐬����Solid�̃f�B�X�N���v�^
		@return Solid�̃C���^�t�F�[�X
	*/
	PHSolidIf* CreateSolid(const PHSolidDesc& desc = PHSolidDesc());
	
	/** @brief Solid�̐����擾����
		@return Solid�̐�
	 */
	int NSolids()const;

	/** @brief Solid���擾����
		@return Solid�̃C���^�t�F�[�X�̔z��ւ̃|�C���^
	 */
	PHSolidIf** GetSolids();

	/** @brief SolidPair�̐����擾����
		@return SolidPair�̐�
	*/
	int NSolidPairs()const;

	/** @brief SolidPair���擾����
		@return 
	*/
	PHSolidPairForLCPIf* GetSolidPair(int i, int j);

	/** @brief �w�肵�����̓��m�̐ڐG�̃��[�h��ݒ肷��
		@param lhs �g�̕Е��̍��̂ւ̃|�C���^
		@param rhs �g�̂����Е��̍��̂ւ̃|�C���^
		@param mode �ڐG�̃��[�h

		����lhs�ƍ���rhs�Ƃ̐ڐG�̃��[�h��mode�ɐݒ肷��D
	  */
	void SetContactMode(PHSolidIf* lhs,	PHSolidIf* rhs, PHSceneDesc::ContactMode mode = PHSceneDesc::MODE_LCP);

	/** @brief �w�肵���W���Ɋ܂܂��S�Ă̍��̓��m�̐ڐG�̃��[�h��ݒ肷��
		@param group ���̂ւ̃|�C���^�z��̐擪�A�h���X
		@param size �|�C���^�z��̒���
		@param mode �ڐG�̃��[�h

		group[0]����group[size-1]�܂ł̑S�Ă̍��̂̑g�̐ڐG�̃��[�h��mode�ɐݒ肷��D
	 */
	void SetContactMode(PHSolidIf** group, size_t length, PHSceneDesc::ContactMode mode = PHSceneDesc::MODE_LCP);

	/** @brief �w�肵�����̂Ƒ��̍��̂Ƃ̑S�Ă̐ڐG�̃��[�h��ݒ肷��
		@param mode �ڐG�̃��[�h
	 */
	void SetContactMode(PHSolidIf* solid, PHSceneDesc::ContactMode mode = PHSceneDesc::MODE_LCP);

	/** @brief �V�[�����ۗL����S�Ă̍��̓��m�̐ڐG�̃��[�h��ݒ肷��
		@param mode �ڐG�̃��[�h
	 */
	void SetContactMode(PHSceneDesc::ContactMode mode = PHSceneDesc::MODE_LCP);

	///	@brief LCP�\���o�̌v�Z�񐔂̎擾�DMODE_LCP�̏ꍇ�̍S���͂̌J��Ԃ��ߎ��v�Z�̉񐔁D
	int GetNumIteration();
	///	@brief LCP�\���o�̌v�Z�񐔂̐ݒ�D
	void SetNumIteration(int n);
	
	/** @brief �֐߂��쐬����
		@param lhs �֐߂����t���鍄��
		@param rhs �֐߂����t���鍄��
		@param desc �֐߂̃f�B�X�N���v�^
		@return �֐߂̃C���^�t�F�[�X
		����lhs�ƍ���rhs��A������֐߂��쐬����D
		desc�ɂ͍쐬�������֐߂̎�ނɑΉ�����f�B�X�N���v�^�iPHJointDesc����h������j��n���D
		lhs�Ƀ\�P�b�g���Crhs�Ƀv���O�����t������D
	 */
	PHJointIf* CreateJoint(PHSolidIf* lhs, PHSolidIf* rhs, const IfInfo* ii, const PHJointDesc& desc);
	template <class T> PHJointIf* CreateJoint(PHSolidIf* lhs, PHSolidIf* rhs, const T& desc){
		return CreateJoint(lhs, rhs, T::GetIfInfo(), desc);
	}
	
	/** @brief �֐߂̐����擾����
	 */
	int NJoints()const;

	/** @brief �֐߂��擾����
	 */
	PHConstraintIf* GetJoint(int i);

	/** @brief �ڐG�S���̐����擾����
	 */
	int NContacts()const;

	/** @brief �ڐG�S�����擾����
	 */
	PHContactPointIf* GetContact(int i);

	/** @brief ���[�g�m�[�h���쐬����
		@param root ���[�g�m�[�h�ƂȂ鍄��
		@return ���[�g�m�[�h�̃C���^�t�F�[�X
		�֐߃c���[�̍��ƂȂ�m�[�h���쐬����D
	 */
	PHRootNodeIf* CreateRootNode(PHSolidIf* root, const PHRootNodeDesc& desc = PHRootNodeDesc());

	/** @brief ���[�g�m�[�h�̐�
	 */
	int NRootNodes()const;

	/** @brief ���[�g�m�[�h���擾����
	 */
	PHRootNodeIf* GetRootNode(int i);

	/** @brief �c���[�m�[�h���쐬����
		@param parent �e�m�[�h�̃C���^�t�F�[�X
		@param child �q�m�[�h�ƂȂ鍄��
		@return �c���[�m�[�h�̃C���^�t�F�[�X
		����child���Q�Ƃ���c���[�m�[�h���쐬���C�����̃m�[�hparent�̎q�m�[�h�Ƃ��Ēǉ�����D
		parent���w�����̂�child���Ȃ��֐߂�CreateTreeNode�̌Ăяo�������O��
		CreateJoint�ɂ���č쐬����Ă��Ȃ���΂Ȃ�Ȃ��D
		����ɁCparent���\�P�b�g���Cchild���v���O���ł���K�v������D
	 */
	PHTreeNodeIf* CreateTreeNode(PHTreeNodeIf* parent, PHSolidIf* child, const PHTreeNodeDesc& desc = PHTreeNodeDesc());

	/** @brief �M�A���쐬����
		@param lhs �M�A�ŘA��������֐�
		@param rhs �M�A�ŘA��������֐�
		@param desc �M�A�̃f�B�X�N���v�^
		@return �M�A�̃C���^�t�F�[�X
		1���R�x�֐�lhs��rhs��A��������M�A���쐬����D
		�A�����ꂽ�֐߂́C
		rhs->GetPosition() = desc.ratio * lhs->GetPosition()
		�𖞂����悤�ɍS�������D
	 */
	PHGearIf* CreateGear(PHJoint1DIf* lhs, PHJoint1DIf* rhs, const PHGearDesc& desc = PHGearDesc());
	
	/** @brief �M�A�̐�
	 */
	int NGears()const;

	/** @brief �M�A���擾����
	 */
	PHGearIf* GetGear(int i);

	/** @brief �p�X���쐬����
		@param desc �p�X�̃f�B�X�N���v�^
		�p�X���쐬����D
		�p�X��PHPathJoint�̋O����\�����邽�߂ɗp����D
	 */
	PHPathIf* CreatePath(const PHPathDesc& desc = PHPathDesc());

	/** @brief �p�X�̐�
	*/
	int NPaths();

	/** @brief �p�X���擾����
	*/
	PHPathIf* GetPath(int i);

	/** @brief raycast�̌������쐬����
	 */
	PHRayIf* CreateRay(const PHRayDesc& desc = PHRayDesc());

	/** @brief �����̐�
	 */
	int NRays();

	/** @brief �������擾����
	 */
	PHRayIf* GetRay(int i);

	/** @brief IK�̃m�[�h�iIK�ɗp���邱�Ƃ̂ł������(=�֐߂Ȃ�)�j���쐬����D
	 */
	PHIKNodeIf* CreateIKNode(const IfInfo* ii, const PHIKNodeDesc& desc = PHIKNodeDesc());
	template <class T> PHIKNodeIf* CreateIKNode(const T& desc){
		return CreateIKNode(T::GetIfInfo(), desc);
	}

	/** @brief IK�m�[�h�̐�
	 */
	int NIKNodes();

	/** @brief IK�m�[�h���擾����
	 */
	PHIKNodeIf* GetIKNode(int i);

	/** @brief IK�̐���_���쐬����D
	 */
	PHIKControlPointIf*	 CreateIKControlPoint(const IfInfo* ii, const PHIKControlPointDesc& desc = PHIKControlPointDesc());
	template <class T> PHIKControlPointIf* CreateIKControlPoint(const T& desc){
		return CreateIKControlPoint(T::GetIfInfo(), desc);
	}

	/** @brief IK����_�̐�
	 */
	int NIKControlPoints();

	/** @brief IK����_���擾����
	 */
	PHIKControlPointIf* GetIKControlPoint(int i);

	/** @brief �ϕ��X�e�b�v���擾����
		@return �ϕ��X�e�b�v
	 */
	double GetTimeStep()const;

	/** @brief �ϕ��X�e�b�v�̋t�����擾����
		@return �ϕ��X�e�b�v�̋t��
	 */
	double GetTimeStepInv()const;

	/** @brief �ϕ��X�e�b�v��ݒ肷��
		@param dt �ϕ��X�e�b�v
	 */
	void SetTimeStep(double dt);
	
	/** @brief �J�E���g�����擾����
		@return �J�E���g��
		�V�[�����쐬����Ă��猻�݂܂ł�Step()�����s�����񐔂�Ԃ��D
	 */
	unsigned GetCount()const;

	/** @brief �J�E���g����ݒ肷��
		@param count �J�E���g��
	 */
	void SetCount(unsigned count);

	/** @brief �V�[���̎�����i�߂�
	 */
	void Step();
	void ClearForce();
	void GenerateForce();
	void Integrate();
	void IntegratePart1();
	void IntegratePart2();

	/** @brief �V�[������ɂ���
	 */
	void Clear();

	/** @brief �d�͂�ݒ肷��
		@param accel �d�͉����x�x�N�g��
	 */
	//�����Ɏ�������ׂ����v���������CNovodex�͂��� 
	void SetGravity(const Vec3d& accel);
	
	/** @brief �d�͂��擾����
		@return �d�͉����x�x�N�g��
	 */
	Vec3d GetGravity();

	/** @brief ��]�ɑ΂����C��R�̊�����ݒ肷��
		@param rate ��]�ɑ΂����C��R�̊��� �W����1.0 �䗦��������ƃV�~�����[�V���������肷��(PHSolid::UpdateVelocity()���ŌĂ΂��j
	 */
	void SetAirResistanceRate(double rate);
	
	/** @brief ��]�ɑ΂����C��R�̊������擾����
		@return ��]�ɑ΂����C��R�̊����@�W����1.0 �䗦��������ƃV�~�����[�V���������肷��(PHSolid::UpdateVelocity()���ŌĂ΂��j
	 */
	double GetAirResistanceRate();

	int NEngines();
	/** @brief i�Ԗڂ̃G���W�����擾����
		@param i �G���W���ԍ��̃C���f�b�N�X
		@return �I�������G���W���ւ̃|�C���^
	*/
	PHEngineIf* GetEngine(int i);

	/** @brief ConstraintEngine���擾����
		@return PHConstraintEngine�ւ̃|�C���^
	*/
	PHConstraintEngineIf* GetConstraintEngine();

	/** @brief GravityEnigne���擾����
		@return GravityEngine�ւ̃|�C���^
	*/
	PHGravityEngineIf*	GetGravityEngine();

	/** @brief PenaltyEngine���擾����
		@return PenaltyEngine�ւ̃|�C���^
	*/
	PHPenaltyEngineIf*  GetPenaltyEngine();

	/** @brief ��Ԃ̕ۑ� (ObjectStates �̕ۑ��j�ɁC
		Constraints �����S���͂��܂߂邩�ǂ����ݒ肷��D
	*/
	void SetStateMode(bool bConstraints);

	/** @brief IKEngine���擾����
		@return IKEngine�ւ̃|�C���^
	*/
	PHIKEngineIf*  GetIKEngine();

};

//@}

}	//	namespace Spr
#endif
