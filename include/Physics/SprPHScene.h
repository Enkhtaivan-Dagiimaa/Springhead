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

namespace Spr{;

struct PHSdkIf;

struct CDShapeIf;
struct CDShapeDesc;
struct PHEngineIf;
struct PHConstraintEngineIf;
struct PHGravityEngineIf;
struct PHPenaltyEngineIf;

typedef PHConstraintDesc PHJointDesc;

/// �����G���W���̃V�[���̏��
struct PHSceneState{
	/// �ϕ��X�e�b�v
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
	
	Vec3f	gravity;	///< �d�͉����x�x�N�g���D�f�t�H���g�l��(0.0f, -9.8f, 0.0f)�D
	int		numIteration;	///< LCP�̔�����

	PHSceneDesc(){Init();}
	void Init(){
		gravity=Vec3f(0,-9.8f,0);
		numIteration = 15;
	}
};

///	�V�[��
struct PHSceneIf : public SceneIf{
public:
	IF_DEF(PHScene);

	/** @brief ����Scene������SDK��Ԃ�
		@return SDK�̃C���^�t�F�[�X
	 */
	virtual PHSdkIf* GetSdk()=0;

	/** @brief Solid���쐬����
		@param desc �쐬����Solid�̃f�B�X�N���v�^
		@return Solid�̃C���^�t�F�[�X
	*/
	virtual PHSolidIf* CreateSolid(const PHSolidDesc& desc = PHSolidDesc())=0;
	
	/** @brief Solid�̐����擾����
		@return Solid�̐�
	 */
	virtual int NSolids()const =0;

	/** @brief Solid���擾����
		@return Solid�̃C���^�t�F�[�X�̔z��ւ̃|�C���^
	 */
	virtual PHSolidIf** GetSolids()=0;

	/** @brief �w�肵�����̓��m�̐ڐG�̃��[�h��ݒ肷��
		@param lhs �g�̕Е��̍��̂ւ̃|�C���^
		@param rhs �g�̂����Е��̍��̂ւ̃|�C���^
		@param mode �ڐG�̃��[�h

		����lhs�ƍ���rhs�Ƃ̐ڐG�̃��[�h��mode�ɐݒ肷��D
	  */
	virtual void SetContactMode(PHSolidIf* lhs,	PHSolidIf* rhs, PHSceneDesc::ContactMode mode = PHSceneDesc::MODE_LCP)=0;

	/** @brief �w�肵���W���Ɋ܂܂��S�Ă̍��̓��m�̐ڐG�̃��[�h��ݒ肷��
		@param group ���̂ւ̃|�C���^�z��̐擪�A�h���X
		@param size �|�C���^�z��̒���
		@param mode �ڐG�̃��[�h

		group[0]����group[size-1]�܂ł̑S�Ă̍��̂̑g�̐ڐG�̃��[�h��mode�ɐݒ肷��D
	 */
	virtual void SetContactMode(PHSolidIf** group, size_t length, PHSceneDesc::ContactMode mode = PHSceneDesc::MODE_LCP)=0;

	/** @brief �w�肵�����̂Ƒ��̍��̂Ƃ̑S�Ă̐ڐG�̃��[�h��ݒ肷��
		@param mode �ڐG�̃��[�h
	 */
	virtual void SetContactMode(PHSolidIf* solid, PHSceneDesc::ContactMode mode = PHSceneDesc::MODE_LCP)=0;

	/** @brief �V�[�����ۗL����S�Ă̍��̓��m�̐ڐG�̃��[�h��ݒ肷��
		@param mode �ڐG�̃��[�h
	 */
	virtual void SetContactMode(PHSceneDesc::ContactMode mode = PHSceneDesc::MODE_LCP)=0;

	///	@brief LCP�\���o�̌v�Z�񐔂̎擾�DMODE_LCP�̏ꍇ�̍S���͂̌J��Ԃ��ߎ��v�Z�̉񐔁D
	virtual int GetNumIteration()=0;
	///	@brief LCP�\���o�̌v�Z�񐔂̐ݒ�D
	virtual void SetNumIteration(int n)=0;
	
	/** @brief �֐߂��쐬����
		@param lhs �֐߂����t���鍄��
		@param rhs �֐߂����t���鍄��
		@param desc �֐߂̃f�B�X�N���v�^
		@return �֐߂̃C���^�t�F�[�X
		����lhs�ƍ���rhs��A������֐߂��쐬����D
		desc�ɂ͍쐬�������֐߂̎�ނɑΉ�����f�B�X�N���v�^�iPHJointDesc����h������j��n���D
		lhs�Ƀ\�P�b�g���Crhs�Ƀv���O�����t������D
	 */
	virtual PHJointIf* CreateJoint(PHSolidIf* lhs, PHSolidIf* rhs, const IfInfo* ii, const PHJointDesc& desc)=0;
	template <class T> PHJointIf* CreateJoint(PHSolidIf* lhs, PHSolidIf* rhs, const T& desc){
		return CreateJoint(lhs, rhs, T::GetIfInfo(), desc);
	}
	
	/** @brief �֐߂̐����擾����
	 */
	virtual int NJoints()const=0;

	/** @brief �֐߂��擾����
	 */
	virtual PHJointIf* GetJoint(int i)=0;

	/** @brief �ڐG�S���̐����擾����
	 */
	virtual int NContacts()const=0;

	/** @brief �ڐG�S�����擾����
	 */
	virtual PHContactPointIf* GetContact(int i)=0;

	/** @brief ���[�g�m�[�h���쐬����
		@param root ���[�g�m�[�h�ƂȂ鍄��
		@return ���[�g�m�[�h�̃C���^�t�F�[�X
		�֐߃c���[�̍��ƂȂ�m�[�h���쐬����D
	 */
	virtual PHRootNodeIf* CreateRootNode(PHSolidIf* root, const PHRootNodeDesc& desc = PHRootNodeDesc())=0;

	/** @brief ���[�g�m�[�h�̐�
	 */
	virtual int NRootNodes()const=0;

	/** @brief ���[�g�m�[�h���擾����
	 */
	virtual PHRootNodeIf* GetRootNode(int i)=0;

	/** @brief �c���[�m�[�h���쐬����
		@param parent �e�m�[�h�̃C���^�t�F�[�X
		@param child �q�m�[�h�ƂȂ鍄��
		@return �c���[�m�[�h�̃C���^�t�F�[�X
		����child���Q�Ƃ���c���[�m�[�h���쐬���C�����̃m�[�hparent�̎q�m�[�h�Ƃ��Ēǉ�����D
		parent���w�����̂�child���Ȃ��֐߂�CreateTreeNode�̌Ăяo�������O��
		CreateJoint�ɂ���č쐬����Ă��Ȃ���΂Ȃ�Ȃ��D
		����ɁCparent���\�P�b�g���Cchild���v���O���ł���K�v������D
	 */
	virtual PHTreeNodeIf* CreateTreeNode(PHTreeNodeIf* parent, PHSolidIf* child, const PHTreeNodeDesc& desc = PHTreeNodeDesc())=0;

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
	virtual PHGearIf* CreateGear(PHJoint1DIf* lhs, PHJoint1DIf* rhs, const PHGearDesc& desc = PHGearDesc())=0;
	
	/** @brief �M�A�̐�
	 */
	virtual int NGears()const=0;

	/** @brief �M�A���擾����
	 */
	virtual PHGearIf* GetGear(int i)=0;

	/** @brief �p�X���쐬����
		@param desc �p�X�̃f�B�X�N���v�^
		�p�X���쐬����D
		�p�X��PHPathJoint�̋O����\�����邽�߂ɗp����D
	 */
	virtual PHPathIf* CreatePath(const PHPathDesc& desc = PHPathDesc())=0;

	/** @brief �p�X�̐�
	*/
	virtual int NPaths() = 0;

	/** @brief �p�X���擾����
	*/
	virtual PHPathIf* GetPath(int i)=0;


	/** @brief �ϕ��X�e�b�v���擾����
		@return �ϕ��X�e�b�v
	 */
	virtual double GetTimeStep()const=0;
	
	/** @brief �ϕ��X�e�b�v��ݒ肷��
		@param dt �ϕ��X�e�b�v
	 */
	virtual void SetTimeStep(double dt)=0;
	
	/** @brief �J�E���g�����擾����
		@return �J�E���g��
		�V�[�����쐬����Ă��猻�݂܂ł�Step()�����s�����񐔂�Ԃ��D
	 */
	virtual unsigned GetCount()const=0;

	/** @brief �J�E���g����ݒ肷��
		@param count �J�E���g��
	 */
	virtual void SetCount(unsigned count)=0;
	
	/** @brief �V�[���̎�����i�߂�
	 */
	virtual void Step()=0;
	virtual void ClearForce()=0;
	virtual void GenerateForce()=0;
	virtual void Integrate()=0;

	/** @brief �V�[������ɂ���
	 */
	virtual void Clear()=0;

	/** @brief �d�͂�ݒ肷��
		@param accel �d�͉����x�x�N�g��
	 */
	//�����Ɏ�������ׂ����v���������CNovodex�͂��� 
	virtual void SetGravity(const Vec3d& accel)=0;
	
	/** @brief �d�͂��擾����
		@return �d�͉����x�x�N�g��
	 */
	virtual Vec3d GetGravity()=0;

	virtual int NEngines() = 0;
	/** @brief i�Ԗڂ̃G���W�����擾����
		@param i �G���W���ԍ��̃C���f�b�N�X
		@return �I�������G���W���ւ̃|�C���^
	*/
	virtual PHEngineIf* GetEngine(int i) = 0;

	/** @brief ConstraintEngine���擾����
		@return PHConstraintEngine�ւ̃|�C���^
	*/
	virtual PHConstraintEngineIf* GetConstraintEngine() = 0;

	/** @brief GravityEnigne���擾����
		@return GravityEngine�ւ̃|�C���^
	*/
	virtual PHGravityEngineIf*	GetGravityEngine() = 0;

	/** @brief PenaltyEngine���擾����
		@return PenaltyEngine�ւ̃|�C���^
	*/
	virtual PHPenaltyEngineIf*  GetPenaltyEngine() = 0;

};

//@}

}	//	namespace Spr
#endif
